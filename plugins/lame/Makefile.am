gmerlin_plugindir = @gmerlin_plugindir@

AM_CPPFLAGS = -I$(top_srcdir)/include

AM_LDFLAGS = @GMERLIN_PLUGIN_LDFLAGS@ -avoid-version -module
AM_CFLAGS = @LAME_CFLAGS@ -DLOCALE_DIR=\"$(localedir)\"

gmerlin_plugin_LTLIBRARIES = \
c_lame.la \
e_lame.la

e_lame_la_SOURCES = e_lame.c xing.c bglame.c
e_lame_la_LIBADD = $(top_builddir)/lib/libgmerlin_encoders.la @LAME_LIBS@

c_lame_la_SOURCES = c_lame.c bglame.c
c_lame_la_LIBADD = @LAME_LIBS@

noinst_HEADERS = xing.h bglame.h
