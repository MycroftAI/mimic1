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

#ifndef FLITE_HTS_ENGINE_H
#define FLITE_HTS_ENGINE_H

#include "config.h"

#if HAVE_HTSENGINE == 0

typedef struct _Flite_HTS_Engine {
    void *not_supported;
} Flite_HTS_Engine;

#include "cst_val.h"
#include "cst_utterance.h"

CST_VAL_USER_TYPE_DCLS(flitehtsengine, Flite_HTS_Engine);
void delete_flitehtsengine(Flite_HTS_Engine * f);

#else /* HAVE_HTSENGINE */

#ifdef __cplusplus
#define FLITE_HTS_ENGINE_H_START extern "C" {
#define FLITE_HTS_ENGINE_H_END   }
#else
#define FLITE_HTS_ENGINE_H_START
#define FLITE_HTS_ENGINE_H_END
#endif /* __CPLUSPLUS */

FLITE_HTS_ENGINE_H_START;

#include "HTS_engine.h"

typedef struct _Flite_HTS_Engine {
    HTS_Engine engine;
    int is_engine_loaded;
} Flite_HTS_Engine;

#include "cst_val.h"
#include "cst_utterance.h"

CST_VAL_USER_TYPE_DCLS(flitehtsengine, Flite_HTS_Engine);
void delete_flitehtsengine(Flite_HTS_Engine * f);

/* Flite_HTS_Engine_initialize: initialize system */
void Flite_HTS_Engine_initialize(Flite_HTS_Engine * f);

/* Flite_HTS_Engine_load: load HTS voice */
HTS_Boolean Flite_HTS_Engine_load(Flite_HTS_Engine * f, const char *fn);

/* Flite_HTS_Engine_set_sampling_frequency: set sampling frequency */
void Flite_HTS_Engine_set_sampling_frequency(Flite_HTS_Engine * f, size_t i);

/* Flite_HTS_Engine_set_fperiod: set frame period */
void Flite_HTS_Engine_set_fperiod(Flite_HTS_Engine * f, size_t i);

/* Flite_HTS_Engine_set_audio_buff_size: set audio buffer size */
void Flite_HTS_Engine_set_audio_buff_size(Flite_HTS_Engine * engine,
                                          size_t i);

/* Flite_HTS_Engine_set_volume: set volume in dB */
void Flite_HTS_Engine_set_volume(Flite_HTS_Engine * engine, double d);

/* Flite_HTS_Engine_set_alpha: set alpha */
void Flite_HTS_Engine_set_alpha(Flite_HTS_Engine * f, double d);

/* Flite_HTS_Engine_set_beta: set beta */
void Flite_HTS_Engine_set_beta(Flite_HTS_Engine * f, double d);

/* Flite_HTS_Engine_add_half_tone: add half-tone */
void Flite_HTS_Engine_add_half_tone(Flite_HTS_Engine * f, double d);

/* Flite_HTS_Engine_set_msd_threshold: set MSD threshold */
void Flite_HTS_Engine_set_msd_threshold(Flite_HTS_Engine * f,
                                        size_t stream_index, double d);

/* Flite_HTS_Engine_set_gv_weight: set GV weight */
void Flite_HTS_Engine_set_gv_weight(Flite_HTS_Engine * f, size_t stream_index,
                                    double d);

/* Flite_HTS_Engine_set_speed: set speech speed */
void Flite_HTS_Engine_set_speed(Flite_HTS_Engine * engine, double f);

/* Flite_HTS_Engine_synthesize: synthesize speech */
HTS_Boolean Flite_HTS_Engine_synthesize(Flite_HTS_Engine * f, const char *txt,
                                        const char *wav);

/* Flite_HTS_Engine_clear: free system */
void Flite_HTS_Engine_clear(Flite_HTS_Engine * f);

/* Flite_HTS_Engine_get_sampling_frequency: get sampling frequency from engine */
size_t Flite_HTS_Engine_get_sampling_frequency(Flite_HTS_Engine * f);

/* Generate wave for utt */
cst_utterance *hts_synth(cst_utterance *utt);

/* Find .htsvoice file path for voice v */
char *mimic_hts_get_voice_file(const cst_voice *const v);

FLITE_HTS_ENGINE_H_END;

#endif /* HAVE_HTSENGINE */

#endif /* FLITE_HTS_ENGINE_H */
