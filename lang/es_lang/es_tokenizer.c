#include "es_lang.h"

static const char* es_whitespace = " \t\n\r";
static const char* es_singlecharsymbols = "€😃";
static const char* es_prepunctuation = "¿¡«\"'``({[";
static const char* es_punctuation = "»\"'``.,:;!?(){}[]";

void es_init_tokenizer(cst_voice *v)
{
    feat_set_string(v->features, "text_whitespace", es_whitespace);
    feat_set_string(v->features, "text_postpunctuation", es_punctuation);
    feat_set_string(v->features, "text_prepunctuation", es_prepunctuation);
    feat_set_string(v->features, "text_singlecharsymbols", es_singlecharsymbols);
}
