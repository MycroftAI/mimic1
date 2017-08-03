/* (C) Sergio Oller 2017
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * * Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above
 *   copyright notice, this list of conditions and the following disclaimer
 *   in the documentation and/or other materials provided with the
 *   distribution.
 * * Neither the name of the  nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#include "mimic.h"
#include <string.h>

#include "cmu_lex.h"
#include "cst_phoneset.h"
#include "es_lang.h"
#include "flite_hts_engine.h"
#include "usenglish.h"

phoneset_map_t saga_to_thisvoice_phoneset_map[] = {
    { "J", "ny" },  { "N", "n" }, { "tS", "ch" }, { "T", "th" }, { "z", "s" },
    { "jj", "ll" }, { "L", "l" }, { "j", "i" },   { "w", "u" },  { "B", "b" },
    { "D", "d" },   { "G", "g" }, { "dZ", "ll" }, { NULL, NULL }
};

cst_voice *cstr_upc_upm_spanish_hts = NULL;

cst_voice *register_cstr_upc_upm_spanish_hts(const char *voxdir)
{
    (void) voxdir;
    cst_voice *vox;
    Flite_HTS_Engine *flite_hts = NULL;

    if (cstr_upc_upm_spanish_hts)
        return cstr_upc_upm_spanish_hts; /* Already registered */

    vox = new_voice();
    vox->name = "cstr_upc_upm_spanish_hts";

    /* Sets up language specific parameters in the cstr_upc_upm_spanish_hts. */
    es_init(vox);

    /* Things that weren't filled in already. */
    mimic_feat_set_string(vox->features, "name", "cstr_upc_upm_spanish_hts");
    mimic_feat_set_string(vox->features, "dialect", "castilla");

    /* No standard segment durations are needed as its done at the */
    /* HMM state level */
    mimic_feat_set_string(vox->features, "no_segment_duration_model", "1");
    mimic_feat_set_string(vox->features, "no_f0_target_model", "1");

    /* HTS engine */
    flite_hts = cst_alloc(Flite_HTS_Engine, 1);
    Flite_HTS_Engine_initialize(flite_hts);
    /* load HTS voice */
    char *fn_voice = mimic_hts_get_voice_file(vox);
    if (fn_voice != NULL)
    {
        mimic_feat_set_string(vox->features, "htsvoice_file", fn_voice);
        cst_free(fn_voice);
    }
    else
    {
        mimic_feat_set_string(vox->features, "htsvoice_file",
                              "cstr_upc_upm_spanish_hts.htsvoice");
    }
    mimic_feat_set(vox->features, "flite_hts", flitehtsengine_val(flite_hts));
    mimic_feat_set(vox->features, "phoneset_map",
                   userdata_val(saga_to_thisvoice_phoneset_map));
    /* Waveform synthesis */
    mimic_feat_set(vox->features, "wave_synth_func", uttfunc_val(&hts_synth));
    mimic_feat_set_float(vox->features, "postfiltering_coefficient", 0.1);
    mimic_feat_set_int(vox->features, "sample_rate",
                       Flite_HTS_Engine_get_sampling_frequency(flite_hts));

    cstr_upc_upm_spanish_hts = vox;

    return cstr_upc_upm_spanish_hts;
}

void unregister_cstr_upc_upm_spanish_hts(cst_voice *vox)
{
    if (vox != cstr_upc_upm_spanish_hts)
        return;
    delete_voice(vox);
    cstr_upc_upm_spanish_hts = NULL;
}
