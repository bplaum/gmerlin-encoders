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

#ifdef FORMAT_MPEG1VIDEO
#define NAME "mpeg1video"
static const ffmpeg_format_info_t format =
    {
      .label =     "MPEG-1 video",
      .name = NAME,
      .extension =  "m1v",
      .max_video_streams = 1,
      .video_codecs = (enum AVCodecID[]){  AV_CODEC_ID_MPEG1VIDEO,
                                           AV_CODEC_ID_NONE },
      .flags = FLAG_CONSTANT_FRAMERATE | FLAG_PIPE,
    };
#endif

#ifdef FORMAT_MPEG2VIDEO
#define NAME "mpeg2video"
static const ffmpeg_format_info_t format =
    {
      .label =       "MPEG-2 video",
      .name = NAME,
      .extension =  "m2v",
      .max_video_streams = 1,
      .video_codecs = (enum AVCodecID[]){  AV_CODEC_ID_MPEG2VIDEO,
                                         AV_CODEC_ID_NONE },
      .flags = FLAG_CONSTANT_FRAMERATE | FLAG_PIPE,
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
      .name =           "e_" NAME,       /* Unique short name */
      .long_name =      format.label,
      .description =    TRS("Based on ffmpeg (http://www.ffmpeg.org)"),
      .type =           BG_PLUGIN_ENCODER_VIDEO,
      .flags =          BG_PLUGIN_FILE | BG_PLUGIN_PIPE,
      .priority =       5,
      .create =         create_ffmpeg,
      .destroy =        bg_ffmpeg_destroy,
      .get_parameters = bg_ffmpeg_get_parameters,
      .set_parameter =  bg_ffmpeg_set_parameter,
      .get_extensions = get_extensions_ffmpeg,
      
    },
    
    .max_video_streams =         1,
    
    .get_video_parameters = bg_ffmpeg_get_video_parameters,

    .set_callbacks =        bg_ffmpeg_set_callbacks,
    
    .open =                 bg_ffmpeg_open,
    .open_io =                 bg_ffmpeg_open_io,
    
    
    .add_video_stream =     bg_ffmpeg_add_video_stream,
    .set_video_pass =       bg_ffmpeg_set_video_pass,
    .set_video_parameter =  bg_ffmpeg_set_video_parameter,
    
    .get_video_sink =       bg_ffmpeg_get_video_sink,
    .get_video_packet_sink =     bg_ffmpeg_get_video_packet_sink,
    
    .start =                bg_ffmpeg_start,
    
    .close =                bg_ffmpeg_close,
  };

/* Include this into all plugin modules exactly once
   to let the plugin loader obtain the API version */
BG_GET_PLUGIN_API_VERSION;
