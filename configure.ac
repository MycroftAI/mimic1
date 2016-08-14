dnl Use autogen.sh to regenerate the configure script
AC_PREREQ([2.62])
AC_INIT([mimic],
        [1.1.0.9000],
        [https://github.com/MycroftAI/mimic/issues])

AC_CONFIG_AUX_DIR([config])
AC_CONFIG_MACRO_DIR([m4])


AC_CANONICAL_HOST
AC_CANONICAL_BUILD

AM_INIT_AUTOMAKE([-Wall foreign subdir-objects])
AC_PROG_CC
AM_PROG_AR
AC_PROG_RANLIB
AM_SILENT_RULES([yes])
AC_PROG_INSTALL
AC_CONFIG_SRCDIR([include/mimic.h])
AC_CONFIG_HEADERS([include/config.h])
LT_PREREQ([2.2])
AC_C_BIGENDIAN

PKG_PROG_PKG_CONFIG

dnl By default disable building static and shared libraries.
dnl use ./configure --enable-shared (or --enable-static or both)
dnl to build them.
LT_INIT([disable-static, disable-shared])


dnl openmp support for multi-threaded test:
AC_OPENMP

dnl
dnl Does this platform support mmap(2)
dnl Note we don't use AC_FUNC_MMAP because we really only care whether
dnl read-only mappings work.
dnl
MMAPTYPE=
dnl These numbers must match the assignments below
AC_DEFINE([MMAP_TYPE_NONE], [0], [enum for mmap_types]) 
AC_DEFINE([MMAP_TYPE_POSIX], [1], [enum for mmap_types]) 
AC_DEFINE([MMAP_TYPE_WIN32], [2], [enum for mmap_types]) 
AC_CHECK_FUNC([mmap], [MMAPTYPE=posix])

dnl Force no mmap for chrome
AS_IF([test "$host_os" = "chrome"], [MMAPTYPE=none])

dnl AC_CHECK_FUNC doesn't seem to work for Win32 API calls...
AS_IF([ test "x$MMAPTYPE" = "x" ],
      [ AS_CASE([ "${host_os}" ],
                [ cygwin*|mingw* ], [ MMAPTYPE=win32],
                [ android* ], [ MMAPTYPE=posix],
                [ MMAPTYPE=none] )])

dnl These numbers must match the definitions above
AS_CASE( [ ${MMAPTYPE} ],
         [ win32 ], [ AC_DEFINE([MMAP_TYPE], [2], [setting an mmap type])],
         [ posix ], [ AC_DEFINE([MMAP_TYPE], [1], [setting an mmap type])],
         [ none ],  [ AC_DEFINE([MMAP_TYPE], [0], [setting an mmap type])])

AC_CHECK_FUNCS([pow])
AC_CHECK_LIB([m], [pow])

dnl Whether or not we have sockets (we don't in Windows)
AC_CHECK_HEADERS([sys/socket.h])


dnl: pkg-config mimic.pc file:
PKGCONFIG_MIMIC_CFLAGS="-I${includedir}/ttsmimic -I${includedir}/ttsmimic/lang"
PKGCONFIG_MIMIC_LIBS="-L${libdir}"
PKGCONFIG_MIMIC_DEPS=

dnl Check ICU
PKG_CHECK_MODULES([ICU], [icu-i18n],
                  [m4_ignore], [m4_ignore])
AC_DEFINE([U_CHARSET_IS_UTF8], [1], [mimic accepts UTF-8 only])

PKGCONFIG_MIMIC_DEPS="${PKGCONFIG_MIMIC_DEPS} icu-i18n"


dnl
dnl determine audio type or use none if none supported on this platform
dnl
AUDIODRIVER=none

dnl check for OSS in FreeBSD and Sun:
AUDIODETECTED_OSS="false"
AC_CHECK_HEADER(machine/soundcard.h,
              [AUDIODRIVER="oss"
               AUDIODETECTED_OSS="true"])
AUDIODEFS_OSS=-DCST_AUDIO_FREEBSD
AUDIOLIBS_OSS=

AUDIODETECTED_SUN="false"
AC_CHECK_HEADER(sys/audioio.h,
              [AUDIODRIVER="sun"
               AUDIODETECTED_SUN="true"
              ])
AUDIODEFS_SUN=-DCST_AUDIO_SUNOS
AUDIOLIBS_SUN=

dnl Check PulseAudio
AUDIODETECTED_PULSEAUDIO="false"
PKG_CHECK_MODULES([PULSEAUDIO], [libpulse],
                  [AUDIODRIVER="pulseaudio"
                   AUDIODETECTED_PULSEAUDIO="true"],
                  [m4_ignore])
AUDIODEFS_PULSEAUDIO="-DCST_AUDIO_PULSEAUDIO ${PULSEAUDIO_CFLAGS}"
AUDIOLIBS_PULSEAUDIO="${PULSEAUDIO_LIBS}"

dnl Check PortAudio is at least (v19-dev)
AUDIODETECTED_PORTAUDIO="false"
PKG_CHECK_MODULES([PORTAUDIO], [portaudio-2.0 >= 19],
                  [AUDIODRIVER="portaudio"
                   AUDIODETECTED_PORTAUDIO="true"],
                  [m4_ignore])
AUDIODEFS_PORTAUDIO="-DCST_AUDIO_PORTAUDIO ${PORTAUDIO_CFLAGS}"
AUDIOLIBS_PORTAUDIO="${PORTAUDIO_LIBS}"

dnl Check for ALSA sound library
AUDIODETECTED_ALSA="false"
PKG_CHECK_MODULES([ALSA], [alsa >= 1.0.11],
                  [AUDIODRIVER="alsa"
                   AUDIODETECTED_ALSA="true"],
                  [m4_ignore])
AUDIODEFS_ALSA="-DCST_AUDIO_ALSA ${ALSA_CFLAGS}"
AUDIOLIBS_ALSA="${ALSA_LIBS}"

dnl check for wince audio
AUDIODETECTED_WINCE="false"
AC_CHECK_HEADERS([mmsystem.h],
                 [AUDIODRIVER="wince"
                  AUDIODETECTED_WINCE="true"],
                 [],
                 AC_INCLUDES_DEFAULT)
AUDIODEFS_WINCE=-DCST_AUDIO_WINCE
AUDIOLIBS_WINCE=-lwinmm

dnl For wince you get wince, for android and chrome you get none:
AS_CASE([ ${host_os} ],
        [ wince* ], [ AUDIODRIVER="wince"],
        [ android* ], [ AUDIODRIVER="none"],
        [ chrome* ], [AUDIODRIVER="none"])

dnl
dnl allow the user to override the one detected above
dnl
AC_ARG_WITH([audio],
   [AS_HELP_STRING([--with-audio],
      [with specific audio support (none alsa portaudio etc) ])],
   [AUDIODRIVER=$with_audio])

AS_CASE( [ ${AUDIODRIVER} ],
         [ alsa ],
           [ AUDIODEFS="${AUDIODEFS_ALSA}"
             AUDIOLIBS="${AUDIOLIBS_ALSA}"
             AUDIODETECTED="${AUDIODETECTED_ALSA}"
             PKGCONFIG_MIMIC_DEPS="${PKGCONFIG_MIMIC_DEPS} alsa"
           ],
         [ pulseaudio ],
           [ AUDIODEFS="${AUDIODEFS_PULSEAUDIO}"
             AUDIOLIBS="${AUDIOLIBS_PULSEAUDIO}"
             AUDIODETECTED="${AUDIODETECTED_PULSEAUDIO}"
             PKGCONFIG_MIMIC_DEPS="${PKGCONFIG_MIMIC_DEPS} libpulse"
           ],
         [ portaudio ],
           [ AUDIODEFS="${AUDIODEFS_PORTAUDIO}"
             AUDIOLIBS="${AUDIOLIBS_PORTAUDIO}"
             AUDIODETECTED="${AUDIODETECTED_PORTAUDIO}"
             PKGCONFIG_MIMIC_DEPS="${PKGCONFIG_MIMIC_DEPS} portaudio-2.0"
	   ],
         [ sun ],
           [ AUDIODEFS="${AUDIODEFS_SUN}"
             AUDIOLIBS="${AUDIOLIBS_SUN}"
             AUDIODETECTED="${AUDIODETECTED_SUN}"
           ],
         [ oss|*bsd ],
           [ AUDIODRIVER=oss
             AUDIODEFS="${AUDIODEFS_OSS}"
             AUDIOLIBS="${AUDIOLIBS_OSS}"
             AUDIODETECTED="${AUDIODETECTED_OSS}"
           ],
         [ wince|win32 ],
           [ AUDIODEFS="${AUDIODEFS_WINCE}"
             AUDIOLIBS="${AUDIOLIBS_WINCE}"
             AUDIODETECTED="${AUDIODETECTED_WINCE}"
           ],
         [ none ],
           [ AUDIODEFS=-DCST_AUDIO_NONE
             AUDIOLIBS=
             AUDIODETECTED="true"
           ])

AS_IF([ test "${AUDIODETECTED}" = "false" ],
      [AC_MSG_WARN([Using $AUDIODRIVER as audio module, it was not autodetected])])

AC_SUBST(AUDIODRIVER)
AC_SUBST(AUDIODEFS)
AC_SUBST(AUDIOLIBS)

dnl
dnl language/lexicon/voice parameters 
dnl
dnl By default all languages, lexicons and voices are compiled
dnl --------------------- Global language switch ---------------------
AC_ARG_ENABLE([lang-all],
  [AS_HELP_STRING([--disable-lang-all],
      [disable all languages support])],
  [],
  [enable_lang_all=yes]) dnl by default all languages are included

AC_ARG_ENABLE([voices-all],
  [AS_HELP_STRING([--disable-voices-all],
      [disable embedding all voices])],
  [],
  [enable_voices_all=yes])

AC_ARG_ENABLE([lang-english],
  [AS_HELP_STRING([--disable-lang-english],
      [disable English language support])],
  [],
  [enable_lang_english=${enable_lang_all}])

AC_ARG_ENABLE([lang-indic],
  [AS_HELP_STRING([--disable-lang-indic],
      [disable Indic language support])],
  [],
  [enable_lang_indic=${enable_lang_all}])

AC_ARG_ENABLE([lang-es],
  [AS_HELP_STRING([--disable-lang-es],
      [disable Spanish language support])],
  [],
  [enable_lang_es=${enable_lang_all}])

dnl -------------------- Language analysis ---------------------------
dnl For US English en-us:
AC_ARG_ENABLE([usenglish],
  [AS_HELP_STRING([--disable-usenglish],
      [disable US English text analysis support])],
  [],
  [enable_usenglish=${enable_lang_english}])

dnl For indic:
AC_ARG_ENABLE([indic_analysis],
  [AS_HELP_STRING([--disable-indic-analysis],
      [disable indic text analysis support])],
  [],
  [enable_indic_analysis=${enable_lang_indic}])

AC_ARG_ENABLE([es_analysis],
  [AS_HELP_STRING([--disable-es-analysis],
      [disable Spanish text analysis support])],
  [],
  [enable_es_analysis=${enable_lang_es}])


dnl ------------------- Lexicons ------------------------------
dnl US Lexicon:
AC_ARG_ENABLE([cmulex],
  [AS_HELP_STRING([--disable-cmulex],
      [disable US English CMU Lexicon support])],
  [],
  [enable_cmulex=${enable_lang_english}])


dnl indic Lexicons:
AC_ARG_ENABLE([indiclex],
  [AS_HELP_STRING([--disable-indiclex],
      [disable indic and grapheme Lexicon support])],
  [],
  [enable_indiclex=${enable_lang_indic}])

dnl ----------------- Voices -------------------------------

AC_ARG_ENABLE([voices-english],
  [AS_HELP_STRING([--disable-voices-english],
      [disable embedding all English voices])],
  [],
  [enable_voices_english=${enable_voices_all}])

dnl cmu_us_kal
AC_ARG_ENABLE([cmu_us_kal],
  [AS_HELP_STRING([--disable-cmu_us_kal],
       [disable cmu_us_kal 8kHz diphone voice])],
  [],
  [enable_cmu_us_kal=${enable_voices_english}])

dnl: Dependencies for cmu_us_kal
AS_IF([test "x$enable_cmu_us_kal" != xno],
  [enable_usenglish="yes"
   enable_cmulex="yes"])

dnl cmu_time_awb
AC_ARG_ENABLE([cmu_time_awb],
  [AS_HELP_STRING([--disable-cmu_time_awb],
       [disable cmu_time_awb clunits voice])],
  [],
  [enable_cmu_time_awb=${enable_voices_english}])

dnl: Dependencies for cmu_time_awb
AS_IF([test "x$enable_cmu_time_awb" != xno],
  [enable_usenglish="yes"
   enable_cmulex="yes"])

dnl cmu_us_kal16
AC_ARG_ENABLE([cmu_us_kal16],
  [AS_HELP_STRING([--disable-cmu_us_kal16],
       [disable cmu_us_kal16 16kHz diphone voice])],
  [],
  [enable_cmu_us_kal16=${enable_voices_english}])

dnl: Dependencies for cmu_us_kal16
AS_IF([test "x$enable_cmu_us_kal16" != xno],
  [enable_usenglish="yes"
   enable_cmulex="yes"])

dnl cmu_us_awb
AC_ARG_ENABLE([cmu_us_awb],
  [AS_HELP_STRING([--disable-cmu_us_awb],
       [disable cmu_us_awb clustergen voice])],
  [],
  [enable_cmu_us_awb=${enable_voices_english}])

dnl: Dependencies for cmu_us_awb
AS_IF([test "x$enable_cmu_us_awb" != xno],
  [enable_usenglish="yes"
   enable_cmulex="yes"])

dnl cmu_us_rms
AC_ARG_ENABLE([cmu_us_rms],
  [AS_HELP_STRING([--disable-cmu_us_rms],
       [disable cmu_us_rms clustergen voice])],
  [],
  [enable_cmu_us_rms=${enable_voices_english}])

dnl: Dependencies for cmu_us_rms
AS_IF([test "x$enable_cmu_us_rms" != xno],
  [enable_usenglish="yes"
   enable_cmulex="yes"])

dnl cmu_us_slt
AC_ARG_ENABLE([cmu_us_slt],
  [AS_HELP_STRING([--disable-cmu_us_slt],
       [disable cmu_us_slt clustergem voice])],
  [],
  [enable_cmu_us_slt=${enable_voices_english}])

dnl: Dependencies for cmu_us_slt
AS_IF([test "x$enable_cmu_us_slt" != xno],
  [enable_usenglish="yes"
   enable_cmulex="yes"])


dnl vid_gb_ap
AC_ARG_ENABLE([vid_gb_ap],
  [AS_HELP_STRING([--disable-vid_gb_ap],
       [disable vid_gb_ap clustergen voice])],
  [],
  [enable_vid_gb_ap=${enable_voices_english}])

dnl: Dependencies for vid_gb_ap
AS_IF([test "x$enable_vid_gb_ap" != xno],
  [enable_usenglish="yes"
   enable_cmulex="yes"])

dnl To generate the pkg-config file .pc we want the libraries in the order
dnl the linker will need them. For that reason, we first list voices, then
dnl lexicons then languages and we append the lib required on each case.

AM_CONDITIONAL([VOICE_CMU_US_KAL], [test "x$enable_cmu_us_kal" != xno])
AS_IF([test "x$enable_cmu_us_kal" != xno ],
  [ AC_DEFINE([ENABLE_CMU_US_KAL], [1], [Enable cmu_us_kal voice])
    PKGCONFIG_MIMIC_LIBS="${PKGCONFIG_MIMIC_LIBS} -lttsmimic_lang_cmu_us_kal"
  ],
  [ AC_DEFINE([ENABLE_CMU_US_KAL], [0], [Enable cmu_us_kal voice]) ])

AM_CONDITIONAL([VOICE_CMU_TIME_AWB], [test "x$enable_cmu_time_awb" != xno])
AS_IF([test "x$enable_cmu_time_awb" != xno ],
  [ AC_DEFINE([ENABLE_CMU_TIME_AWB], [1], [Enable cmu_time_awb voice])
    PKGCONFIG_MIMIC_LIBS="${PKGCONFIG_MIMIC_LIBS} -lttsmimic_lang_cmu_time_awb"
  ],
  [ AC_DEFINE([ENABLE_CMU_TIME_AWB], [0], [Enable cmu_time_awb voice]) ])

AM_CONDITIONAL([VOICE_CMU_US_KAL16], [test "x$enable_cmu_us_kal16" != xno])
AS_IF([test "x$enable_cmu_us_kal16" != xno ],
  [ AC_DEFINE([ENABLE_CMU_US_KAL16], [1], [Enable cmu_us_kal16 voice])
    PKGCONFIG_MIMIC_LIBS="${PKGCONFIG_MIMIC_LIBS} -lttsmimic_lang_cmu_us_kal16"
  ],
  [ AC_DEFINE([ENABLE_CMU_US_KAL16], [0], [Enable cmu_us_kal16 voice]) ])

AM_CONDITIONAL([VOICE_CMU_US_AWB], [test "x$enable_cmu_us_awb" != xno])
AS_IF([test "x$enable_cmu_us_awb" != xno ],
  [ AC_DEFINE([ENABLE_CMU_US_AWB], [1], [Enable cmu_us_awb voice]) 
    PKGCONFIG_MIMIC_LIBS="${PKGCONFIG_MIMIC_LIBS} -lttsmimic_lang_cmu_us_awb"
  ],
  [ AC_DEFINE([ENABLE_CMU_US_AWB], [0], [Enable cmu_us_awb voice]) ])

AM_CONDITIONAL([VOICE_CMU_US_RMS], [test "x$enable_cmu_us_rms" != xno])
AS_IF([test "x$enable_cmu_us_rms" != xno ],
  [ AC_DEFINE([ENABLE_CMU_US_RMS], [1], [Enable cmu_us_rms voice]) 
    PKGCONFIG_MIMIC_LIBS="${PKGCONFIG_MIMIC_LIBS} -lttsmimic_lang_cmu_us_rms"
  ],
  [ AC_DEFINE([ENABLE_CMU_US_RMS], [0], [Enable cmu_us_rms voice]) ])

AM_CONDITIONAL([VOICE_CMU_US_SLT], [test "x$enable_cmu_us_slt" != xno])
AS_IF([test "x$enable_cmu_us_slt" != xno ],
  [ AC_DEFINE([ENABLE_CMU_US_SLT], [1], [Enable cmu_us_slt voice])
    PKGCONFIG_MIMIC_LIBS="${PKGCONFIG_MIMIC_LIBS} -lttsmimic_lang_cmu_us_slt"
  ],
  [ AC_DEFINE([ENABLE_CMU_US_SLT], [0], [Enable cmu_us_slt voice]) ])

AM_CONDITIONAL([VOICE_VID_GB_AP], [test "x$enable_vid_gb_ap" != xno])
AS_IF([test "x$enable_vid_gb_ap" != xno ],
  [ AC_DEFINE([ENABLE_VID_GB_AP], [1], [Enable vid_gb_ap voice])
    PKGCONFIG_MIMIC_LIBS="${PKGCONFIG_MIMIC_LIBS} -lttsmimic_lang_vid_gb_ap"
  ],
  [ AC_DEFINE([ENABLE_VID_GB_AP], [0], [Enable vid_gb_ap voice]) ])

AM_CONDITIONAL([LEX_CMULEX], [test "x$enable_cmulex" != xno])
AS_IF([test "x$enable_cmulex" != xno ],
  [ AC_DEFINE([ENABLE_CMULEX], [1], [Enable CMU US English lexicon])
    PKGCONFIG_MIMIC_LIBS="${PKGCONFIG_MIMIC_LIBS} -lttsmimic_lang_cmulex"
  ],
  [ AC_DEFINE([ENABLE_CMULEX], [0], [Enable CMU US English lexicon]) ])

AM_CONDITIONAL([LEX_INDIC], [test "x$enable_indiclex" != xno])
AS_IF([test "x$enable_indiclex" != xno ],
  [ AC_DEFINE([ENABLE_INDICLEX], [1], [Enable Indic lexicon])
    PKGCONFIG_MIMIC_LIBS="${PKGCONFIG_MIMIC_LIBS} -lttsmimic_lang_cmu_indic_lex"
    PKGCONFIG_MIMIC_LIBS="${PKGCONFIG_MIMIC_LIBS} -lttsmimic_lang_cmu_grapheme_lex"
  ],
  [ AC_DEFINE([ENABLE_INDICLEX], [0], [Enable Indic lexicon]) ])

AM_CONDITIONAL([LANG_INDIC_ANALYSIS],
  [test "x$enable_indic_analysis" != xno])
AS_IF([test "x$enable_indic_analysis" != xno ],
  [ AC_DEFINE([ENABLE_INDIC_ANALYSIS], [1], [Enable Indic text analysis])
    PKGCONFIG_MIMIC_LIBS="${PKGCONFIG_MIMIC_LIBS} -lttsmimic_lang_cmu_grapheme_lang"
    PKGCONFIG_MIMIC_LIBS="${PKGCONFIG_MIMIC_LIBS} -lttsmimic_lang_cmu_indic_lang"
  ],
  [ AC_DEFINE([ENABLE_INDIC_ANALYSIS], [0], [Enable Indic text analysis]) ])

AM_CONDITIONAL([LANG_USENGLISH], [test "x$enable_usenglish" != xno])
AS_IF([test "x$enable_usenglish" != xno ],
  [ AC_DEFINE([ENABLE_USENGLISH], [1], [Enable US English text analysis])
    PKGCONFIG_MIMIC_LIBS="${PKGCONFIG_MIMIC_LIBS} -lttsmimic_lang_usenglish" ],
  [ AC_DEFINE([ENABLE_USENGLISH], [0], [Enable US English text analysis]) ])

AM_CONDITIONAL([LANG_ES_ANALYSIS], [test "x$enable_es_analysis" != xno])
AS_IF([test "x$enable_es_analysis" != xno ],
  [ AC_DEFINE([ENABLE_ES_ANALYSIS], [1], [Enable Spanish text analysis])
    PKGCONFIG_MIMIC_LIBS="${PKGCONFIG_MIMIC_LIBS} -lttsmimic_lang_es" ],
  [ AC_DEFINE([ENABLE_ES_ANALYSIS], [0], [Enable Spanish text analysis]) ])


PKGCONFIG_MIMIC_LIBS="${PKGCONFIG_MIMIC_LIBS} -lttsmimic_lang_all_langs -lttsmimic_lang_all_voices -lttsmimic"

AC_SUBST(PKGCONFIG_MIMIC_CFLAGS)
AC_SUBST(PKGCONFIG_MIMIC_LIBS)
AC_SUBST(PKGCONFIG_MIMIC_DEPS)

AC_SUBST(EXEEXT)

AC_CONFIG_FILES([Makefile mimic.pc])
AC_OUTPUT

AS_IF( [ test "${AUDIODRIVER}" = "none" && test "$with_audio" != "none" ],
  [ AC_MSG_WARN([No audio driver could be determined, and no sound output will be possible. Mimic will work but only be able to write output to disk.])
  ])

