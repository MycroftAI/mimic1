#include "cutest.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "cst_tokenstream.h"
#include <mimic.h>
cst_val *mimic_set_voice_list(const char *voxdir);

void *mimic_set_lang_list(void);

void common_init(void)
{
    mimic_init();
    mimic_set_lang_list();

    if (mimic_voice_list == NULL)
        mimic_set_voice_list("../voices");
}

void test_no_voice_list(void)
{
    TEST_CHECK(mimic_voice_select("rms") == NULL);
    TEST_CHECK(mimic_voice_select
               ("http://www.festvox.org/flite/packed/flite-2.0/voices/cmu_us_rms.flitevox")
               == NULL);
    TEST_CHECK(mimic_voice_select("../voices/cmu_us_rms.flitevox") == NULL);

}

void test_null(void)
{
    common_init();
    TEST_CHECK(mimic_voice_select(NULL) ==
               (void *) val_voice(val_car(mimic_voice_list)));
}

void test_empty_string(void)
{
    common_init();
    TEST_CHECK(mimic_voice_select("") != NULL);
}

void test_local_voice(void)
{
    common_init();
    TEST_CHECK(mimic_voice_select("rms") != NULL);
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
               ("http://www.festvox.org/flite/packed/flite-2.0/voices/invalid.flitevox")
               == NULL);
}

void test_file_voice(void)
{
    common_init();
    TEST_CHECK(mimic_voice_select("../voices/cmu_us_rms.flitevox") != NULL);
}

TEST_LIST =
{
    {"no voice list", test_no_voice_list},
    {"local voice", test_local_voice},
    {"voice file", test_file_voice},
    {"empty string", test_empty_string},
    {"NULL", test_null},
    {"illegal voice url", test_invalid_url},
    {"voice url", test_url_voice},
    {0}
};
