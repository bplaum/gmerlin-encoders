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
#include <gmerlin_encoders.h>

#include <gmerlin/plugin.h>
#include <gmerlin/utils.h>
#include <gmerlin/translation.h>
#include <gmerlin/cfg_registry.h>

#include <theora/theora.h>

#include "ogg_common.h"

#ifdef HAVE_THEORAENC
extern const bg_ogg_codec_t bg_theora_codec;
#endif

extern const bg_ogg_codec_t bg_vorbis_codec;

#ifdef HAVE_FLAC
extern const bg_ogg_codec_t bg_flacogg_codec;
#endif

#ifdef HAVE_OPUS
extern const bg_ogg_codec_t bg_opus_codec;
#endif

static bg_ogg_codec_t const * const audio_codecs[] =
  {
    &bg_vorbis_codec,
#ifdef HAVE_FLAC
    &bg_flacogg_codec,
#endif
#ifdef HAVE_OPUS
    &bg_opus_codec,
#endif
    NULL,
  };

static bg_ogg_codec_t const * const video_codecs[] =
  {
#ifdef HAVE_THEORAENC
    &bg_theora_codec,
#endif
    NULL,
  };

static const bg_parameter_info_t * get_audio_parameters_oggvideo(void * data)
  {
  return bg_ogg_encoder_get_audio_parameters(data, audio_codecs);
  }

static const bg_parameter_info_t * get_video_parameters_oggvideo(void * data)
  {
  return bg_ogg_encoder_get_video_parameters(data, video_codecs);
  }

static int add_audio_stream_oggvideo(void * data,
                                   const gavl_dictionary_t * m,
                                   const gavl_audio_format_t * format)
  {
  bg_ogg_stream_t * s;
  s = bg_ogg_encoder_add_audio_stream(data, m, format);
  return s->index;
  }

static int
add_audio_stream_compressed_oggvideo(void * data,
                                   const gavl_dictionary_t * m,
                                   const gavl_audio_format_t * format,
                                   const gavl_compression_info_t * ci)
  {
  bg_ogg_stream_t * s;
  
  s = bg_ogg_encoder_add_audio_stream_compressed(data, m, format, ci);
  
  if(ci->id == GAVL_CODEC_ID_VORBIS)
    bg_ogg_encoder_init_stream(data, s, &bg_vorbis_codec);
#ifdef HAVE_OPUS
  else if(ci->id == GAVL_CODEC_ID_OPUS)
    bg_ogg_encoder_init_stream(data, s, &bg_opus_codec);
#endif  
#ifdef HAVE_FLAC
  else if(ci->id == GAVL_CODEC_ID_FLAC)
    bg_ogg_encoder_init_stream(data, s, &bg_flacogg_codec);
#endif
#ifdef HAVE_SPEEX
  else if(ci->id == GAVL_CODEC_ID_SPEEX)
    bg_ogg_encoder_init_stream(data, s, &bg_speex_codec);
#endif
  return s->index;
  }

static int
add_video_stream_oggvideo(void * data,
                        const gavl_dictionary_t * m,
                        const gavl_video_format_t * format)
  {
  bg_ogg_stream_t * s;
  s = bg_ogg_encoder_add_video_stream(data, m, format);
  return s->index;
  }

static int
add_video_stream_compressed_oggvideo(void * data,
                                   const gavl_dictionary_t * m,
                                   const gavl_video_format_t * format,
                                   const gavl_compression_info_t * ci)
  {
  bg_ogg_stream_t * s;
  s = bg_ogg_encoder_add_video_stream_compressed(data, m, format, ci);

#ifdef HAVE_THEORAENC
  if(ci->id == GAVL_CODEC_ID_THEORA)
    bg_ogg_encoder_init_stream(data, s, &bg_theora_codec);
#endif

#ifdef HAVE_SCHROEDINGER
  if(ci->id == GAVL_CODEC_ID_DIRAC)
    bg_ogg_encoder_init_stream(data, s, &bg_schroedinger_codec);

#endif

  return s->index;
  }


static void
set_audio_parameter_oggvideo(void * data, int stream,
                           const char * name, const gavl_value_t * val)
  {
  int i;
  const char * codec_name;
  bg_ogg_encoder_t * enc = data;
  bg_ogg_stream_t * s = &enc->audio_streams[stream];
  
  if(!name)
    return;
  if(!strcmp(name, "codec"))
    {
    i = 0;
    codec_name = bg_multi_menu_get_selected_name(val);
    
    while(audio_codecs[i])
      {
      if(!strcmp(audio_codecs[i]->name, codec_name))
        {
        bg_ogg_encoder_init_stream(data, enc->audio_streams + stream, audio_codecs[i]);
        break;
        }
      i++;
      }
    bg_cfg_section_apply(bg_multi_menu_get_selected(val),
                         NULL,
                         s->codec->set_parameter,
                         s->codec_priv);
    }
  }

static void
set_video_parameter_oggvideo(void * data, int stream,
                           const char * name, const gavl_value_t * val)
  {
  int i;
  const char * codec_name;
  bg_ogg_encoder_t * enc = data;
  bg_ogg_stream_t * s = &enc->video_streams[stream];
  
  if(!name)
    return;
  if(!strcmp(name, "codec"))
    {
    i = 0;

    codec_name = bg_multi_menu_get_selected_name(val);
    
    while(video_codecs[i])
      {
      if(!strcmp(video_codecs[i]->name, codec_name))
        {
        bg_ogg_encoder_init_stream(data, enc->video_streams + stream, video_codecs[i]);
        break;
        }
      i++;
      }
    bg_cfg_section_apply(bg_multi_menu_get_selected(val),
                         NULL,
                         s->codec->set_parameter,
                         s->codec_priv);
    }
  else
    bg_ogg_encoder_set_video_parameter(data, stream, name, val);
  }

static int
open_oggvideo(void * data, const char * file,
            const gavl_dictionary_t * metadata)
  {
  return bg_ogg_encoder_open(data, file, NULL, metadata,
                             "ogv");
  }

static int
open_io_oggvideo(void * data, gavl_io_t * io,
                 const gavl_dictionary_t * metadata)
  {
  return bg_ogg_encoder_open(data, NULL, io, metadata,
                             "ogv");
  }

static int writes_compressed_audio_oggvideo(void* data,
                                            const gavl_audio_format_t * format,
                                            const gavl_compression_info_t * ci)
  {
  if((ci->id == GAVL_CODEC_ID_VORBIS)
#ifdef HAVE_FLAC
     || (ci->id == GAVL_CODEC_ID_FLAC)
#endif
#ifdef HAVE_SPEEX
     || (ci->id == GAVL_CODEC_ID_SPEEX)
#endif
#ifdef HAVE_OPUS
     || (ci->id == GAVL_CODEC_ID_OPUS)
#endif
     )
    return 1;
  else
    return 0;
  }

static int writes_compressed_video_oggvideo(void * data,
                                            const gavl_video_format_t * format,
                                            const gavl_compression_info_t * ci)
  {
  if((ci->id == GAVL_CODEC_ID_THEORA) ||
     (ci->id == GAVL_CODEC_ID_DIRAC))
    return 1;
  else
    return 0;
  }

static const char * get_extensions_oggvideo(void * priv)
  {
  return "ogv";
  }

const bg_encoder_plugin_t the_plugin =
  {
    .common =
    {
      BG_LOCALE,
      .name =            "e_oggvideo",       /* Unique short name */
      .long_name =       TRS("OGG Video"),
      .description =     TRS("Encoder for Ogg A/V files."),
      .type =            BG_PLUGIN_ENCODER,
      .flags =           BG_PLUGIN_FILE | BG_PLUGIN_PIPE,
      .priority =        5,
      .create =            bg_ogg_encoder_create,
      .destroy =           bg_ogg_encoder_destroy,
#if 0
      .get_parameters =    get_parameters_oggvideo,
      .set_parameter =     set_parameter_oggvideo,
#endif
      .get_extensions = get_extensions_oggvideo,  

    },
    .max_audio_streams =   -1,
    .max_video_streams =   -1,
    
    .set_callbacks =       bg_ogg_encoder_set_callbacks,
    .writes_compressed_audio = writes_compressed_audio_oggvideo,
    .writes_compressed_video = writes_compressed_video_oggvideo,
    
    .open =                open_oggvideo,
    .open_io =                open_io_oggvideo,
    
    .get_audio_parameters =    get_audio_parameters_oggvideo,
    .get_video_parameters =    get_video_parameters_oggvideo,

    .add_audio_stream =        add_audio_stream_oggvideo,
    .add_video_stream =        add_video_stream_oggvideo,

    .add_audio_stream_compressed = add_audio_stream_compressed_oggvideo,
    .add_video_stream_compressed = add_video_stream_compressed_oggvideo,
    
    .set_audio_parameter =     set_audio_parameter_oggvideo,
    .set_video_parameter =     set_video_parameter_oggvideo,
    .set_video_pass      =     bg_ogg_encoder_set_video_pass,
    
    .start =                  bg_ogg_encoder_start,
    
    .get_audio_sink =        bg_ogg_encoder_get_audio_sink,
    .get_video_sink =        bg_ogg_encoder_get_video_sink,

    .get_audio_packet_sink =        bg_ogg_encoder_get_audio_packet_sink,
    .get_video_packet_sink =        bg_ogg_encoder_get_video_packet_sink,
    
    .close =               bg_ogg_encoder_close,
  };

/* Include this into all plugin modules exactly once
   to let the plugin loader obtain the API version */
BG_GET_PLUGIN_API_VERSION;
