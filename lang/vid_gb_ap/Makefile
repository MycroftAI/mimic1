########################################################-*-mode:Makefile-*-
##                                                                       ##
##                  Language Technologies Institute                      ##
##                     Carnegie Mellon University                        ##
##                      Copyright (c) 2000-2008                          ##
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
VOXNAME=ap
TOP=../..
DIRNAME=lang/vid_gb_$(VOXNAME)
BUILD_DIRS = 
ALL_DIRS= 
FILE_BASE=vid_gb_ap
H = voxdefs.h $(FILE_BASE)_cg_17_durmodel.h \
    $(FILE_BASE)_cg_17_mcep_trees.h \
    $(FILE_BASE)_cg_f0_trees.h \
    $(FILE_BASE)_cg_17_durmodel.h

SRCS = $(FILE_BASE).c $(FILE_BASE)_cg_17_mcep_trees.c \
       $(FILE_BASE)_cg.c $(FILE_BASE)_cg_17_params.c \
       $(FILE_BASE)_cg_17_durmodel.c $(FILE_BASE)_cg_phonestate.c \
       $(FILE_BASE)_cg_10_durmodel.c \
       $(FILE_BASE)_cg_08_durmodel.c \
       $(FILE_BASE)_cg_f0_trees.c \
	   $(FILE_BASE)_cg_01_params.c \
	   $(FILE_BASE)_cg_12_mcep_trees.c \
	   $(FILE_BASE)_cg_01_mcep_trees.c \
	   $(FILE_BASE)_cg_12_params.c

OBJS = $(SRCS:.c=.o)

FILES = Makefile $(SCM) $(SRCS) $(H)
LIBNAME = mimic_vid_gb_$(VOXNAME)
LOCAL_INCLUDES = -I$(TOP)/lang/usenglish -I$(TOP)/lang/cmulex $(VOXDEFS)

ALL = 

include $(TOP)/config/common_make_rules


