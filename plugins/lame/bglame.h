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



#include <lame/lame.h>
#include <gmerlin/plugin.h>

typedef struct bg_lame_s bg_lame_t;

bg_lame_t * bg_lame_create(void);

void bg_lame_destroy(bg_lame_t *);

void bg_lame_set_parameter(bg_lame_t *,
                           const char * name,
                           const gavl_value_t * v);

gavl_audio_sink_t * bg_lame_open(bg_lame_t * lame,
                                 gavl_dictionary_t * s);

void bg_lame_set_packet_sink(bg_lame_t * lame,
                             gavl_packet_sink_t * sink);

/* Audio parameters */

static const bg_parameter_info_t audio_parameters[] =
  {
#ifdef USE_VBR
    {
      .name =        "bitrate_mode",
      .long_name =   TRS("Bitrate mode"),
      .type =        BG_PARAMETER_STRINGLIST,
      .val_default = GAVL_VALUE_INIT_STRING("CBR"),
      .multi_names = (char const *[]){ "CBR",
                              "ABR",
                              "VBR",
                              NULL },
      .multi_labels = (char const *[]){ TRS("Constant"),
                               TRS("Average"),
                               TRS("Variable"),
                               NULL },
    },
#endif // LAME_FILE
    {
      .name =        "stereo_mode",
      .long_name =   TRS("Stereo mode"),
      .type =        BG_PARAMETER_STRINGLIST,
      .val_default = GAVL_VALUE_INIT_STRING("Auto"),
      .multi_names = (char const *[]){ "Stereo",
                              "Joint stereo",
                              "Auto",
                              NULL },
      .multi_labels = (char const *[]){ TRS("Stereo"),
                               TRS("Joint stereo"),
                               TRS("Auto"),
                              NULL },
      .help_string = TRS("Stereo: Completely independent channels\n\
Joint stereo: Improve quality (save bits) by using similarities of the channels\n\
Auto (recommended): Select one of the above depending on quality or bitrate setting")
    },
    {
      .name =        "quality",
      .long_name =   TRS("Encoding speed"),
      .type =        BG_PARAMETER_SLIDER_INT,
      .val_min =     GAVL_VALUE_INIT_INT(0),
      .val_max =     GAVL_VALUE_INIT_INT(9),
      .val_default = GAVL_VALUE_INIT_INT(2),
      .help_string = TRS("0: Slowest encoding, best quality\n\
9: Fastest encoding, worst quality")
    },
    {
      .name =        "cbr_bitrate",
      .long_name =   TRS("Bitrate (kbps)"),
      .type =        BG_PARAMETER_INT,
      .val_min =     GAVL_VALUE_INIT_INT(8),
      .val_max =     GAVL_VALUE_INIT_INT(320),
      .val_default = GAVL_VALUE_INIT_INT(128),
      .help_string = TRS("Bitrate in kbps. If your selection is no \
valid mp3 bitrate, we'll choose the closest value.")
    },
#ifdef LAME_FILE
    {
      .name =        "vbr_quality",
      .long_name =   TRS("VBR Quality"),
      .type =        BG_PARAMETER_SLIDER_INT,
      .val_min =     GAVL_VALUE_INIT_INT(0),
      .val_max =     GAVL_VALUE_INIT_INT(9),
      .val_default = GAVL_VALUE_INIT_INT(4),
      .help_string = TRS("VBR Quality level. 0: best, 9: worst")
    },
    {
      .name =        "abr_bitrate",
      .long_name =   TRS("ABR overall bitrate (kbps)"),
      .type =        BG_PARAMETER_INT,
      .val_min =     GAVL_VALUE_INIT_INT(8),
      .val_max =     GAVL_VALUE_INIT_INT(320),
      .val_default = GAVL_VALUE_INIT_INT(128),
      .help_string = TRS("Average bitrate for ABR mode")
    },
    {
      .name =        "abr_min_bitrate",
      .long_name =   TRS("ABR min bitrate (kbps)"),
      .type =        BG_PARAMETER_INT,
      .val_min =     GAVL_VALUE_INIT_INT(0),
      .val_max =     GAVL_VALUE_INIT_INT(320),
      .val_default = GAVL_VALUE_INIT_INT(0),
      .help_string = TRS("Minimum bitrate for ABR mode. 0 means let lame decide. \
If your selection is no valid mp3 bitrate, we'll choose the closest value.")
    },
    {
      .name =        "abr_max_bitrate",
      .long_name =   TRS("ABR max bitrate (kbps)"),
      .type =        BG_PARAMETER_INT,
      .val_min =     GAVL_VALUE_INIT_INT(0),
      .val_max =     GAVL_VALUE_INIT_INT(320),
      .val_default = GAVL_VALUE_INIT_INT(0),
      .help_string = TRS("Maximum bitrate for ABR mode. 0 means let lame decide. \
If your selection is no valid mp3 bitrate, we'll choose the closest value.")
    },
#endif // LAME_FILE
    { /* End of parameters */ }
  };
