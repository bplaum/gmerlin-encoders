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
#include <gavl/gavlsocket.h>
#include <gavl/utils.h>
#include <gavl/sap.h>

/*
  v=0
  o=- 0 0 IN IP4 127.0.0.1
  s=No Name
  c=IN IP4 239.1.0.1
  t=0 0
  a=tool:libavformat LIBAVFORMAT_VERSION
  m=audio 3330 RTP/AVP 97
  a=rtpmap:97 MPEG4-GENERIC/44100/2
  a=fmtp:97 profile-level-id=1;mode=AAC-hbr;sizelength=13;indexlength=3;indexdeltalength=3; config=121056E500

  
 */

/*
 *  Linebreaks: 
 */

#define SDP_SESSION_ID       "id"
#define SDP_SESSION_VERSION  "version"
#define SDP_SESSION_ADDR     "addr"
#define SDP_SESSION_USER     "user"
#define SDP_SESSION_NETTYPE  "nettype"
#define SDP_SESSION_ADDRTYPE "addrtype"

#define SDP_HEAD "head"
#define SDP_TAIL "tail"




void bg_sdp_init(gavl_dictionary_t * dict, const char *sdp, gavl_socket_address_t * addr)
  {
  char * pos;
  char * tmp_string;
  //  char ** list;
  int done;
  const char * start;
  const char * end;

  char * user = NULL;
  char * nettype = NULL;
  char * addrtype = NULL;
  
  char str[GAVL_SOCKET_ADDR_STR_LEN];

  /* Parse o= */

  if(!gavl_parse_sdp_o(sdp,
                       &user,
                       NULL,
                       NULL,
                       &nettype,
                       &addrtype,
                       NULL))
    goto fail;

#if 0  
  start = strstr(sdp, "o=");
  if(!start)
    goto fail;

  start+=2;
  
  end = strchr(start, '\r');
  if(!end)
    goto fail;

  tmp_string = gavl_strndup(start, end);
  
  list = gavl_strbreak(tmp_string, ' ');
#endif
  
  gavl_dictionary_set_string_nocopy(dict, SDP_SESSION_USER, user);
  gavl_dictionary_set_string_nocopy(dict, SDP_SESSION_NETTYPE, nettype);
  gavl_dictionary_set_string_nocopy(dict, SDP_SESSION_ADDRTYPE, addrtype);
  
  gavl_dictionary_set_long(dict, SDP_SESSION_VERSION, 0);
  gavl_dictionary_set_long(dict, SDP_SESSION_ID, gavl_time_get_realtime());
  
  gavl_socket_address_to_string(addr, str);
  if((pos = strchr(str, ':')))
    *pos = '\0';

  gavl_dictionary_set_string(dict, SDP_SESSION_ADDR, str);
  
  
  /* Assmble head and tail */
  tmp_string = NULL;

  start = sdp;

  done = 0;

  while(!done)
    {
    if((end = strchr(start, '\r')))
      {
      end += 2;
      }
    else
      {
      end = start + strlen(start);
      done = 1;
      }
    
    if(gavl_string_starts_with(start, "m="))
      {
      gavl_dictionary_set_string(dict, SDP_TAIL, start);
      break;
      }
    
       
    
    if(!gavl_string_starts_with(start, "v=") &&
       !gavl_string_starts_with(start, "o=") &&
       !gavl_string_starts_with(start, "s=") &&
       !gavl_string_starts_with(start, "i="))
      {
      tmp_string = gavl_strncat(tmp_string, start, end);
      if(done)
        tmp_string = gavl_strcat(tmp_string, "\r\n");
      
      }
    start = end;
    
    }
  
  gavl_dictionary_set_string_nocopy(dict, SDP_HEAD, tmp_string);
  
      
  //  fprintf(stderr, "Initialized SDP\n");
  //  gavl_dictionary_dump(dict, 2);
  
  fail:
  
  }

static char * add_meta_string(char * str, const gavl_dictionary_t * m,
                              const char * gavl_key,
                              const char * sdp_key, int multi)
  {
  char * tmp_string;
  
  if(!gavl_dictionary_get(m, gavl_key))
    return str;
  
  if(multi)
    {
    char * val = gavl_metadata_join_arr(m, gavl_key, "; ");
    tmp_string = gavl_sprintf("a=%s:%s\r\n", sdp_key, val); 
    free(val);
    }
  else
    tmp_string = gavl_sprintf("a=%s:%s\r\n", sdp_key, gavl_dictionary_get_string(m, gavl_key)); 

  str = gavl_strcat(str, tmp_string);
  free(tmp_string);
  return str;
  
  }


char * bg_sdp_update(gavl_dictionary_t * dict,
                     const gavl_dictionary_t * m)
  {
  int64_t version;
  int64_t id = 0;
  const char * stream_name;
  const char * stream_info;
  
  char * ret;
  

  gavl_dictionary_get_long(dict, SDP_SESSION_VERSION, &version);
  
  gavl_dictionary_set_long(dict, SDP_SESSION_VERSION, version+1);

  gavl_dictionary_get_long(dict, SDP_SESSION_ID, &id);
  
  if((!(stream_name = gavl_dictionary_get_string(m, GAVL_META_STATION)) &&
      !(stream_name = gavl_dictionary_get_string(m, GAVL_META_TITLE)) &&
      !(stream_name = gavl_dictionary_get_string(m, GAVL_META_LABEL))))
    stream_name = "Gmerlin rtp stream";
  
  stream_info = gavl_dictionary_get_string(m, GAVL_META_LABEL);
  if(!stream_info)
    stream_info = gavl_dictionary_get_string(m, GAVL_META_TITLE);
  if(!stream_info)
    stream_info = "No title available";
  
  // Dictionaries auto remove  the trailing \r\n
  
  ret = gavl_sprintf("v=0\r\no=%s %"PRId64" %"PRId64" %s %s %s\r\ns=%s\r\ni=%s\r\n%s\r\n",
                     gavl_dictionary_get_string(dict, SDP_SESSION_USER), id, version,
                     gavl_dictionary_get_string(dict, SDP_SESSION_NETTYPE),
                     gavl_dictionary_get_string(dict, SDP_SESSION_ADDRTYPE),
                     gavl_dictionary_get_string(dict, SDP_SESSION_ADDR),
                     stream_name,
                     stream_info,
                     gavl_dictionary_get_string(dict, SDP_HEAD)
                     );

  ret = add_meta_string(ret, m, GAVL_META_ARTIST, "artist", 1);
  ret = add_meta_string(ret, m, GAVL_META_TITLE, "title", 0);
  ret = add_meta_string(ret, m, GAVL_META_ALBUM, "album", 0);
  ret = add_meta_string(ret, m, GAVL_META_YEAR, "date", 0);
  ret = add_meta_string(ret, m, GAVL_META_GENRE, "genre", 1);
  ret = add_meta_string(ret, m, GAVL_META_LOGO_URL, "artwork", 0);
  
  ret = gavl_strcat(ret, gavl_dictionary_get_string(dict, SDP_TAIL));

  // Dictionaries auto remove  the trailing \r\n
  ret = gavl_strcat(ret, "\r\n");
  
  
  //   fprintf(stderr, "Got SDP: %s\n", ret);

  return ret;
  
  }
