###########################################################################
##                                                                       ##
##                  Language Technologies Institute                      ##
##                     Carnegie Mellon University                        ##
##                       Copyright (c) 2001-2008                         ##
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
##  Top level user programs                                              ##
##                                                                       ##
###########################################################################
TOP=..
DIRNAME=main
BUILD_DIRS = 
ALL_DIRS= 
SRCS = mimic_main.c mimic_time_main.c t2p_main.c compile_regexes.c \
       mimicvox_info_main.c
OBJS = $(SRCS:.c=.o) mimic_voice_list.o 
FILES = Makefile $(SRCS)
LOCAL_INCLUDES = 

ALL = shared_libs \
      $(BINDIR)/mimic$(EXEEXT) \
      $(BINDIR)/t2p$(EXEEXT) $(BINDIR)/compile_regexes$(EXEEXT) \
      $(BINDIR)/mimicvox_info$(EXEEXT) \
      mimic_voice_list.c each $(EXTRABINS)

VOICES=$(VOXES)
VOICELIBS=$(VOICES:%=mimic_%)

mimic_LIBS = $(VOICELIBS) $(LANGS:%=mimic_%) $(LEXES:%=mimic_%) mimic

mimic_LIBS_flags = -L$(LIBDIR) $(mimic_LIBS:%=-l%)
mimic_LIBS_deps = $(mimic_LIBS:%=$(LIBDIR)/lib%.a)

include $(TOP)/config/common_make_rules

# We need to set these vars, even if we're not going to build them
# so make clean can remove them
SHAREDARLIBS= $(mimic_LIBS:%=$(LIBDIR)/lib%.shared.a)
SHAREDLIBS = $(SHAREDARLIBS:%.shared.a=%.so)
VERSIONSHAREDLIBS = $(SHAREDLIBS:%=%.${PROJECT_VERSION}) \
                    $(SHAREDLIBS:%=%.${PROJECT_SHLIB_VERSION})

mimic_time_LIBS = mimic_cmu_time_awb mimic_usenglish mimic_cmulex
mimic_time_LIBS_flags = -L$(LIBDIR) $(mimic_time_LIBS:%=-l%)
mimic_time_LIBS_deps = $(mimic_time_LIBS:%=$(LIBDIR)/lib%.a)

LOCAL_CLEAN = $(BINDIR)/mimic$(EXEEXT) $(BINDIR)/mimic_time$(EXEEXT) \
              $(BINDIR)/t2p$(EXEEXT) \
              $(SHAREDARLIBS) $(SHAREDLIBS) $(VERSIONSHAREDLIBS) \
              $(mimic_LIBS_deps) $(VOICES:%=$(BINDIR)/mimic_%) \
              mimic_voice_list.c

ifdef SHFLAGS
mimic_LIBS_flags += -Wl,-rpath $(LIBDIR) 
shared_libs: $(SHAREDLIBS)
else
shared_libs: nothing
endif

.NOTPARALLEL: $(ALL)

mimic_lang_list: 
	rm -f mimic_lang_list.c
	$(TOP)/tools/make_lang_list $(LANGS) $(LEXES)
	$(MAKE) mimic_lang_list.o

$(BINDIR)/mimic$(EXEEXT): mimic_main.o mimic_lang_list $(mimic_LIBS_deps)
	$(TOP)/tools/make_voice_list $(VOICES)
	rm -f mimic_voice_list.o
	$(MAKE) mimic_voice_list.o
	$(CC) $(CFLAGS) -o $@ mimic_main.o mimic_voice_list.o mimic_lang_list.o $(mimic_LIBS_flags) $(LDFLAGS)

$(BINDIR)/mimicvox_info$(EXEEXT): mimicvox_info_main.o mimic_lang_list $(mimic_LIBS_deps)
	$(TOP)/tools/make_voice_list $(VOICES)
	rm -f mimic_voice_list.o
	$(MAKE) mimic_voice_list.o
	$(CC) $(CFLAGS) -o $@ mimicvox_info_main.o mimic_voice_list.o mimic_lang_list.o $(mimic_LIBS_flags) $(LDFLAGS)

$(BINDIR)/mimic_time$(EXEEXT): mimic_time_main.o $(mimic_time_LIBS_deps)
	$(CC) $(CFLAGS) -o $@ mimic_time_main.o $(mimic_time_LIBS_flags) $(MIMICLIBFLAGS) $(LDFLAGS) 

$(BINDIR)/t2p$(EXEEXT): t2p_main.o $(mimic_LIBS_deps)
	$(CC) $(CFLAGS) -o $@ t2p_main.o $(mimic_LIBS_flags) $(LDFLAGS)

$(BINDIR)/compile_regexes$(EXEEXT): compile_regexes.o $(MIMICLIBS)
	$(CC) $(CFLAGS) -o $@ compile_regexes.o $(MIMICLIBFLAGS) $(LDFLAGS)

each:
	@ for i in $(VOICES) ; \
	do \
	   $(MAKE) VOICE=$$i $(BINDIR)/mimic_$$i ; \
	done

$(BINDIR)/mimic_${VOICE}: mimic_main.o mimic_lang_list $(mimic_LIBS_deps)
	$(TOP)/tools/make_voice_list $(VOICE)
	rm -f mimic_voice_list.o
	$(MAKE) mimic_voice_list.o
	$(CC) $(CFLAGS) -o $@ mimic_main.o mimic_voice_list.o mimic_lang_list.o $(mimic_LIBS_flags) $(LDFLAGS)

install:
#       The basic binaries
	@ $(INSTALL) -m 755 $(BINDIR)/mimic $(DESTDIR)$(INSTALLBINDIR)
	@ for i in $(VOICES) ; \
	do \
	   $(INSTALL) $(BINDIR)/mimic_$$i $(DESTDIR)$(INSTALLBINDIR); \
	done
	$(INSTALL) -m 755 $(BINDIR)/mimic_time $(DESTDIR)$(INSTALLBINDIR)
#       The libraries: static and shared (if built)
	cp -pd $(mimic_LIBS_deps) $(DESTDIR)$(INSTALLLIBDIR)
ifdef SHFLAGS
	cp -pd $(SHAREDLIBS) $(VERSIONSHAREDLIBS) $(DESTDIR)$(INSTALLLIBDIR)
endif

