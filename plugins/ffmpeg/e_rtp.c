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

#include <ffmpeg_common.h>
#include <gmerlin/translation.h>

#include <gavl/utils.h>
#include <gavl/log.h>
#define LOG_DOMAIN "e_rtp"


static const ffmpeg_format_info_t format =
  {
      .label=       "Realtime transport protocol",
      .name =       "rtp",
      .protocol =   "rtp",
      
      .max_audio_streams = -1,
      .max_video_streams = -1,
      .audio_codecs = (enum AVCodecID[]){  AV_CODEC_ID_AAC,
                                           AV_CODEC_ID_NONE },
      .video_codecs = (enum AVCodecID[]){  AV_CODEC_ID_H264,
                                           AV_CODEC_ID_NONE },
  };

static const char * ffmpeg_get_protocols_rtp(void * data)
  {
  return format.protocol;
  }

static int ffmpeg_open_rtp(void * data, const char * filename,
                           const gavl_dictionary_t * metadata)
  {
  char * host = NULL;
  ffmpeg_priv_t * priv  = data;

  gavl_url_split(filename, NULL, NULL, NULL, &host, &priv->rtp_port, NULL);

  priv->rtp_base_address = gavl_sprintf("rtp://%s", host);
  return 1;
  }

static int start_stream(bg_ffmpeg_stream_t * st)
  {
  char * url = gavl_sprintf("%s:%d", st->ffmpeg->rtp_base_address, st->ffmpeg->rtp_port);
  
  if(avio_open(&st->fmtctx->pb, url, AVIO_FLAG_WRITE) < 0)
    {
    gavl_log(GAVL_LOG_ERROR, LOG_DOMAIN, "avio_open failed");
    return 0;
    }
  
  if(avformat_write_header(st->fmtctx, NULL))
    {
    gavl_log(GAVL_LOG_ERROR, LOG_DOMAIN, "avformat_write_header failed");
    return 0;
    }

  st->ffmpeg->rtp_port += 2;
  
  }

static int start_rtp(void * data)
  {
  ffmpeg_priv_t * priv;
  int i;
  AVFormatContext ** fmtctx;
  char buf[4096];
  priv = data;

  if(!bg_ffmpeg_start(data))
    return 0;

  for(i = 0; i < priv->num_audio_streams; i++)
    {
    start_stream(&priv->audio_streams[i]);
    }
  for(i = 0; i < priv->num_video_streams; i++)
    {
    start_stream(&priv->video_streams[i]);
    }
  /* Other types not supported yet (maybe in the future?) */

  /* Create SDP */
  fmtctx = calloc(priv->num_audio_streams + priv->num_audio_streams,
                  sizeof(*fmtctx));

  for(i = 0; i < priv->num_audio_streams; i++)
    fmtctx[i] = priv->audio_streams[i].fmtctx;

  for(i = 0; i < priv->num_video_streams; i++)
    fmtctx[priv->num_audio_streams+i]
      = priv->video_streams[i].fmtctx;

  av_sdp_create(fmtctx, priv->num_audio_streams + priv->num_video_streams, buf, sizeof(buf));
  fprintf(stderr, "Got SDP:\n%s\n", buf);
  }

static void * create_ffmpeg()
  {
  return bg_ffmpeg_create(&format);
  }

const bg_encoder_plugin_t the_plugin =
  {
    .common =
    {
      BG_LOCALE,
      .name =           "e_rtp",       /* Unique short name */
      .long_name =      format.label,
      .description =    TRS("Based on ffmpeg (http://www.ffmpeg.org)."),
      .type =           BG_PLUGIN_ENCODER,
      .flags =          BG_PLUGIN_URL,
      .priority =       5,
      .create =         create_ffmpeg,
      .destroy =        bg_ffmpeg_destroy,
      .get_parameters = bg_ffmpeg_get_parameters,
      .set_parameter =  bg_ffmpeg_set_parameter,
      .get_protocols = ffmpeg_get_protocols_rtp,
    },
    
    .max_audio_streams =         -1,
    .max_video_streams =         -1,
    .max_text_streams = -1,
    
    .get_audio_parameters = bg_ffmpeg_get_audio_parameters,
    .get_video_parameters = bg_ffmpeg_get_video_parameters,

    .set_callbacks =        bg_ffmpeg_set_callbacks,
    
    .open =                 ffmpeg_open_rtp,
    
    .writes_compressed_audio = bg_ffmpeg_writes_compressed_audio,
    .writes_compressed_video = bg_ffmpeg_writes_compressed_video,
    
    .add_audio_stream =     bg_ffmpeg_add_audio_stream,
    .add_video_stream =     bg_ffmpeg_add_video_stream,
    .add_text_stream =     bg_ffmpeg_add_text_stream,

    .add_audio_stream_compressed =     bg_ffmpeg_add_audio_stream_compressed,
    .add_video_stream_compressed =     bg_ffmpeg_add_video_stream_compressed,

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
