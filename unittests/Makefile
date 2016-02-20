###########################################################################
##                                                                       ##
##                  Language Technologies Institute                      ##
##                     Carnegie Mellon University                        ##
##                        Copyright (c) 1999                             ##
##                        All Rights Reserved.                           ##
##                                                                       ##
##  Permission is hereby granted, free of charge, to use and distribute  ##
##  this software and its documentation without restriction, including   ##
##  without limitation the rights to use, copy, modify, merge, publish,  ##
##  distribute, sublicense, and/or sell copies of this work, and to      ##
##  permit persons to whom this work is furnished to do so, subject to   ##
##  the following conditions:                                            ##
##   1. The code must retain the above copyright notice, this list of    ##
##      conditions and the following disclaimer.                         ##
##   2. Any modifications must be clearly marked as such.                ##
##   3. Original authors' names are not deleted.                         ##
##   4. The authors' names are not used to endorse or promote products   ##
##      derived from this software without specific prior written        ##
##      permission.                                                      ##
##                                                                       ##
##  CARNEGIE MELLON UNIVERSITY AND THE CONTRIBUTORS TO THIS WORK         ##
##  DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING      ##
##  ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT   ##
##  SHALL CARNEGIE MELLON UNIVERSITY NOR THE CONTRIBUTORS BE LIABLE      ##
##  FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES    ##
##  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN   ##
##  AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,          ##
##  ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF       ##
##  THIS SOFTWARE.                                                       ##
##                                                                       ##
###########################################################################
##                                                                       ##
##    Testsuite                                                          ##
##                                                                       ##
###########################################################################
TOP=..
DIRNAME=unittests
BUILD_DIRS = 
ALL_DIRS=
DATAFILES=data.one
SRCS = token_test_main.c hrg_test_main.c \
       regex_test_main.c nums_test_main.c \
       lex_test_main.c utt_test_main.c lpc_test_main.c \
       lpc_test2_main.c

OTHERS = kal_test_main.c

FILES = Makefile $(SRCS) $(DATAFILES) $(OTHERS)

LOCAL_INCLUDES = -I$(TOP)/lang/usenglish

MAIN_EXECS = $(SRCS:_main.c=$(EXEEXT))
lex_test_LIBS = -lmimic_cmulex
nums_test_LIBS = -lmimic_usenglish
utt_test_LIBS = -lmimic_cmulex
lex_lookup_LIBS = -lmimic_cmulex
ldom_time_LIBS = -L/home/awb/data/ldom/time_mimic/mimic/lib -lcmu_time_awb -lmimic_usenglish -lmimic_cmulex
by_word_LIBS = -lmimic_cmu_us_kal -lmimic_usenglish -lmimic_cmulex
voice_select_test_LIBS = $(TOP)/main/mimic_lang_list.o $(TOP)/main/mimic_voice_list.o -lmimic_cmu_us_kal -lmimic_cmu_time_awb -lmimic_cmu_us_kal16 -lmimic_cmu_us_awb -lmimic_cmu_us_rms -lmimic_cmu_us_slt -lmimic_usenglish -lmimic_cmu_indic_lang -lmimic_cmu_grapheme_lang -lmimic_cmulex -lmimic_cmu_indic_lex -lmimic_cmu_grapheme_lex -lmimic  -lm 
#kal_test_LIBS = -lmimic_cmu_us_kal -lmimic_usenglish -lmimic_cmulex \
#	          /home/awb/src/malloc/gmalloc.o

ALL = $(MAIN_EXECS) voice_select_test
LOCAL_CLEAN = $(MAIN_EXECS)

include $(TOP)/config/common_make_rules

voice_select_test: voice_select_test_main.c $(MIMIC_LIBS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $< $($(@:=_LIBS)) $(MIMICLIBFLAGS) $(LDFLAGS) 

MAIN_O = $(SRCS:%=%_main.o)
$(MAIN_O) : %_main.o : %_main.c
	$(CC) $(CFLAGS) -o $@ $< 
$(ALL) : %$(EXEEXT) : %_main.o $(MIMICLIBS)
	$(CC) $(CFLAGS) -o $@ $@_main.o $($(@:=_LIBS)) $(MIMICLIBFLAGS) $(LDFLAGS)

