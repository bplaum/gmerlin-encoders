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



#include <string.h>
#include <stdlib.h>
#include <config.h>
#include <gmerlin/translation.h>

#include <gmerlin_encoders.h>

#include <gmerlin/plugin.h>
#include <gmerlin/utils.h>

#include <vorbis/vorbisenc.h>

#include "ogg_common.h"

extern const bg_ogg_codec_t bg_vorbis_codec;

static const bg_parameter_info_t * get_audio_parameters_vorbis(void * data)
  {
  return bg_vorbis_codec.get_parameters();
  }

static int add_audio_stream_vorbis(void * data, const gavl_dictionary_t *m,
                                   const gavl_audio_format_t * format)
  {
  bg_ogg_stream_t * s;
  s = bg_ogg_encoder_add_audio_stream(data, m, format);
  bg_ogg_encoder_init_stream(data, s, &bg_vorbis_codec);
  return s->index;
  }

static int
open_vorbis(void * data, const char * file,
            const gavl_dictionary_t * metadata)
  {
  return bg_ogg_encoder_open(data, file, NULL, metadata,
                             "ogg");
  }

static int
open_io_vorbis(void * data, gavl_io_t * io,
               const gavl_dictionary_t * metadata)
  {
  return bg_ogg_encoder_open(data, NULL, io, metadata,
                             "ogg");
  }

static int writes_compressed_audio_vorbis(void* data,
                                          const gavl_audio_format_t * format,
                                          const gavl_compression_info_t * ci)
  {
  if(ci->id == GAVL_CODEC_ID_VORBIS)
    return 1;
  else
    return 0;
  }

static int add_audio_stream_compressed_vorbis(void * data,
                                              const gavl_dictionary_t * m,
                                              const gavl_audio_format_t * format,
                                              const gavl_compression_info_t * ci)
  {
  bg_ogg_stream_t * s;
  s = bg_ogg_encoder_add_audio_stream_compressed(data, m, format, ci);
  bg_ogg_encoder_init_stream(data, s, &bg_vorbis_codec);
  return s->index;
  }

static const char * get_extensions_vorbis(void * priv)
  {
  return "ogg";
  }

const bg_encoder_plugin_t the_plugin =
  {
    .common =
    {
      BG_LOCALE,
      .name =            "e_vorbis",       /* Unique short name */
      .long_name =       TRS("Ogg Vorbis"),
      .description =     TRS("Encoder for Vorbis files"),
      .type =            BG_PLUGIN_ENCODER_AUDIO,
      .flags =           BG_PLUGIN_FILE | BG_PLUGIN_PIPE,
      .priority =        5,
      .create =            bg_ogg_encoder_create,
      .destroy =           bg_ogg_encoder_destroy,
      .get_extensions = get_extensions_vorbis,  
    },
    .max_audio_streams =   1,
    .max_video_streams =   0,

    .set_callbacks =       bg_ogg_encoder_set_callbacks,
    .writes_compressed_audio = writes_compressed_audio_vorbis,
    .open =                open_vorbis,
    .open_io =             open_io_vorbis,

    
    .get_audio_parameters =    get_audio_parameters_vorbis,

    .add_audio_stream =        add_audio_stream_vorbis,
    .add_audio_stream_compressed =        add_audio_stream_compressed_vorbis,
    
    .set_audio_parameter =   bg_ogg_encoder_set_audio_parameter,

    .start =                 bg_ogg_encoder_start,

    .get_audio_sink =        bg_ogg_encoder_get_audio_sink,
    .get_audio_packet_sink = bg_ogg_encoder_get_audio_packet_sink,

    .close =                 bg_ogg_encoder_close,
  };

/* Include this into all plugin modules exactly once
   to let the plugin loader obtain the API version */
BG_GET_PLUGIN_API_VERSION;
