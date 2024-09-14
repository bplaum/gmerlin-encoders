gmerlin_plugindir = @gmerlin_plugindir@

AM_CPPFLAGS = -I$(top_srcdir)/include

AM_LDFLAGS = @GMERLIN_PLUGIN_LDFLAGS@ -avoid-version -module
AM_CFLAGS = @AVFORMAT_CFLAGS@ @AVCODEC_CFLAGS@ -DLOCALE_DIR=\"$(localedir)\"

gmerlin_plugin_LTLIBRARIES = \
c_ffmpeg_ac3.la \
c_ffmpeg_alaw.la \
c_ffmpeg_ulaw.la \
c_ffmpeg_mpeg4.la \
c_ffmpeg_x264.la \
c_ffmpeg_mp2.la \
c_ffmpeg_jpeg.la \
c_ffmpeg_mpeg1.la \
c_ffmpeg_mpeg2.la \
c_ffmpeg_tga.la \
c_ffmpeg_vp8.la \
e_mpeg1video.la \
e_mpeg2video.la \
e_au.la \
e_aiff.la \
e_mp2.la \
e_ac3.la \
e_adts.la \
e_wma.la \
e_avi.la \
e_mpeg.la \
e_vob.la \
e_dvd.la \
e_asf.la \
e_mpegts.la \
e_matroska.la \
e_webm.la \
e_mp4.la

common_sources = ffmpeg_common.c codecs.c codec.c
codec_sources = codecs.c codec.c

LIBS = @AVFORMAT_LIBS@

e_mpeg1video_la_SOURCES = e_mpeg1video.c $(common_sources)
e_mpeg2video_la_SOURCES = e_mpeg2video.c $(common_sources)

e_au_la_SOURCES = e_au.c $(common_sources)

e_aiff_la_SOURCES = e_aiff.c $(common_sources)

e_mp2_la_SOURCES = e_mp2.c $(common_sources)

e_ac3_la_SOURCES = e_ac3.c $(common_sources)

e_adts_la_SOURCES = e_adts.c $(common_sources)

e_wma_la_SOURCES = e_wma.c $(common_sources)

e_avi_la_SOURCES = e_avi.c $(common_sources)

e_mpeg_la_SOURCES = e_mpeg.c $(common_sources)

e_vob_la_SOURCES = e_vob.c $(common_sources)

e_dvd_la_SOURCES = e_dvd.c $(common_sources)

e_asf_la_SOURCES = e_asf.c $(common_sources)

e_mpegts_la_SOURCES = e_mpegts.c $(common_sources)

e_matroska_la_SOURCES = e_matroska.c $(common_sources)

e_webm_la_SOURCES = e_webm.c $(common_sources)

e_mp4_la_SOURCES = e_mp4.c $(common_sources)

c_ffmpeg_mpeg4_la_SOURCES = c_ffmpeg_mpeg4.c $(common_sources)

c_ffmpeg_x264_la_SOURCES = c_ffmpeg_x264.c $(common_sources)

c_ffmpeg_mp2_la_SOURCES = c_ffmpeg_mp2.c $(common_sources)

c_ffmpeg_ac3_la_SOURCES = c_ffmpeg_ac3.c $(common_sources)

c_ffmpeg_alaw_la_SOURCES = c_ffmpeg_alaw.c $(common_sources)

c_ffmpeg_ulaw_la_SOURCES = c_ffmpeg_ulaw.c $(common_sources)

c_ffmpeg_jpeg_la_SOURCES = c_ffmpeg_jpeg.c $(common_sources)

c_ffmpeg_mpeg1_la_SOURCES = c_ffmpeg_mpeg1.c $(common_sources)

c_ffmpeg_mpeg2_la_SOURCES = c_ffmpeg_mpeg2.c $(common_sources)

c_ffmpeg_tga_la_SOURCES = c_ffmpeg_tga.c $(common_sources)

c_ffmpeg_vp8_la_SOURCES = c_ffmpeg_vp8.c $(common_sources)

noinst_HEADERS = ffmpeg_common.h params.h
EXTRA_DIST= _codec_plugin.c _e_ffmpeg_video.c _e_ffmpeg_audio.c _e_ffmpeg.c
