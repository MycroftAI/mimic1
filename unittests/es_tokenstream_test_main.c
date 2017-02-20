#include "cutest.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "cst_tokenstream.h"
#include "cst_lexicon.h"
#include "cst_synth.h"

#include "es_lang.h"

#ifndef TEST_FILE
#define TEST_FILE "es_text.txt"
#endif

static const cst_synth_module text_to_words[] = {
    {"tokenizer_func", default_tokenization},
    {"textanalysis_func", default_textanalysis},
    {"pos_tagger_func", NULL},
    {"phrasing_func", NULL},
    {"lexical_insertion_func", NULL},
    {"pause_insertion_func", NULL},
    {"intonation_func", NULL},
    {"postlex_func", NULL},
    {"duration_model_func", NULL},
    {"f0_model_func", NULL},
    {"wave_synth_func", NULL},
    {"post_synth_hook_func", NULL},
    {NULL, NULL}
};



void test_es_tokenizer(void)
{
    /* One entry per token */
    char *names[] = { "Hola", "hoy", "es", "17", "de", "julio", "Patata",
        "es", "un", "sustantivo", "Qué", "día", "tan",
        "bonito", "Te", "gusta", "tomar", "el", "sol"
    };
    char *puncs[] = { ",", "", "", "", "", ".",
        "»", "", "", ".",
        "", "", "", "!", "", "", "", "", "?"
    };
    char *prepuncs[] = { "", "", "", "", "", "",
        "«", "", "", "",
        "¡", "", "", "", "¿", "", "", "", ""
    };
    int expected_num_tok = 19;  /* Expected number of tokens */
    char *word_list[] = { "hola", "hoy", "es", "diecisiete", "de", "julio",
        "patata", "es", "un", "sustantivo", "qué", "día", "tan", "bonito",
        "te", "gusta", "tomar", "el", "sol"};
    int expected_num_words = 19;

    cst_tokenstream *ts;
    cst_utterance *u;
    cst_relation *rel;
    cst_item *it;
    int i = 0, j = 0;
    const char *name, *prepunc, *punc;
    cst_voice *v = new_voice();
    const char *text_line;
    es_init(v);
    /* Read file line by line. Each ts_get(ts) will return a line */
    ts = ts_open(TEST_FILE, "\n\r", "", "", "");
    TEST_CHECK(ts != NULL);
    while (!ts_eof(ts))
    {
        /* For each line, create an utterance, create its tokens and
         * convert it to words */
        text_line = ts_get(ts);
        u = new_utterance();
        utt_set_input_text(u, text_line);
        utt_init(u, v);
        apply_synth_method(u, text_to_words);
        /* First check: Tokens are correct */
        rel = utt_relation(u, "Token");
        TEST_CHECK(rel != NULL);
        it = relation_head(rel);
        while (it != NULL)
        {
            name = item_feat_string(it, "name");
            /* printf("name: %s\n", name); */
            TEST_CHECK(strcmp(name, names[i]) == 0);
            prepunc = item_feat_string(it, "prepunctuation");
            TEST_CHECK(strcmp(prepunc, prepuncs[i]) == 0);
            punc = item_feat_string(it, "punc");
            TEST_CHECK(strcmp(punc, puncs[i]) == 0);
            it = item_next(it);
            i++;
        }
        /* Second check: Words are correct */
        rel = utt_relation(u, "Word");
        TEST_CHECK(rel != NULL);
        for (it = relation_head(rel); it; it = item_next(it)) {
            /*printf("name: %s\n", item_feat_string(it, "name"));*/
            TEST_CHECK(cst_streq(item_feat_string(it, "name"), word_list[j]));
            j++;
        }
        delete_utterance(u);
    }
        TEST_CHECK(i == expected_num_tok);      /* verify number of tokens */
        TEST_CHECK(j == expected_num_words);      /* verify number of words */
    ts_close(ts);
    delete_voice(v);
}

TEST_LIST = {{"Spanish token to words", test_es_tokenizer}, {0}};
