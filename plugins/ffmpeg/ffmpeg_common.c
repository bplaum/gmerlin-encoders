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

#include <stdlib.h>
#include <string.h>

#include <config.h>

#include "ffmpeg_common.h"
#include <gmerlin/translation.h>
#include <gmerlin/utils.h>
#include <gmerlin/log.h>

#include <gavl/metatags.h>

#define LOG_DOMAIN "ffmpeg"

//#define DUMP_AUDIO_PACKETS
// #define DUMP_VIDEO_PACKETS
// #define DUMP_TEXT_PACKETS



static int write_frame(bg_ffmpeg_stream_t * s)
  {
  if(s->fmtctx)
    {
    if(av_write_frame(s->fmtctx, s->pkt) != 0)
      {
      return 0;
      }
    }
  else
    {
    if(av_interleaved_write_frame(s->ffmpeg->fmtctx, s->pkt) != 0)
      {
      return 0;
      }
    }
  
  return 1;
  }

static void copy_extradata(AVCodecParameters * avctx,
                           const gavl_compression_info_t * ci);

static void set_audio_compression(AVCodecParameters * avctx,
                                  const gavl_compression_info_t * ci)
  {
  if(ci->bitrate > 0)
    avctx->bit_rate = ci->bitrate;
  
  if(ci->block_align > 0)
    avctx->block_align = ci->block_align;
  
  copy_extradata(avctx, ci);
  }

static void set_video_compression(AVCodecParameters * avctx,
                                  const gavl_compression_info_t * ci)
  {
  copy_extradata(avctx, ci);
  
  }


void * bg_ffmpeg_create(const ffmpeg_format_info_t * format)
  {
  ffmpeg_priv_t * ret;

  ret = calloc(1, sizeof(*ret));
  
  ret->format = format;

  ret->audio_parameters =
    bg_ffmpeg_create_audio_parameters(format);
  ret->video_parameters =
    bg_ffmpeg_create_video_parameters(format);
  
  return ret;
  }

void bg_ffmpeg_set_callbacks(void * data,
                             bg_encoder_callbacks_t * cb)
  {
  ffmpeg_priv_t * f = data;
  f->cb = cb;
  }
                             

void bg_ffmpeg_destroy(void * data)
  {
  ffmpeg_priv_t * priv;
  priv = data;

  if(priv->parameters)
    bg_parameter_info_destroy_array(priv->parameters);
  if(priv->audio_parameters)
    bg_parameter_info_destroy_array(priv->audio_parameters);
  if(priv->video_parameters)
    bg_parameter_info_destroy_array(priv->video_parameters);

  if(priv->audio_streams)
    free(priv->audio_streams);
  if(priv->video_streams)
    free(priv->video_streams);
  if(priv->text_streams)
    free(priv->text_streams);

  if(priv->rtp_base_address)
    free(priv->rtp_base_address);
      
  free(priv);

  }

const bg_parameter_info_t * bg_ffmpeg_get_parameters(void * data)
  {
  ffmpeg_priv_t * priv;
  priv = data;
  return priv->parameters;
  }

const bg_parameter_info_t * bg_ffmpeg_get_audio_parameters(void * data)
  {
  ffmpeg_priv_t * priv;
  priv = data;
  return priv->audio_parameters;
  }

const bg_parameter_info_t * bg_ffmpeg_get_video_parameters(void * data)
  {
  ffmpeg_priv_t * priv;
  priv = data;
  return priv->video_parameters;
  }

void bg_ffmpeg_set_parameter(void * data, const char * name,
                             const gavl_value_t * v)
  {
  //  ffmpeg_priv_t * priv = data;

  if(!name)
    {
    return;
    }
  
  }

static void set_metadata(ffmpeg_priv_t * priv,
                         const gavl_dictionary_t * m)
  {
  const char * val;

  if(!priv->fmtctx)
    return;
  
  if((val = gavl_dictionary_get_string(m, GAVL_META_TITLE)))
    av_dict_set(&priv->fmtctx->metadata, "title", val, 0);
  
  if((val = gavl_dictionary_get_string(m, GAVL_META_AUTHOR)))
    av_dict_set(&priv->fmtctx->metadata, "composer", val, 0);
  
  if((val = gavl_dictionary_get_string(m, GAVL_META_ALBUM)))
    av_dict_set(&priv->fmtctx->metadata, "album", val, 0);

  if((val = gavl_dictionary_get_string(m, GAVL_META_COPYRIGHT)))
    av_dict_set(&priv->fmtctx->metadata, "copyright", val, 0);

  if((val = gavl_dictionary_get_string(m, GAVL_META_COMMENT)))
    av_dict_set(&priv->fmtctx->metadata, "comment", val, 0);
  
  if((val = gavl_dictionary_get_string(m, GAVL_META_GENRE)))
    av_dict_set(&priv->fmtctx->metadata, "genre", val, 0);
  
  if((val = gavl_dictionary_get_string(m, GAVL_META_DATE)))
    av_dict_set(&priv->fmtctx->metadata, "date", val, 0);

  if((val = gavl_dictionary_get_string(m, GAVL_META_TRACKNUMBER)))
    av_dict_set(&priv->fmtctx->metadata, "track", val, 0);
  }

static void set_chapters(AVFormatContext * ctx,
                         const gavl_chapter_list_t * chapter_list,
                         const gavl_dictionary_t * m)
  {
#if LIBAVUTIL_VERSION_INT >= AV_VERSION_INT(51,5,0)
  int i;
  gavl_time_t dur;
  int num_chapters;
  int timescale;
  const char * name;
  
  if(!chapter_list || !gavl_chapter_list_is_valid(chapter_list))
    return;

  num_chapters = gavl_chapter_list_get_num(chapter_list);
  timescale = gavl_chapter_list_get_timescale(chapter_list);
  
  ctx->chapters =
    av_malloc(num_chapters * sizeof(*ctx->chapters));
  ctx->nb_chapters = num_chapters;
  
  for(i = 0; i < num_chapters; i++)
    {
    ctx->chapters[i] = av_mallocz(sizeof(*(ctx->chapters[i])));
    ctx->chapters[i]->time_base.num = 1;
    ctx->chapters[i]->time_base.den = timescale;
    ctx->chapters[i]->start = gavl_chapter_list_get_time(chapter_list, i);
    if(i < num_chapters - 1)
      ctx->chapters[i]->end = gavl_chapter_list_get_time(chapter_list, i+1);
    else
      {
      if(gavl_dictionary_get_long(m, GAVL_META_APPROX_DURATION, &dur))
        ctx->chapters[i]->end = gavl_time_scale(timescale, dur);
      }
    if((name = gavl_chapter_list_get_label(chapter_list, i)))
      av_dict_set(&ctx->chapters[i]->metadata, "title", name, 0);
    }
#else
  if(!chapter_list || !chapter_list->num_chapters)
    return;
  gavl_log(GAVL_LOG_WARNING, LOG_DOMAIN,
         "Not writing chapters (ffmpeg version too old)");
#endif
  }

static char*ffmpeg_string(char*instr)
{
   size_t len = strlen(instr);
   char*outstr = av_malloc(len+1);
   if(outstr) {
      strcpy(outstr, instr);
      outstr[len] = 0;
   }
   return outstr;
}

static int ffmpeg_open(void * data, const char * filename,
                       gavl_io_t * io,
                       const gavl_dictionary_t * metadata)
  {
  const gavl_dictionary_t * cl;
  ffmpeg_priv_t * priv;
  const AVOutputFormat *fmt;
  
  priv = data;
  if(!priv->format)
    return 0;
  
  /* Initialize format context */
  fmt = bg_ffmpeg_guess_format(priv->format);
  if(!fmt)
    return 0;
  priv->fmtctx = avformat_alloc_context();

  if(filename)
    {
    if(!strcmp(filename, "-"))
      {
      if(!(priv->format->flags & FLAG_PIPE))
        {
        gavl_log(GAVL_LOG_ERROR, LOG_DOMAIN, "%s cannot be written to a pipe",
               priv->format->name);
        return 0;
        }
      priv->fmtctx->url = ffmpeg_string("pipe:");
      }
    else
      {
      char * tmp_string =
        gavl_filename_ensure_extension(filename,
                                     priv->format->extension);

      if(!bg_encoder_cb_create_output_file(priv->cb, tmp_string))
        {
        free(tmp_string);
        return 0;
        }
      priv->fmtctx->url = ffmpeg_string(tmp_string);
      free(tmp_string);
      }
    }
  else if(io)
    {
    if(!(priv->format->flags & FLAG_PIPE) &&
       !gavl_io_can_seek(io))
      {
      gavl_log(GAVL_LOG_ERROR, LOG_DOMAIN, "%s cannot be written to a pipe",
             priv->format->name);
      return 0;
      }
    priv->io = io;
    priv->fmtctx->flags |= AVFMT_FLAG_CUSTOM_IO;
    }
  else
    return 0;
  
  priv->fmtctx->max_delay = (int)(0.7 * (float)AV_TIME_BASE);
  priv->fmtctx->oformat = fmt;
    
  /* Add metadata */

  if(metadata)
    {
    set_metadata(priv, metadata);

    if((cl = gavl_dictionary_get_chapter_list(metadata)))
      set_chapters(priv->fmtctx, cl, metadata);
    }
  
  return 1;
  }

int bg_ffmpeg_open(void * data, const char * filename,
                   const gavl_dictionary_t * metadata)
  {
  return ffmpeg_open(data, filename, NULL, metadata);
  }

int bg_ffmpeg_open_io(void * data, gavl_io_t * io,
                      const gavl_dictionary_t * metadata)
  {
  return ffmpeg_open(data, NULL, io, metadata);
  }

static void init_stream(ffmpeg_priv_t * priv, bg_ffmpeg_stream_t * com)
  {
  com->pkt = av_packet_alloc();

  if(priv->fmtctx)
    com->stream = avformat_new_stream(priv->fmtctx, NULL);
  else
    {
    com->fmtctx = avformat_alloc_context();
    
    }
  
  com->m = gavl_stream_get_metadata_nc(&com->s);
  com->ffmpeg = priv;
  }


int bg_ffmpeg_add_audio_stream(void * data,
                               const gavl_dictionary_t * m,
                               const gavl_audio_format_t * format)
  {
  ffmpeg_priv_t * priv;
  bg_ffmpeg_stream_t * st;
  const char *lang;
  priv = data;
  
  priv->audio_streams =
    realloc(priv->audio_streams,
            (priv->num_audio_streams+1)*sizeof(*priv->audio_streams));
  
  st = priv->audio_streams + priv->num_audio_streams;
  memset(st, 0, sizeof(*st));
  gavl_init_audio_stream(&st->s);

  init_stream(priv, st);
  
  st->aformat = gavl_stream_get_audio_format_nc(&st->s);
  gavl_audio_format_copy(st->aformat, format);
  
  bg_ffmpeg_set_audio_format_params(st->stream->codecpar, st->aformat);
  
  st->stream->codecpar->codec_type = AVMEDIA_TYPE_AUDIO;
  
  /* Set language */
  lang = gavl_dictionary_get_string(m, GAVL_META_LANGUAGE);
  if(lang)
    av_dict_set(&st->stream->metadata, "language", lang, 0);
  
  priv->num_audio_streams++;
  return priv->num_audio_streams-1;
  }

int bg_ffmpeg_add_video_stream(void * data,
                               const gavl_dictionary_t * m,
                               const gavl_video_format_t * format)
  {
  ffmpeg_priv_t * priv;
  bg_ffmpeg_stream_t * st;
  priv = data;

  priv->video_streams =
    realloc(priv->video_streams,
            (priv->num_video_streams+1)*sizeof(*priv->video_streams));
  
  st = priv->video_streams + priv->num_video_streams;
  memset(st, 0, sizeof(*st));
  
  gavl_init_video_stream(&st->s);
  init_stream(priv, st);
  
  st->vformat = gavl_stream_get_video_format_nc(&st->s);
  gavl_video_format_copy(st->vformat, format);
  
  st->stream->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;

  bg_ffmpeg_set_video_dimensions_params(st->stream->codecpar, format);
  
  st->dts = GAVL_TIME_UNDEFINED;
  
  priv->num_video_streams++;
  return priv->num_video_streams-1;
  }

/*
  int bg_ffmpeg_write_subtitle_text(void * data,const char * text,
                                  int64_t start,
                                  int64_t duration, int stream)
*/

static gavl_sink_status_t
write_text_packet_func(void * data, gavl_packet_t * p)
  {
  bg_ffmpeg_stream_t * st = data;

  ffmpeg_priv_t * priv = st->ffmpeg;

#ifdef DUMP_TEXT_PACKETS  
  bg_dprintf("write_text_packet\n");
  gavl_packet_dump(p);
#endif
  
  st->pkt->data    = p->buf.buf;
  st->pkt->size     = p->buf.len + 1; // Let's hope the packet got padded!!!

#if 1
  st->pkt->pts= av_rescale_q(p->pts,
                                 st->time_base,
                                 st->stream->time_base);
  
  st->pkt->duration= av_rescale_q(p->duration,
                                      st->time_base,
                                      st->stream->time_base);
#endif
  
  //  pkt.convergence_duration = pkt.duration;
  st->pkt->dts = st->pkt->pts;
  st->pkt->stream_index = st->stream->index;
  
  if(!write_frame(st))
    {
    priv->got_error = 1;
    return GAVL_SINK_ERROR;
    }
  st->pkt->data = NULL;
  return GAVL_SINK_OK;
  }

int bg_ffmpeg_add_text_stream(void * data,
                              const gavl_dictionary_t * m,
                              uint32_t * timescale)
  {
  ffmpeg_priv_t * priv;
  bg_ffmpeg_stream_t * st;
  const char * lang;
  priv = data;

  priv->text_streams =
    realloc(priv->text_streams,
            (priv->num_text_streams+1)*sizeof(*priv->text_streams));
  
  st = priv->text_streams + priv->num_text_streams;
  memset(st, 0, sizeof(*st));

  init_stream(priv, st);
  
  st->stream->codecpar->codec_type = AVMEDIA_TYPE_SUBTITLE;
  
  if((lang = gavl_dictionary_get_string(m, GAVL_META_LANGUAGE)))
    av_dict_set(&st->stream->metadata, "language", lang, 0);
  
  st->stream->codecpar->codec_type = AVMEDIA_TYPE_SUBTITLE;
  st->stream->codecpar->codec_id = AV_CODEC_ID_TEXT;

  st->time_base.num = 1;
  st->time_base.den = *timescale;

  /* Might get changed */
  st->stream->time_base.num = 1;
  st->stream->time_base.den = *timescale;
  
  priv->num_text_streams++;
  return priv->num_text_streams-1;
  }

void bg_ffmpeg_set_audio_parameter(void * data, int stream, const char * name,
                                   const gavl_value_t * v)
  {
  bg_ffmpeg_stream_t * st;
  ffmpeg_priv_t * priv = data;
  st = priv->audio_streams + stream;

  if(name && !strcmp(name, "codec") && !st->codec)
    {
    enum AVCodecID id;
    const gavl_dictionary_t * codec;
    const char * codec_name;

    codec = gavl_value_get_dictionary(v);
    codec_name = gavl_dictionary_get_string(codec, BG_CFG_TAG_NAME);
    
    if((id = bg_ffmpeg_find_audio_encoder(st->ffmpeg->format, codec_name)))
      {
      st->codec = bg_ffmpeg_codec_create(AVMEDIA_TYPE_AUDIO,
                                             st->stream->codecpar,
                                             id, st->ffmpeg->format);
      }
    }
  
  bg_ffmpeg_codec_set_parameter(st->codec, name, v);
  }

void bg_ffmpeg_set_video_parameter(void * data, int stream, const char * name,
                                   const gavl_value_t * v)
  {
  bg_ffmpeg_stream_t * st;
  ffmpeg_priv_t * priv = data;
  st = priv->video_streams + stream;
  
  if(name && !strcmp(name, "codec") && !st->codec)
    {
    enum AVCodecID id;
    const gavl_dictionary_t * codec = gavl_value_get_dictionary(v);
    const char * codec_name = gavl_dictionary_get_string(codec, BG_CFG_TAG_NAME);
    
    if((id = bg_ffmpeg_find_video_encoder(st->ffmpeg->format, codec_name)))
      {
      st->codec = bg_ffmpeg_codec_create(AVMEDIA_TYPE_VIDEO,
                                             st->stream->codecpar,
                                             id, st->ffmpeg->format);
      }
    }
  
  bg_ffmpeg_codec_set_parameter(st->codec, name, v);
  }


static int64_t rescale_video_timestamp(bg_ffmpeg_stream_t * st,
                                       int64_t ts)
  {
  AVRational framerate;
  
  if(st->vformat->framerate_mode == GAVL_FRAMERATE_CONSTANT)
    {
    framerate.num = st->vformat->frame_duration;
    framerate.den = st->vformat->timescale;

    return av_rescale_q(ts / st->vformat->frame_duration,
                        framerate,
                        st->stream->time_base);
    }
  else
    {
    framerate.num = 1;
    framerate.den = st->vformat->timescale;
    
    return av_rescale_q(ts,
                        framerate,
                        st->stream->time_base);
    }
  
  }

static gavl_sink_status_t
write_video_packet_func(void * priv, gavl_packet_t * packet)
  {
  bg_ffmpeg_stream_t * st = priv;
  ffmpeg_priv_t * f = st->ffmpeg;

#ifdef DUMP_VIDEO_PACKETS  
  bg_dprintf("\nwrite_video_packet\n");
  gavl_packet_dump(packet);
#endif
  
  if(packet->pts == GAVL_TIME_UNDEFINED)
    return 1; // Drop undecodable packet
  
  st->pkt->data = packet->buf.buf;
  st->pkt->size = packet->buf.len;

  st->pkt->pts      = rescale_video_timestamp(st, packet->pts);
  st->pkt->dts      = rescale_video_timestamp(st, packet->dts);
  
  st->pkt->duration = rescale_video_timestamp(st, packet->duration);
  
  // fprintf(stderr, "Video PTS: %"PRId64" Duration: %"PRId64"\n", st->pkt->pts, st->pkt->duration);

  if(st->ci.flags & GAVL_COMPRESSION_HAS_B_FRAMES)
    {
    if(st->dts == GAVL_TIME_UNDEFINED)
      st->dts = packet->pts - 3*packet->duration;
    
    st->pkt->dts= rescale_video_timestamp(st, st->dts);
    st->dts += packet->duration;
    }
  else
    st->pkt->dts = st->pkt->pts;
  
  if(packet->flags & GAVL_PACKET_KEYFRAME)  
    st->pkt->flags |= AV_PKT_FLAG_KEY;
  else
    st->pkt->flags &= ~AV_PKT_FLAG_KEY;
  
  st->pkt->stream_index= st->stream->index;
  
  /* write the compressed frame in the media file */
  if(!write_frame(st))
    {
    f->got_error = 1;
    return GAVL_SINK_ERROR;
    }
  st->pkt->data = NULL;
  return GAVL_SINK_OK;
  }



static gavl_sink_status_t
write_audio_packet_func(void * data, gavl_packet_t * packet)
  {
  ffmpeg_priv_t * f;
  bg_ffmpeg_stream_t * st = data;
  AVRational time_base;
  f = st->ffmpeg;

#ifdef DUMP_AUDIO_PACKETS  
  bg_dprintf("write_audio_packet\n");
  gavl_packet_dump(packet);
#endif
  
  if(packet->pts == GAVL_TIME_UNDEFINED)
    return 1; // Drop undecodable packet
    
  st->pkt->data = packet->buf.buf;
  st->pkt->size = packet->buf.len;

  time_base.num = 1;
  time_base.den = st->aformat->samplerate;

  //  fprintf(stderr, "AUDIO 1 PTS: %"PRId64" Duration: %"PRId64"\n", packet->pts, packet->duration);
  
  st->pkt->pts= av_rescale_q(packet->pts,
                                 time_base,
                                 st->stream->time_base);
  
  st->pkt->duration= av_rescale_q(packet->duration,
                                      time_base,
                                      st->stream->time_base);
  
  st->pkt->dts = st->pkt->pts;

  //  fprintf(stderr, "AUDIO 2 PTS: %"PRId64" Duration: %"PRId64"\n", st->pkt->pts, st->pkt->duration);
  
  if(st->ci.flags & GAVL_COMPRESSION_SBR)
    {
    st->pkt->pts /= 2;
    st->pkt->dts /= 2;
    st->pkt->duration /= 2;
    }
  
  st->pkt->flags |= AV_PKT_FLAG_KEY;
  st->pkt->stream_index= st->stream->index;
  
  /* write the compressed frame in the media file */
  if(!write_frame(st))
    {
    f->got_error = 1;
    return GAVL_SINK_ERROR;
    }
  st->pkt->data = NULL;
  return GAVL_SINK_OK;
  }


static int open_audio_encoder(bg_ffmpeg_stream_t * st)
  {
  st->psink = gavl_packet_sink_create(NULL, write_audio_packet_func, st);
  
  if(st->flags & STREAM_IS_COMPRESSED)
    {
    bg_ffmpeg_set_audio_format_params(st->stream->codecpar,
                                      st->aformat);

    if(st->ci.flags & GAVL_COMPRESSION_SBR)
      st->stream->codecpar->sample_rate /= 2;
    
    return 1;
    }

  st->asink = bg_ffmpeg_codec_open_audio(st->codec, &st->s);
  if(!st->asink)
    return 0;

  gavl_stream_get_compression_info(&st->s, &st->ci);
  
  set_audio_compression(st->stream->codecpar, &st->ci);
  
  st->stream->codecpar->codec_id = st->codec->id;
  
  bg_ffmpeg_codec_set_packet_sink(st->codec, st->psink);
  
  st->flags |= STREAM_ENCODER_INITIALIZED;
  return 1;
  }

static void set_framerate(bg_ffmpeg_stream_t * st)
  {
  if(st->vformat->framerate_mode == GAVL_FRAMERATE_CONSTANT)
    {
    st->stream->time_base.den = st->vformat->timescale;
    st->stream->time_base.num = st->vformat->frame_duration;
    }
  else
    {
    st->stream->time_base.den = st->vformat->timescale;
    st->stream->time_base.num = 1;
    }
  }

static int open_video_encoder(bg_ffmpeg_stream_t * st)
  {
  st->psink = gavl_packet_sink_create(NULL, write_video_packet_func, st);
  if(st->flags & STREAM_IS_COMPRESSED)
    {
    set_framerate(st);
    bg_ffmpeg_set_video_dimensions_params(st->stream->codecpar, st->vformat);

    st->stream->sample_aspect_ratio.num = st->stream->codecpar->sample_aspect_ratio.num;
    st->stream->sample_aspect_ratio.den = st->stream->codecpar->sample_aspect_ratio.den;
    
    return 1;
    }
  
  st->vsink = bg_ffmpeg_codec_open_video(st->codec, &st->s);
  if(!st->vsink)
    return 0;

  gavl_stream_get_compression_info(&st->s, &st->ci);
  set_video_compression(st->stream->codecpar, &st->ci);
  st->stream->codecpar->codec_id = st->codec->id;
  
  bg_ffmpeg_codec_set_packet_sink(st->codec, st->psink);

  st->stream->sample_aspect_ratio.num = st->stream->codecpar->sample_aspect_ratio.num;
  st->stream->sample_aspect_ratio.den = st->stream->codecpar->sample_aspect_ratio.den;
  
  //  fprintf(stderr, "Opened video encoder\n");
  //  gavl_compression_info_dump(&st->ci);
  
  st->flags |= STREAM_ENCODER_INITIALIZED;
  return 1;
  }

#define IO_BUFFER_SIZE 2048 // Too large values increase the latency

#if LIBAVFORMAT_VERSION_MAJOR < 61
static int io_write(void * opaque, uint8_t * buf, int size)
#else
static int io_write(void * opaque, const uint8_t * buf, int size)
#endif
  {
  return gavl_io_write_data(opaque, buf, size);
  }

static int64_t io_seek(void * opaque, int64_t off, int whence)
  {
  return gavl_io_seek(opaque, off, whence);
  }

int bg_ffmpeg_start(void * data)
  {
  ffmpeg_priv_t * priv;
  int i;
  priv = data;
  
  
  /* Open encoders */
  for(i = 0; i < priv->num_audio_streams; i++)
    {
    if(!open_audio_encoder(&priv->audio_streams[i]))
      return 0;
    }

  for(i = 0; i < priv->num_video_streams; i++)
    {
    if(!open_video_encoder(&priv->video_streams[i]))
      return 0;
    }

  for(i = 0; i < priv->num_text_streams ; i++)
    {
    
    priv->text_streams[i].psink =
      gavl_packet_sink_create(NULL, write_text_packet_func,
                              &priv->text_streams[i]);

    }

  if(priv->fmtctx)
    {
    if(priv->io)
      {
      priv->io_buffer = av_malloc(IO_BUFFER_SIZE);
      priv->fmtctx->pb = avio_alloc_context(priv->io_buffer,
                                            IO_BUFFER_SIZE,
                                            1, // write_flag
                                            priv->io,
                                            NULL,
                                            io_write,
                                            gavl_io_can_seek(priv->io) ? io_seek : NULL);
      }
    else if(avio_open(&priv->fmtctx->pb, priv->fmtctx->url, AVIO_FLAG_WRITE) < 0)
      {
      gavl_log(GAVL_LOG_ERROR, LOG_DOMAIN, "avio_open failed");
      return 0;
      }
  
    if(avformat_write_header(priv->fmtctx, NULL))
      {
      gavl_log(GAVL_LOG_ERROR, LOG_DOMAIN, "avformat_write_header failed");
      return 0;
      }
    
    }
  
  priv->initialized = 1;
  return 1;
  }

void bg_ffmpeg_get_audio_format(void * data, int stream,
                                gavl_audio_format_t*ret)
  {
  ffmpeg_priv_t * priv;
  priv = data;

  gavl_audio_format_copy(ret, priv->audio_streams[stream].aformat);
                         
  }


gavl_audio_sink_t *
bg_ffmpeg_get_audio_sink(void * data, int stream)
  {
  ffmpeg_priv_t * priv;
  priv = data;
  return priv->audio_streams[stream].asink;
  }

gavl_video_sink_t *
bg_ffmpeg_get_video_sink(void * data, int stream)
  {
  ffmpeg_priv_t * priv;
  priv = data;
  return priv->video_streams[stream].vsink;
  }

static void cleanup_stream(bg_ffmpeg_stream_t * com)
  {
  if(com->fmtctx)
    {
    if(com->ffmpeg->initialized)
      {
      av_write_trailer(com->fmtctx);
      avio_close(com->fmtctx->pb);
      }
    avformat_free_context(com->fmtctx);
    }
  
  if(com->codec)
    {
    bg_ffmpeg_codec_destroy(com->codec);
    com->codec = NULL;
    }

  if(com->pkt)
    av_packet_free(&com->pkt);

  gavl_compression_info_free(&com->ci);

  if(com->psink)
    gavl_packet_sink_destroy(com->psink);


  
  }

#if 0
static int close_audio_encoder(ffmpeg_priv_t * priv,
                               bg_ffmpeg_stream_t * st)
  {
  close_common(st);
  return 1;
  }

static int close_text_encoder(ffmpeg_priv_t * priv,
                              bg_ffmpeg_stream_t * st)
  {
  close_common(st);
  return 1;
  }

static int close_video_encoder(ffmpeg_priv_t * priv,
                                bg_ffmpeg_stream_t * st)
  {
  close_common(st);
  return 1;
  }
#endif

int bg_ffmpeg_close(void * data, int do_delete)
  {
  ffmpeg_priv_t * priv;
  int i;
  priv = data;

  // Flush the streams

  for(i = 0; i < priv->num_audio_streams; i++)
    {
    bg_ffmpeg_stream_t * st = &priv->audio_streams[i];

    if(!(st->flags & STREAM_IS_COMPRESSED))
      bg_ffmpeg_codec_flush(st->codec);
    }
  for(i = 0; i < priv->num_video_streams; i++)
    {
    bg_ffmpeg_stream_t * st = &priv->video_streams[i];
    if(!(st->flags & STREAM_IS_COMPRESSED))
      bg_ffmpeg_codec_flush(st->codec);
    }
  
  if(priv->initialized)
    {
    if(priv->fmtctx)
      {
      av_write_trailer(priv->fmtctx);
    
      if(priv->io)
        av_free(priv->fmtctx->pb);
      else
        avio_close(priv->fmtctx->pb);
      }
    
    }

  // Close the encoders

  for(i = 0; i < priv->num_audio_streams; i++)
    {
    cleanup_stream(&priv->audio_streams[i]);
    }
  for(i = 0; i < priv->num_video_streams; i++)
    {
    cleanup_stream(&priv->video_streams[i]);
    }
  for(i = 0; i < priv->num_text_streams; i++)
    {
    cleanup_stream(&priv->text_streams[i]);
    }
  
  if(priv->fmtctx)
    {
    if(do_delete && !priv->io)
      remove(priv->fmtctx->url);
    avformat_free_context(priv->fmtctx);
    priv->fmtctx = NULL;
    }
  
  if(priv->io_buffer)
    av_free(priv->io_buffer);
  
  
  return 1;
  }

int bg_ffmpeg_writes_compressed_audio(void * priv,
                                      const gavl_audio_format_t * format,
                                      const gavl_compression_info_t * info)
  {
  int i;
  enum AVCodecID ffmpeg_id;
  ffmpeg_priv_t * f = priv;
  
  ffmpeg_id = bg_codec_id_gavl_2_ffmpeg(info->id);
  
  i = 0;
  while(f->format->audio_codecs[i] != AV_CODEC_ID_NONE)
    {
    if(f->format->audio_codecs[i] == ffmpeg_id)
      return 1;
    i++;
    }
  
  return 0;
  }

int bg_ffmpeg_writes_compressed_video(void * priv,
                                      const gavl_video_format_t * format,
                                      const gavl_compression_info_t * info)
  {
  int i;
  enum AVCodecID ffmpeg_id;
  ffmpeg_priv_t * f = priv;

  ffmpeg_id = bg_codec_id_gavl_2_ffmpeg(info->id);

  i = 0;
  while(f->format->video_codecs[i] != AV_CODEC_ID_NONE)
    {
    if(f->format->video_codecs[i] == ffmpeg_id)
      return 1;
    i++;
    }
  
  return 0;
  }

static void copy_extradata(AVCodecParameters * avctx,
                           const gavl_compression_info_t * ci)
  {
  //  fprintf(stderr, "Copying extradata %d bytes\n", ci->global_header_len);
  
  if(ci->codec_header.len)
    {
    avctx->extradata_size = ci->codec_header.len;
    avctx->extradata =
      av_malloc(avctx->extradata_size + AV_INPUT_BUFFER_PADDING_SIZE);
    memcpy(avctx->extradata,
           ci->codec_header.buf,
           ci->codec_header.len);
    memset(avctx->extradata + avctx->extradata_size, 0, AV_INPUT_BUFFER_PADDING_SIZE);
    //    avctx->flags |= CODEC_FLAG_GLOBAL_HEADER;
    }
  }

int
bg_ffmpeg_add_audio_stream_compressed(void * priv,
                                      const gavl_dictionary_t * m,
                                      const gavl_audio_format_t * format,
                                      const gavl_compression_info_t * info)
  {
  int ret;
  bg_ffmpeg_stream_t * st;
  ffmpeg_priv_t * f = priv;

  ret = bg_ffmpeg_add_audio_stream(priv, m, format);
  st = f->audio_streams + ret;

  gavl_compression_info_copy(&st->ci, info);
  st->flags |= STREAM_IS_COMPRESSED;
    
  st->stream->codecpar->codec_id = bg_codec_id_gavl_2_ffmpeg(st->ci.id);

  st->stream->time_base.num = 1;
  st->stream->time_base.den = st->aformat->samplerate;
  
  if(st->ci.bitrate)
    {
    st->stream->codecpar->bit_rate = st->ci.bitrate;
    // st->stream->codecpar->rc_max_rate = st->ci.bitrate;
    }
  /* Set compression data */
  set_audio_compression(st->stream->codecpar, &st->ci);
  return ret;
  }

int bg_ffmpeg_add_video_stream_compressed(void * priv,
                                          const gavl_dictionary_t * m,
                                          const gavl_video_format_t * format,
                                          const gavl_compression_info_t * info)
  {
  int ret;
  bg_ffmpeg_stream_t * st;
  ffmpeg_priv_t * f = priv;

  ret = bg_ffmpeg_add_video_stream(priv, m, format);
  st = f->video_streams + ret;

  gavl_compression_info_copy(&st->ci, info);
  st->flags |= STREAM_IS_COMPRESSED;
  st->stream->codecpar->codec_id = bg_codec_id_gavl_2_ffmpeg(st->ci.id);
  st->dts = GAVL_TIME_UNDEFINED;

  if(st->ci.bitrate)
    st->stream->codecpar->bit_rate = st->ci.bitrate;

  if(st->ci.block_align)
    st->stream->codecpar->block_align = st->ci.block_align;
  
  //  if(st->ci.video_buffer_size)
  //    st->stream->codecpar->rc_buffer_size = st->ci.video_buffer_size * 8;
  
  /* Set compression data */
  set_video_compression(st->stream->codecpar, &st->ci);
  return ret;
  }

gavl_packet_sink_t *
bg_ffmpeg_get_audio_packet_sink(void * data, int stream)
  {
  ffmpeg_priv_t * f = data;
  return f->audio_streams[stream].psink;
  }

gavl_packet_sink_t *
bg_ffmpeg_get_video_packet_sink(void * data, int stream)
  {
  ffmpeg_priv_t * f = data;
  return f->video_streams[stream].psink;
  }

gavl_packet_sink_t *
bg_ffmpeg_get_text_packet_sink(void * data, int stream)
  {
  ffmpeg_priv_t * f = data;
  return f->text_streams[stream].psink;
  }

const  AVOutputFormat * bg_ffmpeg_guess_format(const ffmpeg_format_info_t * format)
  {
  if(format->ffmpeg_name)
    return av_guess_format(format->ffmpeg_name,
                           NULL, NULL);
  else
    return av_guess_format(format->name,
                           NULL, NULL);
  }
