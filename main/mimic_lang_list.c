/* Generated automatically from make_lang_list */

#include "mimic.h"

void usenglish_init(cst_voice *v);
cst_lexicon *cmulex_init(void);

void cmu_indic_lang_init(cst_voice *v);
cst_lexicon *cmu_indic_lex_init(void);

void cmu_grapheme_lang_init(cst_voice *v);
cst_lexicon *cmu_grapheme_lex_init(void);


void mimic_set_lang_list(void)
{
   mimic_add_lang("eng",usenglish_init,cmulex_init);
   mimic_add_lang("usenglish",usenglish_init,cmulex_init);
   mimic_add_lang("cmu_indic_lang",cmu_indic_lang_init,cmu_indic_lex_init);
   mimic_add_lang("cmu_grapheme_lang",cmu_grapheme_lang_init,cmu_grapheme_lex_init);
}

