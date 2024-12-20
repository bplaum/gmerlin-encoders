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
#include <gmerlin_encoders.h>

#include <gmerlin/utils.h>
#include <gavl/metatags.h>
#include <gavl/utils.h>

#define GENRE_MAX 0x94

static const char *id3_genres[GENRE_MAX] =
  {
    "Blues", "Classic Rock", "Country", "Dance",
    "Disco", "Funk", "Grunge", "Hip-Hop",
    "Jazz", "Metal", "New Age", "Oldies",
    "Other", "Pop", "R&B", "Rap", "Reggae",
    "Rock", "Techno", "Industrial", "Alternative",
    "Ska", "Death Metal", "Pranks", "Soundtrack",
    "Euro-Techno", "Ambient", "Trip-Hop", "Vocal",
    "Jazz+Funk", "Fusion", "Trance", "Classical",
    "Instrumental", "Acid", "House", "Game",
    "Sound Clip", "Gospel", "Noise", "Alt",
    "Bass", "Soul", "Punk", "Space",
    "Meditative", "Instrumental Pop",
    "Instrumental Rock", "Ethnic", "Gothic",
    "Darkwave", "Techno-Industrial", "Electronic",
    "Pop-Folk", "Eurodance", "Dream",
    "Southern Rock", "Comedy", "Cult",
    "Gangsta Rap", "Top 40", "Christian Rap",
    "Pop/Funk", "Jungle", "Native American",
    "Cabaret", "New Wave", "Psychedelic", "Rave",
    "Showtunes", "Trailer", "Lo-Fi", "Tribal",
    "Acid Punk", "Acid Jazz", "Polka", "Retro",
    "Musical", "Rock & Roll", "Hard Rock", "Folk",
    "Folk/Rock", "National Folk", "Swing",
    "Fast-Fusion", "Bebob", "Latin", "Revival",
    "Celtic", "Bluegrass", "Avantgarde",
    "Gothic Rock", "Progressive Rock",
    "Psychedelic Rock", "Symphonic Rock", "Slow Rock",
    "Big Band", "Chorus", "Easy Listening",
    "Acoustic", "Humour", "Speech", "Chanson",
    "Opera", "Chamber Music", "Sonata", "Symphony",
    "Booty Bass", "Primus", "Porn Groove",
    "Satire", "Slow Jam", "Club", "Tango",
    "Samba", "Folklore", "Ballad", "Power Ballad",
    "Rhythmic Soul", "Freestyle", "Duet",
    "Punk Rock", "Drum Solo", "A Cappella",
    "Euro-House", "Dance Hall", "Goa",
    "Drum & Bass", "Club-House", "Hardcore",
    "Terror", "Indie", "BritPop", "Negerpunk",
    "Polsk Punk", "Beat", "Christian Gangsta Rap",
    "Heavy Metal", "Black Metal", "Crossover",
    "Contemporary Christian", "Christian Rock",
    "Merengue", "Salsa", "Thrash Metal",
    "Anime", "JPop", "Synthpop"
  };


#define TITLE_POS     3
#define TITLE_LEN    30

#define ARTIST_POS   33
#define ARTIST_LEN   30

#define ALBUM_POS    63
#define ALBUM_LEN    30

#define YEAR_POS     93
#define YEAR_LEN      4

#define COMMENT_POS  97
#define COMMENT_LEN  28

#define TRACK_POS   126
#define GENRE_POS   127

struct bgen_id3v1_s
  {
  char data[128];
  };

static void set_string(char * dst, const gavl_dictionary_t * m,
                       const char * key, int max_len,
                       gavl_charset_converter_t * cnv)
  {
  int out_len;

  char * tmp_string;

  const char * src = gavl_dictionary_get_string(m, key);
  
  if(!src)
    return;

  tmp_string =
    gavl_convert_string(cnv,
                      src, -1, &out_len);

  if(!tmp_string) /* String could not be converted */
    return;
  
  if(out_len > max_len)
    out_len = max_len;
  memcpy(dst, tmp_string, out_len);
  free(tmp_string);
  }

bgen_id3v1_t * bgen_id3v1_create(const gavl_dictionary_t * m)
  {
  int track;
  int i;
  char * tmp_string;
  int year;
  gavl_charset_converter_t * cnv;
  bgen_id3v1_t * ret;
  const char * genre;
  ret = calloc(1, sizeof(*ret));

  ret->data[0] = 'T';
  ret->data[1] = 'A';
  ret->data[2] = 'G';

  cnv = gavl_charset_converter_create("UTF-8", "ISO-8859-1");

  set_string(&ret->data[TITLE_POS],  m, GAVL_META_TITLE,  TITLE_LEN, cnv);
  set_string(&ret->data[ARTIST_POS], m, GAVL_META_ARTIST, ARTIST_LEN, cnv);
  set_string(&ret->data[ALBUM_POS],  m, GAVL_META_ALBUM,  ALBUM_LEN, cnv);

  /* Year */
    
  year = bg_metadata_get_year(m);
  if(year)
    {
    tmp_string = gavl_sprintf("%d", year);
    if(strlen(tmp_string) == 4)
      {
      memcpy(&ret->data[YEAR_POS], tmp_string, 4);
      }
    free(tmp_string);
    }

  /* Comment */

  set_string(&ret->data[COMMENT_POS], m, GAVL_META_COMMENT, COMMENT_LEN, cnv);

  /* Track */

  if(gavl_dictionary_get_int(m, GAVL_META_TRACKNUMBER, &track) &&
     (track > 0) && (track < 255))
    ret->data[TRACK_POS] = track;
  
  /* Genre */
  
  ret->data[GENRE_POS] = 0xff;

  genre = gavl_dictionary_get_string(m, GAVL_META_GENRE);
    
  if(genre)
    {
    for(i = 0; i < GENRE_MAX; i++)
      {
      if(!strcasecmp(genre, id3_genres[i]))
        {
        ret->data[GENRE_POS] = i;
        break;
        }
      }
    }
  
  gavl_charset_converter_destroy(cnv);
  
  return ret;
  }

int bgen_id3v1_write(gavl_io_t * output, const bgen_id3v1_t * tag)
  {
  if(gavl_io_write_data(output, (uint8_t*)tag->data, 128) < 128)
    return 0;
  return 1;
  }

void bgen_id3v1_destroy(bgen_id3v1_t * tag)
  {
  free(tag);
  }

