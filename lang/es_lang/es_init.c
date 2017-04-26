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
#include "cst_ffeatures.h"
#include "es_lang_internal.h"

DEF_STATIC_CONST_VAL_STRING(es_dialect_default, "castilla");

void es_init(cst_voice *v)
{
    /* Basic generic functions that need to be registered always */
    basic_ff_register(v->ffunctions);

    /* 1. Tokenizer: Splits text into tokens */
    es_init_tokenizer(v);

    /* 2. Utterance break function */
    feat_set(v->features, "utt_break", breakfunc_val(&default_utt_break));

    /* 3. Text analyser: es_text_analysis.c */
    feat_set(v->features, "tokentowords_func", itemfunc_val(&es_tokentowords));

    /* 4. very simple POS tagger */
    ff_register(v->ffunctions, "gpos", es_guess_pos);
    feat_set(v->features, "pos_tagger_func", uttfunc_val(&es_pos_tagger));

    /* 5. Phrasing */
    feat_set(v->features, "phrasing_func", uttfunc_val(&es_phrasing));

    /* 6. Phoneset */
    feat_set(v->features, "phoneset", phoneset_val(&es_saga_phoneset));
    feat_set_string(v->features, "silence", es_saga_phoneset.silence);

    /* Lexical insertion: takes words and turns them into phones */
    feat_set(v->features, "lexical_insertion_func",
             uttfunc_val(&es_lexical_insertion));

    feat_set(v->features, "intonation_func", uttfunc_val(&es_intonation));
    feat_set(v->features, "phoneset_map", userdata_val(NULL));
    feat_set(v->features, "dialect", &es_dialect_default);
}
