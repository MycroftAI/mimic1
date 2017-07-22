/* ----------------------------------------------------------------- */
/*           The English TTS System "Flite+hts_engine"               */
/*           developed by HTS Working Group                          */
/*           http://hts-engine.sourceforge.net/                      */
/* ----------------------------------------------------------------- */
/*                                                                   */
/*  Copyright (c) 2005-2016  Nagoya Institute of Technology          */
/*                           Department of Computer Science          */
/*                                                                   */
/*                2005-2008  Tokyo Institute of Technology           */
/*                           Interdisciplinary Graduate School of    */
/*                           Science and Engineering                 */
/*                                                                   */
/* All rights reserved.                                              */
/*                                                                   */
/* Redistribution and use in source and binary forms, with or        */
/* without modification, are permitted provided that the following   */
/* conditions are met:                                               */
/*                                                                   */
/* - Redistributions of source code must retain the above copyright  */
/*   notice, this list of conditions and the following disclaimer.   */
/* - Redistributions in binary form must reproduce the above         */
/*   copyright notice, this list of conditions and the following     */
/*   disclaimer in the documentation and/or other materials provided */
/*   with the distribution.                                          */
/* - Neither the name of the HTS working group nor the names of its  */
/*   contributors may be used to endorse or promote products derived */
/*   from this software without specific prior written permission.   */
/*                                                                   */
/* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND            */
/* CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,       */
/* INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF          */
/* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE          */
/* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS */
/* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,          */
/* EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED   */
/* TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,     */
/* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON */
/* ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,   */
/* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY    */
/* OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE           */
/* POSSIBILITY OF SUCH DAMAGE.                                       */
/* ----------------------------------------------------------------- */
/* mimic adaptations by Sergio Oller, 2017 */

#include "config.h"
#include "cst_synth.h"
#include "cst_utt_utils.h"
#include <math.h>
#include "cst_file.h"
#include "cst_val.h"
#include "cst_string.h"
#include "cst_alloc.h"
#include "cst_item.h"
#include "cst_relation.h"
#include "cst_utterance.h"
#include "cst_tokenstream.h"
#include "cst_string.h"
#include "cst_regex.h"
#include "cst_features.h"
#include "cst_utterance.h"
#include "mimic.h"
#include "cst_synth.h"
#include "cst_utt_utils.h"

#include "flite_hts_engine.h"

#if HAVE_HTSENGINE == 0

#include "cst_error.h"

CST_VAL_REGISTER_TYPE(flitehtsengine, Flite_HTS_Engine);

void delete_flitehtsengine(Flite_HTS_Engine * f)
{
    cst_errmsg("HTS Engine not supported in this compiled version of mimic");
    return;
}

#else /* HAVE_HTSENGINE */


#define MAXBUFLEN 1024

CST_VAL_REGISTER_TYPE(flitehtsengine, Flite_HTS_Engine);

/* create_label: create label per phoneme */
static void create_label(cst_item *item, char *label)
{
    const char *p1 = ffeature_string(item, "p.p.name");
    const char *p2 = ffeature_string(item, "p.name");
    const char *p3 = ffeature_string(item, "name");
    const char *p4 = ffeature_string(item, "n.name");
    const char *p5 = ffeature_string(item, "n.n.name");

    if (strcmp(p3, "pau") == 0)
    {
        /* for pause */
        int a3 =
            ffeature_int(item,
                         "p.R:SylStructure.parent.R:Syllable.syl_numphones");
        int c3 =
            ffeature_int(item,
                         "n.R:SylStructure.parent.R:Syllable.syl_numphones");
        int d2 =
            ffeature_int(item,
                         "p.R:SylStructure.parent.parent.R:Word.word_numsyls");
        int f2 =
            ffeature_int(item,
                         "n.R:SylStructure.parent.parent.R:Word.word_numsyls");
        int g1 =
            ffeature_int(item,
                         "p.R:SylStructure.parent.parent.R:Phrase.parent.lisp_num_syls_in_phrase");
        int g2 =
            ffeature_int(item,
                         "p.R:SylStructure.parent.parent.R:Phrase.parent.lisp_num_words_in_phrase");
        int i1 =
            ffeature_int(item,
                         "n.R:SylStructure.parent.parent.R:Phrase.parent.lisp_num_syls_in_phrase");
        int i2 =
            ffeature_int(item,
                         "n.R:SylStructure.parent.parent.R:Phrase.parent.lisp_num_words_in_phrase");
        int j1, j2, j3;
        if (item_next(item) != NULL)
        {
            j1 = ffeature_int(item,
                              "n.R:SylStructure.parent.parent.R:Phrase.parent.lisp_total_syls");
            j2 = ffeature_int(item,
                              "n.R:SylStructure.parent.parent.R:Phrase.parent.lisp_total_words");
            j3 = ffeature_int(item,
                              "n.R:SylStructure.parent.parent.R:Phrase.parent.lisp_total_phrases");
        }
        else
        {
            j1 = ffeature_int(item,
                              "p.R:SylStructure.parent.parent.R:Phrase.parent.lisp_total_syls");
            j2 = ffeature_int(item,
                              "p.R:SylStructure.parent.parent.R:Phrase.parent.lisp_total_words");
            j3 = ffeature_int(item,
                              "p.R:SylStructure.parent.parent.R:Phrase.parent.lisp_total_phrases");
        }
        sprintf(label, "%s^%s-%s+%s=%s@xx_xx/A:%s_%s_%s/B:xx-xx-xx@xx-xx&xx-xx#xx-xx$xx-xx!xx-xx;xx-xx|xx/C:%s+%s+%s/D:%s_%s/E:xx+xx@xx+xx&xx+xx#xx+xx/F:%s_%s/G:%s_%s/H:xx=xx^xx=xx|xx/I:%s=%s/J:%d+%d-%d",    /* */
                strcmp(p1, "0") == 0 ? "xx" : p1,       /* p1 */
                strcmp(p2, "0") == 0 ? "xx" : p2,       /* p2 */
                p3,             /* p3 */
                strcmp(p4, "0") == 0 ? "xx" : p4,       /* p4 */
                strcmp(p5, "0") == 0 ? "xx" : p5,       /* p5 */
                a3 == 0 ? "xx" : ffeature_string(item, "p.R:SylStructure.parent.R:Syllable.stress"),    /* a1 */
                a3 == 0 ? "xx" : ffeature_string(item, "p.R:SylStructure.parent.R:Syllable.accented"),  /* a2 */
                a3 == 0 ? "xx" : val_string(val_string_n(a3)),  /* a3 */
                c3 == 0 ? "xx" : ffeature_string(item, "n.R:SylStructure.parent.R:Syllable.stress"),    /* c1 */
                c3 == 0 ? "xx" : ffeature_string(item, "n.R:SylStructure.parent.R:Syllable.accented"),  /* c2 */
                c3 == 0 ? "xx" : val_string(val_string_n(c3)),  /* c3 */
                d2 == 0 ? "xx" : ffeature_string(item, "p.R:SylStructure.parent.parent.R:Word.gpos"),   /* d1 */
                d2 == 0 ? "xx" : val_string(val_string_n(d2)),  /* d2 */
                f2 == 0 ? "xx" : ffeature_string(item, "n.R:SylStructure.parent.parent.R:Word.gpos"),   /* f1 */
                f2 == 0 ? "xx" : val_string(val_string_n(f2)),  /* f2 */
                g1 == 0 ? "xx" : val_string(val_string_n(g1)),  /* g1 */
                g2 == 0 ? "xx" : val_string(val_string_n(g2)),  /* g2 */
                i1 == 0 ? "xx" : val_string(val_string_n(i1)),  /* i1 */
                i2 == 0 ? "xx" : val_string(val_string_n(i2)),  /* i2 */
                j1,             /* j1 */
                j2,             /* j2 */
                j3);            /* j3 */
    }
    else
    {
        /* for no pause */
        int p6 = ffeature_int(item, "R:SylStructure.pos_in_syl") + 1;
        int a3 =
            ffeature_int(item,
                         "R:SylStructure.parent.R:Syllable.p.syl_numphones");
        int b3 =
            ffeature_int(item,
                         "R:SylStructure.parent.R:Syllable.syl_numphones");
        int b4 =
            ffeature_int(item,
                         "R:SylStructure.parent.R:Syllable.pos_in_word") + 1;
        int b12 =
            ffeature_int(item,
                         "R:SylStructure.parent.R:Syllable.lisp_distance_to_p_stress");
        int b13 =
            ffeature_int(item,
                         "R:SylStructure.parent.R:Syllable.lisp_distance_to_n_stress");
        int b14 =
            ffeature_int(item,
                         "R:SylStructure.parent.R:Syllable.lisp_distance_to_p_accent");
        int b15 =
            ffeature_int(item,
                         "R:SylStructure.parent.R:Syllable.lisp_distance_to_n_accent");
        int c3 =
            ffeature_int(item,
                         "R:SylStructure.parent.R:Syllable.n.syl_numphones");
        int d2 =
            ffeature_int(item,
                         "R:SylStructure.parent.parent.R:Word.p.word_numsyls");
        int e2 =
            ffeature_int(item,
                         "R:SylStructure.parent.parent.R:Word.word_numsyls");
        int e3 =
            ffeature_int(item,
                         "R:SylStructure.parent.parent.R:Word.pos_in_phrase")
            + 1;
        int e7 =
            ffeature_int(item,
                         "R:SylStructure.parent.parent.R:Word.lisp_distance_to_p_content");
        int e8 =
            ffeature_int(item,
                         "R:SylStructure.parent.parent.R:Word.lisp_distance_to_n_content");
        int f2 =
            ffeature_int(item,
                         "R:SylStructure.parent.parent.R:Word.n.word_numsyls");
        int g1 =
            ffeature_int(item,
                         "R:SylStructure.parent.parent.R:Phrase.parent.p.lisp_num_syls_in_phrase");
        int g2 =
            ffeature_int(item,
                         "R:SylStructure.parent.parent.R:Phrase.parent.p.lisp_num_words_in_phrase");
        int h2 =
            ffeature_int(item,
                         "R:SylStructure.parent.parent.R:Phrase.parent.lisp_num_words_in_phrase");
        int h3 =
            ffeature_int(item,
                         "R:SylStructure.parent.R:Syllable.sub_phrases") + 1;
        const char *h5 =
            ffeature_string(item,
                            "R:SylStructure.parent.parent.R:Phrase.parent.daughtern.R:SylStructure.daughtern.endtone");
        int i1 =
            ffeature_int(item,
                         "R:SylStructure.parent.parent.R:Phrase.parent.n.lisp_num_syls_in_phrase");
        int i2 =
            ffeature_int(item,
                         "R:SylStructure.parent.parent.R:Phrase.parent.n.lisp_num_words_in_phrase");
        int j1 =
            ffeature_int(item,
                         "R:SylStructure.parent.parent.R:Phrase.parent.lisp_total_syls");
        int j2 =
            ffeature_int(item,
                         "R:SylStructure.parent.parent.R:Phrase.parent.lisp_total_words");
        int j3 =
            ffeature_int(item,
                         "R:SylStructure.parent.parent.R:Phrase.parent.lisp_total_phrases");
        sprintf(label, "%s^%s-%s+%s=%s@%d_%d/A:%s_%s_%s/B:%d-%d-%d@%d-%d&%d-%d#%d-%d$%d-%d!%s-%s;%s-%s|%s/C:%s+%s+%s/D:%s_%s/E:%s+%d@%d+%d&%d+%d#%s+%s/F:%s_%s/G:%s_%s/H:%d=%d^%d=%d|%s/I:%s=%s/J:%d+%d-%d",    /* */
                strcmp(p1, "0") == 0 ? "xx" : p1,       /* p1 */
                strcmp(p2, "0") == 0 ? "xx" : p2,       /* p2 */
                p3,             /* p3 */
                strcmp(p4, "0") == 0 ? "xx" : p4,       /* p4 */
                strcmp(p5, "0") == 0 ? "xx" : p5,       /* p5 */
                p6,             /* p6 */
                b3 - p6 + 1,    /* p7 */
                a3 == 0 ? "xx" : ffeature_string(item, "R:SylStructure.parent.R:Syllable.p.stress"),    /* a1 */
                a3 == 0 ? "xx" : ffeature_string(item, "R:SylStructure.parent.R:Syllable.p.accented"),  /* a2 */
                a3 == 0 ? "xx" : val_string(val_string_n(a3)),  /* a3 */
                ffeature_int(item, "R:SylStructure.parent.R:Syllable.stress"),  /* b1 */
                ffeature_int(item, "R:SylStructure.parent.R:Syllable.accented"),        /* b2 */
                b3,             /* b3 */
                b4,             /* b4 */
                e2 - b4 + 1,    /* b5 */
                ffeature_int(item, "R:SylStructure.parent.R:Syllable.syl_in") + 1,      /* b6 */
                ffeature_int(item, "R:SylStructure.parent.R:Syllable.syl_out") + 1,     /* b7 */
                ffeature_int(item, "R:SylStructure.parent.R:Syllable.ssyl_in"), /* b8 */
                ffeature_int(item, "R:SylStructure.parent.R:Syllable.ssyl_out"),        /* b9 */
                ffeature_int(item, "R:SylStructure.parent.R:Syllable.asyl_in"), /* b10 */
                ffeature_int(item, "R:SylStructure.parent.R:Syllable.asyl_out"),        /* b11 */
                b12 == 0 ? "xx" : val_string(val_string_n(b12)),        /* b12 */
                b13 == 0 ? "xx" : val_string(val_string_n(b13)),        /* b13 */
                b14 == 0 ? "xx" : val_string(val_string_n(b14)),        /* b14 */
                b15 == 0 ? "xx" : val_string(val_string_n(b15)),        /* b15 */
                ffeature_string(item, "R:SylStructure.parent.R:Syllable.syl_vowel"),    /* b16 */
                c3 == 0 ? "xx" : ffeature_string(item, "R:SylStructure.parent.R:Syllable.n.stress"),    /* c1 */
                c3 == 0 ? "xx" : ffeature_string(item, "R:SylStructure.parent.R:Syllable.n.accented"),  /* c2 */
                c3 == 0 ? "xx" : val_string(val_string_n(c3)),  /* c3 */
                d2 == 0 ? "xx" : ffeature_string(item, "R:SylStructure.parent.parent.R:Word.p.gpos"),   /* d1 */
                d2 == 0 ? "xx" : val_string(val_string_n(d2)),  /* d2 */
                ffeature_string(item, "R:SylStructure.parent.parent.R:Word.gpos"),      /* e1 */
                e2,             /* e2 */
                e3,             /* e3 */
                h2 - e3 + 1,    /* e4 */
                ffeature_int(item, "R:SylStructure.parent.parent.R:Word.content_words_in"),     /* e5 */
                ffeature_int(item, "R:SylStructure.parent.parent.R:Word.content_words_out"),    /* e6 */
                e7 == 0 ? "xx" : val_string(val_string_n(e7)),  /* e7 */
                e8 == 0 ? "xx" : val_string(val_string_n(e8)),  /* e8 */
                f2 == 0 ? "xx" : ffeature_string(item, "R:SylStructure.parent.parent.R:Word.n.gpos"),   /* f1 */
                f2 == 0 ? "xx" : val_string(val_string_n(f2)),  /* f2 */
                g1 == 0 ? "xx" : val_string(val_string_n(g1)),  /* g1 */
                g2 == 0 ? "xx" : val_string(val_string_n(g2)),  /* g2 */
                ffeature_int(item, "R:SylStructure.parent.parent.R:Phrase.parent.lisp_num_syls_in_phrase"),     /* h1 */
                h2,             /* h2 */
                h3,             /* h3 */
                j3 - h3 + 1,    /* h4 */
                strcmp(h5, "0") == 0 ? "NONE" : h5,     /* h5 */
                i1 == 0 ? "xx" : val_string(val_string_n(i1)),  /* i1 */
                i2 == 0 ? "xx" : val_string(val_string_n(i2)),  /* i2 */
                j1,             /* j1 */
                j2,             /* j2 */
                j3);            /* j3 */
    }
}

/* Flite_HTS_Engine_initialize: initialize system */
void Flite_HTS_Engine_initialize(Flite_HTS_Engine * f)
{
    HTS_Engine_initialize(&f->engine);
    f->is_engine_loaded = 0;
}

/* Flite_HTS_Engine_load: load HTS voice */
HTS_Boolean Flite_HTS_Engine_load(Flite_HTS_Engine * f, const char *fn)
{
    HTS_Boolean result;
    char *voices = cst_strdup(fn);
    result = HTS_Engine_load(&f->engine, &voices, 1);
    free(voices);
    if (result == TRUE)
        f->is_engine_loaded = 1;
    return result;
}

int Flite_HTS_Engine_is_loaded(Flite_HTS_Engine * f)
{
    return f->is_engine_loaded;
}

/* Flite_HTS_Engine_set_sampling_frequency: set sampling frequency */
void Flite_HTS_Engine_set_sampling_frequency(Flite_HTS_Engine * f, size_t i)
{
    HTS_Engine_set_sampling_frequency(&f->engine, i);
}

/* Flite_HTS_Engine_set_fperiod: set frame period */
void Flite_HTS_Engine_set_fperiod(Flite_HTS_Engine * f, size_t i)
{
    HTS_Engine_set_fperiod(&f->engine, i);
}

/* Flite_HTS_Engine_set_audio_buff_size: set audio buffer size */
void Flite_HTS_Engine_set_audio_buff_size(Flite_HTS_Engine * f, size_t i)
{
    HTS_Engine_set_audio_buff_size(&f->engine, i);
}

/* Flite_HTS_Engine_set_volume: set volume in dB */
void Flite_HTS_Engine_set_volume(Flite_HTS_Engine * f, double d)
{
    HTS_Engine_set_volume(&f->engine, d);
}

/* Flite_HTS_Engine_set_alpha: set alpha */
void Flite_HTS_Engine_set_alpha(Flite_HTS_Engine * f, double d)
{
    HTS_Engine_set_alpha(&f->engine, d);
}

/* Flite_HTS_Engine_set_beta: set beta */
void Flite_HTS_Engine_set_beta(Flite_HTS_Engine * f, double d)
{
    HTS_Engine_set_beta(&f->engine, d);
}

/* Flite_HTS_Engine_add_half_tone: add half-tone */
void Flite_HTS_Engine_add_half_tone(Flite_HTS_Engine * f, double d)
{
    HTS_Engine_add_half_tone(&f->engine, d);
}

/* Flite_HTS_Engine_set_msd_threshold: set MSD threshold */
void Flite_HTS_Engine_set_msd_threshold(Flite_HTS_Engine * f,
                                        size_t stream_index, double d)
{
    HTS_Engine_set_msd_threshold(&f->engine, stream_index, d);
}

/* Flite_HTS_Engine_set_gv_weight: set GV weight */
void Flite_HTS_Engine_set_gv_weight(Flite_HTS_Engine * f, size_t stream_index,
                                    double d)
{
    HTS_Engine_set_gv_weight(&f->engine, stream_index, d);
}

/* Flite_HTS_Engine_set_speed: set speech speed */
void Flite_HTS_Engine_set_speed(Flite_HTS_Engine * f, double d)
{
    HTS_Engine_set_speed(&f->engine, d);
}

size_t Flite_HTS_Engine_get_sampling_frequency(Flite_HTS_Engine * f)
{
    return HTS_Engine_get_sampling_frequency(&f->engine);
}

/* Flite_HTS_Engine_clear: free system */
void Flite_HTS_Engine_clear(Flite_HTS_Engine * f)
{
    HTS_Engine_clear(&f->engine);
    f->is_engine_loaded = 0;
}

/* Flite_HTS_Engine_copy_wave: Copy generated wave to a int16_t* array */
static int16_t *Flite_HTS_Engine_copy_wave(Flite_HTS_Engine * f)
{
    double x;
    int16_t xs;
    size_t i;
    int16_t *rawwave =
        cst_alloc(int16_t, HTS_Engine_get_nsamples(&f->engine));
    if (rawwave == NULL)
        return NULL;
    for (i = 0; i < HTS_Engine_get_nsamples(&f->engine); ++i)
    {
        x = HTS_Engine_get_generated_speech(&f->engine, i);
        if (x > 32767.0)
            xs = 32767;
        else if (x < -32768.0)
            xs = -32768;
        else
            xs = (int16_t) x;
        rawwave[i] = xs;
    }
    return rawwave;
}

cst_wave *wave_from_hts_engine(Flite_HTS_Engine * f)
{
    int16_t *rawwave = Flite_HTS_Engine_copy_wave(f);
    if (rawwave == NULL)
        return NULL;
    cst_wave *w = new_wave();
    if (w == NULL)
        return NULL;
    w->samples = rawwave;
    w->type = cst_strdup("riff");
    w->sample_rate = HTS_Engine_get_sampling_frequency(&f->engine);
    w->num_samples = HTS_Engine_get_nsamples(&f->engine);
    w->num_channels = 1;
    return w;
}

cst_utterance *hts_synth(cst_utterance *utt)
{
    Flite_HTS_Engine *flite_hts;
    size_t label_size = 0, i;
    cst_wave *w;
    cst_item *s = NULL;
    char **label_data = NULL;


    flite_hts = val_flitehtsengine(utt_feat_val(utt, "flite_hts"));

    if (!Flite_HTS_Engine_is_loaded(flite_hts))
    {
        const char *fn_voice =
            get_param_string(utt->features, "htsvoice_file", NULL);
        if (Flite_HTS_Engine_load(flite_hts, fn_voice) != TRUE)
        {
            fprintf(stderr,
                    "flite_hts_engine: HTS voice cannot be loaded.\n");
            Flite_HTS_Engine_clear(flite_hts);
            return NULL;
        }
    }

    for (s = relation_head(utt_relation(utt, "Segment")); s; s = item_next(s))
        label_size++;

    if (label_size == 0)
    {
        w = new_wave();
        utt_set_wave(utt, w);
        return utt;
    }

    label_data = (char **) cst_alloc(char *, label_size);
    for (i = 0, s = relation_head(utt_relation(utt, "Segment")); s;
         s = item_next(s), i++)
    {
        label_data[i] = (char *) cst_alloc(char, MAXBUFLEN);
        create_label(s, label_data[i]);
    }

    /* Set options from features */


    float postfiltering_coefficient = get_param_float(utt->features, "postfiltering_coefficient", 0.0); /* [0.0 - 1.0] */
    Flite_HTS_Engine_set_beta(flite_hts, postfiltering_coefficient);


    float dur_stretch =
        get_param_float(utt->features, "duration_stretch", 1.0);
    Flite_HTS_Engine_set_speed(flite_hts, 1.0 / dur_stretch);

    float add_half_tone =
        get_param_float(utt->features, "add_half_tone", 0.0);
    Flite_HTS_Engine_add_half_tone(flite_hts, add_half_tone);

    float vu_threshold = get_param_float(utt->features, "vu_threshold", 0.5);   /* [0.0 - 1.0 ] */
    Flite_HTS_Engine_set_msd_threshold(flite_hts, 1, vu_threshold);

    float gv_weight_spectrum = get_param_float(utt->features, "gv_weight_spectrum", 1.0);       /* [ 0.0 --  ] */
    Flite_HTS_Engine_set_gv_weight(flite_hts, 0, gv_weight_spectrum);

    float gv_weight_lf0 = get_param_float(utt->features, "gv_weight_lf0", 1.0); /* [ 0.0--    ] */
    Flite_HTS_Engine_set_gv_weight(flite_hts, 1, gv_weight_lf0);

    float volume_db = get_param_float(utt->features, "volume_db", 0.0); /* negative means lower, positive means higher */
    Flite_HTS_Engine_set_volume(flite_hts, volume_db);

    /* speech synthesis part */
    HTS_Engine_synthesize_from_strings(&flite_hts->engine, label_data,
                                       label_size);

    w = wave_from_hts_engine(flite_hts);
    utt_set_wave(utt, w);
    HTS_Engine_refresh(&flite_hts->engine);

    for (i = 0; i < label_size; i++)
        free(label_data[i]);
    free(label_data);
    return utt;
}

void delete_flitehtsengine(Flite_HTS_Engine * f)
{
    Flite_HTS_Engine_clear(f);
    cst_free(f);
    return;
}


char *mimic_hts_get_voice_file(const cst_voice *const v)
{
    const char *vname = get_param_string(v->features, "name", NULL);
    size_t i = 0;
    if (vname == NULL)
        return NULL;
    /* The "+256" accounts for all the characters like "/mimic/voices/" and ".htsvoice". */
    char *full_file = cst_alloc(char, strlen(PKGDATADIR) + strlen(vname) + 256);
    const char *directories[] =
        { ".", "voices", "../voices", PKGDATADIR "/voices/", NULL };
    for (i = 0; directories[i] != NULL; i++)
    {
        sprintf(full_file, "%s/%s.htsvoice", directories[i], vname);
        if (cst_file_exists(full_file))
        {
            return full_file;
        }
    }
    cst_free(full_file);
    return NULL;
}

#endif /* HAVE_HTSENGINE */
