AC_INIT([gmerlin-encoders], [2.0.0])
AC_CONFIG_SRCDIR([include/gmerlin_encoders.h])
AM_INIT_AUTOMAKE

AM_CONFIG_HEADER(include/config.h)

SAVE_CFLAGS=$CFLAGS
SAVE_LDFLAGS=$LDFLAGS
AC_DISABLE_STATIC
AC_LANG(C)
AC_PROG_LIBTOOL

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
GMERLIN_CHECK_THEORADEC

dnl
dnl Speex
dnl

GMERLIN_CHECK_SPEEX

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
dnl faac
dnl

GMERLIN_CHECK_FAAC

dnl
dnl libavcodec
dnl

GMERLIN_CHECK_AVCODEC

dnl
dnl libavformat
dnl

GMERLIN_CHECK_AVFORMAT

dnl
dnl libshout
dnl

GMERLIN_CHECK_SHOUT

dnl
dnl schroedinger
dnl

GMERLIN_CHECK_SCHROEDINGER


dnl
dnl Check if we have at least one video encoder
dnl available
dnl 

AM_CONDITIONAL(HAVE_OGG_VIDEO,
               test "x$have_theora" = "xtrue" -o "x$have_schroedinger" = "xtrue")

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
gmerlin_encoders.spec \
include/Makefile \
lib/Makefile \
m4/Makefile \
utils/Makefile \
plugins/Makefile \
plugins/flac/Makefile \
plugins/ogg/Makefile \
plugins/lame/Makefile \
plugins/faac/Makefile \
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

echo -n "speex:        "
if test "x$have_speex" = "xtrue"; then
echo "Found (CFLAGS=$SPEEX_CFLAGS LIBS=$SPEEX_LIBS)"
else
echo "Missing (Go to: http://www.speex.org/)"
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
echo "Missing (Go to: http://www.mp3dev.org/)"
fi

echo -n "faac:         "
if test "x$have_faac" = "xtrue"; then
echo "Found (CFLAGS=$FAAC_CFLAGS LIBS=$FAAC_LIBS)"
else
echo "Missing (Go to: http://www.audiocoding.com/)"
fi

echo -n "ffmpeg:       "
if test "x$have_avformat" = "xtrue"; then
echo "Found (CFLAGS=$AVFORMAT_CFLAGS LIBS=$AVFORMAT_LIBS)"
else
echo "Missing (Go to: http://ffmpeg.org/)"
fi

echo -n "libshout:     "
if test "x$have_shout" = "xtrue"; then
echo "Found (CFLAGS=$SHOUT_CFLAGS LIBS=$SHOUT_LIBS)"
else
echo "Missing (Go to: http://icecast.org/)"
fi

echo -n "schroedinger: "
if test "x$have_schroedinger" = "xtrue"; then
echo "Found (CFLAGS=$SCHROEDINGER_CFLAGS LIBS=$SCHROEDINGER_LIBS)"
else
echo "Missing (Go to: http://diracvideo.org/)"
fi

echo
echo "If you installed a library but it was not detected, check the file INSTALL"
echo "for troubleshooting tips. Also note that if the configure script reaches"
echo "this point, all missing packages are optional so compilation should succeed"
echo "anyway."
