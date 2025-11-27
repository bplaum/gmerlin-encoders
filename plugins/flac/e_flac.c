/*****************************************************************
 * gmerlin-encoders - encoder plugins for gmerlin
 *
 * Copyright (c) 2001 - 2024 Members of the Gmerlin project
 * http://github.com/bplaum
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * *****************************************************************/



#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include <config.h>

#include <gavl/metatags.h>

#include <gmerlin/plugin.h>
#include <gmerlin/pluginfuncs.h>
#include <gmerlin/utils.h>
#include <gmerlin/log.h>
#define LOG_DOMAIN "e_flac"

#include <gmerlin/translation.h>

#include <gavl/numptr.h>


#include <bgflac.h>
#include <vorbiscomment.h>


typedef struct
  {
  bg_flac_t * enc;
  
  char * filename;

  
  /* Configuration stuff */
  int use_vorbis_comment;
  int use_seektable;
  int num_seektable_entries;
  
  int add_cover;   // From config
  
  const gavl_dictionary_t * cover;
  
  int compressed;
  
  int64_t samples_written;

  bg_encoder_callbacks_t * cb;
  
  int64_t data_start;
  int64_t seektable_start;
  
  int64_t bytes_written;

  /* Table with *all* frames */
  FLAC__StreamMetadata_SeekPoint * frame_table;
  uint32_t frame_table_len;
  uint32_t frame_table_alloc;
  
  /* Generated seek table */
  FLAC__StreamMetadata_SeekPoint * seektable; 
  
  //  gavl_compression_info_t ci;
  // gavl_dictionary_t m_stream;
  // gavl_audio_format_t format;

  gavl_dictionary_t stream;
  
  int fixed_blocksize;

  gavl_audio_sink_t * sink;
  gavl_packet_sink_t * psink_int;
  gavl_packet_sink_t * psink_ext;
  
  const gavl_dictionary_t * m_global;

  int write_seektable;
  
  gavl_io_t * io;

  int streaming;
  } flac_t;

static int write_data(flac_t * f, const uint8_t * data, int len)
  {
  if(gavl_io_write_data(f->io, data, len) < len)
    return 0;
  f->bytes_written += len;
  return 1;
  }

static int write_comment(flac_t * f, int last)
  {
  int ret = 0;
  uint8_t * buf;
  uint8_t * ptr;
  uint8_t * comment_ptr;
  gavl_io_t * io;
  
  int len;

  io = gavl_io_create_mem_write();
  bg_vorbis_comment_write(io, gavl_stream_get_metadata(&f->stream), f->m_global, 0);
  comment_ptr = gavl_io_mem_get_buf(io, &len);
  
  buf = malloc(4 + len);
  ptr = buf;
  
  ptr[0] = 0x04;
  if(last)
    ptr[0] |= 0x80;
  ptr++;

  GAVL_24BE_2_PTR(len, ptr); ptr += 3;
  
  memcpy(ptr, comment_ptr, len);
  
  write_data(f, buf, len+4);
  free(buf);
  free(comment_ptr);
  gavl_io_destroy(io);
  return ret;
  }

static int write_seektable(FLAC__StreamMetadata_SeekPoint * index,
                           int len,
                           flac_t * f,
                           int last)
  {
  int i;
  int len_bytes;
  uint8_t buf[18];
  int ret = 0;
  uint8_t * ptr;

  ptr = buf;
  
  ptr[0] = 3;
  if(last)
    ptr[0] |= 0x80;

  ptr++;
  
  len_bytes = len * 18;
  GAVL_24BE_2_PTR(len_bytes, ptr);
  write_data(f, buf, 4);
  
  for(i = 0; i < len; i++)
    {
    ptr = buf;
    GAVL_64BE_2_PTR(index[i].sample_number, ptr); ptr += 8;
    GAVL_64BE_2_PTR(index[i].stream_offset, ptr); ptr += 8;
    GAVL_16BE_2_PTR(index[i].frame_samples, ptr); ptr += 2;
    write_data(f, buf, 18);
    }
  
  return ret;
  }

static void * create_flac()
  {
  flac_t * ret;
  ret = calloc(1, sizeof(*ret));
  return ret;
  }

static void set_callbacks_flac(void * data, bg_encoder_callbacks_t * cb)
  {
  flac_t * flac = data;
  flac->cb = cb;
  }

static const bg_parameter_info_t parameters[] =
  {
    {
      .name =        "use_vorbis_comment",
      .long_name =   TRS("Write vorbis comment"),
      .type =        BG_PARAMETER_CHECKBUTTON,
      .val_default = GAVL_VALUE_INIT_INT(1),
      .help_string = TRS("Write Vorbis comment containing metadata to the file")
    },
    {
      .name =        "add_cover",
      .long_name =   TRS("Add cover"),
      .type =        BG_PARAMETER_CHECKBUTTON,
      .val_default = GAVL_VALUE_INIT_INT(0),
      .help_string = TRS("Write the album cover to the flac file if available")
    },
    {
      .name =        "use_seektable",
      .long_name =   TRS("Write seek table"),
      .type =        BG_PARAMETER_CHECKBUTTON,
      .val_default = GAVL_VALUE_INIT_INT(1),
      .help_string = TRS("Write seektable (strongly recommended)")
    },
    {
      .name =        "num_seektable_entries",
      .long_name =   TRS("Entries in the seektable"),
      .type =        BG_PARAMETER_INT,
      .val_min = GAVL_VALUE_INIT_INT(1),
      .val_max = GAVL_VALUE_INIT_INT(1000000),
      .val_default = GAVL_VALUE_INIT_INT(100),
      .help_string = TRS("Maximum number of entries in the seek table. Default is 100, larger numbers result in\
 shorter seeking times but also in larger files.")
    },
    { /* End of parameters */ }
  };

static const bg_parameter_info_t * get_parameters_flac(void * data)
  {
  return parameters;
  }

static void set_parameter_flac(void * data,
                               const char * name,
                               const gavl_value_t * v)
  {
  flac_t * flac;
  flac = data;
  
  if(!name)
    return;

  else if(!strcmp(name, "use_vorbis_comment"))
    flac->use_vorbis_comment = v->v.i;
  else if(!strcmp(name, "add_cover"))
    flac->add_cover = v->v.i;
  else if(!strcmp(name, "use_seektable"))
    flac->use_seektable = v->v.i;
  else if(!strcmp(name, "num_seektable_entries"))
    flac->num_seektable_entries = v->v.i;
  }

static int streaminfo_callback(void * data, uint8_t * si, int len)
  {
  int first;
  flac_t * flac = data;

  int last = 0;
  
  if(flac->bytes_written)
    first = 0;
  else
    first = 1;
  
  if(first)
    {
    /* Set or clear the "last metadata packet" flag */

    if(flac->write_seektable || flac->use_vorbis_comment || flac->cover)
      last = 0;
    else
      last = 1;

    if(last)
      si[4] |= 0x80;
    else
      si[4] &= 0x7f;
    
    /* Write stream info */
    if(!write_data(flac, si, len))
      return 0;

    if(flac->use_vorbis_comment)
      {
      if(flac->write_seektable || flac->cover)
        last = 0;
      else
        last = 1;
      
      write_comment(flac, last);
      }
    if(flac->write_seektable)
      {
      if(flac->cover)
        last = 0;
      else
        last = 1;
      
      flac->seektable_start = flac->bytes_written;
      write_seektable(flac->seektable, flac->num_seektable_entries, flac, last);
      }
    if(flac->cover)
      {
      /* Write cover */

      uint8_t * buf;
      int len;

      gavl_io_t * io_mem = gavl_io_create_mem_write();
      bg_flac_cover_tag_write(io_mem, flac->cover, 1);
      buf = gavl_io_mem_get_buf(io_mem, &len);
      write_data(flac, buf, len);
      
      free(buf);
      gavl_io_destroy(io_mem);
      }
    
    }
  else if(!flac->streaming)
    {
    gavl_io_seek(flac->io, 0, SEEK_SET);
    if(!write_data(flac, si, len))
      return 0;
    }
  return 1;
  }

static int open_io_flac(void * data, gavl_io_t * io,
                        const gavl_dictionary_t * m)
  {
  int result = 1;
  flac_t * flac = data;
  flac->enc = bg_flac_create();
  flac->io = io;

  if(!gavl_io_can_seek(flac->io))
    flac->streaming = 1;
  
  bg_flac_set_callbacks(flac->enc, streaminfo_callback, flac);
  
  /* Create seektable */

  flac->write_seektable = flac->use_seektable;
  
  if(flac->streaming)
    flac->write_seektable = 0;

  /* Check if we have a cover */

  if(flac->add_cover)
    {
    const char * var;

    fprintf(stderr, "Cover:\n");
    gavl_value_dump(gavl_dictionary_get(m, GAVL_META_COVER_URL), 2);
    fprintf(stderr, "\n");
    
    flac->cover = gavl_dictionary_get_image_max(m, GAVL_META_COVER_URL, 600, 600, "image/jpeg");

    if(flac->cover &&
       (!(var = gavl_dictionary_get_string(flac->cover, GAVL_META_URI)) ||
        gavl_string_starts_with(var, "http://")))
      flac->cover = NULL;
    }
  
  if(flac->write_seektable)
    {
    int i;
    flac->seektable = calloc(flac->num_seektable_entries, sizeof(*flac->seektable));
    for(i = 0; i < flac->num_seektable_entries; i++)
      flac->seektable[i].sample_number = 0xFFFFFFFFFFFFFFFFLL;
    }

  flac->m_global = m;
  
  return result;
  
  }

static int open_flac(void * data, const char * filename,
                     const gavl_dictionary_t * m)
  {
  gavl_io_t * io;
  
  flac_t * flac = data;

  if(!strcmp(filename, "-"))
    {
    io = gavl_io_create_file(stdout, 1, 0, 0);
    }
  else
    {
    FILE * out;
    flac->filename = gavl_filename_ensure_extension(filename, "flac");
    if(!bg_encoder_cb_create_output_file(flac->cb, flac->filename))
      return 0;
    
    if(!(out = fopen(flac->filename, "wb")))
      {
      gavl_log(GAVL_LOG_ERROR, LOG_DOMAIN, "Cannot open %s: %s",
             flac->filename, strerror(errno));
      }
    io = gavl_io_create_file(out, 1, 1, 1);
    }

  return open_io_flac(data, io, m);
  
  }

static int add_audio_stream_flac(void * data,
                                 const gavl_dictionary_t * m,
                                 const gavl_audio_format_t * format)
  {
  flac_t * flac;
  flac = data;
  
  /* Copy and adjust format */
  gavl_init_audio_stream(&flac->stream);
  gavl_audio_format_copy(gavl_stream_get_audio_format_nc(&flac->stream), format);
  gavl_dictionary_copy(gavl_stream_get_metadata_nc(&flac->stream), m);
  
  return 0;
  }

static void append_packet(flac_t * f, int samples)
  {
  if(f->streaming)
    return;
  
  if(f->frame_table_len + 1 > f->frame_table_alloc)
    {
    f->frame_table_alloc += 10000;
    f->frame_table = realloc(f->frame_table,
                             f->frame_table_alloc * sizeof(*f->frame_table));
    }

  f->frame_table[f->frame_table_len].sample_number = f->samples_written;
  f->frame_table[f->frame_table_len].frame_samples = samples;
  f->frame_table[f->frame_table_len].stream_offset = f->bytes_written - f->data_start;
  f->frame_table_len++;

  //  fprintf(stderr, "Append packet %ld %d -> %ld\n", f->samples_written, samples,
  //          f->samples_written + samples);
  
  f->samples_written += samples;
  
  }

static gavl_sink_status_t
write_audio_packet_func_flac(void * priv, gavl_packet_t * packet)
  {
  flac_t * flac = priv;
  
  if(flac->data_start < 0)
    flac->data_start = flac->bytes_written;
  
  append_packet(flac, packet->duration);
  
  if(write_data(flac, packet->buf.buf, packet->buf.len))
    return GAVL_SINK_OK;
  else
    return GAVL_SINK_ERROR;
  }

static int start_flac(void * data)
  {
  flac_t * flac;
  flac = data;

  if(flac->compressed)
    {
    if(!(flac->psink_ext = bg_flac_start_compressed(flac->enc, &flac->stream)))
      return 0;
    }
  else
    {
    if(!(flac->sink = bg_flac_start_uncompressed(flac->enc, &flac->stream)))
      return 0;
    }
  
  flac->psink_int =
    gavl_packet_sink_create(NULL, write_audio_packet_func_flac, flac);
  bg_flac_set_sink(flac->enc, flac->psink_int);

  flac->data_start = -1;
  
  return 1;
  }

static gavl_audio_sink_t * get_audio_sink_flac(void * data, int stream)
  {
  flac_t * flac;
  flac = data;
  return flac->sink;
  }

static gavl_packet_sink_t * get_audio_packet_sink_flac(void * data, int stream)
  {
  flac_t * flac;
  flac = data;
  return flac->psink_ext;
  }

static void build_seek_table(flac_t * flac)
  {
  int i;

  /* We encoded fewer frames than we have in the seektable: Placeholders will remain there */
  if(flac->frame_table_len <= flac->num_seektable_entries)
    {
    memcpy(flac->seektable, flac->frame_table,
           flac->frame_table_len * sizeof(*flac->seektable));
    }
  /* More common case: We have more frames than we will have in the seek table */
  else
    {
    int index = 0;
    int64_t next_seek_sample;
    
    /* First entry is always copied */
    memcpy(flac->seektable, flac->frame_table,
           sizeof(*flac->seektable));
    
    index = 1;
    next_seek_sample = (flac->samples_written * index) / flac->num_seektable_entries;
    
    for(i = 1; i < flac->frame_table_len; i++)
      {
      if(flac->frame_table[i].sample_number >= next_seek_sample)
        {
        memcpy(flac->seektable + index, flac->frame_table + i,
               sizeof(*flac->seektable));
        index++;
        next_seek_sample = (flac->samples_written * index) / flac->num_seektable_entries;

        if(index >= flac->num_seektable_entries)
          break;
        }
      }
    }
  }

static void finalize(flac_t * flac)
  {
  int last = 0;
  
  if(!flac->io)
    return;
  
  /* Update stream info */


  
  /* Seek table */
  if(flac->write_seektable) // Build seek table
    {
    if(!flac->cover)
      last = 1;
    
    build_seek_table(flac);
    gavl_io_seek(flac->io, flac->seektable_start, SEEK_SET);
    write_seektable(flac->seektable,
                    flac->num_seektable_entries,
                    flac, last);
    }
  }

static int close_flac(void * data, int do_delete)
  {
  flac_t * flac;
  flac = data;

  /* Flush and free the codec */
  if(flac->enc)
    {
    bg_flac_free(flac->enc);
    flac->enc = NULL;
    }

  /* Finalize output file */
  if(flac->io)
    {
    if(do_delete && flac->filename)
      {
      gavl_io_destroy(flac->io);
      flac->io = NULL;
      remove(flac->filename);
      }
    else
      {
      finalize(flac);
      gavl_io_destroy(flac->io);
      flac->io = NULL;
      }
    }

  if(flac->filename)
    {
    free(flac->filename);
    flac->filename = NULL;
    }
  
  if(flac->seektable)
    {
    free(flac->seektable);
    flac->seektable = NULL;
    }

  if(flac->frame_table)
    {
    free(flac->frame_table);
    flac->frame_table = NULL;
    }
  if(flac->psink_int)
    {
    gavl_packet_sink_destroy(flac->psink_int);
    flac->psink_int = NULL;
    }
  if(flac->psink_ext)
    {
    gavl_packet_sink_destroy(flac->psink_ext);
    flac->psink_ext = NULL;
    }
  if(flac->sink)
    {
    gavl_audio_sink_destroy(flac->sink);
    flac->sink = NULL;
    }

  gavl_dictionary_reset(&flac->stream);
  
  return 1;
  }

static void destroy_flac(void * priv)
  {
  flac_t * flac;
  flac = priv;
  close_flac(priv, 1);
  free(flac);
  }

static void set_audio_parameter_flac(void * data, int stream,
                                     const char * name,
                                     const gavl_value_t * val)
  {
  flac_t * flac;
  flac = data;
  bg_flac_set_parameter(flac->enc, name, val);
  }

/* Compressed packet support */

static int writes_compressed_audio_flac(void * priv,
                                        const gavl_audio_format_t * format,
                                        const gavl_compression_info_t * info)
  {
  if((info->id == GAVL_CODEC_ID_FLAC) && (info->codec_header.len == 42))
    return 1;
  return 0;
  }

static int
add_audio_stream_compressed_flac(void * priv,
                                 const gavl_dictionary_t * m,
                                 const gavl_audio_format_t * format,
                                 const gavl_compression_info_t * info)
  {
  flac_t * flac = priv;

  gavl_init_audio_stream(&flac->stream);
  gavl_stream_set_compression_info(&flac->stream, info);
  gavl_audio_format_copy(gavl_stream_get_audio_format_nc(&flac->stream), format);
  
  gavl_dictionary_copy(gavl_stream_get_metadata_nc(&flac->stream), m);
  flac->compressed = 1;
  return 0;
  }

static const bg_parameter_info_t * get_audio_parameters_flac(void * priv)
  {
  return bg_flac_get_parameters();
  }

static const char * get_extensions_flac(void * data)
  {
  return "flac";
  }


const bg_encoder_plugin_t the_plugin =
  {
    .common =
    {
      BG_LOCALE,
      .name =            "e_flac",       /* Unique short name */
      .long_name =       TRS("Flac"),
      .description =     TRS("Encoder for flac files. Based on libflac (http://flac.sourceforge.net)"),
      .type =            BG_PLUGIN_ENCODER_AUDIO,
      .flags =           BG_PLUGIN_FILE,
      .priority =        5,
      
      .create =            create_flac,
      .destroy =           destroy_flac,
      .get_parameters =    get_parameters_flac,
      .set_parameter =     set_parameter_flac,
      .get_extensions = get_extensions_flac,

    },
    .max_audio_streams =   1,
    .max_video_streams =   0,
    
    .set_callbacks =       set_callbacks_flac,
    
    .open =                open_flac,
    .open_io =                open_io_flac,
    
    .get_audio_parameters =    get_audio_parameters_flac,

    .writes_compressed_audio = writes_compressed_audio_flac,
    
    .add_audio_stream =        add_audio_stream_flac,
    .add_audio_stream_compressed = add_audio_stream_compressed_flac,
    
    
    .set_audio_parameter =     set_audio_parameter_flac,

    .get_audio_sink =          get_audio_sink_flac,
    .get_audio_packet_sink =   get_audio_packet_sink_flac,
    .start =                   start_flac,
    
    .close =               close_flac
  };

/* Include this into all plugin modules exactly once
   to let the plugin loader obtain the API version */
BG_GET_PLUGIN_API_VERSION;
