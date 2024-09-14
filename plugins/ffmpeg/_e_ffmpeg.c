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

#ifdef FORMAT_AVI
#define NAME "avi"
static const ffmpeg_format_info_t format =
  {
      .label =      "AVI",
      .name =       NAME,
      .extension =  "avi",
      .max_audio_streams = 1,
      .max_video_streams = 1,
      .audio_codecs = (enum AVCodecID[]){  AV_CODEC_ID_PCM_S16LE,
                                           AV_CODEC_ID_PCM_U8,
                                           AV_CODEC_ID_PCM_ALAW,
                                           AV_CODEC_ID_PCM_MULAW,
                                           AV_CODEC_ID_MP3,
                                           AV_CODEC_ID_MP2,
                                           AV_CODEC_ID_AC3,
                                           AV_CODEC_ID_NONE },

      .video_codecs = (enum AVCodecID[]){  AV_CODEC_ID_MPEG4,
                                           AV_CODEC_ID_MSMPEG4V3,
                                           AV_CODEC_ID_MJPEG,
                                           AV_CODEC_ID_NONE },
      .flags = FLAG_CONSTANT_FRAMERATE,
  };
#endif

#ifdef FORMAT_MPEG
#define NAME "mpeg"
static const ffmpeg_format_info_t format =
  {
      .label =      "MPEG-1",
      .name =       NAME,
      .extension =  "mpg",
      .max_audio_streams = -1,
      .max_video_streams = -1,
      .audio_codecs = (enum AVCodecID[]){  AV_CODEC_ID_MP2,
                                           AV_CODEC_ID_MP3,
                                           AV_CODEC_ID_NONE },

      .video_codecs = (enum AVCodecID[]){  AV_CODEC_ID_MPEG1VIDEO,
                                           AV_CODEC_ID_NONE },
      .flags = FLAG_CONSTANT_FRAMERATE | FLAG_PIPE,
  };
#endif

#ifdef FORMAT_VOB
#define NAME "vob"
static const ffmpeg_format_info_t format =
  {
      .label =      "MPEG-2 (generic)",
      .name =       NAME,
      .extension =  "vob",
      .max_audio_streams = -1,
      .max_video_streams = -1,
      .audio_codecs = (enum AVCodecID[]){  AV_CODEC_ID_MP2,
                                           AV_CODEC_ID_MP3,
                                           AV_CODEC_ID_AC3,
                                           AV_CODEC_ID_NONE },

      .video_codecs = (enum AVCodecID[]){  AV_CODEC_ID_MPEG2VIDEO,
                                           AV_CODEC_ID_NONE },
      .flags = FLAG_CONSTANT_FRAMERATE | FLAG_PIPE,
  };
#endif

#ifdef FORMAT_DVD
#define NAME "dvd"
static const ffmpeg_format_info_t format =
  {
      .label =      "MPEG-2 (dvd)",
      .name =       "dvd",
      .extension =  "vob",
      .max_audio_streams = -1,
      .max_video_streams = -1,
      .audio_codecs = (enum AVCodecID[]){  AV_CODEC_ID_MP2,
                                           AV_CODEC_ID_AC3,
                                           AV_CODEC_ID_NONE },

      .video_codecs = (enum AVCodecID[]){  AV_CODEC_ID_MPEG2VIDEO,
                                           AV_CODEC_ID_NONE },
      .flags = FLAG_CONSTANT_FRAMERATE | FLAG_PIPE,
  };
#endif

#ifdef FORMAT_ASF
#define NAME "asf"
static const ffmpeg_format_info_t format =
  {
      .label =      "ASF",
      .name =       NAME,
      .extension =  "asf",
      .max_audio_streams = 1,
      .max_video_streams = 1,
      .audio_codecs = (enum AVCodecID[]){
#if LIBAVCODEC_BUILD >= ((51<<16)+(32<<8)+0)
                                       AV_CODEC_ID_WMAV2,
                                       AV_CODEC_ID_WMAV1,
#endif
                                       AV_CODEC_ID_MP3,
                                       AV_CODEC_ID_MP2,
                                       AV_CODEC_ID_NONE },
      
      .video_codecs = (enum AVCodecID[]){  AV_CODEC_ID_WMV1,
                                       // AV_CODEC_ID_WMV2, /* Crash */
                                       AV_CODEC_ID_NONE },
  };
#endif

#ifdef FORMAT_MPEGTS
#define NAME "mpegts"
static const ffmpeg_format_info_t format =
  {
      .label =       "MPEG-2 Transport stream",
      .name =        NAME,
      .extension =   "ts",
      .max_audio_streams = 1,
      .max_video_streams = 1,
      .audio_codecs = (enum AVCodecID[]){ AV_CODEC_ID_MP3,
                                        AV_CODEC_ID_MP2,
                                        AV_CODEC_ID_AC3,
                                        AV_CODEC_ID_NONE },
      
      .video_codecs = (enum AVCodecID[]){ AV_CODEC_ID_MPEG1VIDEO,
                                        AV_CODEC_ID_MPEG2VIDEO,
                                        AV_CODEC_ID_NONE },
      .flags = FLAG_CONSTANT_FRAMERATE | FLAG_PIPE,
 
  };
#endif

#ifdef FORMAT_MATROSKA
#define NAME "matroska"
static const ffmpeg_format_info_t format =
  {
      .label =       "Matroska",
      .name =        NAME,
      .extension =   "mkv",
      .max_audio_streams = -1,
      .max_video_streams = -1,
      .audio_codecs = (enum AVCodecID[]){ AV_CODEC_ID_MP3,
                                          AV_CODEC_ID_MP2,
                                          AV_CODEC_ID_AC3,
                                          AV_CODEC_ID_VORBIS,
                                          AV_CODEC_ID_DTS,
                                          AV_CODEC_ID_AAC,
                                          AV_CODEC_ID_NONE },
      
      .video_codecs = (enum AVCodecID[]){ AV_CODEC_ID_H264,
                                          AV_CODEC_ID_MPEG4,
                                          AV_CODEC_ID_MPEG1VIDEO,
                                          AV_CODEC_ID_MPEG2VIDEO,
                                          AV_CODEC_ID_VP8,
                                          AV_CODEC_ID_MSMPEG4V3,
                                          AV_CODEC_ID_NONE },
  };
#endif

#ifdef FORMAT_WEBM
#define NAME "webm"
static const ffmpeg_format_info_t format =
  {
      .label =      "Webm",
      .name =       NAME,
      .extension =  "webm",
      .max_audio_streams = -1,
      .max_video_streams = -1,
      .audio_codecs = (enum AVCodecID[]){ AV_CODEC_ID_VORBIS,
                                          AV_CODEC_ID_NONE },
      
      .video_codecs = (enum AVCodecID[]){ AV_CODEC_ID_VP8,
                                          AV_CODEC_ID_NONE },
      .flags = FLAG_PIPE,
  };
#endif

#ifdef FORMAT_MP4
#define NAME "mp4"
static const ffmpeg_format_info_t format =
  {
      .label =       "MP4",
      .name =        NAME,
      .extension =   "mp4",
      .max_audio_streams = -1,
      .max_video_streams = -1,
      .audio_codecs = (enum AVCodecID[]){  AV_CODEC_ID_AAC,
                                           AV_CODEC_ID_NONE },

      .video_codecs = (enum AVCodecID[]){  AV_CODEC_ID_MPEG4,
                                           AV_CODEC_ID_H264,
                                           AV_CODEC_ID_NONE },
  };
#endif

static void * create_ffmpeg()
  {
  return bg_ffmpeg_create(&format);
  }

const bg_encoder_plugin_t the_plugin =
  {
    .common =
    {
      BG_LOCALE,
      .name =           "e_" NAME,       /* Unique short name */
      .long_name =      format.label,
      .description =    TRS("Based on ffmpeg (http://www.ffmpeg.org)."),
      .type =           BG_PLUGIN_ENCODER,
      .flags =          BG_PLUGIN_FILE | BG_PLUGIN_PIPE,
      .priority =       5,
      .create =         create_ffmpeg,
      .destroy =        bg_ffmpeg_destroy,
      .get_parameters = bg_ffmpeg_get_parameters,
      .set_parameter =  bg_ffmpeg_set_parameter,
    },
    
    .max_audio_streams =         -1,
    .max_video_streams =         -1,
    .max_text_streams = -1,
    
    .get_audio_parameters = bg_ffmpeg_get_audio_parameters,
    .get_video_parameters = bg_ffmpeg_get_video_parameters,

    .set_callbacks =        bg_ffmpeg_set_callbacks,
    
    .open =                 bg_ffmpeg_open,
    .open_io =              bg_ffmpeg_open_io,
    
    .writes_compressed_audio = bg_ffmpeg_writes_compressed_audio,
    .writes_compressed_video = bg_ffmpeg_writes_compressed_video,
    
    .add_audio_stream =     bg_ffmpeg_add_audio_stream,
    .add_video_stream =     bg_ffmpeg_add_video_stream,
    .add_text_stream =     bg_ffmpeg_add_text_stream,

    .add_audio_stream_compressed =     bg_ffmpeg_add_audio_stream_compressed,
    .add_video_stream_compressed =     bg_ffmpeg_add_video_stream_compressed,

    .set_video_pass =       bg_ffmpeg_set_video_pass,
    .set_audio_parameter =  bg_ffmpeg_set_audio_parameter,
    .set_video_parameter =  bg_ffmpeg_set_video_parameter,

    .get_audio_sink =     bg_ffmpeg_get_audio_sink,
    .get_audio_packet_sink =     bg_ffmpeg_get_audio_packet_sink,

    .get_video_sink =     bg_ffmpeg_get_video_sink,
    .get_video_packet_sink =     bg_ffmpeg_get_video_packet_sink,
    
    .start =                bg_ffmpeg_start,
    
    .get_text_sink = bg_ffmpeg_get_text_packet_sink,

    
    .close =                bg_ffmpeg_close,
  };

/* Include this into all plugin modules exactly once
   to let the plugin loader obtain the API version */
BG_GET_PLUGIN_API_VERSION;
