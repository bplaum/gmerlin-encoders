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

// #include <faac.h>

#include <config.h>
#include <gmerlin/plugin.h>
#include <gmerlin/translation.h>

#define USE_VBR
#include "bglame.h"


static void * create_codec()
  {
  return bg_lame_create();
  }

static void destroy_codec(void * priv)
  {
  bg_lame_destroy(priv);
  }

static const bg_parameter_info_t * get_parameters(void * priv)
  {
  return audio_parameters;
  }

static void set_parameter(void * priv, const char * name,
                          const gavl_value_t * val)
  {
  bg_lame_set_parameter(priv, name, val);
  }

static gavl_audio_sink_t * open_audio(void * priv,
                                      gavl_dictionary_t * s)
  {
  return bg_lame_open(priv, s);
  }

static void set_packet_sink(void * priv, gavl_packet_sink_t * s)
  {
  bg_lame_set_packet_sink(priv, s);
  }

gavl_codec_id_t compressions[] =
  {
    GAVL_CODEC_ID_MP3,
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
      .name =            "c_lame",       /* Unique short name */
      .long_name =       TRS("MP3"),
      .description =     TRS("lame based MP3 encoder"),
      .type =            BG_PLUGIN_COMPRESSOR_AUDIO,
      .priority =        5,
      .create =          create_codec,
      .destroy =         destroy_codec,
      .get_parameters =    get_parameters,
      .set_parameter =     set_parameter,
    },
    .open_encode_audio = open_audio,
    .set_packet_sink = set_packet_sink,
    .get_compressions = get_compressions,
  };
/* Include this into all plugin modules exactly once
   to let the plugin loader obtain the API version */
BG_GET_PLUGIN_API_VERSION;
