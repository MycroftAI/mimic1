/* Copyright 2017 Sergio Oller
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

/* This file defines the phoneset used by Saga to transcribe text.
 *
 * A cst_phoneset consists fundamentally of:
 *  - A list of phones
 *  - A list of linguistic features for each phone
 *
 * Most phonesets define the same set of features (see es_saga_featnames),
 * related to how are these phones pronounced.
 *
 * The features of each phone have been extracted from the Saga
 * documentation. (Listerri and Mariño 1993, "Spanish adaptation of SAMPA
 * and automatic phonetic transcription" Technical report for ESPRIT PROJECT
 * 6819)
 *
 */
#include "cst_phoneset.h"
#include "cst_val.h"
#include "stdlib.h"

static const char *const es_saga_featnames[] = {
    "vc", /* vowel or consonant */
    "vlng", /* vowel length: short long dipthong schwa */
    "vheight", /* vowel height: high mid low */
    "vfront", /* vowel frontness: front mid back */
    "vrnd", /* lip rounding */
    "ctype", /* consonant type: stop fricative affricative nasal liquid trill
                tap approximant */
    "cplace", /* place of articulation: labial alveolar palatal labio-dental
                 dental interdental velar */
    "cvox", /* consonant voicing */
    NULL
};

static const char *const es_saga_phonenames[] = {
    "p",  "b",  "t",  "d",  "k",  "g",  "m",  "n",   "J",  "N",

    "tS", "f",  "T",  "s",  "z",  "jj", "x",  "l",   "L",  "rr",

    "j",  "w",  "B",  "D",  "G",  "r",  "a",  "e",   "i",  "o",

    "u",  "a1", "e1", "i1", "o1", "u1", "dZ", "pau", "h#", "brth",

    NULL
};

#if 0
/* These features are converted to integers below and are kept here for readability */
static const int es_fv_000[] = { "-", "0", "-", "-", "-", "s", "l", "-", -1 };	/* "p" */
static const int es_fv_001[] = { "-", "0", "-", "-", "-", "s", "l", "+", -1 }; 	/* "b" */
static const int es_fv_002[] = { "-", "0", "-", "-", "-", "s", "d", "-", -1 }; 	/* "t" */
static const int es_fv_003[] = { "-", "0", "-", "-", "-", "s", "d", "+", -1 }; 	/* "d" */
static const int es_fv_004[] = { "-", "0", "-", "-", "-", "s", "v", "-", -1 }; 	/* "k" */
static const int es_fv_005[] = { "-", "0", "-", "-", "-", "s", "v", "+", -1 }; 	/* "g" */
static const int es_fv_006[] = { "-", "0", "-", "-", "-", "n", "l", "+", -1 }; 	/* "m" */
static const int es_fv_007[] = { "-", "0", "-", "-", "-", "n", "a", "+", -1 }; 	/* "n" */
static const int es_fv_008[] = { "-", "0", "-", "-", "-", "n", "p", "+", -1 }; 	/* "J" */
static const int es_fv_009[] = { "-", "0", "-", "-", "-", "n", "v", "+", -1 }; 	/* "N" */
	
static const int es_fv_010[] = { "-", "0", "-", "-", "-", "a", "p", "-", -1 }; 	/* "tS" */
static const int es_fv_011[] = { "-", "0", "-", "-", "-", "f", "b", "-", -1 }; 	/* "f" */
static const int es_fv_012[] = { "-", "0", "-", "-", "-", "f", "i", "-", -1 }; 	/* "T" */
static const int es_fv_013[] = { "-", "0", "-", "-", "-", "f", "a", "-", -1 }; 	/* "s" */
static const int es_fv_014[] = { "-", "0", "-", "-", "-", "f", "a", "+", -1 }; 	/* "z" */
static const int es_fv_015[] = { "-", "0", "-", "-", "-", "f", "p", "+", -1 }; 	/* "jj" */
static const int es_fv_016[] = { "-", "0", "-", "-", "-", "f", "v", "-", -1 }; 	/* "x" */
static const int es_fv_017[] = { "-", "0", "-", "-", "-", "l", "a", "+", -1 }; 	/* "l" */
static const int es_fv_018[] = { "-", "0", "-", "-", "-", "l", "p", "+", -1 }; 	/* "L" */
static const int es_fv_019[] = { "-", "0", "-", "-", "-", "tr", "a", "+", -1 }; 	/* "rr" */

static const int es_fv_020[] = { "-", "0", "-", "-", "-", "ap", "p", "+", -1 }; 	/* "j" */
static const int es_fv_021[] = { "-", "0", "-", "-", "-", "ap", "v", "+", -1 }; 	/* "w" */
static const int es_fv_022[] = { "-", "0", "-", "-", "-", "ap", "l", "+", -1 }; 	/* "B" */
static const int es_fv_023[] = { "-", "0", "-", "-", "-", "ap", "d", "+", -1 }; 	/* "D" */
static const int es_fv_024[] = { "-", "0", "-", "-", "-", "ap", "v", "+", -1 }; 	/* "G" */
static const int es_fv_025[] = { "-", "0", "-", "-", "-", "t", "a", "+", -1 }; 	/* "r" */
static const int es_fv_026[] = { "+", "l", "3", "2", "-", "0", "0", "-", -1 }; 	/* "a" */
static const int es_fv_027[] = { "+", "l", "2", "1", "-", "0", "0", "-", -1 }; 	/* "e" */
static const int es_fv_028[] = { "+", "l", "1", "1", "-", "0", "0", "-", -1 }; 	/* "i" */
static const int es_fv_029[] = { "+", "l", "2", "3", "+", "0", "0", "-", -1 }; 	/* "o" */

static const int es_fv_030[] = { "+", "l", "1", "3", "+", "0", "0", "-", -1 }; 	/* "u" */
static const int es_fv_031[] = { "+", "l", "3", "2", "-", "0", "0", "-", -1 }; 	/* "a1" */
static const int es_fv_032[] = { "+", "l", "2", "1", "-", "0", "0", "-", -1 }; 	/* "e1" */
static const int es_fv_033[] = { "+", "l", "1", "1", "-", "0", "0", "-", -1 }; 	/* "i1" */
static const int es_fv_034[] = { "+", "l", "2", "3", "+", "0", "0", "-", -1 }; 	/* "o1" */
static const int es_fv_035[] = { "+", "l", "1", "3", "+", "0", "0", "-", -1 }; 	/* "u1" */
static const int es_fv_036[] = { "-", "0", "-", "-", "-", "a", "p", "+", -1 }; 	/* "dZ" */
static const int es_fv_037[] = { "-", "0", "-", "-", "-", "0", "0", "-", -1 }; 	/* "pau" */
static const int es_fv_038[] = { "-", "0", "-", "-", "-", "0", "0", "-", -1 }; 	/* "h#" */
static const int es_fv_039[] = { "-", "0", "-", "-", "-", "0", "0", "-", -1 }; 	/* "brth" */
#endif

/* The numbers defined here are related to the feature values above
 * through the featvals array defined below */
static const int es_fv_000[] = { 3, 4, 3, 3, 3, 5, 1, 3, -1 }; /* "p" */
static const int es_fv_001[] = { 3, 4, 3, 3, 3, 5, 1, 0, -1 }; /* "b" */
static const int es_fv_002[] = { 3, 4, 3, 3, 3, 5, 8, 3, -1 }; /* "t" */
static const int es_fv_003[] = { 3, 4, 3, 3, 3, 5, 8, 0, -1 }; /* "d" */
static const int es_fv_004[] = { 3, 4, 3, 3, 3, 5, 15, 3, -1 }; /* "k" */
static const int es_fv_005[] = { 3, 4, 3, 3, 3, 5, 15, 0, -1 }; /* "g" */
static const int es_fv_006[] = { 3, 4, 3, 3, 3, 13, 1, 0, -1 }; /* "m" */
static const int es_fv_007[] = { 3, 4, 3, 3, 3, 13, 9, 0, -1 }; /* "n" */
static const int es_fv_008[] = { 3, 4, 3, 3, 3, 13, 11, 0, -1 }; /* "J" */
static const int es_fv_009[] = { 3, 4, 3, 3, 3, 13, 15, 0, -1 }; /* "N" */

static const int es_fv_010[] = { 3, 4, 3, 3, 3, 9, 11, 3, -1 }; /* "tS" */
static const int es_fv_011[] = { 3, 4, 3, 3, 3, 12, 14, 3, -1 }; /* "f" */
static const int es_fv_012[] = { 3, 4, 3, 3, 3, 12, 17, 3, -1 }; /* "T" */
static const int es_fv_013[] = { 3, 4, 3, 3, 3, 12, 9, 3, -1 }; /* "s" */
static const int es_fv_014[] = { 3, 4, 3, 3, 3, 12, 9, 0, -1 }; /* "z" */
static const int es_fv_015[] = { 3, 4, 3, 3, 3, 12, 11, 0, -1 }; /* "jj" */
static const int es_fv_016[] = { 3, 4, 3, 3, 3, 12, 15, 3, -1 }; /* "x" */
static const int es_fv_017[] = { 3, 4, 3, 3, 3, 1, 9, 0, -1 }; /* "l" */
static const int es_fv_018[] = { 3, 4, 3, 3, 3, 1, 11, 0, -1 }; /* "L" */
static const int es_fv_019[] = { 3, 4, 3, 3, 3, 18, 9, 0, -1 }; /* "rr" */

static const int es_fv_020[] = { 3, 4, 3, 3, 3, 20, 11, 0, -1 }; /* "j" */
static const int es_fv_021[] = { 3, 4, 3, 3, 3, 20, 15, 0, -1 }; /* "w" */
static const int es_fv_022[] = { 3, 4, 3, 3, 3, 20, 1, 0, -1 }; /* "B" */
static const int es_fv_023[] = { 3, 4, 3, 3, 3, 20, 8, 0, -1 }; /* "D" */
static const int es_fv_024[] = { 3, 4, 3, 3, 3, 20, 15, 0, -1 }; /* "G" */
static const int es_fv_025[] = { 3, 4, 3, 3, 3, 19, 9, 0, -1 }; /* "r" */
static const int es_fv_026[] = { 0, 1, 2, 7, 3, 4, 4, 3, -1 }; /* "a" */
static const int es_fv_027[] = { 0, 1, 7, 6, 3, 4, 4, 3, -1 }; /* "e" */
static const int es_fv_028[] = { 0, 1, 6, 6, 3, 4, 4, 3, -1 }; /* "i" */
static const int es_fv_029[] = { 0, 1, 7, 2, 0, 4, 4, 3, -1 }; /* "o" */

static const int es_fv_030[] = { 0, 1, 6, 2, 0, 4, 4, 3, -1 }; /* "u" */
static const int es_fv_031[] = { 0, 1, 2, 7, 3, 4, 4, 3, -1 }; /* "a1" */
static const int es_fv_032[] = { 0, 1, 7, 6, 3, 4, 4, 3, -1 }; /* "e1" */
static const int es_fv_033[] = { 0, 1, 6, 6, 3, 4, 4, 3, -1 }; /* "i1" */
static const int es_fv_034[] = { 0, 1, 7, 2, 0, 4, 4, 3, -1 }; /* "o1" */
static const int es_fv_035[] = { 0, 1, 6, 2, 0, 4, 4, 3, -1 }; /* "u1" */
static const int es_fv_036[] = { 3, 4, 3, 3, 3, 9, 11, 0, -1 }; /* "dZ" */
static const int es_fv_037[] = { 3, 4, 3, 3, 3, 4, 4, 3, -1 }; /* "pau" */
static const int es_fv_038[] = { 3, 4, 3, 3, 3, 4, 4, 3, -1 }; /* "h#" */
static const int es_fv_039[] = { 3, 4, 3, 3, 3, 4, 4, 3, -1 }; /* "brth" */

static const int *const es_saga_fvtable[] = {
    es_fv_000, es_fv_001, es_fv_002, es_fv_003, es_fv_004, es_fv_005, es_fv_006,
    es_fv_007, es_fv_008, es_fv_009, es_fv_010, es_fv_011, es_fv_012, es_fv_013,
    es_fv_014, es_fv_015, es_fv_016, es_fv_017, es_fv_018, es_fv_019, es_fv_020,
    es_fv_021, es_fv_022, es_fv_023, es_fv_024, es_fv_025, es_fv_026, es_fv_027,
    es_fv_028, es_fv_029, es_fv_030, es_fv_031, es_fv_032, es_fv_033, es_fv_034,
    es_fv_035, es_fv_036, es_fv_037, es_fv_038, es_fv_039
};

DEF_STATIC_CONST_VAL_STRING(featval_0, "+");
DEF_STATIC_CONST_VAL_STRING(featval_1, "l");
DEF_STATIC_CONST_VAL_STRING(featval_2, "3");
DEF_STATIC_CONST_VAL_STRING(featval_3, "-");
DEF_STATIC_CONST_VAL_STRING(featval_4, "0");
DEF_STATIC_CONST_VAL_STRING(featval_5, "s");
DEF_STATIC_CONST_VAL_STRING(featval_6, "1");
DEF_STATIC_CONST_VAL_STRING(featval_7, "2");
DEF_STATIC_CONST_VAL_STRING(featval_8, "d");
DEF_STATIC_CONST_VAL_STRING(featval_9, "a");
DEF_STATIC_CONST_VAL_STRING(featval_10, "r");
DEF_STATIC_CONST_VAL_STRING(featval_11, "p");
DEF_STATIC_CONST_VAL_STRING(featval_12, "f");
DEF_STATIC_CONST_VAL_STRING(featval_13, "n");
DEF_STATIC_CONST_VAL_STRING(featval_14, "b");
DEF_STATIC_CONST_VAL_STRING(featval_15, "v");
DEF_STATIC_CONST_VAL_STRING(featval_16, "g");
DEF_STATIC_CONST_VAL_STRING(featval_17, "i"); /* interdental */
DEF_STATIC_CONST_VAL_STRING(featval_18, "tr"); /* trill */
DEF_STATIC_CONST_VAL_STRING(featval_19, "t"); /* tap */
DEF_STATIC_CONST_VAL_STRING(featval_20, "ap"); /* approximant */

static const cst_val *const es_saga_featvals[] = {
    (cst_val *) &featval_0,  (cst_val *) &featval_1,
    (cst_val *) &featval_2,  (cst_val *) &featval_3,
    (cst_val *) &featval_4,  (cst_val *) &featval_5,
    (cst_val *) &featval_6,  (cst_val *) &featval_7,
    (cst_val *) &featval_8,  (cst_val *) &featval_9,
    (cst_val *) &featval_10, (cst_val *) &featval_11,
    (cst_val *) &featval_12, (cst_val *) &featval_13,
    (cst_val *) &featval_14, (cst_val *) &featval_15,
    (cst_val *) &featval_16, (cst_val *) &featval_17,
    (cst_val *) &featval_18, (cst_val *) &featval_19,
    (cst_val *) &featval_20, NULL
};

const cst_phoneset es_saga_phoneset = {
    "es_saga",
    es_saga_featnames,
    es_saga_featvals,
    es_saga_phonenames,
    "pau",
    40,
    es_saga_fvtable,
    0 /* not freeable */
};
