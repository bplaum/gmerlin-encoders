/*****************************************************************
 * gmerlin-encoders - encoder plugins for gmerlin
 *
 * Copyright (c) 2001 - 2012 Members of the Gmerlin project
 * gmerlin-general@lists.sourceforge.net
 * http://gmerlin.sourceforge.net
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

#include <faac.h>

#include <config.h>
#include <gmerlin/plugin.h>
#include <gmerlin/translation.h>

#include "faac_codec.h"


static void * create_codec()
  {
  return bg_faac_create();
  }

static void destroy_codec(void * priv)
  {
  bg_faac_destroy(priv);
  }

static const bg_parameter_info_t * get_parameters(void * priv)
  {
  return bg_faac_get_parameters(priv);
  }

static void set_parameter(void * priv, const char * name,
                          const gavl_value_t * val)
  {
  bg_faac_set_parameter(priv, name, val);
  }

static gavl_audio_sink_t * open_audio(void * priv,
                                      gavl_compression_info_t * ci,
                                      gavl_audio_format_t * fmt,
                                      gavl_dictionary_t * m)
  {
  return bg_faac_open(priv, ci, fmt, m);
  }

static void set_packet_sink(void * priv, gavl_packet_sink_t * s)
  {
  bg_faac_set_packet_sink(priv, s);
  }

gavl_codec_id_t compressions[] =
  {
    GAVL_CODEC_ID_AAC,
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
      .name =            "c_faac",       /* Unique short name */
      .long_name =       TRS("AAC"),
      .description =     TRS("faac based AAC encoder"),
      .type =            BG_PLUGIN_COMPRESSOR_AUDIO,
      .flags =           0,
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
