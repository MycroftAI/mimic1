/*************************************************************************/
/*                                                                       */
/*                  Language Technologies Institute                      */
/*                     Carnegie Mellon University                        */
/*                      Copyright (c) 1999-2007                          */
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
/*               Date:  November 2007                                    */
/*************************************************************************/
/*                                                                       */
/*  A hts voice defintion			                 */
/*                                                                       */
/*************************************************************************/

#include <string.h>
#include "mimic.h"

#include "usenglish.h"
#include "cmu_lex.h"
#include "flite_hts_engine.h"

cst_voice *cmu_us_slt_hts = NULL;

cst_voice *register_cmu_us_slt_hts(const char *voxdir)
{
    (void) voxdir;
    cst_voice *vox;
    cst_lexicon *lex;
    Flite_HTS_Engine *flite_hts = NULL;


    if (cmu_us_slt_hts)
        return cmu_us_slt_hts;  /* Already registered */

    vox = new_voice();
    vox->name = "slt_hts";

    /* Sets up language specific parameters in the cmu_us_slt_hts. */
    usenglish_init(vox);

    /* Things that weren't filled in already. */
    mimic_feat_set_string(vox->features, "name", "cmu_us_slt_hts");

    /* Lexicon */
    lex = cmu_lex_init();
    mimic_feat_set(vox->features, "lexicon", lexicon_val(lex));
    mimic_feat_set(vox->features, "postlex_func", uttfunc_val(lex->postlex));

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
                              "cmu_us_slt_hts.htsvoice");
    }
    mimic_feat_set(vox->features, "flite_hts", flitehtsengine_val(flite_hts));

    /* Waveform synthesis */
    mimic_feat_set(vox->features, "wave_synth_func", uttfunc_val(&hts_synth));
    mimic_feat_set_int(vox->features, "sample_rate",
                       Flite_HTS_Engine_get_sampling_frequency(flite_hts));

    cmu_us_slt_hts = vox;

    return cmu_us_slt_hts;
}

void unregister_cmu_us_slt_hts(cst_voice *vox)
{
    if (vox != cmu_us_slt_hts)
        return;
    delete_voice(vox);
    cmu_us_slt_hts = NULL;
}
