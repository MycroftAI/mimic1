
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

