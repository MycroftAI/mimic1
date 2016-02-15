/* Generated automatically from make_voice_list */

#include "mimic.h"

cst_voice *register_cmu_us_slt(const char *voxdir);

cst_val *mimic_set_voice_list(const char *voxdir)
{
   mimic_voice_list = cons_val(voice_val(register_cmu_us_slt(voxdir)),mimic_voice_list);
   mimic_voice_list = val_reverse(mimic_voice_list);
   return mimic_voice_list;
}

