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
#include "cst_uregex.h"
#include "es_lang.h"
#include "mimic.h"

cst_utterance *es_textanalysis(cst_utterance *u);
cst_val *es_tokentowords(cst_item *token);
static cst_val *es_tokentowords_one(cst_item *token, const char *name);

cst_utterance *es_textanalysis(cst_utterance *u)
{
    if (!feat_present(u->features, "tokentowords_func"))
        utt_set_feat(u, "tokentowords_func", itemfunc_val(es_tokentowords));

    return default_textanalysis(u);
}

cst_val *es_tokentowords(cst_item *token)
{
    return es_tokentowords_one(token, item_feat_string(token, "name"));
}

static cst_val *es_tokentowords_one(cst_item *token, const char *name)
{
    cst_val *list_of_words;
    char *name_mod;
    /* Each token has defined four features:
        - The token name: ffeature_string(token,"name");
        - The token prepunctuation: ffeature_string(token,"prepunctuation");
        - The token postpunctuation: ffeature_string(token,"punc");
        - The token whitespace: ffeature_string(token,"whitespace");

       We can also traverse the list of tokens for contextual information:
        - The previous token: cst_item *prev_token = item_prev(token);
        - The next token: cst_item *next_token = item_next(token);

       So we can combine both traversing and accessing features:
        - The name of the next token: ffeature_string(item_next(token), "name");

       As text analysis requires constantly of checking context, we can use
       feature paths:
       Feature paths can be up to 200 characters long.
       Their syntax is implemented in src/hrg/cst_ffeature.c. You may the
       internal_ff function. In summary, feature paths are strings composed of
       operators separated by dots. The most common operators applicable to
       tokens are:
        - "n": Advances to the next token
        - "nn": Advances to the next next token
        - "p": Advances to the previous token
        - "pp": Advances to the previous previous token

        Other cst_item usually require other operators that we won't explore
       now:
        - "parent": Advances to the parent item
        - "daughter": Advances to the first daughter item
        - "daughtern": Advances to the last daughter item
        - "RNameOfRelation": Moves to another relation.

        After the last operator, we typically want access to a token feature:
        - Token. Previous. Name: ffeature_string(token,"p.name");
        - Token. PreviousPrevious. Name: ffeature_string(token,"pp.name");
        - Token. Previous. Name: ffeature_string(token,"p.name");
        - Look the next-next-next token punctuation: ffeature_string(token,
       "nn.n.punc");

    */
    if (strcmp(name, "€") == 0)
    {
        list_of_words = cons_val(string_val("euro"), NULL);
    }
    else if (name[cst_strlen(name)] == '%')
    {
        name_mod = cst_strdup(name);
        name_mod[cst_strlen(name_mod) - 1] = '\0';
        list_of_words = es_tokentowords_one(token, name_mod);
        list_of_words = val_append(
            list_of_words,
            cons_val(string_val("por"), cons_val(string_val("ciento"), NULL)));
        cst_free(name_mod);
    }
    else if (strcmp(name, "17") == 0) /* TODO: Handle all numbers */
    {
        list_of_words = cons_val(string_val("diecisiete"), NULL);
    }
    else
    {
        char *word_to_lower = cst_tolower_utf8(name);
        if (word_to_lower == NULL)
            return cons_val(NULL, NULL);
        list_of_words = cons_val(string_val(word_to_lower), NULL);
        free(word_to_lower);
    }
    return list_of_words;
}
