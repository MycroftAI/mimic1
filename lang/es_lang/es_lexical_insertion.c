/* Copyright 2017 Sergio Oller
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

/* Note that if your Saga version is GPL-3, mimic will become GPL-3 when
 * linked to it.
 */
#include "Saga.h"
#include "cst_phoneset.h"
#include "es_lang.h"
#include "mimic.h"

char *text_to_phones_sep_words(const char *text, SagaEngine *engine)
{
    if (SagaEngine_TranscribeText(engine, text, "UTF-8") < 0)
    {
        SagaEngine_Refresh(engine);
        cst_errmsg("Error in SagaEngine_TranscribeText\n");
        return NULL;
    }
    char *phonetrans = SagaEngine_GetFnmPalOutput(engine, 1);
    SagaEngine_Refresh(engine);
    SagaEngine_ClearOutputs(engine);
    return phonetrans;
}

cst_utterance *es_lexical_insertion(cst_utterance *u)
{
    cst_tokenstream *ts;
    cst_item *word;
    cst_relation *sylstructure, *seg, *syl;
    char *phonetrans;
    const char *phone_name;
    char phone_name_with_stress[3];
    int is_next_vowel_stressed = 0;

    cst_item *ssword, *sssyl, *segitem, *sylitem;

    syl = utt_relation_create(u, "Syllable");
    sylstructure = utt_relation_create(u, "SylStructure");
    seg = utt_relation_create(u, "Segment");
    const cst_val *val = utt_feat_val(u, "phoneset_map");
    phoneset_map_t *phone_map = (phoneset_map_t *) val_userdata(val);

    /* SagaEngine: load the engine for phonetic transcriptions */
    const char *dialect = utt_feat_string(u, "dialect");
    SagaEngine *engine = SagaEngine_NewFromVariant(dialect);
    if (engine == NULL)
    {
        cst_errmsg("Could not create SagaEngine\n");
        return u;
    }
    SagaEngine_EnableFnmPalOutput(engine, 1);

    word = relation_head(utt_relation(u, "Word"));
    if (word == NULL)
    {
        return u;
    }

    /* We do the transcription word per word. This
   * is not optimal, because in some cases
   * ellisions are made, but as a first approximation
   * it is not bad.
   * TODO: Do the transcription as a whole
   */
    for (; word; word = item_next(word))
    {
        ssword = NULL;
        phonetrans =
            text_to_phones_sep_words(item_feat_string(word, "name"), engine);
        if (phonetrans == NULL)
        {
            continue;
        }

        sssyl = NULL;
        sylitem = NULL;
        ts = ts_open_string(phonetrans, " ", "/-'", "", "", 0);
        while (!ts_eof(ts))
        {
            const char *pname = ts_get(ts);
            if (ssword == NULL) /* first syllable in word */
            {
                ssword = relation_append(sylstructure, word);
                /* pos = ffeature_string(word, "pos"); */
                is_next_vowel_stressed = 0;
            }
            if (strcmp(pname, "/") == 0 || strcmp(pname, "-") == 0)
            {
                if (strcmp(pname, "/") == 0)
                {
                    fprintf(stderr, "Saga gave a word break were mimic didn't. "
                                    "Treat it as a syllable break. Suspicious "
                                    "word: %s\n",
                            item_feat_string(word, "name"));
                }
                // next syllable
                sylitem = NULL;
                is_next_vowel_stressed = 0;
                continue;
            }

            if (sylitem == NULL) /* First item in syllable */
            {
                sylitem = relation_append(syl, NULL);
                sssyl = item_add_daughter(ssword, sylitem);
                item_set_string(sssyl, "stress", "0");
            }

            /* This "phoneme" is just a stress mark */
            if (strcmp(pname, "'") == 0)
            {
                item_set_string(sssyl, "stress", "1");
                is_next_vowel_stressed = 1;
                continue;
            }

            segitem = relation_append(seg, NULL);

            // map phones from saga to voice phoneset:
            phone_name = map_phones(pname, phone_map);

            if (strlen(phone_name) == 1 && is_next_vowel_stressed == 1 &&
                (phone_name[0] == 'a' || phone_name[0] == 'e' ||
                 phone_name[0] == 'i' || phone_name[0] == 'o' ||
                 phone_name[0] == 'u'))
            {
                phone_name_with_stress[0] = phone_name[0];
                phone_name_with_stress[1] = '1';
                phone_name_with_stress[2] = '\0';
                item_set_string(segitem, "name", phone_name_with_stress);
            }
            else
            {
                item_set_string(segitem, "name", phone_name);
            }
            item_add_daughter(sssyl, segitem);
            is_next_vowel_stressed = 0;
        }
        ts_close(ts);
        free(phonetrans);
    }
    SagaEngine_Clear(engine);
    free(engine);
    return u;
}
