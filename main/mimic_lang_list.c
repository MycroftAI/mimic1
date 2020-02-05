/*************************************************************************/
/*                                                                       */
/*                  Language Technologies Institute                      */
/*                     Carnegie Mellon University                        */
/*                         Copyright (c) 2001                            */
/*                        All Rights Reserved.                           */
/*                                                                       */
/*  Permission is hereby granted, free of charge, to use and distribute  */
/*  this software and its documentation without restriction, including   */
/*  without limitation the rights to use, copy, modify, merge, publish,  */
/*  distribute, sublicense, and/or sell copies of this work, and to      */
/*  permit persons to whom this work is furnished to do so, subject to   */
/*  the following conditions:                                            */
/*   1. The code must retain the above copyright notice, this list of    */
/*      conditions and the following disclaimer.                         */
/*   2. Any modifications must be clearly marked as such.                */
/*   3. Original authors' names are not deleted.                         */
/*   4. The authors' names are not used to endorse or promote products   */
/*      derived from this software without specific prior written        */
/*      permission.                                                      */
/*                                                                       */
/*  CARNEGIE MELLON UNIVERSITY AND THE CONTRIBUTORS TO THIS WORK         */
/*  DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING      */
/*  ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT   */
/*  SHALL CARNEGIE MELLON UNIVERSITY NOR THE CONTRIBUTORS BE LIABLE      */
/*  FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES    */
/*  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN   */
/*  AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,          */
/*  ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF       */
/*  THIS SOFTWARE.                                                       */
/*                                                                       */
/*************************************************************************/
/*             Author:  Alan W Black (awb@cs.cmu.edu)                    */
/*               Date:  January 2001                                     */
/*************************************************************************/
/*                                                                       */
/*  Set lang list                                                        */
/*                                                                       */
/*************************************************************************/
/* Copyright 2016-2017 Sergio Oller, under the same terms as above       */
/*************************************************************************/

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

