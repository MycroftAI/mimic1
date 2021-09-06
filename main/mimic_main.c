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
/*  Simple top level program                                             */
/*                                                                       */
/*************************************************************************/

#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <signal.h>
#endif

#include "mimic.h"

cst_val *mimic_set_voice_list(const char *voxdir);
void *mimic_set_lang_list(void);

void cst_alloc_debug_summary();

/* Its not very appropriate that these are declared here */
void usenglish_init(cst_voice *v);
cst_lexicon *cmu_lex_init(void);

#ifdef _WIN32
BOOL WINAPI windows_signal_handler(DWORD signum)
{
    mimic_audio_shutdown(signum);
    if (signum == CTRL_C_EVENT)
        return TRUE;
    else
        return FALSE;
}
#else
void sigint_handler(int signum)
{
    mimic_audio_shutdown(signum);
}
#endif

static void mimic_version()
{
    printf("  Carnegie Mellon University, Copyright (c) 1999-2011, all rights reserved\n");
    printf("  mimic developers, Copyright (c) 2016-2020, all rights reserved\n");
    printf("  version: %s-%s\n",
           PACKAGE_NAME, PACKAGE_VERSION);
}

static void mimic_usage()
{
    printf("mimic: a small simple speech synthesizer\n");
    mimic_version();
    printf("Report bugs to %s.\n", PACKAGE_BUGREPORT);
    printf("usage: mimic TEXT/FILE [WAVEFILE]\n"
           "  Converts text in TEXTFILE to a waveform in WAVEFILE\n"
           "  If text contains a space the it is treated as a literal\n"
           "  textstring and spoken, and not as a file name\n"
           "  if WAVEFILE is unspecified or \"play\" the result is\n"
           "  played on the current systems audio device.  If WAVEFILE\n"
           "  is \"none\" the waveform is discarded (good for benchmarking)\n"
           "  Other options must appear before these options\n"
           "  --version   Output mimic version number\n"
           "  --help      Output usage string\n"
           "  -o WAVEFILE Explicitly set output filename\n"
           "  -f TEXTFILE Explicitly set input filename\n"
           "  -t TEXT     Explicitly set input textstring\n"
           "  -p PHONES   Explicitly set input textstring and synthesize as phones\n"
           "  --set F=V   Set feature (guesses type)\n"
           "  -s F=V      Set feature (guesses type)\n"
           "  --seti F=V  Set int feature\n"
           "  --setf F=V  Set float feature\n"
           "  --sets F=V  Set string feature\n"
           "  -ssml       Read input text/file in ssml mode\n"
           "  -b          Benchmark mode\n"
           "  -l          Loop endlessly\n"
           "  -voice NAME Use voice NAME (NAME can be filename or url too)\n"
           "  -voicedir NAME Directory contain voice data\n"
           "  -lv         List voices available\n"
           "  -add_lex FILENAME add lex addenda from FILENAME\n"
           "  -pw         Print words\n"
           "  -ps         Print segments\n"
           "  -psdur      Print segments and their durations (end-time)\n"
           "  -pr RelName Print relation RelName\n"
           "  -voicedump FILENAME Dump selected (cg) voice to FILENAME\n"
           "  -v          Verbose mode\n");
    exit(0);
}

static void mimic_voice_list_print(void)
{
    cst_voice *voice;
    const cst_val *v;

    printf("Voices available: ");
    for (v = mimic_voice_list; v; v = val_cdr(v))
    {
        voice = val_voice(val_car(v));
        printf("%s ", voice->name);
    }
    printf("\n");

    return;
}

static cst_utterance *print_info(cst_utterance *u)
{
    cst_item *item;
    const char *relname;
    int printEndTime = 0;

    relname = utt_feat_string(u, "print_info_relation");
    if (!strcmp(relname, "SegmentEndTime"))
    {
        relname = "Segment";
        printEndTime = 1;
    }

    for (item = relation_head(utt_relation(u, relname)); item;
         item = item_next(item))
    {
        if (!printEndTime)
            printf("%s ", item_feat_string(item, "name"));
        else
            printf("%s:%1.3f ", item_feat_string(item, "name"),
                   item_feat_float(item, "end"));

#if 0
        if (cst_streq("+", ffeature_string(item, "ph_vc")))
            printf("%s",
                   ffeature_string(item, "R:SylStructure.parent.stress"));
        printf(" ");
#endif

    }
    printf("\n");

    return u;
}

static void ef_set(cst_features *f, const char *fv, const char *type)
{
    /* set feature from fv (F=V), guesses type if not explicit type given */
    const char *val;
    char *feat;
    const char *fname;

    if ((val = strchr(fv, '=')) == 0)
    {
        fprintf(stderr,
                "mimic: can't find '=' in featval \"%s\", ignoring it\n", fv);
    }
    else
    {
        feat = cst_strdup(fv);
        feat[cst_strlen(fv) - cst_strlen(val)] = '\0';
        fname = feat_own_string(f, feat);
        val = val + 1;
        if ((type && cst_streq("int", type))
            || ((type == 0) && (cst_regex_match(cst_rx_int, val))))
            feat_set_int(f, fname, atoi(val));
        else if ((type && cst_streq("float", type))
                 || ((type == 0) && (cst_regex_match(cst_rx_double, val))))
            feat_set_float(f, fname, atof(val));
        else
            feat_set_string(f, fname, val);
        cst_free(feat);
    }
}

int main(int argc, char **argv)
{
    struct timeval tv;
    cst_voice *v;
    const char *filename;
    const char *outtype;
    cst_voice *desired_voice = 0;
    const char *voicedir = NULL;
    int i;
    int err;
    float durs;
    double time_start, time_end;
    int mimic_verbose, mimic_loop, mimic_bench;
    int explicit_filename, explicit_text, explicit_phones, ssml_mode;
#define ITER_MAX 3
    int bench_iter = 0;
    cst_features *extra_feats;
    const char *lex_addenda_file = NULL;
    const char *voicedumpfile = NULL;
    cst_audio_streaming_info *asi;

    // Set signal handler to shutdown any playing audio on SIGINT
#ifdef _WIN32
    SetConsoleCtrlHandler(windows_signal_handler, TRUE);
#else
    signal(SIGINT, sigint_handler);
#endif //_WIN32
    filename = 0;
    outtype = "play";           /* default is to play */
    mimic_verbose = FALSE;
    mimic_loop = FALSE;
    mimic_bench = FALSE;
    explicit_text = explicit_filename = explicit_phones = FALSE;
    ssml_mode = FALSE;
    extra_feats = new_features();

    mimic_init();
    mimic_set_lang_list();      /* defined at compilation time */

    for (i = 1; i < argc; i++)
    {
        if (cst_streq(argv[i], "--version"))
        {
            mimic_version();
            return 1;
        }
        else if (cst_streq(argv[i], "-h") || cst_streq(argv[i], "--help")
                 || cst_streq(argv[i], "-?"))
            mimic_usage();
        else if (cst_streq(argv[i], "-v"))
            mimic_verbose = TRUE;
        else if (cst_streq(argv[i], "-lv"))
        {
            if (mimic_voice_list == NULL)
                mimic_set_voice_list(voicedir);
            mimic_voice_list_print();
            exit(0);
        }
        else if (cst_streq(argv[i], "-l"))
            mimic_loop = TRUE;
        else if (cst_streq(argv[i], "-b"))
        {
            mimic_bench = TRUE;
            break;              /* ignore other arguments */
        }
        else if ((cst_streq(argv[i], "-o")) && (i + 1 < argc))
        {
            outtype = argv[i + 1];
            i++;
        }
        else if ((cst_streq(argv[i], "-voice")) && (i + 1 < argc))
        {
            if (mimic_voice_list == NULL)
                mimic_set_voice_list(voicedir);
            desired_voice = mimic_voice_select(argv[i + 1]);
            i++;
        }
        else if ((cst_streq(argv[i], "-voicedir")) && (i + 1 < argc))
        {
            voicedir = argv[i + 1];
            if (mimic_voice_list == NULL)
                mimic_set_voice_list(voicedir);
            i++;
        }
        else if ((cst_streq(argv[i], "-add_lex")) && (i + 1 < argc))
        {
            lex_addenda_file = argv[i + 1];
            i++;
        }
        else if (cst_streq(argv[i], "-f") && (i + 1 < argc))
        {
            filename = argv[i + 1];
            explicit_filename = TRUE;
            i++;
        }
        else if (cst_streq(argv[i], "-pw"))
        {
            feat_set_string(extra_feats, "print_info_relation", "Word");
            feat_set(extra_feats, "post_synth_hook_func",
                     uttfunc_val(&print_info));
        }
        else if (cst_streq(argv[i], "-ps"))
        {
            feat_set_string(extra_feats, "print_info_relation", "Segment");
            feat_set(extra_feats, "post_synth_hook_func",
                     uttfunc_val(&print_info));
        }
        else if (cst_streq(argv[i], "-psdur"))
        {
            // Added by AUP Mar 2013 for extracting durations (end-time) of segments
            // (useful in talking heads, etc.)
            feat_set_string(extra_feats, "print_info_relation",
                            "SegmentEndTime");
            feat_set(extra_feats, "post_synth_hook_func",
                     uttfunc_val(&print_info));
        }
        else if (cst_streq(argv[i], "-ssml"))
        {
            ssml_mode = TRUE;
        }
        else if (cst_streq(argv[i], "-pr") && (i + 1 < argc))
        {
            feat_set_string(extra_feats, "print_info_relation", argv[i + 1]);
            feat_set(extra_feats, "post_synth_hook_func",
                     uttfunc_val(&print_info));
            i++;
        }
        else if (cst_streq(argv[i], "-voicedump") && (i + 1 < argc))
        {
            voicedumpfile = argv[i + 1];
            i++;
        }
        else if ((cst_streq(argv[i], "-set") || cst_streq(argv[i], "-s"))
                 && (i + 1 < argc))
        {
            ef_set(extra_feats, argv[i + 1], 0);
            i++;
        }
        else if (cst_streq(argv[i], "--seti") && (i + 1 < argc))
        {
            ef_set(extra_feats, argv[i + 1], "int");
            i++;
        }
        else if (cst_streq(argv[i], "--setf") && (i + 1 < argc))
        {
            ef_set(extra_feats, argv[i + 1], "float");
            i++;
        }
        else if (cst_streq(argv[i], "--sets") && (i + 1 < argc))
        {
            ef_set(extra_feats, argv[i + 1], "string");
            i++;
        }
        else if (cst_streq(argv[i], "-p") && (i + 1 < argc))
        {
            filename = argv[i + 1];
            explicit_phones = TRUE;
            i++;
        }
        else if (cst_streq(argv[i], "-t") && (i + 1 < argc))
        {
            filename = argv[i + 1];
            explicit_text = TRUE;
            i++;
        }
        else if (filename)
            outtype = argv[i];
        else
            filename = argv[i];
    }

    if (filename == NULL)
        filename = "-";         /* stdin */
    if (mimic_voice_list == NULL)
        mimic_set_voice_list(voicedir);
    if (desired_voice == 0)
        desired_voice = mimic_voice_select(NULL);

    if (desired_voice == 0)
    {
        fprintf(stderr, "No voice given and no voice precompiled\n");
        return 1;
    }
    v = desired_voice;
    feat_copy_into(extra_feats, v->features);
    durs = 0.0;

    if (voicedumpfile != NULL)
    {
        mimic_voice_dump(v, voicedumpfile);
        exit(0);
    }

    if (lex_addenda_file)
        mimic_voice_add_lex_addenda(v, lex_addenda_file);

    if (cst_streq("stream", outtype))
    {
        asi = new_audio_streaming_info();
        asi->asc = audio_stream_chunk;
        feat_set(v->features, "streaming_info",
                 audio_streaming_info_val(asi));
    }

    if (mimic_bench)
    {
        outtype = "none";
        filename =
            "A whole joy was reaping, but they've gone south, you should fetch azure mike.";
        explicit_text = TRUE;
    }

  loop:
    gettimeofday(&tv, NULL);
    time_start = (double) (tv.tv_sec) + (((double) tv.tv_usec) / 1000000.0);

    if (explicit_phones)
        err = mimic_phones_to_speech(filename, v, outtype, &durs);
    else if ((strchr(filename, ' ') && !explicit_filename) || explicit_text)
    {
        if (ssml_mode)
            err = mimic_ssml_text_to_speech(filename, v, outtype, &durs);
        else
            err = mimic_text_to_speech(filename, v, outtype, &durs);
    }
    else
    {
        if (ssml_mode)
            err = mimic_ssml_file_to_speech(filename, v, outtype, &durs);
        else
            err = mimic_file_to_speech(filename, v, outtype, &durs);
    }

    gettimeofday(&tv, NULL);
    time_end = ((double) (tv.tv_sec)) + ((double) tv.tv_usec / 1000000.0);

    if (mimic_verbose || (mimic_bench && bench_iter == ITER_MAX))
        printf("times faster than real-time: %f\n(%f seconds of speech synthesized in %f)\n",
             durs / (float) (time_end - time_start), durs,
             (float) (time_end - time_start));

    if ((mimic_loop || (mimic_bench && bench_iter++ < ITER_MAX))
			&& err == 0)
        goto loop;

    delete_features(extra_feats);
    delete_val(mimic_voice_list);
    mimic_voice_list = 0;
    /*    cst_alloc_debug_summary(); */

    mimic_exit();
    return 0;
}
