AC_INIT([gmerlin-encoders], [2.0.0pre1])
AC_CONFIG_SRCDIR([include/gmerlin_encoders.h])
AM_INIT_AUTOMAKE

AC_CONFIG_HEADERS(include/config.h)

SAVE_CFLAGS=$CFLAGS
SAVE_LDFLAGS=$LDFLAGS
AC_DISABLE_STATIC
AC_LANG(C)

LT_INIT

CFLAGS=$SAVE_CFLAGS

dnl
dnl Endianess
dnl

AC_C_BIGENDIAN(,,AC_MSG_ERROR("Cannot detect endianess"))

dnl
dnl Required libraries
dnl

GMERLIN_REQUIRED="2.0.0"
PKG_CHECK_MODULES(GMERLIN, gmerlin >= $GMERLIN_REQUIRED, , AC_MSG_ERROR("gmerlin not found"))

gmerlin_plugindir='$(libdir)/gmerlin/plugins'

dnl LDFLAGS for plugins
GMERLIN_PLUGIN_LDFLAGS="-export-symbols "`pkg-config --variable=prefix gmerlin`"/share/gmerlin/plugin.sym $SAVE_LDFLAGS"

AC_SUBST(GMERLIN_PLUGIN_LDFLAGS)
AC_SUBST(gmerlin_plugindir)
AC_SUBST(GMERLIN_REQUIRED)

AM_GNU_GETTEXT([external])

dnl
dnl Optional libraries
dnl

dnl
dnl Ogg
dnl

GMERLIN_CHECK_OGG

dnl
dnl Vorbis
dnl

GMERLIN_CHECK_VORBIS

dnl
dnl Theora
dnl

GMERLIN_CHECK_THEORAENC

dnl
dnl OPUS
dnl

GMERLIN_CHECK_OPUS

dnl
dnl FLAC
dnl

GMERLIN_CHECK_FLAC
 
dnl
dnl lame
dnl

GMERLIN_CHECK_LAME

dnl
dnl libavcodec
dnl

GMERLIN_CHECK_AVCODEC

dnl
dnl libavformat
dnl

GMERLIN_CHECK_AVFORMAT

dnl
dnl Check if we have at least one video encoder
dnl available
dnl 

AM_CONDITIONAL(HAVE_OGG_VIDEO, test "x$have_theoraenc" = "xtrue")

dnl
dnl CFLAGS
dnl

LQT_OPT_CFLAGS(["-O3 -fomit-frame-pointer -ffast-math"])

CFLAGS="$CFLAGS -D_REENTRANT -D_FILE_OFFSET_BITS=64 $OPT_CFLAGS $GMERLIN_CFLAGS"

LQT_TRY_CFLAGS(-Wall, CFLAGS="$CFLAGS -Wall")
LQT_TRY_CFLAGS(-Wmissing-declarations, CFLAGS="$CFLAGS -Wmissing-declarations" )
LQT_TRY_CFLAGS(-Wdeclaration-after-statement, CFLAGS="$CFLAGS -Wdeclaration-after-statement")


dnl
dnl LIBS
dnl

LIBS="$GMERLIN_LIBS"

if test "x$prefix" = xNONE; then
   prefix="${ac_default_prefix}"
fi
if test "x$exec_prefix" = xNONE; then
   exec_prefix="${prefix}"
fi

AC_CONFIG_FILES([Makefile \
po/Makefile.in \
include/Makefile \
lib/Makefile \
m4/Makefile \
utils/Makefile \
plugins/Makefile \
plugins/flac/Makefile \
plugins/ogg/Makefile \
plugins/lame/Makefile \
plugins/ffmpeg/Makefile \
])

AC_OUTPUT

echo ""
echo "=== Libraries: ================================="

echo -n "flac:         "
if test "x$have_flac" = "xtrue"; then
echo "Found (CFLAGS=$FLAC_CFLAGS LIBS=$FLAC_LIBS)"
else
echo "Missing (Go to: http://flac.sourceforge.net/)"
fi

echo -n "ogg:          "
if test "x$have_ogg" = "xtrue"; then
echo "Found (CFLAGS=$OGG_CFLAGS LIBS=$OGG_LIBS)"
else
echo "Missing (Go to: http://www.xiph.org/)"
fi

echo -n "vorbis:       "
if test "x$have_vorbis" = "xtrue"; then
echo "Found (CFLAGS=$VORBIS_CFLAGS LIBS=$VORBIS_LIBS)"
else
echo "Missing (Go to: http://www.vorbis.com/)"
fi

echo -n "theoraenc:    "
if test "x$have_theoraenc" = "xtrue"; then
echo "Found (CFLAGS=$THEORAENC_CFLAGS LIBS=$THEORAENC_LIBS)"
else
echo "Missing (Go to: http://www.theora.org/)"
fi

echo -n "opus:         "
if test "x$have_opus" = "xtrue"; then
echo "Found (CFLAGS=$OPUS_CFLAGS LIBS=$OPUS_LIBS)"
else
echo "Missing (Go to: http://www.opus-codec.org/)"
fi


echo -n "lame:         "
if test "x$have_lame" = "xtrue"; then
echo "Found (CFLAGS=$LAME_CFLAGS LIBS=$LAME_LIBS)"
else
echo "Missing (Go to: https://lame.sourceforge.io)"
fi

echo -n "ffmpeg:       "
if test "x$have_avformat" = "xtrue"; then
echo "Found (CFLAGS=$AVFORMAT_CFLAGS LIBS=$AVFORMAT_LIBS)"
else
echo "Missing (Go to: http://ffmpeg.org/)"
fi

echo
echo "If the configure script reaches this point, all missing packages are "
echo "optional so compilation should succeed anyway."
echo
