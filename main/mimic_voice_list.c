/*************************************************************************/
/*                                                                       */
/*                  Language Technologies Institute                      */
/*                     Carnegie Mellon University                        */
/*                         Copyright (c) 2001                            */
/*                        All Rights Reserved.                           */
/*                                                                       */
/*  Permission is hereby granted, free of charge, to use and distribute  */
/*  this software and its documentation without restriction, including   */
/*  without limitation the rights to use, copy, modify, merge, publish,  */
/*  distribute, sublicense, and/or sell copies of this work, and to      */
/*  permit persons to whom this work is furnished to do so, subject to   */
/*  the following conditions:                                            */
/*   1. The code must retain the above copyright notice, this list of    */
/*      conditions and the following disclaimer.                         */
/*   2. Any modifications must be clearly marked as such.                */
/*   3. Original authors' names are not deleted.                         */
/*   4. The authors' names are not used to endorse or promote products   */
/*      derived from this software without specific prior written        */
/*      permission.                                                      */
/*                                                                       */
/*  CARNEGIE MELLON UNIVERSITY AND THE CONTRIBUTORS TO THIS WORK         */
/*  DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING      */
/*  ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT   */
/*  SHALL CARNEGIE MELLON UNIVERSITY NOR THE CONTRIBUTORS BE LIABLE      */
/*  FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES    */
/*  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN   */
/*  AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,          */
/*  ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF       */
/*  THIS SOFTWARE.                                                       */
/*                                                                       */
/*************************************************************************/
/*             Author:  Alan W Black (awb@cs.cmu.edu)                    */
/*               Date:  January 2001                                     */
/*************************************************************************/
/*                                                                       */
/*  Set voice list                                                       */
/*                                                                       */
/*************************************************************************/
/* Copyright 2016-2017 Sergio Oller, under the same terms as above       */
/*************************************************************************/

#include "mimic.h"


#if ENABLE_CMU_TIME_AWB
cst_voice *register_cmu_time_awb(const char *voxdir);
#endif

#if ENABLE_CMU_US_AWB
cst_voice *register_cmu_us_awb(const char *voxdir);
#endif

#if ENABLE_CMU_US_KAL
cst_voice *register_cmu_us_kal(const char *voxdir);
#endif

#if ENABLE_CMU_US_KAL16
cst_voice *register_cmu_us_kal16(const char *voxdir);
#endif

#if ENABLE_CMU_US_RMS
cst_voice *register_cmu_us_rms(const char *voxdir);
#endif

#if ENABLE_CMU_US_SLT
cst_voice *register_cmu_us_slt(const char *voxdir);
#endif

#if ENABLE_CMU_US_SLT_HTS
cst_voice *register_cmu_us_slt_hts(const char *voxdir);
#endif

#if ENABLE_VID_GB_AP
cst_voice *register_vid_gb_ap(const char *voxdir);
#endif

cst_val *mimic_set_voice_list(const char *voxdir)
{
   /* The first voice here is the default voice for mimic */
   #if ENABLE_VID_GB_AP
   mimic_voice_list = cons_val(voice_val(register_vid_gb_ap(voxdir)),mimic_voice_list);
   #endif   
   #if ENABLE_CMU_US_SLT
   mimic_voice_list = cons_val(voice_val(register_cmu_us_slt(voxdir)),mimic_voice_list);
   #endif   
   #if ENABLE_CMU_US_SLT_HTS
   mimic_voice_list = cons_val(voice_val(register_cmu_us_slt_hts(voxdir)),mimic_voice_list);
   #endif   
   #if ENABLE_CMU_US_KAL
   mimic_voice_list = cons_val(voice_val(register_cmu_us_kal(voxdir)),mimic_voice_list);
   #endif   
   #if ENABLE_CMU_US_AWB
   mimic_voice_list = cons_val(voice_val(register_cmu_us_awb(voxdir)),mimic_voice_list);
   #endif   
   #if ENABLE_CMU_US_KAL16
   mimic_voice_list = cons_val(voice_val(register_cmu_us_kal16(voxdir)),mimic_voice_list);
   #endif   
   #if ENABLE_CMU_US_RMS
   mimic_voice_list = cons_val(voice_val(register_cmu_us_rms(voxdir)),mimic_voice_list);
   #endif   
   #if ENABLE_CMU_TIME_AWB
   mimic_voice_list = cons_val(voice_val(register_cmu_time_awb(voxdir)),mimic_voice_list);
   #endif   
   mimic_voice_list = val_reverse(mimic_voice_list);
   return mimic_voice_list;
}

