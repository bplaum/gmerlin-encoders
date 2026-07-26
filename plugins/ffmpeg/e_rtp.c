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

#include <pthread.h>


#include <ffmpeg_common.h>
#include <gmerlin/translation.h>

#include <gavl/utils.h>
#include <gavl/metatags.h>
#include <gavl/log.h>
#define LOG_DOMAIN "e_rtp"

#include <gavl/state.h>


static const ffmpeg_format_info_t format =
  {
      .label=       "Realtime transport protocol",
      .name =       "rtp",
      .protocol =   "rtp",
      
      .max_audio_streams = -1,
      .max_video_streams = -1,
      .flags = FLAG_SAP,
      
      .audio_codecs = (enum AVCodecID[]){  AV_CODEC_ID_AAC,
                                           AV_CODEC_ID_PCM_S16BE,
                                           AV_CODEC_ID_NONE },
      .video_codecs = (enum AVCodecID[]){  AV_CODEC_ID_H264,
                                           AV_CODEC_ID_NONE },
  };

static void ping_rtp(ffmpeg_priv_t * priv)
  {
  sap_sender_ping(priv->sap);
  }

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

  if(host)
    free(host);

  gavl_dictionary_copy(&priv->m, metadata);
  
  
  return 1;
  }

static int start_stream(bg_ffmpeg_stream_t * st)
  {
  if(avio_open(&st->fmtctx->pb, st->uri, AVIO_FLAG_WRITE) < 0)
    {
    gavl_log(GAVL_LOG_ERROR, LOG_DOMAIN, "avio_open failed");
    return 0;
    }
  if(avformat_write_header(st->fmtctx, NULL))
    {
    gavl_log(GAVL_LOG_ERROR, LOG_DOMAIN, "avformat_write_header failed");
    return 0;
    }

  return 1;
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
    if(!start_stream(&priv->audio_streams[i]))
      return 0;

    fprintf(stderr, "Audio packet sink: %p\n", priv->audio_streams[i].psink);

    //  priv->audio_streams[i]->pt = gavl_packet_timer_create(1, );

    priv->audio_streams[i].pt = gavl_packet_timer_create(1, priv->audio_streams[i].aformat->samplerate);
    
    if(!i)
      priv->audio_streams[i].ping_func = ping_rtp;
    }
  for(i = 0; i < priv->num_video_streams; i++)
    {
    

    if(!start_stream(&priv->video_streams[i]))
      return 0;

    fprintf(stderr, "Video packet sink: %p\n", priv->video_streams[i].psink);

    priv->video_streams[i].pt =
      gavl_packet_timer_create(!(priv->video_streams[i].ci.flags & GAVL_COMPRESSION_HAS_B_FRAMES),
                               priv->video_streams[i].vformat->timescale);
    
    if(!i && !priv->num_audio_streams)
      priv->video_streams[i].ping_func = ping_rtp;
    

    }
  
  /* Other stream types not supported yet (maybe in the future?) */
  
  /* Create SDP */
  fmtctx = calloc(priv->num_audio_streams + priv->num_audio_streams,
                  sizeof(*fmtctx));

  for(i = 0; i < priv->num_audio_streams; i++)
    fmtctx[i] = priv->audio_streams[i].fmtctx;

  for(i = 0; i < priv->num_video_streams; i++)
    fmtctx[priv->num_audio_streams+i]
      = priv->video_streams[i].fmtctx;

  av_sdp_create(fmtctx, priv->num_audio_streams + priv->num_video_streams, buf, sizeof(buf));
  //  fprintf(stderr, "Got SDP:\n%s\n", buf);

  priv->sap = sap_sender_create(buf, &priv->m);
  
  free(fmtctx);
  
  return 1;
  }

static void * create_ffmpeg()
  {
  av_log_set_level(AV_LOG_DEBUG);
  return bg_ffmpeg_create(&format);
  }

static void update_metadata(ffmpeg_priv_t * priv, gavl_dictionary_t * m)
  {
  gavl_dictionary_copy_value(m, &priv->m, GAVL_META_STATION);

  if(!gavl_dictionary_get(m, GAVL_META_LABEL))
    gavl_dictionary_copy_value(m, &priv->m, GAVL_META_LABEL);

  if(!gavl_dictionary_get(m, GAVL_META_TITLE))
    gavl_dictionary_copy_value(m, &priv->m, GAVL_META_TITLE);
  
  sap_sender_update(priv->sap, m);
  
  }

static int handle_msg(void * data, gavl_msg_t * msg)
  {
  //  ffmpeg_priv_t * priv = data;

  switch(msg->NS)
    {
    case GAVL_MSG_NS_STATE:
      switch(msg->ID)
        {
        case GAVL_MSG_STATE_CHANGED:
          {
          const char * ctx;
          const char * var;
          gavl_value_t val;

          gavl_value_init(&val);
          
          gavl_msg_get_state(msg,
                             NULL,
                             &ctx,
                             &var, &val, NULL);
          
          if(!strcmp(ctx, GAVL_STATE_CTX_SRC))
            {
            
            if(!strcmp(var, GAVL_STATE_SRC_METADATA))
              {
              gavl_dictionary_t * dict;
              if((dict = gavl_value_get_dictionary_nc(&val)))
                {
                update_metadata(data, dict);
                }
              }
            }
          gavl_value_free(&val);
          }
          break;
        }
      break;
    }
    
  return 1;
  }

static bg_msg_sink_t * add_msg_stream(void * data, int stream_id)
  {
  ffmpeg_priv_t * priv = data;
  if(stream_id != GAVL_META_STREAM_ID_MSG_PROGRAM)
    return NULL;
  
  priv->msg_sink = bg_msg_sink_create(handle_msg, priv, 1);
  return priv->msg_sink;
  
  }

static void destroy_rtp(void * data)
  {
  ffmpeg_priv_t * priv = data;
  if(priv->sap)
    sap_sender_destroy(priv->sap);
  bg_ffmpeg_destroy(data);
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
      .flags =          BG_PLUGIN_URL | BG_PLUGIN_NOMUX | BG_PLUGIN_BROADCAST,
      .priority =       5,
      .create =         create_ffmpeg,
      .destroy =        destroy_rtp,
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
    //    .add_text_stream =     bg_ffmpeg_add_text_stream,
    .add_msg_stream =     add_msg_stream,

    .add_audio_stream_compressed =     bg_ffmpeg_add_audio_stream_compressed,
    .add_video_stream_compressed =     bg_ffmpeg_add_video_stream_compressed,

    .set_audio_parameter =  bg_ffmpeg_set_audio_parameter,
    .set_video_parameter =  bg_ffmpeg_set_video_parameter,

    .get_audio_sink =     bg_ffmpeg_get_audio_sink,
    .get_audio_packet_sink =     bg_ffmpeg_get_audio_packet_sink,

    .get_video_sink =     bg_ffmpeg_get_video_sink,
    .get_video_packet_sink =     bg_ffmpeg_get_video_packet_sink,
    
    .start =                start_rtp,
    
    .get_text_sink = bg_ffmpeg_get_text_packet_sink,

    
    .close =                bg_ffmpeg_close,
  };

/* Include this into all plugin modules exactly once
   to let the plugin loader obtain the API version */
BG_GET_PLUGIN_API_VERSION;
