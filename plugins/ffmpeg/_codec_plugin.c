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



typedef struct
  {
  bg_ffmpeg_codec_context_t * codec;
  } stream_codec_t;

static void * create_codec()
  {
  stream_codec_t * ret = calloc(1, sizeof(*ret));

  ret->codec = bg_ffmpeg_codec_create(
#ifdef IS_AUDIO
                                      AVMEDIA_TYPE_AUDIO,
#else
                                      AVMEDIA_TYPE_VIDEO,
#endif
                                      NULL,
                                      CODEC_ID,
                                      NULL);
  return ret;
  }

static void destroy_codec(void * priv)
  {
  stream_codec_t * c = priv;
  if(c->codec)
    bg_ffmpeg_codec_destroy(c->codec);
  free(c);
  }

#ifndef NO_PARAMS
static const bg_parameter_info_t * get_parameters(void * priv)
  {
  stream_codec_t * c = priv;
  return bg_ffmpeg_codec_get_parameters(c->codec);
  }

static void set_parameter(void * priv, const char * name,
                          const gavl_value_t * val)
  {
  stream_codec_t * c = priv;
  bg_ffmpeg_codec_set_parameter(c->codec, name, val);
  }
#endif

#ifdef IS_AUDIO
static gavl_audio_sink_t * open_audio(void * priv,
                                      gavl_dictionary_t * s)
  {
  stream_codec_t * c = priv;
  return bg_ffmpeg_codec_open_audio(c->codec, s);
  }
#else
static gavl_video_sink_t * open_video(void * priv,
                                      gavl_dictionary_t * s)
  {
  stream_codec_t * c = priv;
  return bg_ffmpeg_codec_open_video(c->codec, s);
  }
#endif

static void set_packet_sink(void * priv, gavl_packet_sink_t * s)
  {
  stream_codec_t * c = priv;
  bg_ffmpeg_codec_set_packet_sink(c->codec, s);
  }

gavl_codec_id_t compressions[] =
  {
    COMPRESSION,
    GAVL_CODEC_ID_NONE
  };

static const gavl_codec_id_t * get_compressions(void * priv)
  {
  return compressions;
  }

const bg_codec_plugin_t the_plugin =
  {
    .common =
    {
      BG_LOCALE,
      .name =            CODEC_NAME,       /* Unique short name */
      .long_name =       CODEC_LONG_NAME,
      .description =     CODEC_DESC,
#ifdef IS_AUDIO
      .type =           BG_PLUGIN_COMPRESSOR_AUDIO,
#else
      .type =           BG_PLUGIN_COMPRESSOR_VIDEO,
#endif
      .priority =        5,
      .create =          create_codec,
      .destroy =         destroy_codec,
#ifndef NO_PARAMS
      .get_parameters =    get_parameters,
      .set_parameter =     set_parameter,
#endif
    },
#ifdef IS_AUDIO
    .open_encode_audio = open_audio,
#else 
    .open_encode_video = open_video,
#endif
    .set_packet_sink = set_packet_sink,
    .get_compressions  = get_compressions,

  };
/* Include this into all plugin modules exactly once
   to let the plugin loader obtain the API version */
BG_GET_PLUGIN_API_VERSION;
