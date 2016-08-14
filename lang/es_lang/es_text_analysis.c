#include "es_lang.h"
#include "cst_icu.h"
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

        Other cst_item usually require other operators that we won't explore now:
        - "parent": Advances to the parent item
        - "daughter": Advances to the first daughter item
        - "daughtern": Advances to the last daughter item
        - "RNameOfRelation": Moves to another relation.

        After the last operator, we typically want access to a token feature:
        - Token. Previous. Name: ffeature_string(token,"p.name");
        - Token. PreviousPrevious. Name: ffeature_string(token,"pp.name");
        - Token. Previous. Name: ffeature_string(token,"p.name");
        - Look the next-next-next token punctuation: ffeature_string(token, "nn.n.punc");

    */
    if (strcmp(name, "€") == 0)
    {
        list_of_words = cons_val(string_val("euro"), NULL);
    } else if (name[cst_strlen(name)] == '%')
    {
        name_mod = cst_strdup(name);
        name_mod[cst_strlen(name_mod)-1] = '\0';
        list_of_words = es_tokentowords_one(token, name_mod);
        list_of_words = val_append(list_of_words,
                                   cons_val(string_val("por"),
                                            cons_val(string_val("ciento"), NULL)));
    } else if (strcmp(name, "17") == 0) /* TO DO: Handle all numbers */
    {
        list_of_words = cons_val(string_val("diecisiete"), NULL);       
    } else
    {
        list_of_words = cons_val(string_val(cst_tolower_l(name, "es_ES")), NULL);
    }
    return list_of_words;
}
