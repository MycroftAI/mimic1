#include "cst_ffeatures.h"
#include "es_lang.h"

void es_init(cst_voice *v)
{
    /* Basic generic functions that need to be registered always */
    basic_ff_register(v->ffunctions);

    /* 1. Tokenizer */
    es_init_tokenizer(v);

    /* 2. Utterance break function */
    feat_set(v->features,"utt_break",breakfunc_val(&default_utt_break));

    /* 3. Text analyser */
    feat_set(v->features,"tokentowords_func",itemfunc_val(&es_tokentowords));

    /* 4. very simple POS tagger */
    /* TO DO */

    /* 5. Phrasing */
    /* TO DO */

    /* 6a. Phoneset */
    /* TO DO */


    /* 8. Intonation */
    /* TO DO */

    /* 10. Duration */
    /* TO DO? */

    /* 11. f0 model */
    /* TO DO? */

}
