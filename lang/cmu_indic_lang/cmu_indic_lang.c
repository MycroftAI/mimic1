/*************************************************************************/
/*                                                                       */
/*                  Language Technologies Institute                      */
/*                     Carnegie Mellon University                        */
/*                         Copyright (c) 2013                            */
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
/*                                                                       */
/*  indic language support                                            */
/*                                                                       */
/*************************************************************************/
#include "flite.h"
#include "cst_val.h"
#include "cst_voice.h"
#include "cst_lexicon.h"
#include "cst_ffeatures.h"
#include "cmu_indic_lang.h"

/* ./bin/compile_regexes cst_rx_not_indic "^[0-9a-zA-Z/:_'-]+$" */
static const unsigned char cst_rx_not_indic_rxprog[] = {
   156, 6, 0, 83, 1, 0, 3, 11, 0, 74, 4, 0, 0, 48, 49, 50, 
   51, 52, 53, 54, 55, 56, 57, 97, 98, 99, 100, 101, 102, 103, 104, 105, 
   106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 
   122, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 
   80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 47, 58, 95, 39, 45, 
   0, 2, 0, 3, 0, 0, 0, 
};
static const cst_regex cst_rx_not_indic_rx = {
   0, 1, NULL, 0, 87,
   (char *)cst_rx_not_indic_rxprog
};
const cst_regex * const cst_rx_not_indic = &cst_rx_not_indic_rx;

cst_val *us_tokentowords(cst_item *token);

/* Note that's an ascii | not the devangari one */
const cst_string * const indic_postpunctuationsymbols = "\"'`.,:;!?(){}[]|";

static cst_val *cmu_indic_tokentowords_one(cst_item *token, const char *name);
cst_val *cmu_indic_tokentowords(cst_item *token) {
  return cmu_indic_tokentowords_one(token, item_feat_string(token, "name"));
}

static cst_val *cmu_indic_tokentowords_one(cst_item *token, const char *name)
{
    /* Return list of words that expand token/name */
    cst_val *r;

    /*    printf("token_name %s name %s\n",item_name(token),name); */

    if (item_feat_present(token,"phones"))
	return cons_val(string_val(name),NULL);

#if 0
    if (item_feat_present(token,"nsw"))
	nsw = item_feat_string(token,"nsw");

    utt = item_utt(token);
    lex = val_lexicon(feat_val(utt->features,"lexicon"));
#endif

    if (cst_regex_match(cst_rx_not_indic,name))
        /* Do English analysis on non-unicode tokens */
        r = us_tokentowords(token);
    else if (cst_strlen(name) > 0)
        r = cons_val(string_val(name),0);
    else
        r = NULL;

    return r;
}

int indic_utt_break(cst_tokenstream *ts,
                    const char *token,
                    cst_relation *tokens)
{
  const char *postpunct = item_feat_string(relation_tail(tokens), "punc");
  const char *ltoken = item_name(relation_tail(tokens));

  if (cst_strchr(ts->whitespace,'\n') != cst_strrchr(ts->whitespace,'\n'))
    /* contains two new lines */
    return TRUE;
  else if ((cst_strlen(ltoken) > 3) &&
           (cst_streq(&ltoken[cst_strlen(ltoken)-3],"।"))) /* devanagari '|' */
      return TRUE;
  else if (strchr(postpunct,':') ||
           strchr(postpunct,'?') ||
           strchr(postpunct,'|') ||  /* if ascii '|' gets used as dvngr '|' */
           strchr(postpunct,'!'))
    return TRUE;
  else if (strchr(postpunct,'.'))
    return TRUE;
  else
    return FALSE;
}

void cmu_indic_lang_init(cst_voice *v)
{
    /* Set indic language stuff */
    feat_set_string(v->features,"language","cmu_indic_lang");

    /* utterance break function */
    feat_set(v->features,"utt_break",breakfunc_val(&indic_utt_break));

    /* Phoneset -- need to get this from voice */
    feat_set(v->features,"phoneset",phoneset_val(&cmu_indic_phoneset));
    feat_set_string(v->features,"silence",cmu_indic_phoneset.silence);

    /* Get information from voice and add to lexicon */

    /* Text analyser -- whitespace defaults */
    feat_set_string(v->features,"text_whitespace",
                    cst_ts_default_whitespacesymbols);
    feat_set_string(v->features,"text_prepunctuation",
                    cst_ts_default_prepunctuationsymbols);
    /* We can't put multi-byte characters in these classes so we can't */
    /* add devanagari end of sentence '|' here, but would like to --   */
    /* But we do add ascii '|' to it as it sometimes gets used the same way */
    feat_set_string(v->features,"text_postpunctuation",
                    indic_postpunctuationsymbols);
    feat_set_string(v->features,"text_singlecharsymbols",
                    cst_ts_default_singlecharsymbols);

    /* Tokenization tokenization function */
    feat_set(v->features,"tokentowords_func",itemfunc_val(&cmu_indic_tokentowords));
    /* Pos tagger (gpos)/induced pos */

    /* Phrasing */
    feat_set(v->features,"phrasing_cart",cart_val(&cmu_indic_phrasing_cart));
        
    /* Intonation, Duration and F0 -- part of cg */
    feat_set_string(v->features,"no_intonation_accent_model","1");

    /* Default ffunctions (required) */
    basic_ff_register(v->ffunctions);

    return;
}
