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
#include <gavl/gavlsocket.h>
#include <gavl/utils.h>
#include <gavl/sap.h>
#include <gavl/log.h>
#define LOG_DOMAIN "sap-sender"
#define SAP_INTERVAL (GAVL_TIME_SCALE*5)


struct sap_sender_s
  {
  gavl_dictionary_t sdp;
  gavl_dictionary_t sap;
  gavl_buffer_t sap_buf;
  int fd; // Socket
  gavl_socket_address_t * addr;
  pthread_mutex_t mutex;
  gavl_time_t last_sap_time;
  };

sap_sender_t * sap_sender_create(const char * sdp_init, const gavl_dictionary_t * m)
  {
  gavl_socket_address_t * local_addr;
  sap_sender_t * ret = calloc(1, sizeof(*ret));

  pthread_mutex_init(&ret->mutex, NULL);

  ret->last_sap_time = GAVL_TIME_UNDEFINED;

  local_addr = gavl_socket_address_create();
  gavl_socket_address_set_local(local_addr, 0, "0.0.0.0");
  
  bg_sdp_init(&ret->sdp, sdp_init, local_addr);
  ret->fd = gavl_udp_socket_create(local_addr);

  gavl_sap_init(&ret->sap, local_addr);
  
  gavl_socket_address_destroy(local_addr);

  ret->addr = gavl_socket_address_create();
  gavl_socket_address_set(ret->addr, "239.255.255.255", 9875, SOCK_DGRAM);
  return ret;
  }

void sap_sender_destroy(sap_sender_t * s)
  {
  /* Send bye */

  gavl_log(GAVL_LOG_INFO, LOG_DOMAIN, "Sending SAP bye");
  gavl_sap_encode(&s->sap_buf, 1, &s->sap);
  gavl_udp_socket_send(s->fd, s->sap_buf.buf, s->sap_buf.len,
                       s->addr);
  
  pthread_mutex_destroy(&s->mutex);
  gavl_dictionary_free(&s->sdp);
  gavl_dictionary_free(&s->sap);
  if(s->fd >= 0)
    gavl_socket_close(s->fd);

  if(s->addr)
    gavl_socket_address_destroy(s->addr);

  free(s);
  }

void sap_sender_ping(sap_sender_t * s)
  {
  gavl_time_t cur;
  
  if(pthread_mutex_trylock(&s->mutex))
    return;

  cur = gavl_time_get_monotonic();
  if((s->last_sap_time == GAVL_TIME_UNDEFINED) ||
     (cur - s->last_sap_time > SAP_INTERVAL))
    {
    //    char * uri;
    s->last_sap_time = cur;
    /* TODO: Send SAP */

    // fprintf(stderr, "Sending SDP: %s\n", gavl_dictionary_get_string(&s->sap, GAVL_SAP_SDP));
    
    //    uri = gavl_sdp_to_uri(s->sdp_string);
    //    fprintf(stderr, "URI: %s\n", uri);
    //    free(uri);

    /* Send SAP packet */

    if(!gavl_udp_socket_send(s->fd, s->sap_buf.buf, s->sap_buf.len,
                            s->addr))
      gavl_log(GAVL_LOG_ERROR, LOG_DOMAIN, "Sending SAP failed");
    
    }
  
  pthread_mutex_unlock(&s->mutex);
  }

void sap_sender_update(sap_sender_t * s, const gavl_dictionary_t * m)
  {
  char * sdp;

  pthread_mutex_lock(&s->mutex);

  sdp = bg_sdp_update(&s->sdp, m);
  gavl_dictionary_set_string_nocopy(&s->sap, GAVL_SAP_SDP, sdp);
  gavl_sap_encode(&s->sap_buf, 0, &s->sap);

  //  fprintf(stderr, "Updated SAP packet:\n");
  //  gavl_hexdump(s->sap_buf.buf, s->sap_buf.len, 16);
  
  s->last_sap_time = GAVL_TIME_UNDEFINED;
  pthread_mutex_unlock(&s->mutex);
  }
