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


void test_local_voice(void)
{
    common_init();
    TEST_CHECK(mimic_voice_select("rms") != NULL);
}


void test_url_voice(void)
{
    common_init();
    TEST_CHECK(mimic_voice_select("http://www.festvox.org/flite/packed/flite-2.0/voices/cmu_us_rms.flitevox") != NULL);
}

void test_file_voice(void)
{
    common_init();
    TEST_CHECK(mimic_voice_select("../voices/cmu_us_rms.flitevox") != NULL);
}


TEST_LIST = {
    { "local voice", test_local_voice },
    { "voice file", test_file_voice },
    { "voice url", test_url_voice },
    {0}
};

