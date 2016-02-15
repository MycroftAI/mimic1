###########################################################################
##                                                                       ##
##                  Language Technologies Institute                      ##
##                     Carnegie Mellon University                        ##
##                         Copyright (c) 2008                            ##
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
##  Windows Mobile support for Flite                                     ##
##                                                                       ##
##  Builds a usable mimic, language and voice .dlls with mingw           ##
##                                                                       ##
##     http://www.mingw.org                                              ##
##                                                                       ##
##  Also an example command line program                                 ## 
##                                                                       ##
##  At top level                                                         ##
##      ./configure --target=i386-mingw32                                ##
##      make                                                             ##
##                                                                       ##
###########################################################################
TOP = ..
DIRNAME = windows
BUILD_DIRS = 
ALL_DIRS = $(BUILD_DIRS) 
H = 
RC = 
SRCS = 
OBJS = $(SRCS:.c=.o)
WINDOWS = mimicDll.cpp stdafx.h
FILES = Makefile $(SRCS) $(H) $(RC) $(WINDOWS)

mimic_LIBS = mimic_$(FL_VOX) mimic_$(FL_LANG) mimic_$(FL_LEX) 
mimic_LIBS_flags = -L$(LIBDIR) $(mimic_LIBS:%=-l%)
mimic_LIBS_deps = $(mimic_LIBS:%=$(LIBDIR)/lib%.a)

LOCAL_CLEAN = mimic.dll mimic.lib mimic.def

ALL = 

include $(TOP)/config/common_make_rules

mimic.def: $(LIBDIR)/libmimic.a
	echo "LIBRARY MIMIC.DLL" >mimic.def
	echo "EXPORTS" >>mimic.def
	$(NM) $(LIBDIR)/libmimic.a | \
	grep "^0" | awk '{if ($$2 == "T") print $$3}' | sed 's/_//' >>mimic.def

mimic.dll: $(LIBDIR)/libmimic.a
	rm -rf mimicdll
	mkdir mimicdll
	(cd mimicdll && ar x ../$(LIBDIR)/libmimic.a)
	$(DLLWRAP) -def mimic.def mimicdll/*.o --output-lib mimic.lib -o mimic.dll
	rm -rf mimicdll



