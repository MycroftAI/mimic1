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
/*  indic Lexical function                                            */
/*                                                                       */
/*************************************************************************/
#include "flite.h"
#include "cst_val.h"
#include "cst_voice.h"
#include "cst_lexicon.h"
#include "cst_ffeatures.h"
#include "cmu_indic_lex.h"
#include "../cmu_indic_lang/cmu_indic_lang.h"

extern cst_lexicon cmu_lex;

static const struct cmu_indic_char cmu_indic_offset_char[128] = {
  /*000*/ {IND_IGNORE, ""},
  /*001*/ {IND_ANUSWAAR, "nX"},
  /*002*/ {IND_ANUSWAAR, "nX"},
  /*003*/ {IND_VISARGA, "h"},

  /*004*/ {IND_INDEPENDENT_VOWEL, "A"},
  /*005*/ {IND_INDEPENDENT_VOWEL, "A"},
  /*006*/ {IND_INDEPENDENT_VOWEL, "A:"},
  /*007*/ {IND_INDEPENDENT_VOWEL, "i"},
  /*008*/ {IND_INDEPENDENT_VOWEL, "i:"},
  /*009*/ {IND_INDEPENDENT_VOWEL, "u"},
  /*00A*/ {IND_INDEPENDENT_VOWEL, "u:"},
  /*00B*/ {IND_INDEPENDENT_VOWEL, "9r="},
  /*00C*/ {IND_INDEPENDENT_VOWEL, "l="},
  /*00D*/ {IND_INDEPENDENT_VOWEL, "ay"},
  /*00E*/ {IND_INDEPENDENT_VOWEL, "e"},
  /*00F*/ {IND_INDEPENDENT_VOWEL, "e"},
  /*010*/ {IND_INDEPENDENT_VOWEL, "aI"},
  /*011*/ {IND_INDEPENDENT_VOWEL, "ow"},
  /*012*/ {IND_INDEPENDENT_VOWEL, "ow"},
  /*013*/ {IND_INDEPENDENT_VOWEL, "o"},
  /*014*/ {IND_INDEPENDENT_VOWEL, "aU"},

  /*015*/ {IND_CONSONANT, "k"},
  /*016*/ {IND_CONSONANT, "kh"},
  /*017*/ {IND_CONSONANT, "g"},
  /*018*/ {IND_CONSONANT, "gh"},
  /*019*/ {IND_CONSONANT, "N"},

  /*01A*/ {IND_CONSONANT, "c"},
  /*01B*/ {IND_CONSONANT, "ch"},
  /*01C*/ {IND_CONSONANT, "J"},
  /*01D*/ {IND_CONSONANT, "Jh"},
  /*01E*/ {IND_CONSONANT, "n~"},

  /*01F*/ {IND_CONSONANT, "tr"},
  /*020*/ {IND_CONSONANT, "tR"},
  /*021*/ {IND_CONSONANT, "dr"},
  /*022*/ {IND_CONSONANT, "dR"},
  /*023*/ {IND_CONSONANT, "nr"},

  /*024*/ {IND_CONSONANT, "tB"},
  /*025*/ {IND_CONSONANT, "tBh"},
  /*026*/ {IND_CONSONANT, "dB"},
  /*027*/ {IND_CONSONANT, "dBh"},
  /*028*/ {IND_CONSONANT, "nB"},
  /*029*/ {IND_CONSONANT, "nB"},

  /*02A*/ {IND_CONSONANT, "p"},
  /*02B*/ {IND_CONSONANT, "ph"},
  /*02C*/ {IND_CONSONANT, "b"},
  /*02D*/ {IND_CONSONANT, "bh"},
  /*02E*/ {IND_CONSONANT, "m"},

  /*02F*/ {IND_CONSONANT, "j"},
  /*030*/ {IND_CONSONANT, "9r"},
  /*031*/ {IND_CONSONANT, "9r"},
  /*032*/ {IND_CONSONANT, "l"},
  /*033*/ {IND_CONSONANT, "lr"},
  /*034*/ {IND_CONSONANT, "lr"},

  /*035*/ {IND_CONSONANT, "v"},
  /*036*/ {IND_CONSONANT, "c}"},
  /*037*/ {IND_CONSONANT, "sr"},
  /*038*/ {IND_CONSONANT, "s"},
  /*039*/ {IND_CONSONANT, "hv"},

  /*03A*/ {IND_VOWEL, "e"},
  /*03B*/ {IND_VOWEL, "e"},

  /*03C*/ {IND_NUKTA, ""},
  /*03D*/ {IND_AVAGRAHA, ""},

  /*03E*/ {IND_VOWEL, "A:"},
  /*03F*/ {IND_VOWEL, "i"},
  /*040*/ {IND_VOWEL, "i:"},
  /*041*/ {IND_VOWEL, "u"},
  /*042*/ {IND_VOWEL, "u:"},
  /*043*/ {IND_VOWEL, "9r="},
  /*044*/ {IND_VOWEL, "9r="},
  /*045*/ {IND_VOWEL, "ay"},
  /*046*/ {IND_VOWEL, "e"},
  /*047*/ {IND_VOWEL, "e"},
  /*048*/ {IND_VOWEL, "aI"},
  /*049*/ {IND_VOWEL, "ow"},
  /*04A*/ {IND_VOWEL, "o"},
  /*04B*/ {IND_VOWEL, "o"},
  /*04C*/ {IND_VOWEL, "aU"},

  /*04D*/ {IND_HALANT, ""},

  /*04E*/ {IND_IGNORE, ""},

  /*04F*/ {IND_VOWEL, "ow"},

  /*050*/ {IND_INDEPENDENT_VOWEL, "A u m"},

  /*051*/ {IND_IGNORE, ""},
  /*052*/ {IND_IGNORE, ""},
  /*053*/ {IND_IGNORE, ""},
  /*054*/ {IND_IGNORE, ""},

  /*055*/ {IND_VOWEL, "e"},
  /*056*/ {IND_VOWEL, "u e"},
  /*057*/ {IND_VOWEL, "u: e"},

  /*058*/ {IND_CONSONANT, "q"},
  /*059*/ {IND_CONSONANT, "x"},
  /*05A*/ {IND_CONSONANT, "G"},
  /*05B*/ {IND_CONSONANT, "z"},
  /*05C*/ {IND_CONSONANT, "rr"},
  /*05D*/ {IND_CONSONANT, "rrh"},
  /*05E*/ {IND_CONSONANT, "f"},
  /*05F*/ {IND_CONSONANT, "j"},

  /*060*/ {IND_INDEPENDENT_VOWEL, "9r="},
  /*061*/ {IND_INDEPENDENT_VOWEL, "lr="},
  /*062*/ {IND_VOWEL, "lr="},
  /*063*/ {IND_VOWEL, "lr="},

  /*064*/ {IND_PUNC, ""},
  /*065*/ {IND_PUNC, ""},

  /*066*/ {IND_DIGIT, ""},
  /*067*/ {IND_DIGIT, ""},
  /*068*/ {IND_DIGIT, ""},
  /*069*/ {IND_DIGIT, ""},
  /*06A*/ {IND_DIGIT, ""},
  /*06B*/ {IND_DIGIT, ""},
  /*06C*/ {IND_DIGIT, ""},
  /*06D*/ {IND_DIGIT, ""},
  /*06E*/ {IND_DIGIT, ""},
  /*06F*/ {IND_DIGIT, ""},

  /*070*/ {IND_PUNC, ""},
  /*071*/ {IND_PUNC, ""},

  /*072*/ {IND_INDEPENDENT_VOWEL, "ay"},
  /*073*/ {IND_INDEPENDENT_VOWEL, "u e"},
  /*074*/ {IND_INDEPENDENT_VOWEL, "u: e"},
  /*075*/ {IND_INDEPENDENT_VOWEL, "ow"},
  /*076*/ {IND_INDEPENDENT_VOWEL, "u e"},
  /*077*/ {IND_INDEPENDENT_VOWEL, "u: e"},

  /*078*/ {IND_IGNORE, ""},

  /*079*/ {IND_CONSONANT, "z"},
  /*07A*/ {IND_CONSONANT, "j"},
  /*07B*/ {IND_CONSONANT, "G"},
  /*07C*/ {IND_CONSONANT, "z"},
  /*07D*/ {IND_CONSONANT, ""},
  /*07E*/ {IND_CONSONANT, "dr"},
  /*07F*/ {IND_CONSONANT, "b"},

};

static int cmu_indic_is_vowel(const char *p)
{
  /* this happens works for the vowels in the indic set */
  if (strchr("aeiouAEIOU",p[0]) == NULL)
    return FALSE;
  else
    return TRUE;
}

static int cmu_indic_lex_ord_to_offset(const int indic_ord) {
  int output=0;
  int i;
  int offset;

  i = indic_ord;
  if ((i >= 0x0900) && (i <= 0x097F))
    offset = 0x0900;
  if ((i >= 0x0980) && (i <= 0x09FF))
    offset = 0x0980;
  if ((i >= 0x0A00) && (i <= 0x0A7F))
    offset = 0x0A00;
  if ((i >= 0x0A80) && (i <= 0x0AFF))
    offset = 0x0A80;
  if ((i >= 0x0B00) && (i <= 0x0B7F))
    offset = 0x0B00;
  if ((i >= 0x0B80) && (i <= 0x0BFF))
    offset = 0x0B80;
  if ((i >= 0x0C00) && (i <= 0x0C7F))
    offset = 0x0C00;
  if ((i >= 0x0C80) && (i <= 0x0CFF))
    offset = 0x0C80;
  if ((i >= 0x0D00) && (i <= 0x0D7F))
    offset = 0x0D00;

  if (!offset) {
    cst_errmsg("Indic language can not process character 0x%x\n", i);
  } else {
    output = i - offset;
  }

  return output;
}

static int cmu_indic_get_char_type(const cst_val *indic_char) {
  int c;

  if (!indic_char) return IND_IGNORE;

  c = val_int(indic_char);
  if ((c < 0x0900) || (c > 0x0D7F))
    return IND_IGNORE;
  c = cmu_indic_lex_ord_to_offset(c);
  return cmu_indic_offset_char[c].type;
}

static const cst_val* cmu_indic_get_char_phoneme(const cst_val *indic_char) {
  int c;

  if (!indic_char) return string_val("");

  c = val_int(indic_char);
  c = cmu_indic_lex_ord_to_offset(c);
  return string_val(cmu_indic_offset_char[c].phoneme);
}

static cst_val *cmu_indic_lex_remove_ignore_chars(const cst_val *indic_ords) {
  cst_val *output = 0;
  const cst_val *v;

  for(v=indic_ords; v; v=val_cdr(v)) {
    if (cmu_indic_get_char_type(val_car(v)) == IND_IGNORE)
      continue;
    output = cons_val(val_car(v), output);
  }
  return val_reverse(output);
}

static cst_val *cmu_indic_lex_map_nukta_chars(const cst_val *indic_ords) {
  cst_val *output = 0;
  const cst_val *v;
  int i;
  int mapped_val;

  for(v=indic_ords; v; v=val_cdr(v)) {
    i = val_int(val_car(v));
    if (val_cdr(v) && cmu_indic_get_char_type(val_car(val_cdr(v))) == IND_NUKTA) {
      switch (i) {
        // Devanagari
        case 2325: mapped_val=2392; break; // क़
        case 2326: mapped_val=2393; break; // ख़
        case 2327: mapped_val=2394; break; // ग़
        case 2332: mapped_val=2395; break; // ज़
        case 2337: mapped_val=2396; break; // ड़
        case 2338: mapped_val=2397; break; // ढ़
        case 2347: mapped_val=2398; break; // फ़
        case 2351: mapped_val=2399; break; // य़

        // Bengali
        case 2465: mapped_val=2524; break;
        case 2566: mapped_val=2525; break;
        case 2479: mapped_val=2527; break;

        // Tamil
        case 2962: mapped_val=2964; break;
        default:
          mapped_val=i;
      };
      output = cons_val(int_val(mapped_val), output);
      v = val_cdr(v);
    } else {
      // Copy current item into output
      output = cons_val(val_car(v), output);
    }
  }

  return val_reverse(output);
}

cst_val *cmu_indic_lex_ord_to_phones(const cst_val *ords,
                                     int cmu_indic_variant_deletes_word_final_schwa)
{
  cst_val *in_ords = 0;
  cst_val *out_phone_strings = 0;
  cst_val *out_phones = 0;

  const cst_val *prev_char = 0;
  const cst_val *cur_char = 0;
  const cst_val *next_char = 0;
  const cst_val *remainder = 0;

  int cur_char_type = 0;
  int next_char_type = 0;

  // Ignore chars (filter)
  in_ords = cmu_indic_lex_remove_ignore_chars(ords);

  if (!in_ords) {
    return out_phones;
  }

  // Map Nukta Chars (filter)
  in_ords = cmu_indic_lex_map_nukta_chars(in_ords);

  prev_char = 0;
  cur_char = val_car(in_ords);
  if (val_cdr(in_ords)) {
    next_char = val_car(val_cdr(in_ords));
    remainder = val_cdr(val_cdr(in_ords));
  }
  else {
    next_char = 0;
    remainder = 0;
  }

  while (cur_char) {
    cur_char_type = cmu_indic_get_char_type(cur_char);
    if (next_char)
      next_char_type = cmu_indic_get_char_type(next_char);
    else
      next_char_type = IND_IGNORE;

    if (cur_char_type == IND_CONSONANT) {
      // Add consonant to the output list
      out_phone_strings = cons_val(cmu_indic_get_char_phoneme(cur_char),
                                   out_phone_strings);

      // If a consonant is followed by a combination vowel, a
      // halant, a punctuation then don't add a schwa after
      // it. Otherwise, insert a schwa. For end-of-word, check
      // whether we should insert schwa in this language.

      if (!next_char) { // We are in last char. Add schwa?
        if ((!prev_char) || // Always add schwa for one-char words
            (!cmu_indic_variant_deletes_word_final_schwa)) {
          out_phone_strings = cons_val(string_val("A"), out_phone_strings);
        } else {
          // Schwa deletion should probably happen depending
          // on whether there is a consonant cluster or not,
          // at the end. But Adding that rule here seems to
          // not have worked properly. Hence, we always
          // delete the final schwa.
        }
      } else { // Not a final char
        if ( (next_char_type != IND_VOWEL) &&
             (next_char_type != IND_PUNC) &&
             (next_char_type != IND_HALANT) &&
             (next_char_type != IND_IGNORE)) {
          out_phone_strings = cons_val(string_val("A"), out_phone_strings);
        }
      }
    } else if ( (cur_char_type == IND_VOWEL) ||
               (cur_char_type == IND_INDEPENDENT_VOWEL) ||
               (cur_char_type == IND_DIGIT)) {
      // Add whatever the pronunciation is
      out_phone_strings = cons_val(cmu_indic_get_char_phoneme(cur_char),
                                   out_phone_strings);
    } else if ( cur_char_type == IND_HALANT) {
      // Ignore
    } else if (cur_char_type == IND_AVAGRAHA) {
      // Lengthen previous vowel
      if (cmu_indic_get_char_type(prev_char) == IND_VOWEL) {
        out_phone_strings = cons_val(cmu_indic_get_char_phoneme(prev_char),
                                     out_phone_strings);
      }
    } else if (cur_char_type == IND_ANUSWAAR) {
      // The realization of anuswaar is context dependent: We
      // only generate a placeholder symbol and let postlexical
      // rules take care of this.
      out_phone_strings = cons_val(string_val("nX"),
                                   out_phone_strings);
    } else if (cur_char_type == IND_VISARGA) {
      // Add the glottal fricative
      out_phone_strings = cons_val(string_val("h"),
                                   out_phone_strings);

    } else {
      // IDEALLY we should warn for unhandled characters!
      // TODO?
    }

    prev_char = cur_char;
    cur_char = next_char;
    if (remainder) {
      next_char = val_car(remainder);
      remainder = val_cdr(remainder);
    } else {
      next_char = 0;
    }
  }

  const cst_val *v, *vx;
  cst_val *ph;
  for(v=out_phone_strings; v; v=val_cdr(v)) {
    ph = val_readlist_string(val_string(val_car(v)));
    for(vx=ph; vx; vx=val_cdr(vx)) {
      out_phones = cons_val(val_car(vx), out_phones);
    }
    delete_val(ph);
  }
  return val_reverse(out_phones);

}

cst_val *cmu_indic_lex_nasal_postfixes(const cst_val *in_phones) {
  // Given a phone sequence containing a special character nX
  // (contextual nasal), replace it with the appropriate nasal phone
  // based on its context

  if ((!in_phones) || !val_car(in_phones)) return 0;

  // Nazalise vowels at ends of words

  if ((val_cdr(in_phones)) &&
      (cmu_indic_is_vowel(val_string(val_car(in_phones)))) &&
      (!strcmp("nX", val_string(val_car(val_cdr(in_phones))))) &&
      ((!val_cdr(val_cdr(in_phones))) || (!val_car(val_cdr(val_cdr(in_phones)))))) {
    if (!strcmp("A", val_string(val_car(in_phones)))) {  // If it's a schwa, it's not nasalized. nX becomes m
      return cons_val(val_car(in_phones),
                      cons_val(string_val("m"),
                               cmu_indic_lex_nasal_postfixes(val_cdr(val_cdr(in_phones)))));
    } else {
      return cons_val(string_val(cst_strcat(val_string(val_car(in_phones)),
                                            "nas")),
                      cmu_indic_lex_nasal_postfixes(val_cdr(val_cdr(in_phones))));
    }
  } else if ( (val_cdr(in_phones)) &&
              (!strcmp("nX", val_string(val_car(in_phones))))) {
    // Choose nasal variant based on next consonant
    const char *next_c = val_string(phone_feature(&cmu_indic_phoneset,
                                            val_string(val_car(val_cdr(in_phones))),
                                            "cplace"));
    char *repl_ph;
    if (next_c) {
      switch (next_c[0]) {
        case 'v': repl_ph = "N"; break;
        case 'p': repl_ph = "n~"; break;
        case 'a': repl_ph = "nr"; break;
        case 'd': repl_ph = "nB"; break;
        case 'l': repl_ph = "m"; break;
        default: repl_ph = "nB";
      };
      return cons_val(string_val(repl_ph),
                      cons_val(val_car(val_cdr(in_phones)),
                               cmu_indic_lex_nasal_postfixes(val_cdr(val_cdr(in_phones)))));
    } else {
      return cons_val(val_car(in_phones),
                      cmu_indic_lex_nasal_postfixes(val_cdr(in_phones)));
    }
  } else {
      return cons_val(val_car(in_phones),
                      cmu_indic_lex_nasal_postfixes(val_cdr(in_phones)));

  }
}

/* For English derived pronunciation (latin scripted tokens) we map them */
/* to (hindi) phones -- this has to modified for other indic languages */
static const char * const eng_to_indic[99][3] =
    {
        {"aa", "A:", NULL },
        {"ae", "A", NULL },  /* changed this to A rather than e */
        {"ah", "A", NULL },
        {"ao", "o", NULL },
        {"aw", "aU", NULL },
        {"ax", "A", NULL },
        {"axr", "A", NULL },
        {"ay", "aI", NULL },
        {"b", "b", NULL },
        {"ch", "c", NULL },
        {"d", "dr", NULL },
        {"dh", "dB", NULL },
        {"eh", "e", NULL },
        {"er", "E", "9r" },
        {"ey", "ay", NULL },
        {"f", "ph", NULL },
        {"g", "g", NULL },
        {"hh", "hv", NULL },
        {"ih", "i", NULL },
        {"iy", "i:", NULL },
        {"jh", "J", NULL },
        {"k", "k", NULL },
        {"l", "l", NULL },
        {"m", "m", NULL },
        {"n", "nB", NULL },
        {"nx", "nB", NULL },
        {"ng", "nB", NULL },
        {"ow", "o", NULL },
        {"oy", "o", "j" },
        {"p", "p", NULL },
        {"r", "9r", NULL },
        {"s", "s", NULL },
        {"sh", "c}", NULL },
        {"t", "tr", NULL },
        {"th", "tBh", NULL },
        {"uh", "u", NULL },
        {"uw", "u:", NULL },
        {"v", "v", NULL },
        {"w", "v", NULL },
        {"y", "j", NULL },
        {"z", "s", NULL },
        {"zh", "c}", NULL },
        {NULL, NULL, NULL }
    };

cst_val *map_english_to_indic_phones(const char *indic_variant,
                                     const cst_val *english_phones)
{
    /* Map English (radio) phones to their Indic equivalent */
    cst_val *ip = NULL;
    const cst_val *v;
    char *english_phone;
    int i;

    for (v=english_phones; v; v=val_cdr(v))
    {
        english_phone = cst_strdup(val_string(val_car(v)));
        if ((english_phone[cst_strlen(english_phone)-1] == '0') ||
            (english_phone[cst_strlen(english_phone)-1] == '1'))
            /* It has a stress calue on it */
            english_phone[cst_strlen(english_phone)-1] = '\0';
        for (i=0; eng_to_indic[i][0]; i++)
        {
            if (cst_streq(english_phone,eng_to_indic[i][0]))
            {
                ip = cons_val(string_val(eng_to_indic[i][1]),ip);
                if (eng_to_indic[i][2])
                    ip = cons_val(string_val(eng_to_indic[i][2]),ip);
            }
            /* if there is no mapping, we drop the phone */
        }
        cst_free(english_phone);
    }
    ip = val_reverse(ip);

    return ip;
}

static cst_val *delete_medial_schwa(const cst_val *rphones)
{
    /* This schwa deletion follows the technique by Narsimhan et al (2001). */
    /* 1. Process input from right to left                                  */
    /* 2. If a schwa is found in a VC_CV context, then delete it.           */

    /* There are exceptions to this: (i) Phonotactic constraints of         */
    /* Hindi not violated, and no (ii) morpheme boundary present on the     */
    /* left. But I don't know how to handle these yet. So this will be      */
    /* wrong more often than the 11% reported in that paper. -- AUP         */
    if (rphones == NULL)
        return NULL;
    else if ((val_length(rphones) > 4) &&
             (cst_streq(val_string(val_car(val_cdr(val_cdr(rphones)))),"A")))
    {   /* we have a schwa comming up */
        if ((cmu_indic_is_vowel(val_string(val_car(rphones)))) &&
            (!cmu_indic_is_vowel(val_string(val_car(val_cdr(rphones))))) &&
            (!cmu_indic_is_vowel(val_string(val_car(val_cdr(val_cdr(val_cdr(rphones))))))) &&
            (cmu_indic_is_vowel(val_string(val_car(val_cdr(val_cdr(val_cdr(val_cdr(rphones)))))))))
            /* delete the schwa */
            return cons_val(val_car(rphones),
                            cons_val(val_car(val_cdr(rphones)),
                                     delete_medial_schwa(
                                      val_cdr(val_cdr(val_cdr(rphones))))));
        else
            return cons_val(val_car(rphones),
                            delete_medial_schwa(val_cdr(rphones)));
    }
    else
        return cons_val(val_car(rphones),
                        delete_medial_schwa(val_cdr(rphones)));
}

static cst_val *cmu_indic_hindi_schwa_fixes(cst_val *phones)
{
    cst_val *dd;

    if ((val_length(phones) > 3) &&
        (cst_streq("A",val_string(val_car(val_cdr(phones))))) &&
        (cst_streq("hv",val_string(val_car(val_cdr(val_cdr(phones)))))) &&
        ((cst_streq("A",val_string(val_car(val_cdr(val_cdr(val_cdr(phones))))))) ||
         (!cmu_indic_is_vowel(val_string(val_car(val_cdr(val_cdr(val_cdr(phones)))))))))
    {
        dd = (cst_val *)(void *)val_cdr(phones);
        set_cdr(phones,val_cdr(val_cdr(phones)));
        set_cdr(dd,NULL);
        delete_val(dd);
        return phones;
    }
    else
        return phones;
}

cst_val *cmu_indic_lex_lts_function(const struct lexicon_struct *l,
                                    const char *word, const char *pos,
                                    const cst_features *feats)
{
    cst_val *utflets = 0;
    cst_val *ords = 0;

    cst_val *phones = 0;
    cst_val *english_phones;
    cst_val *base_phones = NULL, *rp;
    const char *indic_variant = 0;
    int cmu_indic_variant_deletes_word_final_schwa=0;
    const cst_val *v;
    cst_val *tmpv;

    indic_variant = get_param_string(feats, "variant", "none");

    if (!strcmp(indic_variant, "hin")) {
      cmu_indic_variant_deletes_word_final_schwa = 1;
    } else if (!strcmp(indic_variant, "mar")) {
      cmu_indic_variant_deletes_word_final_schwa = 1;
    } else if (!strcmp(indic_variant, "tel")) {
      cmu_indic_variant_deletes_word_final_schwa = 0;
    } else if (!strcmp(indic_variant, "tam")) {
      cmu_indic_variant_deletes_word_final_schwa = 0;
    } else {
      cmu_indic_variant_deletes_word_final_schwa = 0;
      printf("Unknown indic variant: %s!\n", indic_variant);
    }

    if (cst_regex_match(cst_rx_not_indic,word))
    {  /* Its English like so us English Lexicon to find Phones and map
          to indic phones */
        /* printf("awb_debug cmu_indic_lex: English >%s<\n",word); */
        english_phones = lex_lookup(&cmu_lex,word,pos,feats);
        base_phones = map_english_to_indic_phones(indic_variant,english_phones);
        delete_val(english_phones);
    }
    else
    {
        /* string to utf8 chars */
        utflets = cst_utf8_explode(word);

        /* chars to ord */
        for (v=utflets; v; v=val_cdr(v)) {
            tmpv = cst_utf8_ord(val_car(v));
            ords = cons_val(tmpv, ords);
        }
        ords = val_reverse(ords);

        // Ords to Phone List (Map)
        base_phones = val_reverse(cmu_indic_lex_ord_to_phones(ords,
                                 cmu_indic_variant_deletes_word_final_schwa));

        delete_val(utflets);
        cst_free(ords);
    }

    // Postfix Indic Nasals
    phones = cmu_indic_lex_nasal_postfixes(base_phones);
    delete_val(base_phones); base_phones = phones;

    if (cst_streq(indic_variant,"hin"))
    {   /* Do schwa deletion */
        rp = val_reverse(base_phones);
        phones = val_reverse(delete_medial_schwa(rp));
        delete_val(rp); base_phones = phones;

        phones = cmu_indic_hindi_schwa_fixes(base_phones);
    }
    /* phone list to syls (done as post lexical rules) */

#if 0
    printf("cmu_indic_lex.c: indic word \"%s\" \n",word);
    val_print(stdout, phones);
    printf("\n");
#endif

    return phones;
}

static int cmu_indic_contains_vowel(const cst_val *r)
{
    const cst_val *x;

    for (x=r; x; x=val_cdr(x))
    {
        if (cmu_indic_is_vowel(val_string(val_car(x))))
            return TRUE;
    }

    return FALSE;
}

static int cmu_indic_has_vowel_in_syl(const cst_item *i)
{
    const cst_item *n;

    for (n=i; n; n=item_prev(n))
	if (cmu_indic_is_vowel(ffeature_string(n,"name")))
	    return TRUE;
    return FALSE;
}

int cmu_indic_syl_boundary(const cst_item *i,const cst_val *rest)
{
    if (!rest)
        return TRUE;
    else if (!cmu_indic_contains_vowel(rest))
        return FALSE;
    else if (!cmu_indic_has_vowel_in_syl(i))
        return FALSE;
    else if (rest && val_cdr(rest) &&
             cst_streq("n",val_string(val_car(rest))) &&
             !cmu_indic_is_vowel(val_string(val_car(rest))))
        return FALSE;
    else if (rest && val_cdr(rest) &&
             cmu_indic_is_vowel(ffeature_string(i,"name")) &&
             !cmu_indic_is_vowel(val_string(val_car(rest))) &&
             !cmu_indic_is_vowel(val_string(val_car(val_cdr(rest)))))
        return FALSE;
    else if (rest && val_cdr(rest) && val_cdr(val_cdr(rest)) &&
             !cmu_indic_is_vowel(val_string(val_car(rest))) &&
             !cmu_indic_is_vowel(val_string(val_car(val_cdr(rest)))) &&
             !cmu_indic_is_vowel(val_string(val_car(val_cdr(val_cdr(rest))))))
        return FALSE;
    else if (rest && val_cdr(rest) &&
             (cst_streq(val_string(val_car(rest)),
                        val_string(val_car(val_cdr(rest))))))
        return FALSE;
    else
        return TRUE;
}

#if 0
int cmu_indic_syl_boundary(const cst_item *i,const cst_val *rest)
{
    /* For debugging the syl boundary code */
    int x;

    printf("syl boundary %s | ",ffeature_string(i,"name"));
    if (rest)
        printf("%s ",val_string(val_car(rest)));
    if (rest && val_cdr(rest))
        printf("%s ",val_string(val_car(val_cdr(rest))));
    if (rest && val_cdr(rest) && val_cdr(val_cdr(rest)))
        printf("%s ",val_string(val_car(val_cdr(val_cdr(rest)))));

    x = cmu_indic_syl_boundary_x(i,rest);
    printf("is %d",x);

    printf("\n");

    return x;
}
#endif

cst_utterance *cmu_indic_assign_lex_stress(cst_utterance *u)
{
    /* Assign stress to a lexical entry: from indic_lexicon.scm (aup) */
    /* The stress is placed on the syllable with the highest weight.  */
    /* If there is a tie, the last-most syllable with highest weight  */
    /* is chosen.  However, the last syllable of the word does not    */
    /* participate in tie-breaking. That is, it is stressed only when */
    /* there are no ties. (Hussein 1997)                              */
    /* not sure this code actually follows the above rules, but its   */
    /* the same as the festival code -- awb 20140606                  */
    const cst_item *syl, *w;
    const char *x1, *x2, *x3;
    int syl_weight, best_weight, sw;
    const cst_item *stress_position;

    for (w=relation_head(utt_relation(u,"SylStructure")); w; w=item_next(w))
    {
        for (syl=item_daughter(w); syl; syl=item_next(syl))
        {   /* Assign weight to each syllable */
            syl_weight = 0;
            x1 = ffeature_string(syl,"R:SylStructure.daughtern.name");
            x2 = ffeature_string(syl,"R:SylStructure.daughtern.p.name");
            x3 = ffeature_string(syl,"R:SylStructure.daughtern.p.p.name");
            if (cmu_indic_is_vowel(x1))
            {   /* If syllable is open vowel */
                if (cst_streq(x1,"A") || cst_streq(x1,"i") ||
                    cst_streq(x1,"u"))
                    syl_weight = 1;
                else
                    syl_weight = 2;
            }
            else
            {
                if (cmu_indic_is_vowel(x2))
                {
                    if (cst_streq(x2,"A") || cst_streq(x2,"i") ||
                        cst_streq(x2,"u"))
                        syl_weight = 2;
                    else
                        syl_weight = 3;
                }
                else if (cmu_indic_is_vowel(x3))
                    syl_weight = 3;
            }
            item_set_int(syl,"syl_weight",syl_weight);
        }
    }

    for (w=relation_head(utt_relation(u,"SylStructure")); w; w=item_next(w))
    {
        best_weight = 0;
        stress_position = NULL;
        for (syl=item_daughter(w); syl; syl=item_next(syl))
        {
            sw = ffeature_int(syl,"syl_weight");
            if (sw > best_weight)
            {
                best_weight = sw;
                stress_position = syl;
            }
            else if ((sw == best_weight) && item_next(syl))
                stress_position = syl;
        }
        if (stress_position)
            item_set_string(stress_position,"stress","1");
    }

    return u;
}

cst_utterance *cmu_indic_postlex(cst_utterance *u)
{
    /* Post lexical rules */

    cmu_indic_assign_lex_stress(u);

#if 0
    /* Print out words as 'festival' lexical entries */
    const cst_item *word, *syl, *seg;

    for (word=relation_head(utt_relation(u,"Word"));
	 word; word=item_next(word))
    {
        printf("( \"%s\" nil (",ffeature_string(word,"name"));
        for (syl=item_daughter(item_as(word,"SylStructure"));
             syl;syl=item_next(syl))
        {
            printf("(( ");
            for (seg=item_daughter(item_as(syl,"SylStructure")); seg;
                 seg=item_next(seg))
            {
                printf("%s ",ffeature_string(seg,"name"));
            }
            printf(") %s %d) ",
                   ffeature_string(syl,"stress"),
                   ffeature_int(syl,"syl_weight"));

        }
        printf("))\n");
    }
#endif

    return u;
}

cst_lexicon cmu_indic_lex;

cst_lexicon *cmu_indic_lex_init(void)
{
    /* Should it be global const or dynamic */
    /* Can make lts_rules just a cart tree like others */
    cst_lexicon *l;

    if (cmu_indic_lex.lts_function)
        return &cmu_indic_lex;
    l = &cmu_indic_lex;
    l->name = "cmu_indic_lex";

    l->lts_function = cmu_indic_lex_lts_function;
    l->syl_boundary = cmu_indic_syl_boundary;
    l->postlex = cmu_indic_postlex;
    return l;

}
