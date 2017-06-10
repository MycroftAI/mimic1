ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS = -Wall -Werror -I$(top_srcdir)/include $(PCRE2_CFLAGS)

SUBDIRS = .

# instead of pkgincludedir we will use $(includedir)/ttsmimic
headersdir = $(includedir)/ttsmimic/
langheaderdir = $(headersdir)/lang/

EXTRA_DIST = ACKNOWLEDGEMENTS COPYING 
noinst_HEADERS = 
headers_HEADERS =
langheader_HEADERS =
lib_LTLIBRARIES =

lib_LTLIBRARIES += libttsmimic.la
libttsmimic_la_SOURCES = 
libttsmimic_la_CFLAGS =
libttsmimic_la_LDFLAGS = -no-undefined
libttsmimic_la_LIBADD = $(PCRE2_LIBS)

libttsmimic_lang_all_langs_la_SOURCES = main/mimic_lang_list.c
libttsmimic_lang_all_langs_la_LDFLAGS = -no-undefined
libttsmimic_lang_all_langs_la_LIBADD = libttsmimic.la

libttsmimic_lang_all_voices_la_SOURCES = main/mimic_voice_list.c
libttsmimic_lang_all_voices_la_LDFLAGS = -no-undefined
libttsmimic_lang_all_voices_la_LIBADD = libttsmimic.la
TESTS = 

pkgconfiginstalldir = $(libdir)/pkgconfig
pkgconfiginstall_DATA = mimic.pc

voicesinstalldir = $(pkgdatadir)/voices
voicesinstall_DATA = 

################## START: LANG_USENGLISH #################################
if LANG_USENGLISH
    lib_LTLIBRARIES += libttsmimic_lang_usenglish.la
    libttsmimic_lang_all_langs_la_LIBADD += libttsmimic_lang_usenglish.la
endif

EXTRA_DIST += \
  lang/usenglish/make_us_regexes \
  lang/usenglish/us_pos.tree

libttsmimic_lang_usenglish_la_SOURCES = \
  lang/usenglish/us_aswd.c \
  lang/usenglish/us_dur_stats.c \
  lang/usenglish/us_durz_cart.c \
  lang/usenglish/us_durz_cart.h \
  lang/usenglish/usenglish.c \
  lang/usenglish/us_expand.c \
  lang/usenglish/us_f0lr.c \
  lang/usenglish/us_f0_model.c \
  lang/usenglish/us_ffeatures.c \
  lang/usenglish/us_gpos.c \
  lang/usenglish/us_int_accent_cart.c \
  lang/usenglish/us_int_accent_cart.h \
  lang/usenglish/us_int_tone_cart.c \
  lang/usenglish/us_int_tone_cart.h \
  lang/usenglish/us_nums_cart.c \
  lang/usenglish/us_nums_cart.h \
  lang/usenglish/us_phoneset.c \
  lang/usenglish/us_phrasing_cart.c \
  lang/usenglish/us_phrasing_cart.h \
  lang/usenglish/us_pos_cart.c \
  lang/usenglish/us_pos_cart.h \
  lang/usenglish/us_text.c

libttsmimic_lang_usenglish_la_LDFLAGS = -no-undefined
libttsmimic_lang_usenglish_la_LIBADD = libttsmimic.la


langheader_HEADERS += \
  lang/usenglish/usenglish.h \
  lang/usenglish/us_f0.h \
  lang/usenglish/us_ffeatures.h \
  lang/usenglish/us_regexes.h \
  lang/usenglish/us_text.h

#regexes:
#	./make_us_regexes $(BINDIR) >us_regexes.h
#
#us_pos_cart.c: 
#	$(ESTDIR)/../festival/bin/festival \
#           -b $(MIMICDIR)/tools/make_cart.scm \
#               $(MIMICDIR)/tools/make_pos.scm 


################## END: LANG_USENGLISH ###################################

#################### START:  LANG_INDIC ##################################
if LANG_INDIC_ANALYSIS
  lib_LTLIBRARIES += libttsmimic_lang_cmu_grapheme_lang.la \
                     libttsmimic_lang_cmu_indic_lang.la
  libttsmimic_lang_all_langs_la_LIBADD += libttsmimic_lang_cmu_grapheme_lang.la \
                                       libttsmimic_lang_cmu_indic_lang.la
endif

langheader_HEADERS += \
    lang/cmu_grapheme_lang/cmu_grapheme_lang.h

libttsmimic_lang_cmu_grapheme_lang_la_LDFLAGS = -no-undefined
libttsmimic_lang_cmu_grapheme_lang_la_LIBADD = libttsmimic.la

libttsmimic_lang_cmu_grapheme_lang_la_SOURCES = \
  lang/cmu_grapheme_lang/cmu_grapheme_phrasing_cart.h \
  lang/cmu_grapheme_lang/cmu_grapheme_lang.c \
  lang/cmu_grapheme_lang/cmu_grapheme_phrasing_cart.c \
  lang/cmu_grapheme_lang/cmu_grapheme_phoneset.c

EXTRA_DIST += lang/cmu_grapheme_lang/make_grapheme_phoneset

langheader_HEADERS += \
  lang/cmu_indic_lang/cmu_indic_lang.h


libttsmimic_lang_cmu_indic_lang_la_LDFLAGS = -no-undefined
libttsmimic_lang_cmu_indic_lang_la_LIBADD = libttsmimic.la libttsmimic_lang_usenglish.la
libttsmimic_lang_cmu_indic_lang_la_SOURCES = \
  lang/cmu_indic_lang/cmu_indic_lang.c \
  lang/cmu_indic_lang/cmu_indic_phoneset.c \
  lang/cmu_indic_lang/cmu_indic_phrasing_cart.h \
  lang/cmu_indic_lang/cmu_indic_phrasing_cart.c


################# END: LANG_INDIC ########################################

############## START: LANG_ES_ANALYSIS #################################
if LANG_ES_ANALYSIS
    lib_LTLIBRARIES += libttsmimic_lang_es.la
    libttsmimic_lang_all_langs_la_LIBADD += libttsmimic_lang_es.la
endif

libttsmimic_lang_es_la_LDFLAGS = -no-undefined
libttsmimic_lang_es_la_LIBADD = libttsmimic.la
libttsmimic_lang_es_la_SOURCES = \
  lang/es_lang/es_init.c \
  lang/es_lang/es_lang.h \
  lang/es_lang/es_text_analysis.c \
  lang/es_lang/es_tokenizer.c

langheader_HEADERS += \
  lang/es_lang/es_lang.h

############## END: LANG_ES_ANALYSIS  ##################################

############## START: LEX_CMULEX #########################################
if LEX_CMULEX
    lib_LTLIBRARIES += libttsmimic_lang_cmulex.la
    libttsmimic_lang_all_langs_la_LIBADD += libttsmimic_lang_cmulex.la
endif

libttsmimic_lang_cmulex_la_LDFLAGS = -no-undefined
libttsmimic_lang_cmulex_la_LIBADD = libttsmimic.la
libttsmimic_lang_cmulex_la_SOURCES = \
  lang/cmulex/cmu_lex.c \
  lang/cmulex/cmu_lex_data.c \
  lang/cmulex/cmu_lex_entries.c \
  lang/cmulex/cmu_lts_model.c \
  lang/cmulex/cmu_lts_model.h \
  lang/cmulex/cmu_lts_rules.c \
  lang/cmulex/cmu_postlex.c

EXTRA_DIST += \
  lang/cmulex/cmu_lex_data_raw.c \
  lang/cmulex/cmu_lex_entries_huff_table.c \
  lang/cmulex/cmu_lex_num_bytes.c \
  lang/cmulex/cmu_lex_phones_huff_table.c \
  lang/cmulex/cmu_lts_rules.c \
  lang/cmulex/make_cmulex \
  lang/cmulex/make_cmulex_helper.py
 
langheader_HEADERS += \
  lang/cmulex/cmu_lex.h

############## END: LEX_CMULEX ###########################################

############# START: Lexicon for indic: #################################
if LEX_INDIC
    lib_LTLIBRARIES += libttsmimic_lang_cmu_indic_lex.la \
                       libttsmimic_lang_cmu_grapheme_lex.la
    libttsmimic_lang_all_langs_la_LIBADD += libttsmimic_lang_cmu_indic_lex.la \
                                         libttsmimic_lang_cmu_grapheme_lex.la
endif

langheader_HEADERS += \
  lang/cmu_grapheme_lex/cmu_grapheme_lex.h

libttsmimic_lang_cmu_grapheme_lex_la_LDFLAGS = -no-undefined
libttsmimic_lang_cmu_grapheme_lex_la_LIBADD = libttsmimic.la
libttsmimic_lang_cmu_grapheme_lex_la_SOURCES = \
  lang/cmu_grapheme_lex/cmu_grapheme_lex.c \
  lang/cmu_grapheme_lex/grapheme_unitran_tables.c

# mapping:
#	${ESTDIR}/../festival/bin/festival \
#         -b ${TOP}/tools/make_ug.scm \
#            ${FESTVOXDIR}/src/grapheme/unicode_sampa_mapping.scm '(doit)'

libttsmimic_lang_cmu_indic_lex_la_LDFLAGS = -no-undefined
libttsmimic_lang_cmu_indic_lex_la_LIBADD = libttsmimic.la \
                                        libttsmimic_lang_cmulex.la \
                                        libttsmimic_lang_cmu_indic_lang.la
libttsmimic_lang_cmu_indic_lex_la_SOURCES = \
  lang/cmu_indic_lex/cmu_indic_lex.c

langheader_HEADERS += \
  lang/cmu_indic_lex/cmu_indic_lex.h

# END: Lexicon for indic ####################

if VOICE_CMU_US_KAL
    lib_LTLIBRARIES += libttsmimic_lang_cmu_us_kal.la
    libttsmimic_lang_all_voices_la_LIBADD += libttsmimic_lang_cmu_us_kal.la
endif

libttsmimic_lang_cmu_us_kal_la_LDFLAGS = -no-undefined
libttsmimic_lang_cmu_us_kal_la_SOURCES = \
  lang/cmu_us_kal/cmu_us_kal.c \
  lang/cmu_us_kal/cmu_us_kal_diphone.c \
  lang/cmu_us_kal/cmu_us_kal_lpc.c \
  lang/cmu_us_kal/cmu_us_kal_res.c \
  lang/cmu_us_kal/cmu_us_kal_residx.c \
  lang/cmu_us_kal/cmu_us_kal_ressize.c \
  lang/cmu_us_kal/voxdefs.h

libttsmimic_lang_cmu_us_kal_la_CFLAGS = \
  -I$(top_srcdir)/lang/usenglish \
  -I$(top_srcdir)/lang/cmulex

libttsmimic_lang_cmu_us_kal_la_LIBADD = \
  libttsmimic.la \
  libttsmimic_lang_cmulex.la \
  libttsmimic_lang_usenglish.la


############

if VOICE_CMU_TIME_AWB
    lib_LTLIBRARIES += libttsmimic_lang_cmu_time_awb.la
    libttsmimic_lang_all_voices_la_LIBADD += libttsmimic_lang_cmu_time_awb.la
endif

libttsmimic_lang_cmu_time_awb_la_LDFLAGS = -no-undefined
libttsmimic_lang_cmu_time_awb_la_SOURCES = \
  lang/cmu_time_awb/cmu_time_awb.c \
  lang/cmu_time_awb/cmu_time_awb_cart.c \
  lang/cmu_time_awb/cmu_time_awb_clunits.c \
  lang/cmu_time_awb/cmu_time_awb_lex_entry.c \
  lang/cmu_time_awb/cmu_time_awb_lpc.c \
  lang/cmu_time_awb/cmu_time_awb_mcep.c \
  lang/cmu_time_awb/voxdefs.h

libttsmimic_lang_cmu_time_awb_la_CFLAGS = \
  -I$(top_srcdir)/lang/usenglish \
  -I$(top_srcdir)/lang/cmulex

libttsmimic_lang_cmu_time_awb_la_LIBADD = \
  libttsmimic.la \
  libttsmimic_lang_cmulex.la \
  libttsmimic_lang_usenglish.la

##########

if VOICE_CMU_US_KAL16
    lib_LTLIBRARIES += libttsmimic_lang_cmu_us_kal16.la
    libttsmimic_lang_all_voices_la_LIBADD += libttsmimic_lang_cmu_us_kal16.la
endif

libttsmimic_lang_cmu_us_kal16_la_LDFLAGS = -no-undefined
libttsmimic_lang_cmu_us_kal16_la_SOURCES = \
  lang/cmu_us_kal16/cmu_us_kal16.c \
  lang/cmu_us_kal16/cmu_us_kal16_diphone.c \
  lang/cmu_us_kal16/cmu_us_kal16_lpc.c \
  lang/cmu_us_kal16/cmu_us_kal16_res.c \
  lang/cmu_us_kal16/cmu_us_kal16_residx.c \
  lang/cmu_us_kal16/voxdefs.h

libttsmimic_lang_cmu_us_kal16_la_CFLAGS = \
  -I$(top_srcdir)/lang/usenglish \
  -I$(top_srcdir)/lang/cmulex


libttsmimic_lang_cmu_us_kal16_la_LIBADD = \
  libttsmimic.la \
  libttsmimic_lang_cmulex.la \
  libttsmimic_lang_usenglish.la

####################

if VOICE_CMU_US_AWB
    lib_LTLIBRARIES += libttsmimic_lang_cmu_us_awb.la
    libttsmimic_lang_all_voices_la_LIBADD += libttsmimic_lang_cmu_us_awb.la
endif

libttsmimic_lang_cmu_us_awb_la_LDFLAGS = -no-undefined
libttsmimic_lang_cmu_us_awb_la_SOURCES = \
  lang/cmu_us_awb/cmu_us_awb.c \
  lang/cmu_us_awb/cmu_us_awb_cg.c \
  lang/cmu_us_awb/cmu_us_awb_cg_durmodel.c \
  lang/cmu_us_awb/cmu_us_awb_cg_durmodel.h \
  lang/cmu_us_awb/cmu_us_awb_cg_f0_trees.c \
  lang/cmu_us_awb/cmu_us_awb_cg_f0_trees.h \
  lang/cmu_us_awb/cmu_us_awb_cg_phonestate.c \
  lang/cmu_us_awb/cmu_us_awb_cg_single_mcep_trees.c \
  lang/cmu_us_awb/cmu_us_awb_cg_single_mcep_trees.h \
  lang/cmu_us_awb/cmu_us_awb_cg_single_params.c \
  lang/cmu_us_awb/cmu_us_awb_spamf0_accent.c \
  lang/cmu_us_awb/cmu_us_awb_spamf0_accent.h \
  lang/cmu_us_awb/cmu_us_awb_spamf0_accent_params.c \
  lang/cmu_us_awb/cmu_us_awb_spamf0_phrase.c \
  lang/cmu_us_awb/cmu_us_awb_spamf0_phrase.h \
  lang/cmu_us_awb/voxdefs.h

libttsmimic_lang_cmu_us_awb_la_CFLAGS = \
  -I$(top_srcdir)/lang/usenglish \
  -I$(top_srcdir)/lang/cmulex

libttsmimic_lang_cmu_us_awb_la_LIBADD = \
  libttsmimic.la \
  libttsmimic_lang_cmulex.la \
  libttsmimic_lang_usenglish.la


###########

if VOICE_CMU_US_RMS
    lib_LTLIBRARIES += libttsmimic_lang_cmu_us_rms.la
    libttsmimic_lang_all_voices_la_LIBADD += libttsmimic_lang_cmu_us_rms.la
endif

libttsmimic_lang_cmu_us_rms_la_LDFLAGS = -no-undefined
libttsmimic_lang_cmu_us_rms_la_SOURCES = \
  lang/cmu_us_rms/cmu_us_rms.c \
  lang/cmu_us_rms/cmu_us_rms_cg.c \
  lang/cmu_us_rms/cmu_us_rms_cg_durmodel.c \
  lang/cmu_us_rms/cmu_us_rms_cg_durmodel.h \
  lang/cmu_us_rms/cmu_us_rms_cg_f0_trees.c \
  lang/cmu_us_rms/cmu_us_rms_cg_f0_trees.h \
  lang/cmu_us_rms/cmu_us_rms_cg_phonestate.c \
  lang/cmu_us_rms/cmu_us_rms_cg_single_mcep_trees.c \
  lang/cmu_us_rms/cmu_us_rms_cg_single_mcep_trees.h \
  lang/cmu_us_rms/cmu_us_rms_cg_single_params.c \
  lang/cmu_us_rms/cmu_us_rms_spamf0_accent.c \
  lang/cmu_us_rms/cmu_us_rms_spamf0_accent.h \
  lang/cmu_us_rms/cmu_us_rms_spamf0_accent_params.c \
  lang/cmu_us_rms/cmu_us_rms_spamf0_phrase.c \
  lang/cmu_us_rms/cmu_us_rms_spamf0_phrase.h \
  lang/cmu_us_rms/voxdefs.h

libttsmimic_lang_cmu_us_rms_la_CFLAGS = \
  -I$(top_srcdir)/lang/usenglish \
  -I$(top_srcdir)/lang/cmulex

libttsmimic_lang_cmu_us_rms_la_LIBADD = \
  libttsmimic.la \
  libttsmimic_lang_cmulex.la \
  libttsmimic_lang_usenglish.la

###############

if VOICE_CMU_US_SLT
    lib_LTLIBRARIES += libttsmimic_lang_cmu_us_slt.la
    libttsmimic_lang_all_voices_la_LIBADD += libttsmimic_lang_cmu_us_slt.la
endif

libttsmimic_lang_cmu_us_slt_la_LDFLAGS = -no-undefined
libttsmimic_lang_cmu_us_slt_la_SOURCES = \
  lang/cmu_us_slt/cmu_us_slt.c \
  lang/cmu_us_slt/cmu_us_slt_cg.c \
  lang/cmu_us_slt/cmu_us_slt_cg_durmodel.c \
  lang/cmu_us_slt/cmu_us_slt_cg_durmodel.h \
  lang/cmu_us_slt/cmu_us_slt_cg_f0_trees.c \
  lang/cmu_us_slt/cmu_us_slt_cg_f0_trees.h \
  lang/cmu_us_slt/cmu_us_slt_cg_phonestate.c \
  lang/cmu_us_slt/cmu_us_slt_cg_single_mcep_trees.c \
  lang/cmu_us_slt/cmu_us_slt_cg_single_mcep_trees.h \
  lang/cmu_us_slt/cmu_us_slt_cg_single_params.c \
  lang/cmu_us_slt/cmu_us_slt_spamf0_accent.c \
  lang/cmu_us_slt/cmu_us_slt_spamf0_accent.h \
  lang/cmu_us_slt/cmu_us_slt_spamf0_accent_params.c \
  lang/cmu_us_slt/cmu_us_slt_spamf0_phrase.c \
  lang/cmu_us_slt/cmu_us_slt_spamf0_phrase.h \
  lang/cmu_us_slt/voxdefs.h

libttsmimic_lang_cmu_us_slt_la_CFLAGS = \
  -I$(top_srcdir)/lang/usenglish \
  -I$(top_srcdir)/lang/cmulex

libttsmimic_lang_cmu_us_slt_la_LIBADD = \
  libttsmimic.la \
  libttsmimic_lang_cmulex.la \
  libttsmimic_lang_usenglish.la

###############

if VOICE_CMU_US_SLT_HTS
    lib_LTLIBRARIES += libttsmimic_lang_cmu_us_slt_hts.la
    libttsmimic_lang_all_voices_la_LIBADD += libttsmimic_lang_cmu_us_slt_hts.la
    voicesinstall_DATA += voices/cmu_us_slt_hts.htsvoice
else
    EXTRA_DIST += voices/cmu_us_slt_hts.htsvoice
endif

libttsmimic_lang_cmu_us_slt_hts_la_LDFLAGS = -no-undefined
libttsmimic_lang_cmu_us_slt_hts_la_SOURCES = \
  lang/cmu_us_slt_hts/cmu_us_slt_hts.c \
  lang/cmu_us_slt_hts/voxdefs.h

libttsmimic_lang_cmu_us_slt_hts_la_CFLAGS = \
  -I$(top_srcdir)/lang/usenglish \
  -I$(top_srcdir)/lang/cmulex

libttsmimic_lang_cmu_us_slt_hts_la_LIBADD = \
  libttsmimic.la \
  libttsmimic_lang_cmulex.la \
  libttsmimic_lang_usenglish.la


###########

if VOICE_VID_GB_AP
    lib_LTLIBRARIES += libttsmimic_lang_vid_gb_ap.la
    libttsmimic_lang_all_voices_la_LIBADD += libttsmimic_lang_vid_gb_ap.la
endif

libttsmimic_lang_vid_gb_ap_la_LDFLAGS = -no-undefined
libttsmimic_lang_vid_gb_ap_la_SOURCES = \
  lang/vid_gb_ap/vid_gb_ap.c \
  lang/vid_gb_ap/vid_gb_ap_cg_01_mcep_trees.c \
  lang/vid_gb_ap/vid_gb_ap_cg_01_mcep_trees.h \
  lang/vid_gb_ap/vid_gb_ap_cg_01_params.c \
  lang/vid_gb_ap/vid_gb_ap_cg_08_durmodel.c \
  lang/vid_gb_ap/vid_gb_ap_cg_08_durmodel.h \
  lang/vid_gb_ap/vid_gb_ap_cg_10_durmodel.c \
  lang/vid_gb_ap/vid_gb_ap_cg_10_durmodel.h \
  lang/vid_gb_ap/vid_gb_ap_cg_12_mcep_trees.c \
  lang/vid_gb_ap/vid_gb_ap_cg_12_mcep_trees.h \
  lang/vid_gb_ap/vid_gb_ap_cg_12_params.c \
  lang/vid_gb_ap/vid_gb_ap_cg_17_durmodel.c \
  lang/vid_gb_ap/vid_gb_ap_cg_17_durmodel.h \
  lang/vid_gb_ap/vid_gb_ap_cg_17_mcep_trees.c \
  lang/vid_gb_ap/vid_gb_ap_cg_17_mcep_trees.h \
  lang/vid_gb_ap/vid_gb_ap_cg_17_params.c \
  lang/vid_gb_ap/vid_gb_ap_cg.c \
  lang/vid_gb_ap/vid_gb_ap_cg_f0_trees.c \
  lang/vid_gb_ap/vid_gb_ap_cg_f0_trees.h \
  lang/vid_gb_ap/vid_gb_ap_cg_phonestate.c \
  lang/vid_gb_ap/voxdefs.h

libttsmimic_lang_vid_gb_ap_la_CFLAGS = \
  -I$(top_srcdir)/lang/usenglish \
  -I$(top_srcdir)/lang/cmulex

libttsmimic_lang_vid_gb_ap_la_LIBADD = \
  libttsmimic.la \
  libttsmimic_lang_cmulex.la \
  libttsmimic_lang_usenglish.la

######### END VOICES ######################################

###### src/audio ###############
# TODO: Change to a libtool convenience library to specify custom CFLAGS

src_audio_SOURCES = \
  src/audio/au_alsa.c \
  src/audio/auclient.c \
  src/audio/au_command.c \
  src/audio/audio.c \
  src/audio/au_none.c \
  src/audio/au_oss.c \
  src/audio/au_portaudio.c \
  src/audio/au_pulseaudio.c \
  src/audio/auserver.c \
  src/audio/au_streaming.c \
  src/audio/au_sun.c \
  src/audio/au_wince.c \
  src/audio/native_audio.h

libttsmimic_la_CFLAGS += $(AUDIODEFS) -DPKGDATADIR='"$(pkgdatadir)"'
libttsmimic_la_SOURCES += $(src_audio_SOURCES)

libttsmimic_la_LIBADD += $(AUDIOLIBS) $(HTS_LIBS)

###### src/cg ##################
libttsmimic_la_SOURCES += \
  src/cg/cst_mlsa.h \
  src/cg/cst_mlpg.h \
  src/cg/cst_vc.h \
  src/cg/cst_cg_map.h \
  src/cg/cst_spamf0.h \
  src/cg/cst_cg.c \
  src/cg/cst_mlsa.c \
  src/cg/cst_mlpg.c \
  src/cg/cst_vc.c \
  src/cg/cst_cg_load_voice.c \
  src/cg/cst_cg_dump_voice.c \
  src/cg/cst_cg_map.c \
  src/cg/cst_spamf0.c

####### src/hrg ###############
libttsmimic_la_SOURCES += \
  src/hrg/cst_utterance.c \
  src/hrg/cst_relation.c \
  src/hrg/cst_item.c \
  src/hrg/cst_ffeature.c \
  src/hrg/cst_rel_io.c

####### src/hts ###############
libttsmimic_la_SOURCES += \
  src/hts/flite_hts_engine.c

if HTSEngineBundled
  AM_CPPFLAGS += -I$(top_srcdir)/src/hts/hts_engine_API/include
  headers_HEADERS +=src/hts/hts_engine_API/include/HTS_engine.h
  libttsmimic_la_SOURCES += \
    src/hts/hts_engine_API/lib/HTS_audio.c \
    src/hts/hts_engine_API/lib/HTS_engine.c \
    src/hts/hts_engine_API/lib/HTS_hidden.h \
    src/hts/hts_engine_API/lib/HTS_misc.c  \
    src/hts/hts_engine_API/lib/HTS_pstream.c \
    src/hts/hts_engine_API/lib/HTS_sstream.c \
    src/hts/hts_engine_API/lib/HTS_model.c \
    src/hts/hts_engine_API/lib/HTS_vocoder.c \
    src/hts/hts_engine_API/lib/HTS_gstream.c \
    src/hts/hts_engine_API/lib/HTS_label.c
else
  libttsmimic_la_LIBADD += $(HTS_LIBS)
endif

###### src/lexicon #########

libttsmimic_la_SOURCES += \
  src/lexicon/cst_lexicon.c \
  src/lexicon/cst_lts.c \
  src/lexicon/cst_lts_rewrites.c


###### src/regex ########
libttsmimic_la_SOURCES += \
  src/regex/cst_regex.c \
  src/regex/cst_regex_defs.h \
  src/regex/regexp.c \
  src/regex/regsub.c

#regexes:
#	./make_cst_regexes $(BINDIR) >cst_regex_defs.h

###### src/speech #########
libttsmimic_la_SOURCES += \
  src/speech/cst_lpcres.c \
  src/speech/cst_track.c \
  src/speech/cst_track_io.c \
  src/speech/cst_wave.c \
  src/speech/cst_wave_io.c \
  src/speech/cst_wave_utils.c \
  src/speech/g721.c \
  src/speech/g723_24.c \
  src/speech/g723_40.c \
  src/speech/g72x.c \
  src/speech/g72x.h \
  src/speech/rateconv.c

###### src/stats #########
libttsmimic_la_SOURCES += \
  src/stats/cst_cart.c \
  src/stats/cst_ss.c \
  src/stats/cst_viterbi.c

###### src/synth #########
libttsmimic_la_SOURCES += \
  src/synth/cst_ffeatures.c \
  src/synth/cst_phoneset.c \
  src/synth/cst_ssml.c \
  src/synth/cst_synth.c \
  src/synth/cst_utt_utils.c \
  src/synth/cst_voice.c \
  src/synth/mimic.c

###### src/utils #########
libttsmimic_la_SOURCES += \
  src/utils/cst_alloc.c \
  src/utils/cst_args.c \
  src/utils/cst_endian.c \
  src/utils/cst_error.c \
  src/utils/cst_features.c \
  src/utils/cst_file_stdio.c \
  src/utils/cst_mmap_none.c \
  src/utils/cst_mmap_posix.c \
  src/utils/cst_mmap_win32.c \
  src/utils/cst_socket.c \
  src/utils/cst_string.c \
  src/utils/cst_tokenstream.c \
  src/utils/cst_uregex.c \
  src/utils/cst_url.c \
  src/utils/cst_val.c \
  src/utils/cst_val_const.c \
  src/utils/cst_val_user.c \
  src/utils/cst_wchar.c

###### src/wavesynth #########
libttsmimic_la_SOURCES += \
  src/wavesynth/cst_clunits.c \
  src/wavesynth/cst_diphone.c \
  src/wavesynth/cst_reflpc.c \
  src/wavesynth/cst_sigpr.c \
  src/wavesynth/cst_sts.c \
  src/wavesynth/cst_units.c

###############################

libttsmimic_la_LIBADD += $(AUDIOLIBS) -lm


################# main ########################

bin_PROGRAMS = mimic compile_regexes mimicvox_info
if LANG_USENGLISH
if LEX_CMULEX
  bin_PROGRAMS += t2p mimic_time
endif
endif

mimic_SOURCES = main/mimic_main.c
mimic_LDADD = libttsmimic_lang_all_langs.la libttsmimic_lang_all_voices.la libttsmimic.la -lm

compile_regexes_SOURCES = main/compile_regexes.c
compile_regexes_LDADD = libttsmimic.la
t2p_SOURCES = main/t2p_main.c
t2p_LDADD = libttsmimic.la libttsmimic_lang_cmulex.la libttsmimic_lang_usenglish.la

mimic_time_SOURCES = main/mimic_time_main.c
mimic_time_LDADD = libttsmimic.la \
  libttsmimic_lang_cmulex.la \
  libttsmimic_lang_usenglish.la \
  libttsmimic_lang_cmu_time_awb.la

mimicvox_info_SOURCES = \
  main/mimicvox_info_main.c
mimicvox_info_LDADD = libttsmimic.la libttsmimic_lang_all_langs.la libttsmimic_lang_all_voices.la

################# END: main ###################
dist_noinst_SCRIPTS = autogen.sh

lib_LTLIBRARIES += libttsmimic_lang_all_langs.la
lib_LTLIBRARIES += libttsmimic_lang_all_voices.la


########## Unit tests #########################
noinst_HEADERS += unittests/cutest.h

myunittests = unittests/hrg_test \
              unittests/regex_test \
              unittests/string_test \
              unittests/token_test \
              unittests/voice_select \
              unittests/wave_test

unittests_hrg_test_SOURCES = unittests/hrg_test_main.c
unittests_hrg_test_LDADD = libttsmimic.la

if LEX_CMULEX
if LANG_USENGLISH
  myunittests += unittests/lex_test unittests/lts_test unittests/nums_test
endif
endif

if LANG_ES_ANALYSIS
  myunittests += unittests/es_tokenstream_test
  unittests_es_tokenstream_test_SOURCES = unittests/es_tokenstream_test_main.c
  unittests_es_tokenstream_test_CFLAGS = \
    -DTEST_FILE=\"$(top_srcdir)/unittests/es_text.txt\" \
    -I$(top_srcdir)/lang/es_lang
  unittests_es_tokenstream_test_LDADD = libttsmimic.la \
                                        libttsmimic_lang_es.la
endif


check_PROGRAMS = $(myunittests)

unittests_lex_test_SOURCES = unittests/lex_test_main.c
unittests_lex_test_LDADD = libttsmimic.la \
                           libttsmimic_lang_cmulex.la \
                           libttsmimic_lang_all_langs.la

unittests_lts_test_SOURCES = unittests/lts_test_main.c
unittests_lts_test_LDADD = libttsmimic.la \
                           libttsmimic_lang_cmulex.la \
                           libttsmimic_lang_all_langs.la

unittests_nums_test_SOURCES = unittests/nums_test_main.c
unittests_nums_test_CFLAGS = -I$(top_srcdir)/lang/usenglish
unittests_nums_test_LDADD = libttsmimic.la \
                            libttsmimic_lang_cmulex.la \
                            libttsmimic_lang_usenglish.la \
                            libttsmimic_lang_all_langs.la

unittests_regex_test_SOURCES = unittests/regex_test_main.c
unittests_regex_test_LDADD = libttsmimic.la

unittests_string_test_SOURCES = unittests/string_test_main.c
unittests_string_test_LDADD = libttsmimic.la $(PCRE2_LIBS)


unittests_token_test_SOURCES = unittests/token_test_main.c
unittests_token_test_CFLAGS = -DTEST_FILE=\"$(top_srcdir)/unittests/data.one\" \
                              -DTEST_FILE_UTF8=\"$(top_srcdir)/unittests/data_utf8.txt\"
unittests_token_test_LDADD = libttsmimic.la

unittests_voice_select_SOURCES = unittests/voice_select_test_main.c
unittests_voice_select_CFLAGS = -DVOICE_LIST_DIR=\"$(top_srcdir)/voices\" \
                                -DA_VOICE=\"$(top_srcdir)/voices/cmu_us_rms.flitevox\" 
unittests_voice_select_LDADD = libttsmimic.la libttsmimic_lang_all_langs.la libttsmimic_lang_all_voices.la

unittests_wave_test_SOURCES = unittests/wave_test_main.c
unittests_wave_test_LDADD = libttsmimic.la \
                            libttsmimic_lang_all_langs.la \
                            libttsmimic_lang_all_voices.la
unittests_wave_test_CFLAGS = -DVOICE_LIST_DIR=\"$(top_srcdir)/voices\" \
                             -DA_VOICE=\"$(top_srcdir)/voices/cmu_us_rms.flitevox\" 


EXTRA_DIST += unittests/data.one unittests/data_utf8.txt

########## END Unit tests #########################

########## START testsuite #########################
check_PROGRAMS += \
  testsuite/asciiS2U \
  testsuite/asciiU2S \
  testsuite/bin2ascii

if VOICE_CMU_US_KAL
  check_PROGRAMS += testsuite/by_word
endif

check_PROGRAMS += \
  testsuite/combine_waves \
  testsuite/compare_wave

if VOICE_CMU_US_KAL
  check_PROGRAMS += testsuite/kal_test
endif

if LEX_CMULEX
if LANG_USENGLISH
check_PROGRAMS += \
  testsuite/lex_lookup
endif
endif

check_PROGRAMS += \
  testsuite/lpc_resynth \
  testsuite/lpc_test2 \
  testsuite/lpc_test

if VOICE_CMU_US_SLT
  check_PROGRAMS += testsuite/multi_thread
endif

check_PROGRAMS += \
  testsuite/play_client \
  testsuite/play_server \
  testsuite/play_sync \
  testsuite/play_wave \
  testsuite/rfc \
  testsuite/time_voice_load

if LEX_CMULEX
  check_PROGRAMS += testsuite/utt_test
endif
testsuite_asciiS2U_SOURCES = testsuite/asciiS2U_main.c
testsuite_asciiS2U_LDADD = libttsmimic.la

testsuite_asciiU2S_SOURCES = testsuite/asciiU2S_main.c
testsuite_asciiU2S_LDADD = libttsmimic.la

testsuite_bin2ascii_SOURCES = testsuite/bin2ascii_main.c
testsuite_bin2ascii_LDADD = libttsmimic.la

testsuite_by_word_SOURCES = testsuite/by_word_main.c
testsuite_by_word_LDADD = libttsmimic.la \
                          libttsmimic_lang_all_langs.la \
                          libttsmimic_lang_all_voices.la \
                          libttsmimic_lang_cmu_us_kal.la

testsuite_combine_waves_SOURCES = testsuite/combine_waves_main.c
testsuite_combine_waves_LDADD = libttsmimic.la

testsuite_compare_wave_SOURCES = testsuite/compare_wave_main.c
testsuite_compare_wave_LDADD = libttsmimic.la

testsuite_kal_test_SOURCES = testsuite/kal_test_main.c
testsuite_kal_test_LDADD = libttsmimic.la \
                           libttsmimic_lang_all_langs.la \
                           libttsmimic_lang_all_voices.la \
                           libttsmimic_lang_cmu_us_kal.la

testsuite_lex_lookup_SOURCES = testsuite/lex_lookup_main.c
testsuite_lex_lookup_LDADD = libttsmimic.la libttsmimic_lang_all_langs.la \
                             libttsmimic_lang_cmulex.la \
                             libttsmimic_lang_usenglish.la

testsuite_lpc_resynth_SOURCES = testsuite/lpc_resynth_main.c
testsuite_lpc_resynth_LDADD = libttsmimic.la

testsuite_lpc_test2_SOURCES = testsuite/lpc_test2_main.c
testsuite_lpc_test2_LDADD = libttsmimic.la

testsuite_lpc_test_SOURCES = testsuite/lpc_test_main.c
testsuite_lpc_test_LDADD = libttsmimic.la

testsuite_multi_thread_SOURCES = testsuite/multi_thread_main.c
testsuite_multi_thread_CFLAGS = $(OPENMP_CFLAGS)
testsuite_multi_thread_LDADD = libttsmimic.la \
                               libttsmimic_lang_all_langs.la \
                               libttsmimic_lang_all_voices.la \
                               libttsmimic_lang_cmu_us_slt.la

testsuite_play_client_SOURCES = testsuite/play_client_main.c
testsuite_play_client_LDADD = libttsmimic.la

testsuite_play_server_SOURCES = testsuite/play_server_main.c
testsuite_play_server_LDADD = libttsmimic.la

testsuite_play_sync_SOURCES = testsuite/play_sync_main.c
testsuite_play_sync_LDADD = libttsmimic.la

testsuite_play_wave_SOURCES = testsuite/play_wave_main.c
testsuite_play_wave_LDADD = libttsmimic.la

testsuite_rfc_SOURCES = testsuite/rfc_main.c
testsuite_rfc_LDADD = libttsmimic.la

testsuite_utt_test_SOURCES = testsuite/utt_test_main.c
testsuite_utt_test_LDADD = libttsmimic.la libttsmimic_lang_all_langs.la libttsmimic_lang_cmulex.la

testsuite_time_voice_load_SOURCES = testsuite/time_voice_load.c
testsuite_time_voice_load_LDADD = libttsmimic_lang_all_langs.la libttsmimic_lang_all_voices.la libttsmimic.la -lm

TESTS += $(myunittests)
# "testsuite/multi_thread_run.sh"

headers_HEADERS += \
  include/cst_alloc.h \
  include/cst_args.h \
  include/cst_audio.h \
  include/cst_cart.h \
  include/cst_cg.h \
  include/cst_clunits.h \
  include/config.h \
  include/cst_diphone.h \
  include/cst_endian.h \
  include/cst_error.h \
  include/cst_features.h \
  include/cst_ffeatures.h \
  include/cst_file.h \
  include/cst_hrg.h \
  include/cst_item.h \
  include/cst_lexicon.h \
  include/cst_lts.h \
  include/cst_lts_rewrites.h \
  include/cst_phoneset.h \
  include/cst_regex.h \
  include/cst_relation.h \
  include/cst_sigpr.h \
  include/cst_socket.h \
  include/cst_ss.h \
  include/cst_string.h \
  include/cst_sts.h \
  include/cst_synth.h \
  include/cst_tokenstream.h \
  include/cst_track.h \
  include/cst_units.h \
  include/cst_uregex.h \
  include/cst_utterance.h \
  include/cst_utt_utils.h \
  include/cst_val_const.h \
  include/cst_val_defs.h \
  include/cst_val.h \
  include/cst_viterbi.h \
  include/cst_voice.h \
  include/cst_wave.h \
  include/cst_wchar.h \
  include/flite_hts_engine.h \
  include/mimic.h

# Documentation
dist_man1_MANS = man/man1/mimic.1

clean-local:
	find . -name '*.gcno' -exec rm {} \;
	find . -name '*.gcda' -exec rm {} \;
	find . -name '*.gcov' -exec rm {} \;

