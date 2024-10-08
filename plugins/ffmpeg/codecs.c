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

#include "ffmpeg_common.h"
#include "params.h"
#include <gmerlin/utils.h>
#include <gmerlin/translation.h>
#include <gmerlin/log.h>
#include <libavutil/channel_layout.h>
#include <libavutil/opt.h>

#define LOG_DOMAIN "ffmpeg.codecs"

#define ENCODE_PARAM_MP2                 \
  {                                      \
    .name =      "ff_bit_rate_str",        \
    .long_name = TRS("Bit rate (kbps)"),        \
    .type =      BG_PARAMETER_STRINGLIST, \
    .val_default = GAVL_VALUE_INIT_STRING("128"),       \
    .multi_names = (char const *[]){ "32",  "48", "56", "64", "80", "96", "112", \
                   "128", "160", "192", "224", "256", "320", "384",\
                   NULL } \
  },

#define ENCODE_PARAM_MP3                 \
  {                                      \
    .name =      "ff_bit_rate_str",        \
    .long_name = TRS("Bit rate (kbps)"),        \
    .type =      BG_PARAMETER_STRINGLIST, \
    .val_default = GAVL_VALUE_INIT_STRING("128"),       \
    .multi_names = (char const *[]){ "32", "40", "48", "56", "64", "80", "96", \
                   "112", "128", "160", "192", "224", "256", "320",\
                   NULL } \
  },

#define ENCODE_PARAM_AC3 \
  {                                      \
    .name =      "ff_bit_rate_str",        \
    .long_name = TRS("Bit rate (kbps)"),        \
    .type =      BG_PARAMETER_STRINGLIST, \
    .val_default = GAVL_VALUE_INIT_STRING("128"),       \
    .multi_names = (char const *[]){ "32", "40", "48", "56", "64", "80", "96", "112", "128", \
                   "160", "192", "224", "256", "320", "384", "448", "512", \
                   "576", "640", NULL } \
  },

/*
 *  bit_rate can also be set to 1 (open), 2 (variable), 3 (lossless)
 */

#define ENCODE_PARAM_DTS                 \
  {                                      \
    .name =      "ff_bit_rate_str",        \
    .long_name = TRS("Bit rate (kbps)"),        \
    .type =      BG_PARAMETER_STRINGLIST, \
    .val_default = GAVL_VALUE_INIT_STRING("1536"),       \
    .multi_names = (char const *[]){ "32", "56", \
                                     "64", "96", "112", "128", \
                                     "192", "224", "256", "320", "384", \
                                     "448", "512", "576", "640", "768", \
                                     "896", "1024", "1152", "1280", "1344", \
                                     "1408", "1411200", "1472", "1536", "1920", \
                                     "2048", "3072", "3840", \
                                     NULL }                           \
  },

#define ENCODE_PARAM_WMA \
  {                                      \
    .name =      "ff_bit_rate_str",        \
    .long_name = TRS("Bit rate (kbps)"),        \
    .type =      BG_PARAMETER_STRINGLIST, \
    .val_default = GAVL_VALUE_INIT_STRING("128"),       \
    .multi_names = (char const *[]){ "24", "48", "64", "96", "128", NULL } \
  },

#if 0
static const bg_parameter_info_t parameters_libfaac[] =
  {
    {
      .name =      "faac_profile",
      .long_name = TRS("Profile"),
      .type =      BG_PARAMETER_STRINGLIST,
      .val_default = GAVL_VALUE_INIT_STRING("lc"),
      .multi_names = (char const *[]){"main",
                                      "lc",
                                      "ssr",
                                      "ltp",
                                      (char *)0},
      .multi_labels = (char const *[]){TRS("Main"),
                                       TRS("LC"),
                                       TRS("SSR"),
                                       TRS("LTP"),
                                       (char *)0 },
    },
    PARAM_BITRATE_AUDIO,
    {
      .name =        "faac_quality",
      .long_name =   TRS("Quality"),
      .type =        BG_PARAMETER_SLIDER_INT,
      .val_min =     GAVL_VALUE_INIT_INT(10),
      .val_max =     GAVL_VALUE_INIT_INT(500),
      .val_default = GAVL_VALUE_INIT_INT(100),
      .help_string = TRS("Quantizer quality"),
    },
    { /* */ }
  };
#endif

static const bg_parameter_info_t parameters_libvorbis[] =
  {
    PARAM_BITRATE_AUDIO,
    PARAM_RC_MIN_RATE,
    PARAM_RC_MAX_RATE,
    {
      .name =        "quality",
      .long_name =   TRS("Quality"),
      .type =        BG_PARAMETER_SLIDER_INT,
      .val_min =     GAVL_VALUE_INIT_INT(-1),
      .val_max =     GAVL_VALUE_INIT_INT(10),
      .val_default = GAVL_VALUE_INIT_INT(3),
      .help_string = TRS("Quantizer quality"),
    },
    { /* End */ },
  };
    
#define ENCODE_PARAM_VIDEO_RATECONTROL \
  {                                           \
    .name =      "rate_control",                       \
    .long_name = TRS("Rate control"),                     \
    .type =      BG_PARAMETER_SECTION,         \
  },                                        \
    PARAM_BITRATE_VIDEO,                    \
    PARAM_BITRATE_TOLERANCE,                \
    PARAM_RC_MIN_RATE,                      \
    PARAM_RC_MAX_RATE,                      \
    PARAM_RC_BUFFER_SIZE,                   \
    PARAM_RC_INITIAL_COMPLEX,               \
    PARAM_RC_INITIAL_BUFFER_OCCUPANCY

#define ENCODE_PARAM_VIDEO_QUANTIZER_I \
  {                                           \
    .name =      "quantizer",                       \
    .long_name = TRS("Quantizer"),                     \
    .type =      BG_PARAMETER_SECTION,         \
  },                                        \
    PARAM_QMIN,                             \
    PARAM_QMAX,                             \
    PARAM_MAX_QDIFF,                        \
    PARAM_FLAG_QSCALE,                      \
    PARAM_QSCALE,                      \
    PARAM_QCOMPRESS,                        \
    PARAM_QBLUR,                            \
    PARAM_TRELLIS

#define ENCODE_PARAM_VIDEO_QUANTIZER_IP \
  ENCODE_PARAM_VIDEO_QUANTIZER_I,               \
  PARAM_I_QUANT_FACTOR,                         \
  PARAM_I_QUANT_OFFSET

#define ENCODE_PARAM_VIDEO_QUANTIZER_IPB \
  ENCODE_PARAM_VIDEO_QUANTIZER_IP,        \
  PARAM_B_QUANT_FACTOR,                   \
  PARAM_B_QUANT_OFFSET

#define ENCODE_PARAM_VIDEO_FRAMETYPES_IP \
  {                                           \
    .name =      "frame_types",                       \
    .long_name = TRS("Frame types"),                     \
    .type =      BG_PARAMETER_SECTION,         \
  },                                        \
  PARAM_GOP_SIZE,                      \
  PARAM_SCENE_CHANGE_THRESHOLD,       \
  PARAM_FLAG_CLOSED_GOP,         \
  PARAM_FLAG2_STRICT_GOP

#define ENCODE_PARAM_VIDEO_FRAMETYPES_IPB \
  ENCODE_PARAM_VIDEO_FRAMETYPES_IP,        \
  PARAM_MAX_B_FRAMES,                 \
  PARAM_B_FRAME_STRATEGY

#define ENCODE_PARAM_VIDEO_ME \
  {                                           \
    .name =      "motion_estimation",                       \
    .long_name = TRS("Motion estimation"),                     \
    .type =      BG_PARAMETER_SECTION,         \
  },                                        \
    PARAM_ME_METHOD,                        \
    PARAM_ME_CMP,\
    PARAM_ME_CMP_CHROMA,\
    PARAM_ME_RANGE,\
    PARAM_ME_THRESHOLD,\
    PARAM_MB_DECISION,\
    PARAM_DIA_SIZE

#define ENCODE_PARAM_VIDEO_ME_PRE \
  {                                           \
    .name =      "motion_estimation",                       \
    .long_name = TRS("ME pre-pass"),                     \
    .type =      BG_PARAMETER_SECTION,         \
  },                                        \
    PARAM_PRE_ME,\
    PARAM_ME_PRE_CMP,\
    PARAM_ME_PRE_CMP_CHROMA,\
    PARAM_PRE_DIA_SIZE

#define ENCODE_PARAM_VIDEO_QPEL                 \
  {                                           \
    .name =      "qpel_motion_estimation",                       \
    .long_name = TRS("Qpel ME"),                     \
    .type =      BG_PARAMETER_SECTION,         \
  },                                        \
    PARAM_FLAG_QPEL, \
    PARAM_ME_SUB_CMP,\
    PARAM_ME_SUB_CMP_CHROMA,\
    PARAM_ME_SUBPEL_QUALITY

#define ENCODE_PARAM_VIDEO_MASKING \
  {                                \
    .name =      "masking",                       \
    .long_name = TRS("Masking"),                     \
    .type =      BG_PARAMETER_SECTION,         \
  },                                        \
    PARAM_LUMI_MASKING, \
    PARAM_DARK_MASKING, \
    PARAM_TEMPORAL_CPLX_MASKING, \
    PARAM_SPATIAL_CPLX_MASKING, \
    PARAM_BORDER_MASKING, \
    PARAM_P_MASKING

#define ENCODE_PARAM_VIDEO_MISC \
  {                                           \
    .name =      "misc",                       \
    .long_name = TRS("Misc"),                     \
    .type =      BG_PARAMETER_SECTION,         \
  },                                        \
    PARAM_STRICT_STANDARD_COMPLIANCE,       \
    PARAM_NOISE_REDUCTION, \
    PARAM_FLAG_GRAY, \
    PARAM_FLAG_BITEXACT

static const bg_parameter_info_t parameters_mpeg4[] = {
  ENCODE_PARAM_VIDEO_FRAMETYPES_IPB,
  PARAM_FLAG_AC_PRED_MPEG4,
  ENCODE_PARAM_VIDEO_RATECONTROL,
  ENCODE_PARAM_VIDEO_QUANTIZER_IPB,
  PARAM_FLAG_CBP_RD,
  ENCODE_PARAM_VIDEO_ME,
  PARAM_FLAG_4MV,
  PARAM_FLAG_QP_RD,
  ENCODE_PARAM_VIDEO_ME_PRE,
  ENCODE_PARAM_VIDEO_QPEL,
  ENCODE_PARAM_VIDEO_MASKING,
  ENCODE_PARAM_VIDEO_MISC,
  { /* End of parameters */ }
};

static const bg_parameter_info_t parameters_mjpeg[] = {
  ENCODE_PARAM_VIDEO_RATECONTROL,
  ENCODE_PARAM_VIDEO_QUANTIZER_I,
  ENCODE_PARAM_VIDEO_MISC,
  { /* End of parameters */ }
};

static const bg_parameter_info_t parameters_mpeg1[] = {
  ENCODE_PARAM_VIDEO_FRAMETYPES_IPB,
  ENCODE_PARAM_VIDEO_RATECONTROL,
  ENCODE_PARAM_VIDEO_QUANTIZER_IPB,
  ENCODE_PARAM_VIDEO_ME,
  ENCODE_PARAM_VIDEO_ME_PRE,
  ENCODE_PARAM_VIDEO_MASKING,
  ENCODE_PARAM_VIDEO_MISC,
  { /* End of parameters */ }
};


static const bg_parameter_info_t parameters_msmpeg4v3[] = {
  ENCODE_PARAM_VIDEO_FRAMETYPES_IP,
  ENCODE_PARAM_VIDEO_RATECONTROL,
  ENCODE_PARAM_VIDEO_QUANTIZER_IP,
  ENCODE_PARAM_VIDEO_ME,
  ENCODE_PARAM_VIDEO_ME_PRE,
  ENCODE_PARAM_VIDEO_MASKING,
  ENCODE_PARAM_VIDEO_MISC,
  { /* End of parameters */ }
};

static const bg_parameter_info_t parameters_libx264[] = {
  {
    .name =      "libx264_preset",
    .long_name = TRS("Preset"),
    .type =      BG_PARAMETER_STRINGLIST,
    .val_default = GAVL_VALUE_INIT_STRING("medium"),
    .multi_names = (char const *[]){"ultrafast",
                                    "superfast",
                                    "veryfast",
                                    "faster",
                                    "fast",
                                    "medium",
                                    "slow",
                                    "slower",
                                    "veryslow",
                                    "placebo",
                                    (char *)0},
    .multi_labels = (char const *[]){TRS("Ultrafast"),
                                     TRS("Superfast"),
                                     TRS("Veryfast"),
                                     TRS("Faster"),
                                     TRS("Fast"),
                                     TRS("Medium"),
                                     TRS("Slow"),
                                     TRS("Slower"),
                                     TRS("Veryslow"),
                                     TRS("Placebo"),
                                     (char *)0 },
  },
  {
    .name =      "libx264_tune",
    .long_name = TRS("Tune"),
    .type =      BG_PARAMETER_STRINGLIST,
    .val_default = GAVL_VALUE_INIT_STRING("$none"),
    .multi_names = (char const *[]){"$none",
                                    "film",
                                    "animation",
                                    "grain",
                                    "stillimage",
                                    "psnr",
                                    "ssim",
                                    "fastdecode",
                                    "zerolatency",
                                    (char *)0},
    .multi_labels = (char const *[]){TRS("None"),
                                     TRS("Film"),
                                     TRS("Animation"),
                                     TRS("Grain"),
                                     TRS("Still image"),
                                     TRS("PSNR"),
                                     TRS("SSIM"),
                                     TRS("Fast decode"),
                                     TRS("Zero latency"),
                                     (char *)0},
  },
  {
    .name =      "ff_bit_rate_video",
    .long_name = TRS("Bit rate (kbps)"),
    .type =      BG_PARAMETER_INT,
    .val_min     = GAVL_VALUE_INIT_INT(0),
    .val_max     = GAVL_VALUE_INIT_INT(10000),
    .val_default = GAVL_VALUE_INIT_INT(0),
    .help_string = TRS("If > 0 encode with average bitrate"),
  },
  {
    .name =      "libx264_crf",
    .long_name = TRS("Quality-based VBR"),
    .type =      BG_PARAMETER_SLIDER_FLOAT,
    .val_min     = GAVL_VALUE_INIT_FLOAT(-1.0),
    .val_max     = GAVL_VALUE_INIT_FLOAT(51.0),
    .val_default = GAVL_VALUE_INIT_FLOAT(23.0),
    .help_string = TRS("Negative means disable"),
    .num_digits  = 2,
  },
  {
    .name =      "libx264_qp",
    .long_name = TRS("Force constant QP"),
    .type =      BG_PARAMETER_SLIDER_INT,
    .val_min     = GAVL_VALUE_INIT_INT(-1),
    .val_max     = GAVL_VALUE_INIT_INT(69),
    .val_default = GAVL_VALUE_INIT_INT(-1),
    .help_string = TRS("Negative means disable, 0 means lossless"),
  },
  { /* End */ },
};

static const bg_parameter_info_t parameters_libvpx[] = {
  {
    .name = "rc",
    .long_name = TRS("Rate control"),
    .type = BG_PARAMETER_SECTION,
  },
  PARAM_BITRATE_VIDEO,
  PARAM_RC_MIN_RATE,
  PARAM_RC_MAX_RATE,
  PARAM_RC_BUFFER_SIZE,
  PARAM_RC_INITIAL_BUFFER_OCCUPANCY,
  {
    .name =        "ff_qcompress",
    .long_name =   TRS("2-Pass VBR/CBR"),
    .type =        BG_PARAMETER_SLIDER_FLOAT,
    .val_default = GAVL_VALUE_INIT_FLOAT(0.5),
    .val_min =     GAVL_VALUE_INIT_FLOAT(0.0),
    .val_max =     GAVL_VALUE_INIT_FLOAT(1.0),
    .num_digits =  2,
    .help_string = TRS("0: CBR, 1: VBR")
  },
  {
    .name =      "ff_qmin",
    .long_name = TRS("Minimum quantizer scale"),
    .type =      BG_PARAMETER_SLIDER_INT,
    .val_default = GAVL_VALUE_INIT_INT(2),
    .val_min =     GAVL_VALUE_INIT_INT(0),
    .val_max =     GAVL_VALUE_INIT_INT(63),
    .help_string = TRS("recommended value 0-4")
  },
  {
    .name =      "ff_qmax",
    .long_name = TRS("Maximum quantizer scale"),
    .type =      BG_PARAMETER_SLIDER_INT,
    .val_default = GAVL_VALUE_INIT_INT(55),
    .val_min =     GAVL_VALUE_INIT_INT(0),
    .val_max =     GAVL_VALUE_INIT_INT(63),
    .help_string = TRS("Must be larger than minimum, recommended value 50-63")
  },
  {
    .name =      "libvpx_crf",
    .long_name = TRS("Constant quality"),
    .type =      BG_PARAMETER_SLIDER_INT,
    .val_default = GAVL_VALUE_INIT_INT(10),
    .val_min =     GAVL_VALUE_INIT_INT(0),
    .val_max =     GAVL_VALUE_INIT_INT(63),
    .help_string = TRS("Set this to 0 for enabling VBR")
  },
  {
    .name = "speed",
    .long_name = TRS("Speed"),
    .type = BG_PARAMETER_SECTION,
  },
  {
    .name =      "libvpx_deadline",
    .long_name = TRS("Speed"),
    .type =      BG_PARAMETER_STRINGLIST,
    .val_default = GAVL_VALUE_INIT_STRING("good"),
    .multi_names = (char const *[]){"best",
                                    "good",
                                    "realtime",
                                    (char *)0},
    .multi_labels = (char const *[]){TRS("Best quality"),
                                     TRS("Good quality"),
                                     TRS("Realtime"),
                                     (char *)0},
  },
  {
    .name = "libvpx_cpu-used",
    .long_name = TRS("CPU usage modifier"),
    .type = BG_PARAMETER_INT,
    .val_min = GAVL_VALUE_INIT_INT(0),
    .val_max = GAVL_VALUE_INIT_INT(1000), // Bogus
    .val_default = GAVL_VALUE_INIT_INT(0),
  },
  PARAM_THREAD_COUNT,
  {
    .name = "frametypes",
    .long_name = TRS("Frame types"),
    .type = BG_PARAMETER_SECTION,
  },
#if 0 // Has no effect??
  {
    .name = "ff_keyint_min",
    .long_name = TRS("Minimum GOP size"),
    .type = BG_PARAMETER_INT,
    .val_min = GAVL_VALUE_INIT_INT(0),
    .val_max = GAVL_VALUE_INIT_INT(1000), // Bogus
    .val_default = GAVL_VALUE_INIT_INT(0),
  },
#endif
  {
    .name = "ff_gop_size",
    .long_name = TRS("Maximum GOP size"),
    .type = BG_PARAMETER_INT,
    .val_min = GAVL_VALUE_INIT_INT(-1),
    .val_max = GAVL_VALUE_INIT_INT(1000), // Bogus
    .val_default = GAVL_VALUE_INIT_INT(-1),
    .help_string = TRS("Maximum keyframe distance, -1 means automatic"),
  },
  {
    .name = "libvpx_auto-alt-ref",
    .long_name = TRS("Enable alternate reference frames"),
    .help_string = TRS("Enable use of alternate reference frames (2-pass only)"),
    .type = BG_PARAMETER_CHECKBUTTON,
  },
  {
    .name = "libvpx_lag-in-frames",
    .long_name = TRS("Lookahead"),
    .help_string = TRS("Number of frames to look ahead for alternate reference frame selection"),
    .type = BG_PARAMETER_INT,
    .val_min = GAVL_VALUE_INIT_INT(0),
    .val_max = GAVL_VALUE_INIT_INT(100),
    .val_default = GAVL_VALUE_INIT_INT(0),
  },
  {
    .name = "libvpx_arnr-maxframes",
    .long_name = TRS("Frame count for noise reduction"),
    .type = BG_PARAMETER_INT,
    .val_min = GAVL_VALUE_INIT_INT(0),
    .val_max = GAVL_VALUE_INIT_INT(100),
    .val_default = GAVL_VALUE_INIT_INT(0),
  },
  {
    .name =      "libvpx_arnr-type",
    .long_name = TRS("Noise reduction filter"),
    .type =      BG_PARAMETER_STRINGLIST,
    .val_default = GAVL_VALUE_INIT_STRING("backward"),
    .multi_names = (char const *[]){"$none",
                                    "backward",
                                    "forward",
                                    "centered",
                                    (char *)0},
    .multi_labels = (char const *[]){TRS("None"),
                                     TRS("Backward"),
                                     TRS("Forward"),
                                     TRS("Centered"),
                                     (char *)0},
  },
  { /* End */ },
};

static const bg_parameter_info_t parameters_tga[] = {
  {
    .name =      "tga_rle",
    .long_name = TRS("Use RLE compression"),
    .type =      BG_PARAMETER_CHECKBUTTON,
  },
  { /* */ }
};

/* Audio */

static const bg_parameter_info_t parameters_ac3[] = {
  ENCODE_PARAM_AC3
  { /* End of parameters */ }
};

static const bg_parameter_info_t parameters_dca[] = {
  ENCODE_PARAM_DTS
  { /* End of parameters */ }
};

static const bg_parameter_info_t parameters_mp2[] = {
  ENCODE_PARAM_MP2
  { /* End of parameters */ }
};

static const bg_parameter_info_t parameters_mp3[] = {
  ENCODE_PARAM_MP3
  { /* End of parameters */ }
};

static const bg_parameter_info_t parameters_wma[] = {
  ENCODE_PARAM_WMA
  { /* End of parameters */ }
};

static const ffmpeg_codec_info_t audio_codecs[] =
  {
    {
      .name      = "pcm_s16be",
      .long_name = TRS("16 bit PCM"),
      .id        = AV_CODEC_ID_PCM_S16BE,
    },
    {
      .name      = "pcm_s16le",
      .long_name = TRS("16 bit PCM"),
      .id        = AV_CODEC_ID_PCM_S16LE,
    },
    {
      .name      = "pcm_s8",
      .long_name = TRS("8 bit PCM"),
      .id        = AV_CODEC_ID_PCM_S8,
    },
    {
      .name      = "pcm_u8",
      .long_name = TRS("8 bit PCM"),
      .id        = AV_CODEC_ID_PCM_U8,
    },
    {
      .name      = "pcm_alaw",
      .long_name = TRS("alaw"),
      .id        = AV_CODEC_ID_PCM_ALAW,
    },
    {
      .name      = "pcm_mulaw",
      .long_name = TRS("mulaw"),
      .id        = AV_CODEC_ID_PCM_MULAW,
    },
    {
      .name      = "ac3",
      .long_name = TRS("AC3"),
      .id        = AV_CODEC_ID_AC3,
      .parameters = parameters_ac3,
    },
    {
      .name      = "mp2",
      .long_name = TRS("MPEG audio layer 2"),
      .id        = AV_CODEC_ID_MP2,
      .parameters = parameters_mp2,
    },
    {
      .name      = "dca",
      .long_name = TRS("DTS"),
      .id        = AV_CODEC_ID_DTS,
      .parameters = parameters_dca,
    },
    {
      .name      = "wma1",
      .long_name = TRS("Windows media Audio 1"),
      .id        = AV_CODEC_ID_WMAV1,
      .parameters = parameters_wma,
    },
    {
      .name      = "wma2",
      .long_name = TRS("Windows media Audio 2"),
      .id        = AV_CODEC_ID_WMAV2,
      .parameters = parameters_wma,
    },
    {
      .name      = "mp3",
      .long_name = TRS("MPEG audio layer 3"),
      .id        = AV_CODEC_ID_MP3,
      .parameters = parameters_mp3,
    },
    {
      .name       = "aac",
      .long_name  = TRS("AAC"),
      .id         = AV_CODEC_ID_AAC,
      //      .parameters = parameters_libfaac,
      //      .flags      = FLAG_EXTRADATA
    },
    {
      .name      = "libvorbis",
      .long_name = TRS("Vorbis"),
      .id        = AV_CODEC_ID_VORBIS,
      .parameters = parameters_libvorbis,
    },
    { /* End of array */ }
  };

static const ffmpeg_codec_info_t video_codecs[] =
  {
    {
      .name       = "mjpeg",
      .long_name  = TRS("Motion JPEG"),
      .id         = AV_CODEC_ID_MJPEG,
      .parameters = parameters_mjpeg,
      .flags      = FLAG_INTRA_ONLY,
    },
    {
      .name       = "mpeg4",
      .long_name  = TRS("MPEG-4"),
      .id         = AV_CODEC_ID_MPEG4,
      .parameters = parameters_mpeg4,
      .flags      = FLAG_B_FRAMES,
    },
    {
      .name       = "msmpeg4v3",
      .long_name  = TRS("Divx 3 compatible"),
      .id         = AV_CODEC_ID_MSMPEG4V3,
      .parameters = parameters_msmpeg4v3,
    },
    {
      .name       = "mpeg1video",
      .long_name  = TRS("MPEG-1 Video"),
      .id         = AV_CODEC_ID_MPEG1VIDEO,
      .parameters = parameters_mpeg1,
      .flags      = FLAG_CONSTANT_FRAMERATE|FLAG_B_FRAMES,
      .framerates = bg_ffmpeg_mpeg_framerates,
    },
    {
      .name       = "mpeg2video",
      .long_name  = TRS("MPEG-2 Video"),
      .id         = AV_CODEC_ID_MPEG2VIDEO,
      .parameters = parameters_mpeg1,
      .flags      = FLAG_CONSTANT_FRAMERATE|FLAG_B_FRAMES,
      .framerates = bg_ffmpeg_mpeg_framerates,
    },
    {
      .name       = "flv1",
      .long_name  = TRS("Flash 1"),
      .id         = AV_CODEC_ID_FLV1,
      .parameters = parameters_msmpeg4v3,
    },
    {
      .name       = "wmv1",
      .long_name  = TRS("WMV 1"),
      .id         = AV_CODEC_ID_WMV1,
      .parameters = parameters_msmpeg4v3,
    },
    {
      .name       = "rv10",
      .long_name  = TRS("Real Video 1"),
      .id         = AV_CODEC_ID_RV10,
      .parameters = parameters_msmpeg4v3,
    },
    {
      .name       = "libx264",
      .long_name  = TRS("H.264"),
      .id         = AV_CODEC_ID_H264,
      .parameters = parameters_libx264,
      .flags      = FLAG_B_FRAMES,
    },
    {
      .name       = "tga",
      .long_name  = TRS("Targa"),
      .id         = AV_CODEC_ID_TARGA,
      .parameters = parameters_tga,
      .flags      = FLAG_INTRA_ONLY,
    },
    {
      .name       = "libvpx",
      .long_name  = TRS("VP8"),
      .id         = AV_CODEC_ID_VP8,
      .parameters = parameters_libvpx,
      .flags      = 0,
    },
#if 0
    {
      .name       = "wmv2",
      .long_name  = TRS("WMV 2"),
      .id         = AV_CODEC_ID_WMV2,
      .parameters = parameters_msmpeg4v3
    },
#endif
    { /* End of array */ }
  };

static const ffmpeg_codec_info_t **
add_codec_info(const ffmpeg_codec_info_t ** info, enum AVCodecID id, int * num)
  {
  int i;
  /* Check if the codec id is already in the array */

  for(i = 0; i < *num; i++)
    {
    if(info[i]->id == id)
      return info;
    }

  info = realloc(info, ((*num)+1) * sizeof(*info));
  
  info[*num] = NULL;

  i = 0;
  while(audio_codecs[i].name)
    {
    if(audio_codecs[i].id == id)
      {
      info[*num] = &audio_codecs[i];
      break;
      }
    i++;
    }

  if(!info[*num])
    {
    i = 0;
    while(video_codecs[i].name)
      {
      if(video_codecs[i].id == id)
        {
        info[*num] = &video_codecs[i];
        break;
        }
      i++;
      }
    }
  (*num)++;
  return info;
  }


static const bg_parameter_info_t audio_parameters[] =
  {
    {
      .name      = "codec",
      .long_name = TRS("Codec"),
      .type      = BG_PARAMETER_MULTI_MENU,
    },
    { /* */ }
  };

static const bg_parameter_info_t video_parameters[] =
  {
    {
      .name      = "codec",
      .long_name = TRS("Codec"),
      .type      = BG_PARAMETER_MULTI_MENU,
    },
    BG_ENCODER_FRAMERATE_PARAMS,
    { /* */ }
  };

static void create_codec_parameter(bg_parameter_info_t * parameter_info,
                                   const ffmpeg_codec_info_t ** infos,
                                   int num_infos)
  {
  int i;
  parameter_info[0].multi_names_nc =
    calloc(num_infos+1, sizeof(*parameter_info[0].multi_names));
  parameter_info[0].multi_labels_nc =
    calloc(num_infos+1, sizeof(*parameter_info[0].multi_labels));
  parameter_info[0].multi_parameters_nc =
    calloc(num_infos+1, sizeof(*parameter_info[0].multi_parameters));

  for(i = 0; i < num_infos; i++)
    {
    parameter_info[0].multi_names_nc[i]  =
      gavl_strrep(parameter_info[0].multi_names_nc[i], infos[i]->name);

    parameter_info[0].multi_labels_nc[i] =
      gavl_strrep(parameter_info[0].multi_labels_nc[i], infos[i]->long_name);

    if(infos[i]->parameters)
      parameter_info[0].multi_parameters_nc[i] =
        bg_parameter_info_copy_array(infos[i]->parameters);
    }
  gavl_value_set_string(&parameter_info[0].val_default, infos[0]->name);
  bg_parameter_info_set_const_ptrs(&parameter_info[0]);
  }


bg_parameter_info_t * 
bg_ffmpeg_create_audio_parameters(const ffmpeg_format_info_t * format_info)
  {
  int j, num_infos = 0;
  bg_parameter_info_t * ret;
  const ffmpeg_codec_info_t ** infos = NULL;
  
  /* Create codec array */

  if(!format_info->audio_codecs)
    return NULL;
  
  j = 0;
  while(format_info->audio_codecs[j] != AV_CODEC_ID_NONE)
    {
    infos = add_codec_info(infos, format_info->audio_codecs[j],
                           &num_infos);
    j++;
    }
  

  if(!infos)
    return NULL;
  
  /* Create parameters */
  ret = bg_parameter_info_copy_array(audio_parameters);
  create_codec_parameter(&ret[0], infos, num_infos);
  free(infos);
  
  return ret;
  }

bg_parameter_info_t * 
bg_ffmpeg_create_video_parameters(const ffmpeg_format_info_t * format_info)
  {
  int j, num_infos = 0;
  bg_parameter_info_t * ret;
  const ffmpeg_codec_info_t ** infos = NULL;

  /* Create codec array */
  if(!format_info->video_codecs)
    return NULL;
  
  j = 0;
  while(format_info->video_codecs[j] != AV_CODEC_ID_NONE)
    {
    infos = add_codec_info(infos, format_info->video_codecs[j],
                           &num_infos);
    j++;
    }

  if(!infos)
    return NULL;
  
  /* Create parameters */

  /* Create parameters */
  ret = bg_parameter_info_copy_array(video_parameters);
  create_codec_parameter(&ret[0], infos, num_infos);
  free(infos);
  
  return ret;
  }

enum AVCodecID
bg_ffmpeg_find_audio_encoder(const ffmpeg_format_info_t * format,
                             const char * name)
  {
  int i = 0, found = 0;
  enum AVCodecID ret = AV_CODEC_ID_NONE;
  
  while(audio_codecs[i].name)
    {
    if(!strcmp(name, audio_codecs[i].name))
      {
      ret = audio_codecs[i].id;
      break;
      }
    i++;
    }
  if(!format)
    return ret;
  
  i = 0;
  while(format->audio_codecs[i] != AV_CODEC_ID_NONE)
    {
    if(format->audio_codecs[i] == ret)
      {
      found = 1;
      break;
      }
    i++;
    }

  if(!found)
    {
    gavl_log(GAVL_LOG_ERROR, LOG_DOMAIN,
           "Audio codec %s is not supported by %s",
           name, format->name);
    ret = AV_CODEC_ID_NONE;
    }
  
  return ret;
  }

enum AVCodecID
bg_ffmpeg_find_video_encoder(const ffmpeg_format_info_t * format,
                             const char * name)
  {
  int i = 0, found = 0;
  enum AVCodecID ret = AV_CODEC_ID_NONE;
  
  while(video_codecs[i].name)
    {
    if(!strcmp(name, video_codecs[i].name))
      {
      ret = video_codecs[i].id;
      break;
      }
    i++;
    }

  if(!format)
    return ret;
  
  i = 0;
  while(format->video_codecs[i] != AV_CODEC_ID_NONE)
    {
    if(format->video_codecs[i] == ret)
      {
      found = 1;
      break;
      }
    i++;
    }

  if(!found)
    {
    gavl_log(GAVL_LOG_ERROR, LOG_DOMAIN,
           "Video codec %s is not supported by %s",
           name, format->name);
    ret = AV_CODEC_ID_NONE;
    }
  
  return ret;
  }

static const ffmpeg_codec_info_t *
get_codec_info(const ffmpeg_codec_info_t * codecs, enum AVCodecID id) 
  {
  int i = 0;
  while(codecs[i].name)
    {
    if(id == codecs[i].id)
      return &codecs[i];
    i++;
    }
  return NULL;
  }

const ffmpeg_codec_info_t *
bg_ffmpeg_get_codec_info(enum AVCodecID id, int type)
  {
  if(type == AVMEDIA_TYPE_AUDIO)
    return get_codec_info(audio_codecs, id);
  else if(type == AVMEDIA_TYPE_VIDEO)
    return get_codec_info(video_codecs, id);
  return NULL;
  }


const bg_parameter_info_t *
bg_ffmpeg_get_codec_parameters(enum AVCodecID id, int type)
  {
  const ffmpeg_codec_info_t * ci = NULL;
  
  if(type == AVMEDIA_TYPE_AUDIO)
    ci = get_codec_info(audio_codecs, id);
  else if(type == AVMEDIA_TYPE_VIDEO)
    ci =  get_codec_info(video_codecs, id);
  
  if(!ci)
    return NULL;
  return ci->parameters;
  }

const char *
bg_ffmpeg_get_codec_name(enum AVCodecID id)
  {
  const ffmpeg_codec_info_t * info;
  if(!(info = get_codec_info(audio_codecs, id)) &&
     !(info = get_codec_info(video_codecs, id)))
    return NULL;
  return info->name;
  }

typedef struct
  {
  const char * s;
  int i;
  } enum_t;

#define PARAM_INT(n, var) \
  if(!strcmp(n, name)) \
    { \
    ctx->var = val->v.i;                         \
    }

#define PARAM_INT_SCALE(n, var, scale)   \
  if(!strcmp(n, name)) \
    { \
    ctx->var = val->v.i * scale;                 \
    }

#define PARAM_STR_INT_SCALE(n, var, scale) \
  if(!strcmp(n, name)) \
    { \
    ctx->var = atoi(val->v.str) * scale; \
    }


#define PARAM_QP2LAMBDA(n, var)   \
  if(!strcmp(n, name)) \
    { \
    ctx->var = (int)(val->v.d * FF_QP2LAMBDA+0.5);  \
    }
#define PARAM_FLOAT(n, var)   \
  if(!strcmp(n, name)) \
    { \
    ctx->var = val->v.d;                 \
    }

#define PARAM_CMP_CHROMA(n,var)              \
  {                                             \
  if(!strcmp(n, name))                    \
    {                                           \
    if(val->v.i)                            \
      ctx->var |= FF_CMP_CHROMA;                \
    else                                        \
      ctx->var &= ~FF_CMP_CHROMA;               \
                                                \
    }                                           \
  }

#define PARAM_FLAG(n,flag)                  \
  {                                             \
  if(!strcmp(n, name))                    \
    {                                           \
    if(val->v.i)                            \
      ctx->flags |= flag;                \
    else                                        \
      ctx->flags &= ~flag;               \
                                                \
    }                                           \
  }

#define PARAM_FLAG2(n,flag)                  \
  {                                             \
  if(!strcmp(n, name))                    \
    {                                           \
    if(val->v.i)                            \
      ctx->flags2 |= flag;                \
    else                                        \
      ctx->flags2 &= ~flag;               \
                                                \
    }                                           \
  }

static const enum_t compare_func[] =
  {
    { "SAD",  FF_CMP_SAD },
    { "SSE",  FF_CMP_SSE },
    { "SATD", FF_CMP_SATD },
    { "DCT",  FF_CMP_DCT },
    { "PSNR", FF_CMP_PSNR },
    { "BIT",  FF_CMP_BIT },
    { "RD",   FF_CMP_RD },
    { "ZERO", FF_CMP_ZERO },
    { "VSAD", FF_CMP_VSAD },
    { "VSSE", FF_CMP_VSSE },
    { "NSSE", FF_CMP_NSSE }
  };

static const enum_t mb_decision[] =
  {
    { "Use compare function", FF_MB_DECISION_SIMPLE },
    { "Fewest bits",          FF_MB_DECISION_BITS },
    { "Rate distoration",     FF_MB_DECISION_RD }
  };

static const enum_t faac_profile[] =
  {
    { "main", FF_PROFILE_AAC_MAIN },
    { "lc",   FF_PROFILE_AAC_LOW  },
    { "ssr",  FF_PROFILE_AAC_SSR  },
    { "ltp",  FF_PROFILE_AAC_LTP  }
  };

#define PARAM_ENUM(n, var, arr) \
  if(!strcmp(n, name)) \
    { \
    for(i = 0; i < sizeof(arr)/sizeof(arr[0]); i++) \
      {                                             \
      if(!strcmp(val->v.str, arr[i].s))       \
        {                                           \
        ctx->var = arr[i].i;                        \
        break;                                      \
        }                                           \
      }                                             \
    }

#define PARAM_DICT_STRING(n, ffmpeg_key) \
  if(!strcmp(n, name)) \
    { \
    if(val->v.str && (val->v.str[0] != '$')) \
      av_dict_set(options,                      \
                  ffmpeg_key, val->v.str, 0); \
    }

#define PARAM_DICT_FLOAT(n, ffmpeg_key) \
  if(!strcmp(n, name)) \
    { \
    char * str; \
    str = gavl_sprintf("%f", val->v.d); \
    av_dict_set(options, ffmpeg_key, str, 0); \
    free(str); \
    }

#define PARAM_DICT_INT(n, ffmpeg_key) \
  if(!strcmp(n, name)) \
    { \
    char * str; \
    str = gavl_sprintf("%d", val->v.i); \
    av_dict_set(options, ffmpeg_key, str, 0); \
    free(str); \
    }


void
bg_ffmpeg_set_codec_parameter(AVCodecContext * ctx,
                              AVDictionary ** options,
                              const char * name,
                              const gavl_value_t * val)
  {
  int i;
/*
 *   IMPORTANT: To keep the mess at a reasonable level,
 *   *all* parameters *must* appear in the same order as in
 *   the AVCocecContext structure, except the flags, which come at the very end
 */
  
  PARAM_INT_SCALE("ff_bit_rate_video",bit_rate,1000);
  PARAM_INT_SCALE("ff_bit_rate_audio",bit_rate,1000);
  
  PARAM_STR_INT_SCALE("ff_bit_rate_str", bit_rate, 1000);

  PARAM_INT_SCALE("ff_bit_rate_tolerance",bit_rate_tolerance,1000);
  PARAM_INT("ff_gop_size",gop_size);
  PARAM_FLOAT("ff_qcompress",qcompress);
  PARAM_FLOAT("ff_qblur",qblur);
  PARAM_INT("ff_qmin",qmin);
  PARAM_INT("ff_qmax",qmax);
  PARAM_INT("ff_max_qdiff",max_qdiff);
  PARAM_INT("ff_max_b_frames",max_b_frames);
  PARAM_FLOAT("ff_b_quant_factor",b_quant_factor);
  PARAM_INT("ff_strict_std_compliance",strict_std_compliance);
  PARAM_QP2LAMBDA("ff_b_quant_offset",b_quant_offset);
  PARAM_INT("ff_rc_min_rate",rc_min_rate);
  PARAM_INT("ff_rc_max_rate",rc_max_rate);
  PARAM_INT_SCALE("ff_rc_buffer_size",rc_buffer_size,1000);
  PARAM_FLOAT("ff_i_quant_factor",i_quant_factor);
  PARAM_QP2LAMBDA("ff_i_quant_offset",i_quant_offset);
  PARAM_FLOAT("ff_lumi_masking",lumi_masking);
  PARAM_FLOAT("ff_temporal_cplx_masking",temporal_cplx_masking);
  PARAM_FLOAT("ff_spatial_cplx_masking",spatial_cplx_masking);
  PARAM_FLOAT("ff_p_masking",p_masking);
  PARAM_FLOAT("ff_dark_masking",dark_masking);
  PARAM_ENUM("ff_me_cmp",me_cmp,compare_func);
  PARAM_CMP_CHROMA("ff_me_cmp_chroma",me_cmp);
  PARAM_ENUM("ff_me_sub_cmp",me_sub_cmp,compare_func);
  PARAM_CMP_CHROMA("ff_me_sub_cmp_chroma",me_sub_cmp);
  PARAM_ENUM("ff_mb_cmp",mb_cmp,compare_func);
  PARAM_CMP_CHROMA("ff_mb_cmp_chroma",mb_cmp);
  PARAM_ENUM("ff_ildct_cmp",ildct_cmp,compare_func);
  PARAM_CMP_CHROMA("ff_ildct_cmp_chroma",ildct_cmp);
  PARAM_INT("ff_dia_size",dia_size);
  PARAM_INT("ff_last_predictor_count",last_predictor_count);
  PARAM_ENUM("ff_me_pre_cmp",me_pre_cmp,compare_func);
  PARAM_CMP_CHROMA("ff_pre_me_cmp_chroma",me_pre_cmp);
  PARAM_INT("ff_pre_dia_size",pre_dia_size);
  PARAM_INT("ff_me_subpel_quality",me_subpel_quality);
  PARAM_INT("ff_me_range",me_range);
  PARAM_ENUM("ff_mb_decision",mb_decision,mb_decision);
  PARAM_INT_SCALE("ff_rc_initial_buffer_occupancy",rc_initial_buffer_occupancy,1000);
  PARAM_INT("ff_nsse_weight",nsse_weight);
  PARAM_QP2LAMBDA("ff_mb_lmin", mb_lmin);
  PARAM_QP2LAMBDA("ff_mb_lmax", mb_lmax);
  PARAM_INT("ff_bidir_refine",bidir_refine);
  PARAM_INT("ff_keyint_min",keyint_min);
  PARAM_FLAG("ff_flag_qscale",AV_CODEC_FLAG_QSCALE);
  PARAM_FLAG("ff_flag_4mv",AV_CODEC_FLAG_4MV);
  PARAM_FLAG("ff_flag_qpel",AV_CODEC_FLAG_QPEL);
  //  PARAM_FLAG("ff_flag_part",CODEC_FLAG_PART);
  PARAM_FLAG("ff_flag_gray",AV_CODEC_FLAG_GRAY);
  //  PARAM_FLAG("ff_flag_alt_scan",CODEC_FLAG_ALT_SCAN);
  PARAM_INT("ff_trellis",trellis);
  PARAM_FLAG("ff_flag_bitexact",AV_CODEC_FLAG_BITEXACT);
  PARAM_FLAG("ff_flag_ac_pred",AV_CODEC_FLAG_AC_PRED);
  //  PARAM_FLAG("ff_flag_h263p_umv",CODEC_FLAG_H263P_UMV);
  //  PARAM_FLAG("ff_flag_cbp_rd",CODEC_FLAG_CBP_RD);
  //  PARAM_FLAG("ff_flag_qp_rd",CODEC_FLAG_QP_RD);
  //  PARAM_FLAG("ff_flag_h263p_aiv",CODEC_FLAG_H263P_AIV);
  //  PARAM_FLAG("ffx_flag_obmc",CODEC_FLAG_OBMC);
  PARAM_FLAG("ff_flag_loop_filter",AV_CODEC_FLAG_LOOP_FILTER);
  //  PARAM_FLAG("ff_flag_h263p_slice_struct",CODEC_FLAG_H263P_SLICE_STRUCT);
  PARAM_FLAG("ff_flag_closed_gop",AV_CODEC_FLAG_CLOSED_GOP);
  PARAM_FLAG2("ff_flag2_fast",AV_CODEC_FLAG2_FAST);
  //  PARAM_FLAG2("ff_flag2_strict_gop",CODEC_FLAG2_STRICT_GOP);
  PARAM_INT("ff_thread_count",thread_count);
  
  PARAM_DICT_STRING("libx264_preset", "preset");
  PARAM_DICT_STRING("libx264_tune",   "tune");
  PARAM_DICT_FLOAT("libx264_crf", "crf");
  PARAM_DICT_FLOAT("libx264_qp", "qp");

  PARAM_ENUM("faac_profile", profile, faac_profile);

  if(!strcmp(name, "faac_quality"))
    ctx->global_quality = FF_QP2LAMBDA * val->v.i;
  else if(!strcmp(name, "vorbis_quality"))
    ctx->global_quality = FF_QP2LAMBDA * val->v.i;
  
  if(!strcmp(name, "tga_rle"))
    {
    av_opt_set_int(ctx->priv_data, "rle", !!(val->v.i), 0);
    }
  
  PARAM_DICT_STRING("libvpx_deadline", "deadline");
  PARAM_DICT_INT("libvpx_cpu-used",   "cpu-used");
  PARAM_DICT_INT("libvpx_auto-alt-ref", "alt-ref");
  PARAM_DICT_INT("libvpx_lag-in-frames", "lag-in-frames");
  PARAM_DICT_INT("libvpx_arnr-max-frames", "arnr-max-frames");
  PARAM_DICT_INT("libvpx_crf", "crf");
  PARAM_DICT_STRING("libvpx_arnr-type", "arnr-type");
  }

/* Type conversion */

const bg_encoder_framerate_t bg_ffmpeg_mpeg_framerates[] =
  {
    { 24000, 1001 },
    {    24,    1 },
    {    25,    1 },
    { 30000, 1001 },
    {    30,    1 },
    {    50,    1 },
    { 60000, 1001 },
    {    60,    1 },
    { /* End of framerates */ }
  };

#ifdef WORDS_BIGENDIAN
#define PIX_FMT_LE CONVERT_ENDIAN
#define PIX_FMT_BE 0
#else
#define PIX_FMT_LE 0
#define PIX_FMT_BE CONVERT_ENDIAN
#endif

static const struct
  {
  enum AVPixelFormat  ffmpeg_csp;
  gavl_pixelformat_t gavl_csp;
  int convert_flags;
  }
pixelformats[] =
  {
    { AV_PIX_FMT_YUV420P,  GAVL_YUV_420_P },  ///< Planar YUV 4:2:0 (1 Cr & Cb sample per 2x2 Y samples)
    { AV_PIX_FMT_YUYV422,  GAVL_YUY2      },
    { AV_PIX_FMT_YUV422P,  GAVL_YUV_422_P },  ///< Planar YUV 4:2:2 (1 Cr & Cb sample per 2x1 Y samples)
    { AV_PIX_FMT_YUV444P,  GAVL_YUV_444_P }, ///< Planar YUV 4:4:4 (1 Cr & Cb sample per 1x1 Y samples)
    { AV_PIX_FMT_YUV411P,  GAVL_YUV_411_P }, ///< Planar YUV 4:1:1 (1 Cr & Cb sample per 4x1 Y samples)
    { AV_PIX_FMT_YUVJ420P, GAVL_YUVJ_420_P }, ///< Planar YUV 4:2:0 full scale (jpeg)
    { AV_PIX_FMT_YUVJ422P, GAVL_YUVJ_422_P }, ///< Planar YUV 4:2:2 full scale (jpeg)
    { AV_PIX_FMT_YUVJ444P, GAVL_YUVJ_444_P }, ///< Planar YUV 4:4:4 full scale (jpeg)
    { AV_PIX_FMT_GRAY8,    GAVL_GRAY_8 },

    { AV_PIX_FMT_RGB565BE, GAVL_RGB_16, PIX_FMT_BE }, ///< packed RGB 5:6:5, 16bpp, (msb)   5R 6G 5B(lsb), big-endian
    { AV_PIX_FMT_RGB565LE, GAVL_RGB_16, PIX_FMT_LE }, ///< packed RGB 5:6:5, 16bpp, (msb)   5R 6G 5B(lsb), little-endian
    { AV_PIX_FMT_RGB555BE, GAVL_RGB_15, PIX_FMT_BE }, ///< packed RGB 5:5:5, 16bpp, (msb)1A 5R 5G 5B(lsb), big-endian, most significant bit to 0
    { AV_PIX_FMT_RGB555LE, GAVL_RGB_15, PIX_FMT_LE }, ///< packed RGB 5:5:5, 16bpp, (msb)1A 5R 5G 5B(lsb), little-endian, most significant bit to 0

    { AV_PIX_FMT_BGR565BE, GAVL_BGR_16, PIX_FMT_BE }, ///< packed BGR 5:6:5, 16bpp, (msb)   5B 6G 5R(lsb), big-endian
    { AV_PIX_FMT_BGR565LE, GAVL_BGR_16, PIX_FMT_LE }, ///< packed BGR 5:6:5, 16bpp, (msb)   5B 6G 5R(lsb), little-endian
    { AV_PIX_FMT_BGR555BE, GAVL_BGR_15, PIX_FMT_BE }, ///< packed BGR 5:5:5, 16bpp, (msb)1A 5B 5G 5R(lsb), big-endian, most significant bit to 1
    { AV_PIX_FMT_BGR555LE, GAVL_BGR_15, PIX_FMT_LE }, ///< packed BGR 5:5:5, 16bpp, (msb)1A 5B 5G 5R(lsb), little-endian, most significant bit to 1
    { AV_PIX_FMT_RGB24,    GAVL_RGB_24    },  ///< Packed pixel, 3 bytes per pixel, RGBRGB...
    { AV_PIX_FMT_BGR24,    GAVL_BGR_24    },  ///< Packed pixel, 3 bytes per pixel, BGRBGR...
    { AV_PIX_FMT_BGRA,     GAVL_RGBA_32, CONVERT_OTHER },
    
#if 0 // Not needed in the forseeable future    
#if LIBAVUTIL_VERSION_INT < (50<<16)
    { AV_PIX_FMT_RGBA32,        GAVL_RGBA_32   },  ///< Packed pixel, 4 bytes per pixel, BGRABGRA..., stored in cpu endianness
#else
    { AV_PIX_FMT_RGB32,         GAVL_RGBA_32   },  ///< Packed pixel, 4 bytes per pixel, BGRABGRA..., stored in cpu endianness
#endif
    { AV_PIX_FMT_YUV410P,       GAVL_YUV_410_P }, ///< Planar YUV 4:1:0 (1 Cr & Cb sample per 4x4 Y samples)
#endif // Not needed
};

static gavl_pixelformat_t bg_pixelformat_ffmpeg_2_gavl(enum AVPixelFormat p)
  {
  int i;
  for(i = 0; i < sizeof(pixelformats)/sizeof(pixelformats[0]); i++)
    {
    if(pixelformats[i].ffmpeg_csp == p)
      return pixelformats[i].gavl_csp;
    }
  return GAVL_PIXELFORMAT_NONE;
  }

static enum AVPixelFormat bg_pixelformat_gavl_2_ffmpeg(gavl_pixelformat_t p, int * do_convert,
                                                     const enum AVPixelFormat * supported)
  {
  int i, j;
  for(i = 0; i < sizeof(pixelformats)/sizeof(pixelformats[0]); i++)
    {
    if(pixelformats[i].gavl_csp == p)
      {
      j = 0;

      while(supported[j] != AV_PIX_FMT_NONE)
        {
        if(pixelformats[i].ffmpeg_csp == supported[j])
          {
          if(do_convert)
            *do_convert = pixelformats[i].convert_flags;
          return pixelformats[i].ffmpeg_csp;
          }
        j++;
        }
      }
    }
  return AV_PIX_FMT_NONE;
  }

void bg_ffmpeg_choose_pixelformat(const enum AVPixelFormat * supported,
                                  enum AVPixelFormat * ffmpeg_fmt,
                                  gavl_pixelformat_t * gavl_fmt, int * do_convert)
  {
  int i, num;
  gavl_pixelformat_t * gavl_fmts;

  /* Count pixelformats */
  i = 0;
  num = 0;

  while(supported[i] != AV_PIX_FMT_NONE)
    {
    if(bg_pixelformat_ffmpeg_2_gavl(supported[i]) != GAVL_PIXELFORMAT_NONE)
      num++;
    i++;
    }
  
  gavl_fmts = malloc((num+1) * sizeof(gavl_fmts));
  
  i = 0;
  num = 0;
  
  while(supported[i] != AV_PIX_FMT_NONE)
    {
    if((gavl_fmts[num] = bg_pixelformat_ffmpeg_2_gavl(supported[i])) != GAVL_PIXELFORMAT_NONE)
      num++;
    i++;
    }
  gavl_fmts[num] = GAVL_PIXELFORMAT_NONE;

  *gavl_fmt = gavl_pixelformat_get_best(*gavl_fmt, gavl_fmts, NULL);
  *ffmpeg_fmt = bg_pixelformat_gavl_2_ffmpeg(*gavl_fmt, do_convert, supported);
  free(gavl_fmts);
  }

static const struct
  {
  enum AVSampleFormat  ffmpeg_fmt;
  gavl_sample_format_t gavl_fmt;
  int planar;
  }
sampleformats[] =
  {
    { AV_SAMPLE_FMT_U8,   GAVL_SAMPLE_U8,     0 },
    { AV_SAMPLE_FMT_S16,  GAVL_SAMPLE_S16,    0 },    ///< signed 16 bits
    { AV_SAMPLE_FMT_S32,  GAVL_SAMPLE_S32,    0 },    ///< signed 32 bits
    { AV_SAMPLE_FMT_FLT,  GAVL_SAMPLE_FLOAT,  0 },  ///< float
    { AV_SAMPLE_FMT_DBL,  GAVL_SAMPLE_DOUBLE, 0 }, ///< double
    { AV_SAMPLE_FMT_U8P,  GAVL_SAMPLE_U8,     1 },
    { AV_SAMPLE_FMT_S16P, GAVL_SAMPLE_S16,    1 },    ///< signed 16 bits
    { AV_SAMPLE_FMT_S32P, GAVL_SAMPLE_S32,    1 },    ///< signed 32 bits
    { AV_SAMPLE_FMT_FLTP, GAVL_SAMPLE_FLOAT,  1 },  ///< float
    { AV_SAMPLE_FMT_DBLP, GAVL_SAMPLE_DOUBLE, 1 }, ///< double
  };

gavl_sample_format_t bg_sample_format_ffmpeg_2_gavl(enum AVSampleFormat p,
                                                    gavl_interleave_mode_t * il)
  {
  int i;
  for(i = 0; i < sizeof(sampleformats)/sizeof(sampleformats[0]); i++)
    {
    if(sampleformats[i].ffmpeg_fmt == p)
      {
      if(il)
        {
        if(sampleformats[i].planar)
          *il = GAVL_INTERLEAVE_NONE;
        else
          *il = GAVL_INTERLEAVE_ALL;
        }
      return sampleformats[i].gavl_fmt;
      }
    }
  return GAVL_SAMPLE_NONE;
  }

/* Compressed stream support */

static const struct
  {
  gavl_codec_id_t gavl;
  enum AVCodecID    ffmpeg;
  }
codec_ids[] =
  {
    /* Audio */
    { GAVL_CODEC_ID_ALAW,   AV_CODEC_ID_PCM_ALAW  }, //!< alaw 2:1
    { GAVL_CODEC_ID_ULAW,   AV_CODEC_ID_PCM_MULAW }, //!< mu-law 2:1
    { GAVL_CODEC_ID_MP2,    AV_CODEC_ID_MP2       }, //!< MPEG-1 audio layer II
    { GAVL_CODEC_ID_MP3,    AV_CODEC_ID_MP3       }, //!< MPEG-1/2 audio layer 3 CBR/VBR
    { GAVL_CODEC_ID_AC3,    AV_CODEC_ID_AC3       }, //!< AC3
    { GAVL_CODEC_ID_AAC,    AV_CODEC_ID_AAC       }, //!< AAC as stored in quicktime/mp4
    { GAVL_CODEC_ID_VORBIS, AV_CODEC_ID_VORBIS    }, //!< Vorbis (segmented extradata and packets)
    { GAVL_CODEC_ID_AAC,    AV_CODEC_ID_AAC       }, //!< AAC
    { GAVL_CODEC_ID_DTS,    AV_CODEC_ID_DTS       }, //!<
    
    /* Video */
    { GAVL_CODEC_ID_JPEG,      AV_CODEC_ID_MJPEG      }, //!< JPEG image
    { GAVL_CODEC_ID_PNG,       AV_CODEC_ID_PNG        }, //!< PNG image
    { GAVL_CODEC_ID_TIFF,      AV_CODEC_ID_TIFF       }, //!< TIFF image
    { GAVL_CODEC_ID_TGA,       AV_CODEC_ID_TARGA      }, //!< TGA image
    { GAVL_CODEC_ID_MPEG1,     AV_CODEC_ID_MPEG1VIDEO }, //!< MPEG-1 video
    { GAVL_CODEC_ID_MPEG2,     AV_CODEC_ID_MPEG2VIDEO }, //!< MPEG-2 video
    { GAVL_CODEC_ID_MPEG4_ASP, AV_CODEC_ID_MPEG4      }, //!< MPEG-4 ASP (a.k.a. Divx4)
    { GAVL_CODEC_ID_H264,      AV_CODEC_ID_H264       }, //!< H.264 (Annex B)
    { GAVL_CODEC_ID_THEORA,    AV_CODEC_ID_THEORA     }, //!< Theora (segmented extradata
    { GAVL_CODEC_ID_DIRAC,     AV_CODEC_ID_DIRAC      }, //!< Complete DIRAC frames, sequence end code appended to last packet
    { GAVL_CODEC_ID_DV,        AV_CODEC_ID_DVVIDEO    }, //!< DV (several variants)
    { GAVL_CODEC_ID_VP8,       AV_CODEC_ID_VP8        }, //!< VP8 (as in webm)
    { GAVL_CODEC_ID_DIV3,      AV_CODEC_ID_MSMPEG4V3  }, //!< Old style Divx
    { GAVL_CODEC_ID_NONE,      AV_CODEC_ID_NONE       },
  };

enum AVCodecID bg_codec_id_gavl_2_ffmpeg(gavl_codec_id_t gavl)
  {
  int i = 0;
  while(codec_ids[i].gavl != GAVL_CODEC_ID_NONE)
    {
    if(codec_ids[i].gavl == gavl)
      return codec_ids[i].ffmpeg;
    i++;
    }
  return AV_CODEC_ID_NONE;
  }

gavl_codec_id_t bg_codec_id_ffmpeg_2_gavl(enum AVCodecID ffmpeg)
  {
  int i = 0;
  while(codec_ids[i].gavl != GAVL_CODEC_ID_NONE)
    {
    if(codec_ids[i].ffmpeg == ffmpeg)
      return codec_ids[i].gavl;
    i++;
    }
  return GAVL_CODEC_ID_NONE;
  }

/* Channel layout */

struct
  {
  gavl_channel_id_t gavl_id;
  uint64_t    ffmpeg_id;
  }
channel_ids[] =
  {
    { GAVL_CHID_FRONT_CENTER,       AV_CH_FRONT_CENTER },
    { GAVL_CHID_FRONT_LEFT,         AV_CH_FRONT_LEFT     },
    { GAVL_CHID_FRONT_RIGHT,        AV_CH_FRONT_RIGHT   },
    { GAVL_CHID_FRONT_CENTER_LEFT,  AV_CH_FRONT_LEFT_OF_CENTER },
    { GAVL_CHID_FRONT_CENTER_RIGHT, AV_CH_FRONT_RIGHT_OF_CENTER },
    { GAVL_CHID_REAR_LEFT,          AV_CH_BACK_LEFT },
    { GAVL_CHID_REAR_RIGHT,         AV_CH_BACK_RIGHT },
    { GAVL_CHID_REAR_CENTER,        AV_CH_BACK_CENTER },
    { GAVL_CHID_SIDE_LEFT,          AV_CH_SIDE_LEFT },
    { GAVL_CHID_SIDE_RIGHT,         AV_CH_SIDE_RIGHT },
    { GAVL_CHID_LFE,                AV_CH_LOW_FREQUENCY },
    //    { GAVL_CHID_AUX, 0 }
    { GAVL_CHID_NONE, 0 },
  };

static uint64_t chid_gavl_2_ffmpeg(gavl_channel_id_t gavl_id)
  {
  int i = 0;

  while(channel_ids[i].gavl_id != GAVL_CHID_NONE)
    {
    if(channel_ids[i].gavl_id == gavl_id)
      return channel_ids[i].ffmpeg_id;
    i++;
    }
  return 0;
  }

static gavl_channel_id_t chid_ffmpeg_2_gavl(uint64_t ffmpeg_id)
  {
  int i = 0;

  while(channel_ids[i].gavl_id != GAVL_CHID_NONE)
    {
    if(channel_ids[i].ffmpeg_id == ffmpeg_id)
      return channel_ids[i].gavl_id;
    i++;
    }
  return GAVL_CHID_NONE;
  }

uint64_t
bg_ffmpeg_get_channel_mask(gavl_audio_format_t * format)
  {
  int i, idx;
  uint64_t mask = 1;
  uint64_t ret = 0;

  for(i = 0; i < format->num_channels; i++)
    ret |= chid_gavl_2_ffmpeg(format->channel_locations[i]);
  
  idx = 0;
  for(i = 0; i < 64; i++)
    {
    if(ret & mask)
      {
      format->channel_locations[idx] = chid_ffmpeg_2_gavl(mask);
      idx++;
      }
    mask <<= 1;
    }

  return ret;
  }

