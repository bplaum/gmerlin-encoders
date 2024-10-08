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

#include <gmerlin/plugin.h>
#include <gmerlin/translation.h>
#include <gmerlin/charset.h>

#include <gmerlin/log.h>
#define LOG_DOMAIN "shout"

#include <gavl/metatags.h>


#include <bgshout.h>

struct bg_shout_s
  {
  shout_t * s;
  shout_metadata_t * met;
  int metadata_sent;
  int64_t bytes_sent;
  int format;
  bg_charset_converter_t * cnv;
  };


bg_shout_t * bg_shout_create(int format)
  {
  bg_shout_t * ret;
  ret = calloc(1, sizeof(*ret));

  shout_init();
  ret->s = shout_new();
  ret->format = format;

  if(ret->format != SHOUT_FORMAT_OGG)
    ret->cnv = bg_charset_converter_create("UTF-8", "ISO-8859-1");
  
  shout_set_format(ret->s, format);
  return ret; 
  }

static const bg_parameter_info_t parameters[] =
  {
    {
      .name        = "server",
      .long_name   = TRS("Server"),
      .type        = BG_PARAMETER_STRING,
      .val_default = GAVL_VALUE_INIT_STRING("localhost"),
    },
    {
      .name        = "port",
      .long_name   = TRS("Port"),
      .type        = BG_PARAMETER_INT,
      .val_min     = GAVL_VALUE_INIT_INT(1),
      .val_max     = GAVL_VALUE_INIT_INT(65535),
      .val_default = GAVL_VALUE_INIT_INT(8000),
    },
    {
      .name        = "mount",
      .long_name   = TRS("Mount"),
      .type        = BG_PARAMETER_STRING,
      .val_default = GAVL_VALUE_INIT_STRING("/stream.ogg"),
    },
    {
      .name        = "user",
      .long_name   = TRS("User"),
      .type        = BG_PARAMETER_STRING,
      .val_default = GAVL_VALUE_INIT_STRING("source"),
    },
    {
      .name        = "password",
      .long_name   = TRS("Password"),
      .type        = BG_PARAMETER_STRING_HIDDEN,
    },
    {
      .name        = "name",
      .long_name   = TRS("Name"),
      .type        = BG_PARAMETER_STRING,
      .val_default = GAVL_VALUE_INIT_STRING("Test stream"),
    },
    {
      .name        = "description",
      .long_name   = TRS("Description"),
      .type        = BG_PARAMETER_STRING,
      .val_default = GAVL_VALUE_INIT_STRING("Brought to you by gmerlin"),
    },
    {
      .name        = "genre",
      .long_name   = TRS("Genre"),
      .type        = BG_PARAMETER_STRING,
    },
    { /* */ },
  };

const bg_parameter_info_t * bg_shout_get_parameters(void)
  {
  return parameters;
  }

void bg_shout_set_parameter(void * data, const char * name,
                            const gavl_value_t * val)
  {
  bg_shout_t * s = data;
  if(!name)
    return;

  if(!strcmp(name, "server"))
    {
    shout_set_host(s->s, val->v.str);
    }
  else if(!strcmp(name, "port"))
    {
    shout_set_port(s->s, val->v.i);
    }
  else if(!strcmp(name, "mount"))
    {
    shout_set_mount(s->s, val->v.str);
    }
  else if(!strcmp(name, "user"))
    {
    if(val->v.str)
      shout_set_user(s->s, val->v.str);
    }
  else if(!strcmp(name, "password"))
    {
    if(val->v.str)
      shout_set_password(s->s, val->v.str);
    }
  else if(!strcmp(name, "name"))
    {
    if(val->v.str)
      shout_set_name(s->s, val->v.str);
    }
  else if(!strcmp(name, "description"))
    {
    if(val->v.str)
      shout_set_description(s->s, val->v.str);
    }
  else if(!strcmp(name, "genre"))
    {
    if(val->v.str)
      shout_set_genre(s->s, val->v.str);
    }
  }

int bg_shout_open(bg_shout_t * s)
  {
  if(shout_open(s->s))
    {
    gavl_log(GAVL_LOG_ERROR, LOG_DOMAIN, "Connecting failed: %s",
           shout_get_error(s->s));
    return 0;
    }
  gavl_log(GAVL_LOG_INFO, LOG_DOMAIN, "Connected to icecast server");
  
  return 1;
  }

void bg_shout_set_metadata(bg_shout_t * s, const gavl_dictionary_t * m)
  {
  const char * genre;
  if((genre = gavl_dictionary_get_string(m, GAVL_META_GENRE)))
    shout_set_genre(s->s, genre);
  }

void bg_shout_destroy(bg_shout_t * s)
  {
  if(shout_get_connected(s->s) == SHOUTERR_CONNECTED)
    shout_close(s->s);
  shout_free(s->s);
  if(s->cnv)
    bg_charset_converter_destroy(s->cnv);
  
  free(s);
  }

static void flush_metadata(bg_shout_t * s)
  {
  if(shout_set_metadata(s->s, s->met) != SHOUTERR_SUCCESS)
    {
    gavl_log(GAVL_LOG_ERROR, LOG_DOMAIN, "Sending metadata failed: %s",
           shout_get_error(s->s));
    }
  shout_metadata_free(s->met);
  s->met = NULL;
  }

int bg_shout_write(bg_shout_t * s, const uint8_t * data, int len)
  {
  shout_sync(s->s);
  
  if(shout_send(s->s, data, len) != SHOUTERR_SUCCESS)
    {
    gavl_log(GAVL_LOG_ERROR, LOG_DOMAIN, "Sending data failed: %s",
           shout_get_error(s->s));
    return 0;
    }
  s->bytes_sent += len;
  
  //   if(s->met && (s->bytes_sent > 32000))
  //    flush_metadata(s);
  
  return len;
  }

static void metadata_add(bg_shout_t * s,
                         const char * name,
                         const char * val)
  {
  if(!s->cnv)
    shout_metadata_add(s->met, name, val);
  else
    {
    char * tmp_string = bg_convert_string(s->cnv, val, -1, NULL);
    shout_metadata_add(s->met, name, tmp_string);
    free(tmp_string);
    }
  }

void bg_shout_update_metadata(bg_shout_t * s,
                              const gavl_dictionary_t * m)
  {
  const char * artist = NULL;
  const char * title = NULL;
  const char * label = NULL;
  
  if(s->met)
    shout_metadata_free(s->met);
  
  s->met = shout_metadata_new();

  if(m)
    {
    artist = gavl_dictionary_get_string(m, GAVL_META_ARTIST);
    title = gavl_dictionary_get_string(m, GAVL_META_TITLE);
    label = gavl_dictionary_get_string(m, GAVL_META_LABEL);
    }
  
  if(artist && title)
    {
    metadata_add(s, "artist", artist);
    metadata_add(s, "title",  title);
    }
  else if(label)
    {
    metadata_add(s, "song", label);
    }
  else /* Clear everything */
    {
    metadata_add(s, "song", shout_get_name(s->s));
    }
  flush_metadata(s);
  }
