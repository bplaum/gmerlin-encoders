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



#include <config.h>
#include <gmerlin/translation.h>
#include "ffmpeg_common.h"

#ifdef FORMAT_AU
#define NAME "au"
static const ffmpeg_format_info_t format =
  {
    .label = "SUN AU Format",
    .name = "au",
    .extension =  "au",
    .max_audio_streams = 1,
    .audio_codecs = (enum AVCodecID[]){ AV_CODEC_ID_PCM_MULAW,
                                        AV_CODEC_ID_PCM_S16BE,
                                        AV_CODEC_ID_PCM_ALAW,
                                        AV_CODEC_ID_NONE },
  };
#endif

#ifdef FORMAT_AC3
#define NAME "ac3"
static const ffmpeg_format_info_t format =
  {
    .label= "Raw AC3",
    .name = "ac3",
    .extension =  "ac3",
    .max_audio_streams = 1,
    .audio_codecs = (enum AVCodecID[]){  AV_CODEC_ID_AC3,
                                         AV_CODEC_ID_NONE },
    .flags = FLAG_PIPE,
  };
#endif

#ifdef FORMAT_AIFF
#define NAME "aiff"
static const ffmpeg_format_info_t format =
  {
      .label=       "AIFF",
      .name =       "aiff",
      .extension =  "aif",
      .max_audio_streams = 1,
      .audio_codecs = (enum AVCodecID[]){  AV_CODEC_ID_PCM_S16BE,
                                       AV_CODEC_ID_PCM_S8,
                                       AV_CODEC_ID_PCM_ALAW,
                                       AV_CODEC_ID_PCM_MULAW,
                                       AV_CODEC_ID_NONE },
  };
#endif

#ifdef FORMAT_MP2
#define NAME "mp2"
static const ffmpeg_format_info_t format =
  {
      .label=       "MP2",
      .name =       "mp2",
      .extension =  "mp2",
      .max_audio_streams = 1,
      .audio_codecs = (enum AVCodecID[]){  AV_CODEC_ID_MP2,
                                           AV_CODEC_ID_NONE },
      .flags = FLAG_PIPE,
  };

#endif

#ifdef FORMAT_WMA
#define NAME "wma"
static const ffmpeg_format_info_t format =
  {
      .label=       "WMA",
      .name =       NAME,
      .ffmpeg_name = "asf",
      .extension =  "wma",
      .max_audio_streams = 1,
      .audio_codecs = (enum AVCodecID[]){  AV_CODEC_ID_WMAV2,
                                           AV_CODEC_ID_WMAV1,
                                           AV_CODEC_ID_NONE },
    
  };
#endif

#ifdef FORMAT_ADTS
#define NAME "adts"
static const ffmpeg_format_info_t format =
  {
      .label=       "ADTS",
      .name =       NAME,
      .extension =  "aac",
      .max_audio_streams = 1,
      .audio_codecs = (enum AVCodecID[]){  AV_CODEC_ID_AAC,
                                           AV_CODEC_ID_NONE },
      .flags = FLAG_PIPE,
  };
#endif

static void * create_ffmpeg()
  {
  return bg_ffmpeg_create(&format);
  }

static const char * get_extensions_ffmpeg(void * data)
  {
  return format.extension;
  }

const bg_encoder_plugin_t the_plugin =
  {
    .common =
    {
      BG_LOCALE,
      .name =           "e_" NAME,     /* Unique short name */
      .long_name =      format.label,
      .description =    TRS("Based on ffmpeg (http://www.ffmpeg.org)."),
      .type =           BG_PLUGIN_ENCODER_AUDIO,
      .flags =          BG_PLUGIN_FILE | BG_PLUGIN_PIPE,
      .priority =       5,
      .create =         create_ffmpeg,
      .destroy =        bg_ffmpeg_destroy,
      .get_parameters = bg_ffmpeg_get_parameters,
      .set_parameter =  bg_ffmpeg_set_parameter,
      .get_extensions = get_extensions_ffmpeg,
    },
    
    .max_audio_streams =         1,
    
    .get_audio_parameters = bg_ffmpeg_get_audio_parameters,

    .set_callbacks =        bg_ffmpeg_set_callbacks,
    
    .open =                 bg_ffmpeg_open,
    .open_io =                 bg_ffmpeg_open_io,
    .writes_compressed_audio = bg_ffmpeg_writes_compressed_audio,
    
    .add_audio_stream =     bg_ffmpeg_add_audio_stream,
    .add_audio_stream_compressed =     bg_ffmpeg_add_audio_stream_compressed,
    
    .set_audio_parameter =  bg_ffmpeg_set_audio_parameter,
    
    .get_audio_sink =       bg_ffmpeg_get_audio_sink,
    .get_audio_packet_sink =       bg_ffmpeg_get_audio_packet_sink,

    .start =                bg_ffmpeg_start,
    
    .close =                bg_ffmpeg_close,
  };

/* Include this into all plugin modules exactly once
   to let the plugin loader obtain the API version */
BG_GET_PLUGIN_API_VERSION;
