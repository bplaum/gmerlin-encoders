gmerlin_plugindir = @gmerlin_plugindir@

AM_CPPFLAGS = -I$(top_srcdir)/include

AM_LDFLAGS = @GMERLIN_PLUGIN_LDFLAGS@ -avoid-version -module

if HAVE_THEORAENC
theoraenc_plugins = c_theoraenc.la
theora_sources = theora.c
else
theoraenc_plugins =
theora_sources = 
endif

if HAVE_OPUS
opus_plugins = e_opus.la c_opusenc.la
opus_sources = opus.c
else
opus_plugins =
opus_sources =
endif

if HAVE_FLAC
flac_plugins = c_flacenc.la
flac_sources = flac.c
bgflac_libs    = $(top_builddir)/lib/libbgflac.la @FLAC_LIBS@
else
flac_plugins =
flac_sources =
endif

if HAVE_OGG_VIDEO
oggvideo_plugins = e_oggvideo.la
else
oggvideo_plugins =
endif

gmerlin_plugin_LTLIBRARIES = \
e_vorbis.la \
c_vorbisenc.la \
$(theoraenc_plugins) \
$(opus_plugins) \
$(flac_plugins) \
$(oggvideo_plugins)

AM_CFLAGS = -DLOCALE_DIR=\"$(localedir)\"

e_vorbis_la_CFLAGS = @VORBISENC_CFLAGS@ $(AM_CFLAGS)
e_vorbis_la_SOURCES = e_vorbis.c vorbis.c ogg_common.c
e_vorbis_la_LIBADD = $(top_builddir)/lib/libgmerlin_encoders.la @VORBISENC_LIBS@

e_oggvideo_la_CFLAGS = \
@THEORAENC_CFLAGS@ \
@VORBISENC_CFLAGS@ \
@FLAC_CFLAGS@ \
@OPUS_CFLAGS@ \
$(AM_CFLAGS)

e_oggvideo_la_SOURCES = \
e_oggvideo.c \
vorbis.c \
$(flac_sources) \
$(opus_sources) \
$(theora_sources) \
ogg_common.c

e_oggvideo_la_LIBADD = \
$(top_builddir)/lib/libgmerlin_encoders.la \
@VORBISENC_LIBS@ \
@THEORAENC_LIBS@ \
@OPUS_LIBS@ \
$(bgflac_libs)

e_opus_la_CFLAGS = @OPUS_CFLAGS@ $(AM_CFLAGS)
e_opus_la_SOURCES = e_opus.c opus.c ogg_common.c
e_opus_la_LIBADD = $(top_builddir)/lib/libgmerlin_encoders.la @OPUS_LIBS@ @OGG_LIBS@ 


noinst_HEADERS = ogg_common.h

c_theoraenc_la_CFLAGS = \
@THEORAENC_CFLAGS@ \
$(AM_CFLAGS)

c_theoraenc_la_SOURCES = \
theora.c \
c_theoraenc.c \
ogg_common.c

c_theoraenc_la_LIBADD = \
$(top_builddir)/lib/libgmerlin_encoders.la \
@THEORAENC_LIBS@

c_vorbisenc_la_CFLAGS = \
@VORBISENC_CFLAGS@ $(AM_CFLAGS)

c_vorbisenc_la_SOURCES = \
vorbis.c \
c_vorbisenc.c \
ogg_common.c

c_vorbisenc_la_LIBADD = \
$(top_builddir)/lib/libgmerlin_encoders.la \
@VORBISENC_LIBS@

c_opusenc_la_CFLAGS = \
@OPUS_CFLAGS@ $(AM_CFLAGS)

c_opusenc_la_SOURCES = \
opus.c \
c_opusenc.c \
ogg_common.c

c_opusenc_la_LIBADD = \
$(top_builddir)/lib/libgmerlin_encoders.la \
@OPUS_LIBS@ \
@OGG_LIBS@

#

c_flacenc_la_CFLAGS = \
@FLAC_CFLAGS@ $(AM_CFLAGS)

c_flacenc_la_SOURCES = \
flac.c \
c_flacenc.c \
ogg_common.c

c_flacenc_la_LIBADD = \
$(top_builddir)/lib/libgmerlin_encoders.la \
$(bgflac_libs) \
@FLAC_LIBS@ \
@OGG_LIBS@


EXTRA_c_vorbisenc_la_SOURCES = \
_codec_plugin.c
