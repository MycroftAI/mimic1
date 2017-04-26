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
#include "mimic.h"

/* The phrasing function is responsible of defining were
 * breaks "B" and big breaks "BB" should be.
 *
 * A simple usual approach is to add breaks at
 * "," or  ";" or other punctuation characters and
 * add big breaks at full stops
 *
 * Breaks are defined in the Phrase relation,
 * that is created here.
 *
 */
cst_utterance *es_phrasing(cst_utterance *u)
{
    cst_relation *r;
    cst_item *w, *p, *lp = NULL;

    r = utt_relation_create(u, "Phrase");

    p = NULL;
    for (w = relation_head(utt_relation(u, "Word")); w != NULL;
         w = item_next(w))
    {
        if (p == NULL)
        {
            /* if p is NULL, add new empty phrase element to the relation */
            p = relation_append(r, NULL);
            lp = p;
            /* Set a break at this phrase element */
            item_set_string(p, "name", "B");
        }
        /* Add current word to our phrase element */
        item_add_daughter(p, w);
        /* FIXME:
     if  ("w is the last word of a token that has post_punctuation and that
     post_punctuation is a , or a ; or similar...")
         p = NULL;
     */
    }

    if (lp && item_prev(lp)) /* follow festival */
        item_set_string(lp, "name", "BB");

    return u;
}
