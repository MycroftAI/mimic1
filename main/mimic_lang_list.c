#include "mimic.h"

#if ENABLE_USENGLISH
#include "lang/usenglish/usenglish.h"
#endif

#if ENABLE_CMULEX
#include "lang/cmulex/cmu_lex.h"
#endif

#if ENABLE_INDIC_ANALYSIS
#include "lang/cmu_indic_lang/cmu_indic_lang.h"
#include "lang/cmu_grapheme_lang/cmu_grapheme_lang.h"
#endif

#if ENABLE_INDICLEX
cst_lexicon *cmu_indic_lex_init(void);
cst_lexicon *cmu_grapheme_lex_init(void);
#endif

#if ENABLE_ES_ANALYSIS
#include "lang/es_lang/es_lang.h"
#endif


void mimic_set_lang_list(void)
{
   #if (ENABLE_USENGLISH & ENABLE_CMULEX)
   mimic_add_lang("eng",usenglish_init,cmu_lex_init);
   mimic_add_lang("usenglish",usenglish_init,cmu_lex_init);
   #endif
   #if (ENABLE_INDICLEX & ENABLE_INDIC_ANALYSIS)
   mimic_add_lang("cmu_indic_lang",cmu_indic_lang_init,cmu_indic_lex_init);
   mimic_add_lang("cmu_grapheme_lang",cmu_grapheme_lang_init,cmu_grapheme_lex_init);
   #endif
   #if ENABLE_ES_ANALYSIS
   mimic_add_lang("es", es_init, NULL);
   #endif

}

