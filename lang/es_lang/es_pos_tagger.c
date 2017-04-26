/* Copyright 2016-2017 Sergio Oller, Barcelona
 *
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
#include "cst_utterance.h"
#include "cst_val.h"
#include "es_lang_internal.h"

/* The POS tagger will assign a part of speech to a word. Right now it just
 * uses the guessed POS from es_guess_pos, but with a proper dataset something
 * more complex can be built if needed, for instance using n-grams.
 *
 * Improvements here could be used to work on phonetic disambiguations (not
 * very common in Spanish) and even to improve intonation, if an intonation
 * model
 * that depends on part of speech was built.
 */
cst_utterance *es_pos_tagger(cst_utterance *u)
{
    cst_item *word;

    for (word = relation_head(utt_relation(u, "Word")); word;
         word = item_next(word))
    {
        /* TODO: Make it smart */
        item_set_string(word, "pos", val_string(es_guess_pos(word)));
    }
    return u;
}
