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
#include <config.h>
#include <gmerlin/plugin.h>
#include <gmerlin/translation.h>


#include "ogg_common.h"


extern const bg_ogg_codec_t bg_opus_codec;

#define IS_AUDIO
#define CODEC_NAME "c_opusenc"
#define CODEC_LONG_NAME TRS("Opus")
#define CODEC bg_opus_codec
#define COMPRESSION GAVL_CODEC_ID_OPUS

#define CODEC_DESC TRS("Libopus based encoder")

#include "_codec_plugin.c"
