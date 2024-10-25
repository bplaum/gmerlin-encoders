dnl
dnl Standardized linker flags:
dnl We use --as-needed for executables and
dnl --no-undefined for libraries
dnl

AC_DEFUN([GMERLIN_CHECK_LDFLAGS],[

GMERLIN_LIB_LDFLAGS=""
GMERLIN_EXE_LDFLAGS=""

AC_MSG_CHECKING(if linker supports --no-undefined)
OLD_LDFLAGS=$LDFLAGS
LDFLAGS="$LDFLAGS -Wl,--no-undefined"
AC_TRY_LINK([],[],
            [GMERLIN_LIB_LDFLAGS="-Wl,--no-undefined $GMERLIN_LIB_LDFLAGS"; AC_MSG_RESULT(Supported)],
            [AC_MSG_RESULT(Unsupported)])
LDFLAGS=$OLD_LDFLAGS

AC_MSG_CHECKING(if linker supports --as-needed)
OLD_LDFLAGS=$LDFLAGS
LDFLAGS="$LDFLAGS -Wl,--as-needed"
AC_TRY_LINK([],[],
            [GMERLIN_EXE_LDFLAGS="-Wl,--as-needed $GMERLIN_EXE_LDFLAGS"; AC_MSG_RESULT(Supported)],
            [AC_MSG_RESULT(Unsupported)])
LDFLAGS=$OLD_LDFLAGS

AC_SUBST(GMERLIN_LIB_LDFLAGS)
AC_SUBST(GMERLIN_EXE_LDFLAGS)

])

dnl
dnl Check for theora encoder
dnl

AC_DEFUN([GMERLIN_CHECK_THEORAENC],[

AH_TEMPLATE([HAVE_THEORAENC],
            [Do we have theora encoder installed?])

have_theora="false"

THEORAENC_REQUIRED="1.0.0"

AC_ARG_ENABLE(theoraenc,
[AS_HELP_STRING([--disable-theoraenc],[Disable theoraenc (default: autodetect)])
],
[case "${enableval}" in
   yes) test_theoraenc=true ;;
   no)  test_theoraenc=false ;;
esac],[test_theoraenc=true])

if test x$test_theoraenc = xtrue; then

PKG_CHECK_MODULES(THEORAENC, theoraenc, have_theoraenc="true", have_theoraenc="false")
fi

AC_SUBST(THEORAENC_REQUIRED)
AC_SUBST(THEORAENC_LIBS)
AC_SUBST(THEORAENC_CFLAGS)

AM_CONDITIONAL(HAVE_THEORAENC, test x$have_theoraenc = xtrue)

if test "x$have_theoraenc" = "xtrue"; then
AC_DEFINE([HAVE_THEORAENC])
fi

])


dnl
dnl Check for libavcodec
dnl

AC_DEFUN([GMERLIN_CHECK_AVCODEC],[

AH_TEMPLATE([HAVE_AVCODEC],
            [Do we have libavcodec installed?])

have_libavcodec="false"

AVCODEC_REQUIRED="58.54.100"

AC_ARG_ENABLE(avcodec,
[AS_HELP_STRING([--disable-avcodec],[Disable libavcodec (default: autodetect)])],
[case "${enableval}" in
   yes) test_avcodec=true ;;
   no)  test_avcodec=false ;;
esac],[test_avcodec=true])

if test x$test_avcodec = xtrue; then

PKG_CHECK_MODULES(AVCODEC, libavcodec, have_avcodec="true", have_avcodec="false")
fi

AC_SUBST(AVCODEC_REQUIRED)
AC_SUBST(AVCODEC_LIBS)
AC_SUBST(AVCODEC_CFLAGS)

AM_CONDITIONAL(HAVE_AVCODEC, test x$have_avcodec = xtrue)

if test "x$have_avcodec" = "xtrue"; then
AC_DEFINE([HAVE_AVCODEC])
fi

])

AC_DEFUN([GMERLIN_CHECK_AVFORMAT],[

AH_TEMPLATE([HAVE_AVFORMAT],
            [Do we have libavformat installed?])

have_libavformat="false"

AVFORMAT_REQUIRED="58.29.100"

AC_ARG_ENABLE(avformat,
[AS_HELP_STRING([--disable-avformat],[Disable libavformat (default: autodetect)])],
[case "${enableval}" in
   yes) test_avformat=true ;;
   no)  test_avformat=false ;;
esac],[test_avformat=true])

if test x$test_avformat = xtrue; then

PKG_CHECK_MODULES(AVFORMAT, libavformat, have_avformat="true", have_avformat="false")
fi

AC_SUBST(AVFORMAT_REQUIRED)
AC_SUBST(AVFORMAT_LIBS)
AC_SUBST(AVFORMAT_CFLAGS)

AM_CONDITIONAL(HAVE_AVFORMAT, test x$have_avformat = xtrue)

if test "x$have_avformat" = "xtrue"; then
AC_DEFINE([HAVE_AVFORMAT])
fi

])


dnl
dnl Ogg
dnl 

AC_DEFUN([GMERLIN_CHECK_OGG],[

OGG_REQUIRED="1.0"

have_ogg=false
AH_TEMPLATE([HAVE_OGG], [Ogg libraries are there])

AC_ARG_ENABLE(ogg,
[AS_HELP_STRING([--disable-ogg],[Disable ogg (default: autodetect)])],
[case "${enableval}" in
   yes) test_ogg=true ;;
   no)  test_ogg=false ;;
esac],[test_ogg=true])

if test x$test_ogg = xtrue; then
PKG_CHECK_MODULES(OGG, ogg, have_ogg="true", have_ogg="false")
fi

AC_SUBST(OGG_LIBS)
AC_SUBST(OGG_CFLAGS)

AM_CONDITIONAL(HAVE_OGG, test x$have_ogg = xtrue)
 
if test x$have_ogg = xtrue; then
AC_DEFINE(HAVE_OGG)
fi

AC_SUBST(OGG_REQUIRED)

])

dnl
dnl Vorbis
dnl 

AC_DEFUN([GMERLIN_CHECK_VORBIS],[

VORBIS_REQUIRED="1.0"

have_vorbis=false
AH_TEMPLATE([HAVE_VORBIS], [Vorbis libraries are there])

AC_ARG_ENABLE(vorbis,
[AS_HELP_STRING([--disable-vorbis],[Disable vorbis (default: autodetect)])],
[case "${enableval}" in
   yes) test_vorbis=true ;;
   no)  test_vorbis=false ;;
esac],[test_vorbis=true])

if test x$test_vorbis = xtrue; then
PKG_CHECK_MODULES(VORBISENC, vorbisenc, have_vorbis="true", have_vorbis="false")
fi

AM_CONDITIONAL(HAVE_VORBIS, test x$have_vorbis = xtrue)
 
if test x$have_vorbis = xtrue; then
AC_DEFINE(HAVE_VORBIS)

OLD_CFLAGS=$CFLAGS
OLD_LIBS=$LIBS

CFLAGS="$VORBIS_CFLAGS"
LIBS="$VORBIS_LIBS"

AC_CHECK_FUNCS(vorbis_synthesis_restart)

CFLAGS="$OLD_CFLAGS"
LIBS="$OLD_LIBS"


fi

AC_SUBST(VORBIS_REQUIRED)

])



dnl
dnl FLAC
dnl

AC_DEFUN([GMERLIN_CHECK_FLAC],[

FLAC_REQUIRED="1.2.0"
have_flac="false"

AC_ARG_ENABLE(flac,
[AS_HELP_STRING([--disable-flac],[Disable flac (default: autodetect)])],
[case "${enableval}" in
   yes) test_flac=true ;;
   no)  test_flac=false ;;
esac],[test_flac=true])

if test x$test_flac = xtrue; then

AH_TEMPLATE([HAVE_FLAC], [Enable FLAC])

PKG_CHECK_MODULES(FLAC, flac, have_flac="true", have_flac="false")
fi

AC_SUBST(FLAC_CFLAGS)
AC_SUBST(FLAC_LIBS)
AC_SUBST(FLAC_REQUIRED)

AM_CONDITIONAL(HAVE_FLAC, test x$have_flac = xtrue)

if test x$have_flac = xtrue; then
AC_DEFINE(HAVE_FLAC)
fi

])

dnl
dnl lame
dnl

AC_DEFUN([GMERLIN_CHECK_LAME],[
LAME_REQUIRED="3.93"
have_lame="false"

AC_ARG_ENABLE(lame,
[AS_HELP_STRING([--disable-lame],[Disable lame (default: autodetect)])],
[case "${enableval}" in
   yes) test_lame=true ;;
   no)  test_lame=false ;;
esac],[test_lame=true])

AH_TEMPLATE([HAVE_LAME],
            [Do we have lame installed?])

if test x$test_lame = xtrue; then

PKG_CHECK_MODULES(LAME, lame, have_lame="true", have_lame="false")
    
fi

AC_SUBST(LAME_CFLAGS)
AC_SUBST(LAME_LIBS)
AC_SUBST(LAME_REQUIRED)

AM_CONDITIONAL(HAVE_LAME, test x$have_lame = xtrue)
if test x$have_lame = xtrue; then
AC_DEFINE(HAVE_LAME)
fi

])


dnl
dnl Check for opus codec
dnl

AC_DEFUN([GMERLIN_CHECK_OPUS],[

AH_TEMPLATE([HAVE_OPUS],
            [Do we have libopus installed?])

have_opus="false"

OPUS_REQUIRED="1.0.0"

AC_ARG_ENABLE(opus,
[AS_HELP_STRING([--disable-opus],[Disable opus (default: autodetect)])],
[case "${enableval}" in
   yes) test_opus=true ;;
   no)  test_opus=false ;;
esac],[test_opus=true])

if test x$test_opus = xtrue; then

PKG_CHECK_MODULES(OPUS, opus, have_opus="true", have_opus="false")
fi

AC_SUBST(OPUS_REQUIRED)
AC_SUBST(OPUS_LIBS)
AC_SUBST(OPUS_CFLAGS)

AM_CONDITIONAL(HAVE_OPUS, test x$have_opus = xtrue)

if test "x$have_opus" = "xtrue"; then
AC_DEFINE([HAVE_OPUS])
fi

])

