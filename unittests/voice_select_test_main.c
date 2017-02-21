#include "cutest.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "cst_tokenstream.h"
#include <mimic.h>

#ifndef VOICE_LIST_DIR
  #define VOICE_LIST_DIR "../voices"
#endif

#ifndef A_VOICE
  #define A_VOICE "../voices/cmu_us_rms.flitevox"
#endif

cst_val *mimic_set_voice_list(const char *voxdir);

void *mimic_set_lang_list(void);

void common_init(void)
{
    mimic_init();
    mimic_set_lang_list();

    if (mimic_voice_list == NULL)
        mimic_set_voice_list(VOICE_LIST_DIR);
}

void test_no_voice_list(void)
{
    mimic_init();
    mimic_set_lang_list();
    TEST_CHECK(mimic_voice_select("rms") == NULL);
    TEST_CHECK(mimic_voice_select(NULL) == NULL);
}

void test_null(void)
{
    common_init();
    if (mimic_voice_list == NULL)
    {
        fprintf(stderr, "This test makes sense when voices are compiled\n");
        return;
    }
    TEST_CHECK(mimic_voice_select(NULL) ==
               (void *) val_voice(val_car(mimic_voice_list)));
}

void test_empty_string(void)
{
    common_init();
    if (mimic_voice_list == NULL)
    {
        fprintf(stderr, "This test makes sense when voices are compiled\n");
        return;
    }
    TEST_CHECK(mimic_voice_select("") != NULL);
}

void test_local_voice(void)
{
    const char *name;
    common_init();
    if (mimic_voice_list == NULL)
    {
        fprintf(stderr, "This test makes sense when voices are compiled\n");
        return;
    }
    name = val_voice(val_car(mimic_voice_list))->name;
    TEST_CHECK(mimic_voice_select(name) != NULL);
}

void test_url_voice(void)
{
    common_init();
    TEST_CHECK(mimic_voice_select
               ("http://www.festvox.org/flite/packed/flite-2.0/voices/cmu_us_rms.flitevox")
               != NULL);
     
}

void test_invalid_url(void)
{
    common_init();
    TEST_CHECK(mimic_voice_select
               ("http://www.example.com/invalid.flitevox")
               == NULL);
}

void test_file_voice(void)
{
    common_init();
    TEST_CHECK(mimic_voice_select(A_VOICE) != NULL);
}

TEST_LIST =
{
    {"no voice list", test_no_voice_list},
    {"local voice", test_local_voice},
    {"voice file", test_file_voice},
    {"empty string", test_empty_string},
    {"NULL", test_null},
    {"illegal voice url", test_invalid_url},
    /* Skip test that requires internet connection.
     * This test requires the download of several MB from a 3rd party server in a
     * non secure connection. This is bad because:
     * - It may add unexpected traffic on the festvox servers
     * - It downloads a significant amount of information. Users with limited
     *   data per month may suffer unadvertedly.
     * - If there is a bug in mimic that allows for arbitrary code execution
     *   when loading a flitevox file, a malicious user might exploit it by
     *   using non-https connections.
     *
     * As a proper solution a local http server could be setup when testing this
     * or we could simply comment out the test:
     */
    /*{"voice url", test_url_voice},*/
    {0}
};
