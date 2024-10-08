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



/*
 *   IMPORTANT: To keep the mess at a reasonable level,
 *   *all* parameters *must* appear in the same order as in
 *   the AVCocecContext structure, except the flags, which come at the very end
 */


/** Rate control */
#define PARAM_BITRATE_AUDIO                     \
  { \
    .name =      "ff_bit_rate_audio",                 \
    .long_name = TRS("Bit rate (kbps)"),                \
    .type =      BG_PARAMETER_INT,                \
    .val_default = GAVL_VALUE_INIT_INT(0),               \
  }

/** Rate control */
#define PARAM_BITRATE_VIDEO \
  { \
    .name =      "ff_bit_rate_video",                 \
    .long_name = TRS("Bit rate (kbps)"),                \
    .type =      BG_PARAMETER_INT,                \
    .val_default = GAVL_VALUE_INIT_INT(800),               \
  }

/** Rate control */
#define PARAM_BITRATE_TOLERANCE                 \
  {                                               \
    .name =        "ff_bit_rate_tolerance",           \
    .long_name =   TRS("Bitrate tolerance (kbps)"),             \
    .type =        BG_PARAMETER_INT,             \
    .val_default = GAVL_VALUE_INIT_INT(8000),           \
    .help_string = TRS("Number of bits the bitstream is allowed to diverge from the reference.\
 Unused for constant quantizer encoding") \
  }

/** Motion estimation */
#define PARAM_ME_METHOD                         \
  {\
    .name =      "ff_me_method",\
    .long_name = TRS("Motion estimation method"),\
    .type =      BG_PARAMETER_STRINGLIST,\
    .val_default = GAVL_VALUE_INIT_STRING("Zero"),                      \
    .multi_names = (char const *[]){"Zero", "Phods", "Log", "X1", "Epzs", "Full", (char *)0}, \
    .multi_labels = (char const *[]){TRS("Zero"), TRS("Phods"), TRS("Log"), \
TRS("X1"), TRS("Epzs"), TRS("Full"), (char *)0} \
  }

/** Frame types */
#define PARAM_GOP_SIZE                          \
  { \
    .name =      "gop_size", \
    .long_name = TRS("GOP size (0 = intra only)"), \
    .type =      BG_PARAMETER_SLIDER_INT, \
    .val_default = GAVL_VALUE_INIT_INT(250),   \
    .val_min =     GAVL_VALUE_INIT_INT(0),     \
    .val_max =     GAVL_VALUE_INIT_INT(300),   \
  } \

/** Quantizer */
#define PARAM_QCOMPRESS \
  {                     \
    .name =        "ff_qcompress",                   \
    .long_name =   TRS("Quantizer compression"),\
    .type =        BG_PARAMETER_SLIDER_FLOAT,  \
    .val_default = GAVL_VALUE_INIT_FLOAT(0.5), \
    .val_min =     GAVL_VALUE_INIT_FLOAT(0.0), \
    .val_max =     GAVL_VALUE_INIT_FLOAT(1.0), \
    .num_digits =  2, \
    .help_string = TRS("Amount of qscale change between easy & hard scenes") \
  }

/** Quantizer */
#define PARAM_QBLUR \
  {                 \
    .name =        "ff_qblur",                     \
    .long_name =   TRS("Quantizer blur"), \
    .type =        BG_PARAMETER_SLIDER_FLOAT,  \
    .val_default = GAVL_VALUE_INIT_FLOAT(0.0), \
    .val_min =     GAVL_VALUE_INIT_FLOAT(0.0), \
    .val_max =     GAVL_VALUE_INIT_FLOAT(1.0), \
    .num_digits =  2,                  \
    .help_string = TRS("Amount of qscale smoothing over time"),        \
  }

/** Quantizer */
#define PARAM_QMIN \
  {\
    .name =      "ff_qmin", \
    .long_name = TRS("Minimum quantizer scale"), \
    .type =      BG_PARAMETER_SLIDER_INT, \
    .val_default = GAVL_VALUE_INIT_INT(2), \
    .val_min =     GAVL_VALUE_INIT_INT(0), \
    .val_max =     GAVL_VALUE_INIT_INT(31),               \
  }

/** Quantizer */
#define PARAM_QMAX                              \
  {\
    .name =      "ff_qmax", \
    .long_name = TRS("Maximum quantizer scale"), \
    .type =      BG_PARAMETER_SLIDER_INT, \
    .val_default = GAVL_VALUE_INIT_INT(31), \
    .val_min =     GAVL_VALUE_INIT_INT(0), \
    .val_max =     GAVL_VALUE_INIT_INT(31),               \
  } \

/** Quantizer */
#define PARAM_MAX_QDIFF \
  {                     \
    .name =      "ff_max_qdiff",                                  \
    .long_name = TRS("Maximum quantizer difference"),                  \
    .type =      BG_PARAMETER_SLIDER_INT,                               \
    .val_default = GAVL_VALUE_INIT_INT(3),                                \
    .val_min =     GAVL_VALUE_INIT_INT(0),                                \
    .val_max =     GAVL_VALUE_INIT_INT(31),                               \
    .help_string = TRS("Maximum quantizer difference between frames")  \
  }

/** Frame types */
#define PARAM_MAX_B_FRAMES \
  {                                                                     \
    .name =      "ff_max_b_frames",                                       \
    .long_name = TRS("Max B-Frames"),                                          \
    .type =      BG_PARAMETER_SLIDER_INT,                                       \
    .val_default = GAVL_VALUE_INIT_INT(0),                                        \
    .val_min =     GAVL_VALUE_INIT_INT(0),                                        \
    .help_string = TRS("Maximum number of B-frames between non B-frames") \
  }

/** Quantizer */
#define PARAM_B_QUANT_FACTOR \
  {                                                                     \
    .name =      "ff_b_quant_factor",                                     \
    .long_name = TRS("B quantizer factor"),                                    \
    .type =        BG_PARAMETER_SLIDER_FLOAT,                                   \
    .val_default = GAVL_VALUE_INIT_FLOAT(1.25),                                 \
    .val_min =     GAVL_VALUE_INIT_FLOAT(-31.0),                        \
    .val_max =     GAVL_VALUE_INIT_FLOAT(31.0),                                 \
    .num_digits =  2, \
    .help_string = TRS("Quantizer factor between B-frames and non-B-frames"),  \
  }

/** Frame types */
#define PARAM_B_FRAME_STRATEGY \
  {                                                             \
    .name =        "ff_b_frame_strategy",                         \
    .long_name =   TRS("Avoid B-frames in high motion scenes"),        \
    .type =        BG_PARAMETER_CHECKBUTTON,                             \
    .val_default = GAVL_VALUE_INIT_INT(0),                                \
  }

#define PARAM_STRICT_STANDARD_COMPLIANCE \
  {                                                              \
    .name = "ff_strict_std_compliance",                              \
    .long_name =   TRS("Standards compliance"),                         \
    .type =        BG_PARAMETER_SLIDER_INT,                              \
    .val_default = GAVL_VALUE_INIT_INT(0),                                \
    .val_min =     GAVL_VALUE_INIT_INT(-2),                             \
    .val_max =     GAVL_VALUE_INIT_INT(2),                                \
    .help_string = TRS("2: Strictly conform to a older more strict version\
 of the spec or reference software\n\
1: Strictly conform to all the things in the spec no matter what \
consequences\n\
0: Default\n\
-1: Allow unofficial extensions\n\
-2: Allow non standarized experimental things") \
  }

/** Quantizer */
#define PARAM_B_QUANT_OFFSET \
  { \
    .name =       "ff_b_quant_offset",                    \
    .long_name =  TRS("B quantizer offset"),                   \
    .type =       BG_PARAMETER_SLIDER_FLOAT,                    \
    .val_default = GAVL_VALUE_INIT_FLOAT(1.25),                   \
    .val_min =     GAVL_VALUE_INIT_FLOAT(0.0),                    \
    .val_max =     GAVL_VALUE_INIT_FLOAT(31.0),                   \
    .num_digits =  2, \
    .help_string = TRS("Quantizer offset between B-frames and non-B-frames\n"\
    "if > 0 then the last p frame quantizer will be used (q= lastp_q*factor+offset)\n"\
    "if < 0 then normal ratecontrol will be done (q= -normal_q*factor+offset)")\
  }

/** Rate control */
#define PARAM_RC_MIN_RATE \
  { \
    .name =        "ff_rc_min_rate",\
    .long_name =   TRS("Minimum bitrate (kbps)"),\
    .type =        BG_PARAMETER_INT,\
    .val_default = GAVL_VALUE_INIT_INT(0),                        \
    .help_string = TRS("Minimum bitrate (0 means arbitrary)"), \
  }

/** Rate control */
#define PARAM_RC_MAX_RATE \
  {                                             \
    .name =      "ff_rc_max_rate",\
    .long_name = TRS("Maximum bitrate (kbps)"),\
    .type =      BG_PARAMETER_INT,\
    .val_default = GAVL_VALUE_INIT_INT(0),                        \
    .help_string = TRS("Maximum bitrate (0 means arbitrary)"), \
  }

/** Rate control */
#define PARAM_RC_BUFFER_SIZE \
  {                          \
    .name =      "ff_rc_buffer_size",        \
    .long_name = TRS("RC buffer size"),           \
    .type =      BG_PARAMETER_INT,          \
    .val_default = GAVL_VALUE_INIT_INT(0),           \
    .help_string = TRS("Decoder bitstream buffer size in kbits. When encoding " \
"with max and/or min bitrate, this must be specified.") \
  }

/** Rate control */
#define PARAM_RC_BUFFER_AGGRESSIVITY \
  {                                           \
    .name =       "ff_rc_buffer_aggressivity",    \
    .long_name =  TRS("RC buffer aggressivity"),               \
    .type =       BG_PARAMETER_SLIDER_FLOAT,                    \
    .val_default = GAVL_VALUE_INIT_FLOAT(1.0),                   \
    .val_min =     GAVL_VALUE_INIT_FLOAT(0.01),                    \
    .val_max =     GAVL_VALUE_INIT_FLOAT(99.0),                   \
    .num_digits =  2, \
  }

/** Quantizer */
#define PARAM_I_QUANT_FACTOR \
  {                                                                     \
    .name =      "ff_i_quant_factor",                                     \
    .long_name = TRS("I quantizer factor"),                                    \
    .type =        BG_PARAMETER_SLIDER_FLOAT,                                   \
    .val_default = GAVL_VALUE_INIT_FLOAT(-0.8),                         \
    .val_min =     GAVL_VALUE_INIT_FLOAT(-31.0),                        \
    .val_max =     GAVL_VALUE_INIT_FLOAT(31.0),                                 \
    .num_digits =  1, \
    .help_string = TRS("Quantizer factor between P-frames and I-frames.\n"\
"If > 0 then the last P frame quantizer will be used (q= lastp_q*factor+offset).\n"\
"If < 0 then normal ratecontrol will be done (q= -normal_q*factor+offset)"),  \
  }

/** Quantizer */
#define PARAM_I_QUANT_OFFSET \
  {                                                              \
    .name =      "ff_i_quant_offset",                              \
    .long_name = TRS("I quantizer offset"),                             \
    .type =        BG_PARAMETER_SLIDER_FLOAT,                            \
    .val_default = GAVL_VALUE_INIT_FLOAT(0.0),                            \
    .val_min =     GAVL_VALUE_INIT_FLOAT(0.0),                            \
    .val_max =     GAVL_VALUE_INIT_FLOAT(31.0),                                 \
    .num_digits =  1, \
    .help_string = TRS("Quantizer offset between P-frames and I-frames"),  \
  }

/** Rate control */
#define PARAM_RC_INITIAL_COMPLEX \
  {                                                          \
    .name =      "ff_rc_initial_cplx",                           \
    .long_name = TRS("Initial RC complexity"),                      \
    .type =      BG_PARAMETER_SLIDER_FLOAT,\
    .val_default = GAVL_VALUE_INIT_FLOAT(0.0),                            \
    .val_min =     GAVL_VALUE_INIT_FLOAT(0.0),                            \
    .val_max =     GAVL_VALUE_INIT_FLOAT(99.0),                                 \
    .num_digits =  1, \
  }

/** Masking */
#define PARAM_LUMI_MASKING                      \
  {                                             \
    .name =        "ff_lumi_masking",               \
    .long_name = TRS("Luminance masking"),      \
    .type =      BG_PARAMETER_SLIDER_FLOAT,                              \
    .val_default = GAVL_VALUE_INIT_FLOAT(0.0),                            \
    .val_min =     GAVL_VALUE_INIT_FLOAT(0.0),                            \
    .val_max =     GAVL_VALUE_INIT_FLOAT(1.0),                          \
    .num_digits =  2, \
    .help_string = TRS("Encode very bright image parts with reduced quality."\
    " 0 means disabled, 0-0.3 is a sane range."),      \
  }

/** Masking */
#define PARAM_TEMPORAL_CPLX_MASKING                      \
  {                                             \
    .name =        "ff_temporal_cplx_masking",               \
    .long_name = TRS("Temporary complexity masking"),      \
    .type =      BG_PARAMETER_SLIDER_FLOAT,                              \
    .val_default = GAVL_VALUE_INIT_FLOAT(0.0),                            \
    .val_min =     GAVL_VALUE_INIT_FLOAT(0.0),                            \
    .val_max =     GAVL_VALUE_INIT_FLOAT(1.0),                          \
    .num_digits =  2, \
    .help_string = TRS("Encode very fast moving image parts with reduced quality."\
    " 0 means disabled."),      \
  }

/** Masking */
#define PARAM_SPATIAL_CPLX_MASKING             \
  {                                             \
    .name =        "ff_spatial_cplx_masking",               \
    .long_name = TRS("Spatial complexity masking"),      \
    .type =      BG_PARAMETER_SLIDER_FLOAT,                              \
    .val_default = GAVL_VALUE_INIT_FLOAT(0.0),                            \
    .val_min =     GAVL_VALUE_INIT_FLOAT(0.0),                            \
    .val_max =     GAVL_VALUE_INIT_FLOAT(1.0),                          \
    .num_digits =  2, \
    .help_string = TRS("Encode very complex image parts with reduced quality."\
    " 0 means disabled, 0-0.5 is a sane range."),                          \
  }

/** Masking */
#define PARAM_P_MASKING             \
  {                                             \
    .name =        "ff_p_masking",               \
    .long_name = TRS("Inter block masking"),      \
    .type =      BG_PARAMETER_SLIDER_FLOAT,                              \
    .val_default = GAVL_VALUE_INIT_FLOAT(0.0),                            \
    .val_min =     GAVL_VALUE_INIT_FLOAT(0.0),                            \
    .val_max =     GAVL_VALUE_INIT_FLOAT(1.0),                          \
    .num_digits =  2, \
    .help_string = TRS("Encode inter blocks with reduced quality (increases the quality of intra blocks). "\
    " 0 means disabled, 1 will double the bits allocated for intra blocks."),      \
  }

/** Masking */
#define PARAM_DARK_MASKING             \
  {                                             \
    .name =        "ff_dark_masking",               \
    .long_name = TRS("Darkness masking"),      \
    .type =      BG_PARAMETER_SLIDER_FLOAT,                              \
    .val_default = GAVL_VALUE_INIT_FLOAT(0.0),                            \
    .val_min =     GAVL_VALUE_INIT_FLOAT(0.0),                            \
    .val_max =     GAVL_VALUE_INIT_FLOAT(1.0),                            \
    .num_digits =  2, \
    .help_string = TRS("Encode very dark image parts with reduced quality. " \
    "0 means disabled, 0-0.3 is a sane range."),                          \
  }

#define PARAM_PREDICTION_METHOD                 \
  {                                               \
    .name =       "ff_prediction_method",             \
    .long_name = TRS("Precition method"),                \
    .type =      BG_PARAMETER_STRINGLIST,          \
    .val_default = GAVL_VALUE_INIT_STRING("Left"),\
multi_names: (char const *[]){ "Left", "Plane", "Median", (char*)0 },    \
multi_labels: (char const *[]){ TRS("Left"), TRS("Plane"), TRS("Median"), (char*)0 },    \
  }

/** Quantizer */
#define PARAM_MB_QMIN \
  {                   \
  .name =      "ff_mb_qmin", \
    .long_name = TRS("Minimum MB quantizer"),          \
    .type =    BG_PARAMETER_SLIDER_INT,                 \
    .val_default = GAVL_VALUE_INIT_INT(2),\
    .val_min =     GAVL_VALUE_INIT_INT(0),\
    .val_max =     GAVL_VALUE_INIT_INT(31),\
  }

/** Quantizer */
#define PARAM_MB_QMAX \
  {                   \
  .name =      "ff_mb_qmax", \
    .long_name = TRS("Maximum MB quantizer"),          \
    .type =    BG_PARAMETER_SLIDER_INT,                 \
    .val_default = GAVL_VALUE_INIT_INT(31),\
    .val_min =     GAVL_VALUE_INIT_INT(0),\
    .val_max =     GAVL_VALUE_INIT_INT(31),\
  }

#define COMPARE_FUNCS (char const *[]){ "SAD", "SSE", "SATD", "DCT", "PSNR", \
                        "BIT", "RD", "ZERO", "VSAD", "VSSE", "NSSE", (char*)0 }

#define COMPARE_FUNCS_HELP TRS("SAD: Sum of absolute differences\n"\
"SSE: Sum of squared errors\n"\
"SATD: Sum of absolute Hadamard transformed differences\n"\
"DCT: Sum of absolute DCT transformed differences\n"\
"PSNR: Sum of squared quantization errors (low quality)\n"\
"BIT: Number of bits needed for the block\n"\
"RD: Rate distortion optimal (slow)\n"\
"ZERO: 0\n"\
"VSAD: Sum of absolute vertical differences\n"\
"VSSE: Sum of squared vertical differences\n"\
"NSSE: Noise preserving sum of squared differences")

/** Motion estimation */
#define PARAM_ME_CMP \
  {\
    .name = "ff_me_cmp",                          \
    .long_name = "ME compare function",           \
    .type =  BG_PARAMETER_STRINGLIST,                \
    .val_default = GAVL_VALUE_INIT_STRING("SAD"),         \
    .multi_names = COMPARE_FUNCS,          \
    .help_string = TRS("Motion estimation compare function.") COMPARE_FUNCS_HELP \
  }

/** Motion estimation */
#define PARAM_ME_CMP_CHROMA \
  { \
    .name = "ff_me_cmp_chroma",                 \
    .long_name = TRS("Enable chroma ME compare"),    \
    .type = BG_PARAMETER_CHECKBUTTON,                  \
    .val_default = GAVL_VALUE_INIT_INT(0),              \
  }

/** Motion estimation */
#define PARAM_ME_SUB_CMP                        \
  {\
    .name = "ff_me_sub_cmp",                          \
    .long_name = TRS("Subpixel ME compare function"),           \
    .type =  BG_PARAMETER_STRINGLIST,                \
    .val_default = GAVL_VALUE_INIT_STRING("SAD"),         \
    .multi_names = COMPARE_FUNCS,          \
    .help_string = TRS("Subpixel motion estimation compare function.\n")COMPARE_FUNCS_HELP \
  }

/** Motion estimation */
#define PARAM_ME_SUB_CMP_CHROMA \
  { \
    .name = "ff_me_sub_cmp_chroma",                 \
    .long_name = TRS("Enable chroma subpixel ME compare"),    \
    .type = BG_PARAMETER_CHECKBUTTON,                  \
    .val_default = GAVL_VALUE_INIT_INT(0),              \
  }


#define PARAM_MB_CMP \
  {\
    .name = "ff_mb_cmp",                          \
    .long_name = TRS("MB compare function"),           \
    .type =  BG_PARAMETER_STRINGLIST,                \
    .val_default = GAVL_VALUE_INIT_STRING("SAD"),         \
    .multi_names = COMPARE_FUNCS,          \
    .help_string = TRS("Macroblock compare function.\n")COMPARE_FUNCS_HELP \
  }

#define PARAM_MB_CMP_CHROMA \
  { \
    .name = "ff_mb_cmp_chroma",                 \
    .long_name = TRS("Enable chroma macroblock ME compare"),    \
    .type = BG_PARAMETER_CHECKBUTTON,                  \
    .val_default = GAVL_VALUE_INIT_INT(0),              \
  }


#define PARAM_ILDCT_CMP \
  {\
    .name = "ff_ildct_cmp",                          \
    .long_name = TRS("ILDCT compare function"),           \
    .type =  BG_PARAMETER_STRINGLIST,                \
    .val_default = GAVL_VALUE_INIT_STRING("SAD"),         \
    .multi_names = COMPARE_FUNCS,          \
    .help_string = TRS("Interlaced dct compare function.\n")COMPARE_FUNCS_HELP \
  }

#define PARAM_ICDCT_CMP_CHROMA \
  { \
    .name = "ff_ildct_cmp_chroma",                 \
    .long_name = TRS("Enable chroma ILDCT ME compare"),    \
    .type = BG_PARAMETER_CHECKBUTTON,                  \
    .val_default = GAVL_VALUE_INIT_INT(0),              \
  }

/** Motion estimation */
#define PARAM_DIA_SIZE \
  { \
    .name = "ff_dia_size",                        \
    .long_name = TRS("ME diamond size & shape"),\
    .type = BG_PARAMETER_SLIDER_INT,                  \
    .val_default = GAVL_VALUE_INIT_INT(0),              \
    .val_min =     GAVL_VALUE_INIT_INT(-9),             \
    .val_max =     GAVL_VALUE_INIT_INT(9),              \
    .help_string = TRS("Motion estimation diamond size. Negative means shape adaptive.") \
  }

#define PARAM_LAST_PREDICTOR_COUNT \
  { \
    .name = "ff_last_predictor_count",                        \
    .long_name = TRS("Last predictor count"),\
    .type = BG_PARAMETER_SLIDER_INT,                  \
    .val_default = GAVL_VALUE_INIT_INT(0),              \
    .val_min =     GAVL_VALUE_INIT_INT(0),              \
    .val_max =     GAVL_VALUE_INIT_INT(99),              \
    .help_string = TRS("Amount of motion predictors from the previous frame.\n"\
"0 (default)\n"\
"a Will use 2a+1 x 2a+1 macroblock square of motion vector predictors " \
"from the previous frame.")\
  }

/** Motion estimation */
#define PARAM_PRE_ME \
  { \
    .name = "ff_pre_me", \
    .long_name = TRS("ME pre-pass"), \
    .type = BG_PARAMETER_SLIDER_INT,  \
    .val_default = GAVL_VALUE_INIT_INT(0),              \
    .val_min =     GAVL_VALUE_INIT_INT(0),              \
    .val_max =     GAVL_VALUE_INIT_INT(2),              \
    .help_string = TRS("Motion estimation pre-pass\n"\
"0: disabled\n"\
"1: only after I-frames\n"\
"2: always") \
   }

/** Motion estimation */
#define PARAM_ME_PRE_CMP \
  {\
    .name = "ff_me_pre_cmp",                          \
    .long_name = TRS("ME pre-pass compare function"),           \
    .type =  BG_PARAMETER_STRINGLIST,                \
    .val_default = GAVL_VALUE_INIT_STRING("SAD"),         \
    .multi_names = COMPARE_FUNCS,          \
    .help_string = TRS("Motion estimation pre-pass compare function.\n")COMPARE_FUNCS_HELP\
  }

/** Motion estimation */
#define PARAM_ME_PRE_CMP_CHROMA \
  { \
    .name = "ff_me_pre_cmp_chroma",                 \
    .long_name = TRS("Enable chroma ME pre-pass compare"),    \
    .type = BG_PARAMETER_CHECKBUTTON,                  \
    .val_default = GAVL_VALUE_INIT_INT(0),              \
    .val_min =     GAVL_VALUE_INIT_INT(0),              \
    .val_max =     GAVL_VALUE_INIT_INT(1),              \
  }

/** Motion estimation */
#define PARAM_PRE_DIA_SIZE \
  { \
    .name = "ff_pre_dia_size",                        \
    .long_name = TRS("ME pre-pass diamond size & shape"),           \
    .type = BG_PARAMETER_SLIDER_INT,                  \
    .val_default = GAVL_VALUE_INIT_INT(0),              \
    .val_min =     GAVL_VALUE_INIT_INT(-9),             \
    .val_max =     GAVL_VALUE_INIT_INT(9),              \
    .help_string = TRS("Motion estimation pre-pass diamond size. Negative means shape adaptive.") \
  }

/** Motion estimation */
#define PARAM_ME_SUBPEL_QUALITY \
  { \
    .name = "ff_me_subpel_quality",                 \
    .long_name = TRS("Subpel ME quality"),               \
    .type = BG_PARAMETER_SLIDER_INT,                  \
    .val_default = GAVL_VALUE_INIT_INT(8),              \
    .val_min =     GAVL_VALUE_INIT_INT(1),              \
    .val_max =     GAVL_VALUE_INIT_INT(8),              \
    .help_string = TRS("Subpel motion estimation refinement quality (for qpel). Higher values "\
"mean higher quality but slower encoding.") \
  }

/** Motion estimation */
#define PARAM_ME_RANGE \
  {                    \
    .name = "ff_me_range",                          \
    .long_name = TRS("Motion estimation range"),       \
    .type = BG_PARAMETER_SLIDER_INT,                    \
    .val_default = GAVL_VALUE_INIT_INT(0),                \
    .val_min = GAVL_VALUE_INIT_INT(0),                    \
    .val_max = GAVL_VALUE_INIT_INT(1000),                 \
    .help_string = TRS("Motion estimation search range (0 means unlimited)"),  \
  }

/** Motion estimation */
#define PARAM_MB_DECISION \
  {                       \
    .name = "ff_mb_decision",                     \
    .long_name = TRS("MB decision mode"),              \
    .type = BG_PARAMETER_STRINGLIST,             \
    .val_default = GAVL_VALUE_INIT_STRING("Use compare function"),        \
    .multi_names = (char const *[]){ "Use compare function", \
                          "Fewest bits", "Rate distoration", (char*)0 },\
    .multi_labels = (char const *[]){ TRS("Use compare function"), \
                          TRS("Fewest bits"), TRS("Rate distoration"), (char*)0 },\
  }

/** Frame types */
#define PARAM_SCENE_CHANGE_THRESHOLD \
  {                                 \
    .name =      "ff_scenechange_threshold",        \
    .long_name = TRS("Scenechange threshold"),            \
    .type = BG_PARAMETER_INT,             \
    .val_default = GAVL_VALUE_INIT_INT(0),                \
    .val_min = GAVL_VALUE_INIT_INT(-1000000000),                \
    .val_max = GAVL_VALUE_INIT_INT(1000000000),                 \
    .help_string = TRS("Threshold for scene change detection.\n\
Negative values mean more sensitivity (more keyframes)") \
  }

/** Rate control */
#define PARAM_LMIN \
  {                \
    .name = "ff_lmin", \
    .long_name = TRS("Minimum lagrange multiplier"),    \
    .type = BG_PARAMETER_SLIDER_FLOAT,             \
    .val_default = GAVL_VALUE_INIT_FLOAT(2.0), \
    .val_min =     GAVL_VALUE_INIT_FLOAT(1.0), \
    .val_min =     GAVL_VALUE_INIT_FLOAT(31.0), \
    .num_digits =  1, \
    .help_string = TRS("Minimum Lagrange multiplier for ratecontrol. "\
"Should possibly be the same as minimum quantizer scale.")\
  }

/** Rate control */
#define PARAM_LMAX \
  {                \
    .name = "ff_lmax", \
    .long_name = TRS("Maximum lagrange multipler"),    \
    .type = BG_PARAMETER_SLIDER_FLOAT,             \
    .val_default = GAVL_VALUE_INIT_FLOAT(31.0), \
    .val_min =     GAVL_VALUE_INIT_FLOAT(1.0), \
    .val_min =     GAVL_VALUE_INIT_FLOAT(31.0), \
    .num_digits =  1, \
    .help_string = TRS("Maximum Lagrange multiplier for ratecontrol. "\
"Should possibly be the same as maximum quantizer scale.")\
  }

#define PARAM_NOISE_REDUCTION \
  { \
    .name = "ff_noise_reduction", \
    .long_name = TRS("Noise reduction"), \
    .type = BG_PARAMETER_SLIDER_INT, \
    .val_default = GAVL_VALUE_INIT_INT(0),            \
    .val_min = GAVL_VALUE_INIT_INT(0),                \
    .val_max = GAVL_VALUE_INIT_INT(2000),             \
  }

/** Rate control */
#define PARAM_RC_INITIAL_BUFFER_OCCUPANCY \
  { \
    .name = "ff_rc_initial_buffer_occupancy",   \
    .long_name = TRS("Initial RC buffer occupancy"),   \
    .type = BG_PARAMETER_INT,                \
    .val_default = GAVL_VALUE_INIT_INT(0),            \
    .help_string = TRS("Number of kilobits which should be loaded into \
the rc buffer before encoding starts. Must not be larger than \
RC buffer size") \
  }


/* Does nothing */
/** Frame types */

/** Quantizer */

/** Motion estimation */
#define PARAM_ME_THRESHOLD \
  { \
    .name = "ff_me_threshold", \
    .long_name = TRS("ME Theshold"),                   \
    .type = BG_PARAMETER_INT, \
    .val_default = GAVL_VALUE_INIT_INT(0),\
    .val_min =     GAVL_VALUE_INIT_INT(0),\
    .val_max =     GAVL_VALUE_INIT_INT(4000000),\
    .help_string = TRS("Motion estimation threshold. under which no motion estimation is performed, but instead the user specified motion vectors are used") \
  }

#define PARAM_MB_THRESHOLD \
  { \
    .name = "ff_mb_threshold", \
    .long_name = TRS("MB Theshold"),\
    .type = BG_PARAMETER_INT, \
    .val_default = GAVL_VALUE_INIT_INT(0),\
    .val_min =     GAVL_VALUE_INIT_INT(0),\
    .val_max =     GAVL_VALUE_INIT_INT(4000000),\
    .help_string = TRS("Macroblock threshold. under which the user specified macroblock types will be used") \
  }

#define PARAM_NSSE_WEIGHT \
  { \
    .name = "ff_nsse_weight", \
    .long_name = TRS("NSSE weight"),                   \
    .type = BG_PARAMETER_INT,                    \
    .val_default = GAVL_VALUE_INIT_INT(8),\
    .help_string = TRS("Noise vs. SSE weight for the NSSE comparsion function. "\
"0 is identical to SSE")\
  }

/** Masking */
#define PARAM_BORDER_MASKING \
  { \
    .name = "ff_border_masking", \
    .long_name = TRS("Border masking"), \
    .type = BG_PARAMETER_SLIDER_FLOAT,                    \
    .val_default = GAVL_VALUE_INIT_FLOAT(0.0), \
    .val_min =     GAVL_VALUE_INIT_FLOAT(0.0), \
    .val_max =     GAVL_VALUE_INIT_FLOAT(1.0), \
    .num_digits =  2, \
    .help_string = TRS("Encode image parts near the border with reduced quality."\
    " 0 means disabled")\
  }

#define PARAM_MB_LMIN \
  {                \
    .name = "ff_mb_lmin", \
    .long_name = TRS("Minimum MB lagrange multipler"),    \
    .type = BG_PARAMETER_SLIDER_FLOAT,             \
    .val_default = GAVL_VALUE_INIT_FLOAT(2.0), \
    .val_min =     GAVL_VALUE_INIT_FLOAT(1.0), \
    .val_min =     GAVL_VALUE_INIT_FLOAT(31.0), \
    .num_digits =  1, \
    .help_string = TRS("Minimum macroblock Lagrange multiplier.") \
  }

#define PARAM_MB_LMAX \
  {                \
    .name = "ff_mb_lmax", \
    .long_name = TRS("Maximum MB lagrange multipler"),    \
    .type = BG_PARAMETER_SLIDER_FLOAT,             \
    .val_default = GAVL_VALUE_INIT_FLOAT(31.0), \
    .val_min =     GAVL_VALUE_INIT_FLOAT(1.0), \
    .val_min =     GAVL_VALUE_INIT_FLOAT(31.0), \
    .num_digits =  1, \
    .help_string = TRS("Maximum macroblock Lagrange multiplier.") \
  }

/** Motion estimation */
#define PARAM_ME_PENALTY_COMPENSATION  \
  {                \
    .name = "ff_me_penalty_compensation", \
    .long_name = TRS("ME penalty compensation"),    \
    .type = BG_PARAMETER_INT,             \
    .val_default = GAVL_VALUE_INIT_INT(256), \
  }

#define PARAM_BIDIR_REFINE  \
  {                \
    .name = "ff_bidir_refine", \
    .long_name = TRS("Bidir refine"),    \
    .type = BG_PARAMETER_SLIDER_INT,             \
    .val_default = GAVL_VALUE_INIT_INT(0), \
    .val_min =     GAVL_VALUE_INIT_INT(0), \
    .val_max =     GAVL_VALUE_INIT_INT(4), \
  }

#define PARAM_BRD_SCALE  \
  {                \
    .name = "ff_brd_scale", \
    .long_name = TRS("BRD scale"),    \
    .type = BG_PARAMETER_SLIDER_INT,             \
    .val_default = GAVL_VALUE_INIT_INT(0), \
    .val_min =     GAVL_VALUE_INIT_INT(0), \
    .val_max =     GAVL_VALUE_INIT_INT(10), \
  }


/** Frame types */

/** Quantizer */
#define PARAM_QSCALE \
  {                              \
    .name =        "ff_qscale", \
    .long_name =   "Fixed quantizer",              \
    .type =        BG_PARAMETER_SLIDER_INT, \
    .val_default = GAVL_VALUE_INIT_INT(10),    \
    .val_min =     GAVL_VALUE_INIT_INT(1),    \
    .val_max =     GAVL_VALUE_INIT_INT(31),    \
    .help_string = TRS("Quantizer for fixed quality encoding. Lower means better, 1 is not recommended")\
  }

/* Flags */

/** Quantizer */
#define PARAM_FLAG_QSCALE \
  {                              \
    .name =        "ff_flag_qscale", \
    .long_name =   TRS("Use fixed quantizer"),              \
    .type =        BG_PARAMETER_CHECKBUTTON, \
    .val_default = GAVL_VALUE_INIT_INT(0),    \
    .help_string = TRS("Use fixed quality encoding"),\
  }

/** Motion estimation */
#define PARAM_FLAG_4MV \
  {                              \
    .name =        "ff_flag_4mv", \
    .long_name =   TRS("4 MV per MB allowed"),      \
    .type =        BG_PARAMETER_CHECKBUTTON, \
    .val_default = GAVL_VALUE_INIT_INT(0),    \
    .help_string = TRS("Allow 4 motion vectors per macroblock (slightly better quality). Works better if MB decision mode is \"Fewest bits\" or \"Rate distoration\".") \
  }

/** Motion estimation */
#define PARAM_FLAG_QPEL \
  {                              \
    .name =        "ff_flag_qpel", \
    .long_name =   TRS("Use qpel MC"),      \
    .type =        BG_PARAMETER_CHECKBUTTON, \
    .val_default = GAVL_VALUE_INIT_INT(0),    \
    .help_string = TRS("Use 1/4 pixel motion compensation.  Warning: QPEL is not supported by all decoders.") \
  }

#define PARAM_FLAG_PART \
  {                              \
    .name =        "ff_flag_part", \
    .long_name =   TRS("Use data partitioning"),      \
    .type =        BG_PARAMETER_CHECKBUTTON, \
    .val_default = GAVL_VALUE_INIT_INT(0),    \
    .help_string = TRS("Use data partitioning for more robustness if the video is "\
"for transmitting over unreliable channels") \
  }


#define PARAM_FLAG_GRAY \
  {                              \
    .name =        "ff_flag_gray", \
    .long_name =   TRS("Grayscale mode"),      \
    .type =        BG_PARAMETER_CHECKBUTTON, \
    .val_default = GAVL_VALUE_INIT_INT(0),    \
  }

/** Masking */

#define PARAM_FLAG_ALT_SCAN   \
  {                              \
    .name =        "ff_flag_alt_scan", \
    .long_name =   TRS("Use alternative scantable"),      \
    .type =        BG_PARAMETER_CHECKBUTTON, \
    .val_default = GAVL_VALUE_INIT_INT(0),    \
  }

/** Quantizer */
#if LIBAVCODEC_VERSION_INT < ((52<<16)+(0<<8)+0)
#define PARAM_TRELLIS   \
  {                              \
    .name =        "ff_flag_trellis_quant", \
    .long_name =   TRS("Use trellis quantization"),      \
    .type =        BG_PARAMETER_CHECKBUTTON, \
    .val_default = GAVL_VALUE_INIT_INT(0),    \
    .help_string = TRS("Use trellis quantization (improves quality)") \
  }
#else
#define PARAM_TRELLIS   \
  {                              \
    .name =        "ff_trellis", \
    .long_name =   TRS("Use trellis quantization"),      \
    .type =        BG_PARAMETER_CHECKBUTTON, \
    .val_default = GAVL_VALUE_INIT_INT(0),    \
    .help_string = TRS("Use trellis quantization (improves quality)") \
  }
#endif

#define PARAM_FLAG_BITEXACT   \
  {                              \
    .name =        "ff_flag_bitexact", \
    .long_name =   TRS("Use only bitexact stuff"),      \
    .type =        BG_PARAMETER_CHECKBUTTON, \
    .val_default = GAVL_VALUE_INIT_INT(0),    \
    .help_string = TRS("Use only bitexact stuff (except (i)dct)") \
  }

#define PARAM_FLAG_AC_PRED_H263   \
  {                              \
    .name =        "ff_flag_ac_pred", \
    .long_name =   TRS("Advanced intra coding"),  \
    .type =        BG_PARAMETER_CHECKBUTTON, \
    .val_default = GAVL_VALUE_INIT_INT(0),    \
  }

#define PARAM_FLAG_AC_PRED_MPEG4   \
  {                              \
    .name =        "ff_flag_ac_pred", \
    .long_name =   TRS("MPEG-4 AC prediction"),  \
    .type =        BG_PARAMETER_CHECKBUTTON, \
    .val_default = GAVL_VALUE_INIT_INT(0),    \
  }

#define PARAM_FLAG_H263P_UMV   \
  {                              \
    .name =        "ff_flag_h263p_umv", \
    .long_name =   TRS("Unlimited motion vector"),  \
    .type =        BG_PARAMETER_CHECKBUTTON, \
    .val_default = GAVL_VALUE_INIT_INT(0),    \
  }

#define PARAM_FLAG_CBP_RD \
  {                       \
    .name = "ff_flag_cbp_rd", \
    .long_name = TRS("CBP RD"), \
    .type =        BG_PARAMETER_CHECKBUTTON, \
    .val_default = GAVL_VALUE_INIT_INT(0),    \
    .help_string = TRS("Use rate distortion optimization for cbp"),\
  }

#define PARAM_FLAG_QP_RD \
  {                      \
    .name = "ff_flag_qp_rd", \
    .long_name = TRS("QP RD"), \
    .type =        BG_PARAMETER_CHECKBUTTON, \
    .val_default = GAVL_VALUE_INIT_INT(0),    \
    .help_string = TRS("Use rate distortion optimization for qp selection"),\
  }

#define PARAM_FLAG_H263P_AIV \
  {                      \
    .name = "ff_flag_h263p_aiv", \
    .long_name = TRS("Alternative inter vlc"), \
    .type =        BG_PARAMETER_CHECKBUTTON, \
    .val_default = GAVL_VALUE_INIT_INT(0),    \
  }

/* H.263(+) */
#define PARAM_FLAG_OBMC \
  { \
    .name = "ff_flag_obmc", \
    .long_name = TRS("OBMC"), \
    .type =        BG_PARAMETER_CHECKBUTTON, \
    .val_default = GAVL_VALUE_INIT_INT(0),    \
    .help_string = TRS("Overlapped block motion compensation (only supported with with simple MB decision)") \
  }

#define PARAM_FLAG_LOOP_FILTER              \
  { \
    .name = "ff_flag_loop_filter", \
    .long_name = TRS("Loop filter"), \
    .type =        BG_PARAMETER_CHECKBUTTON, \
    .val_default = GAVL_VALUE_INIT_INT(0),    \
  }

#define PARAM_FLAG_H263P_SLICE_STRUCT      \
  { \
    .name = "ff_flag_h263p_slice_struct", \
    .long_name = TRS("H263P slice struct"), \
    .type =        BG_PARAMETER_CHECKBUTTON, \
    .val_default = GAVL_VALUE_INIT_INT(0),    \
  }

/** Frame types */
#define PARAM_FLAG_CLOSED_GOP \
  { \
    .name = "ff_flag_closed_gop", \
    .long_name = TRS("Close all GOPs"), \
    .type =        BG_PARAMETER_CHECKBUTTON, \
    .val_default = GAVL_VALUE_INIT_INT(0),    \
  }

#define PARAM_FLAG2_FAST \
  { \
    .name = "ff_flag2_fast", \
    .long_name = TRS("Allow fast encoding"), \
    .type =        BG_PARAMETER_CHECKBUTTON, \
    .val_default = GAVL_VALUE_INIT_INT(0),    \
    .help_string = TRS("Allow non spec compliant speedup tricks") \
}

/** Frame types */
#define PARAM_FLAG2_STRICT_GOP                     \
  { \
    .name = "ff_flag2_strict_gop", \
    .long_name = TRS("Strictly enforce GOP size"), \
    .type =        BG_PARAMETER_CHECKBUTTON, \
    .val_default = GAVL_VALUE_INIT_INT(0),    \
}


/**  */
#define PARAM_THREAD_COUNT  \
  { \
    .name = "ff_thread_count", \
    .long_name = TRS("Thread count"),    \
    .type = BG_PARAMETER_INT,             \
    .val_default = GAVL_VALUE_INIT_INT(0), \
    .help_string = TRS("Number of threads to use") \
  }
