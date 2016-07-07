#Mimic - The Mycroft TTS Engine

[![Stories in Ready](https://badge.waffle.io/MycroftAI/mimic.png?label=ready&title=Ready)](https://waffle.io/MycroftAI/mimic)
[![Build Status](https://travis-ci.org/MycroftAI/mimic.svg?branch=development)](https://travis-ci.org/MycroftAI/mimic)
[![codecov.io](https://codecov.io/github/MycroftAI/mimic/coverage.svg?branch=development)](https://codecov.io/github/MycroftAI/mimic?branch=development)
[![Coverity Scan](https://img.shields.io/coverity/scan/8420.svg)](https://scan.coverity.com/projects/mycroftai-mimic?tab=overview)

Mimic is a fast, lightweight Text-to-speech engine developed by [Mycroft A.I.](https://mycroft.ai/) and [VocaliD](https://vocalid.co/), based on Carnegie Mellon University’s [Flite (Festival-Lite)](http://www.festvox.org/flite) software. Mimic takes in text and reads it out loud to create a high quality voice. 

######Official project site: [mimic.mycroft.ai](https://mimic.mycroft.ai/)


##Supported platforms

- Linux (ARM & Intel architectures)
- Mac OS X
- Windows

**Untested**
- Android

**Future**
- iOS

##Requirements

###Linux

- A good C compiler (_Recommended:_ gcc or clang)
- GNU make
- pkg-config
- ALSA/PortAudio/PulseAudio (_Recommended:_ ALSA)

####Instructions

- Install *gcc*, *make*, *pkg-config* and *ALSA*

#####On Debian/Ubuntu
```
$ sudo apt-get install gcc make pkg-config libasound2-dev
```

###Mac OSX

- A good C compiler. (_Recommended:_ gcc or clang)
- GNU make
- pkg-config
- PortAudio

####Instructions

- Install *Brew*
  ```
  $ /usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
  ```

- Install *pkg-config* and *PortAudio*
  ```
  $ brew install pkg-config portaudio
  ```

###Windows

* A good C compiler (_Recommended:_ GCC under [Cygwin](https://cygwin.com/) or [mingw32](http://www.mingw.org/))
* GNU Make
* PortAudio

######Note
- Audio device and audio libraries are optional, as mimic can write its output to a waveform file
- Some of the source files are quite large, that some C compilers might choke on these. So, *gcc* is recommended.
- Visual C++ 6.0 is known to fail on the large diphone database files

##Build

- Clone the repository
  ```
  $ git clone https://github.com/MycroftAI/mimic.git
  ```
  
- Navigate to mimic directory
  ```
  $ cd mimic
  ```
  
- Configure
  ```
  $ ./configure
  ```
  
- Build
  ```
  $ make
  ```

######Note

- If changes were made to the _compile flags_ after building, run `make clean` before recompiling with `make`.


##Usage

####Read text

- To an audio device
  ```
  $ mimic -t TEXT
  ```
  
  **_Example_**
  ```
  $ ./bin/mimic -t "Hello. Doctor. Name. Continue. Yesterday. Tomorrow."
  ```

- To an audio file
  ```
  $ mimic -t TEXT -o WAVEFILE
  ```
  
  **_Example_**
  ```
  $ ./bin/mimic -t "Hello. Doctor. Name. Continue. Yesterday. Tomorrow." -o hello.wav
  ```

######Note

- Wave file will be an 8KHz _riff_ headered waveform file. _riff_ is Microsoft's wave format often called .WAV)


####Read text from file

- To an audio device
  ```
  $ mimic -f TEXTFILE
  ```
  
  **_Example_**
  ```
  $ ./bin/mimic -f doc/alice
  ```

- To an audio file
  ```
  $ mimic -f TEXTFILE -o WAVEFILE`
  ```
  **_Example_**
  ```
  $ ./bin/mimic -f doc/alice -o hello.wav
  ```

######Note
- Wave file will be an 8KHz _riff_ headered waveform file. _riff_ is Microsoft's wave format often called .WAV)


####Change voice

- List available internal voices
  ```
  $ mimic -lv
  ```

- Use an internal voice
  ```
  $ mimic -t TEXT -voice VOICE
  ```
  
  **_Example_**
  ```
  $ ./bin/mimic -t "Hello" -voice slt
  ```

- Use an external voice file
  ```
  $ mimic -t TEXT -voice VOICEFILE
  ```
  
  **_Example_**
  ```
  $ ./bin/mimic -t "Hello" -voice voices/cmu_us_slt.flitevox
  ```

- Use an external voice url
  ```
  $ mimic -t TEXT -voice VOICEURL
  ```
  
  **_Example_**
  ```
  $ ./bin/mimic -t "Hello" -voice http://www.festvox.org/flite/packed/flite-2.0/voices/cmu_us_ksp.flitevox
  ```

######Note
- `kal` (diphone) voice is a different technology from the others and is much less computationally expensive but more robotic
- Voice names are identified as loadable files if the name includes a "`/`" (slash) otherwise they are treated as internal names
- The `voices/` directory contains several flitevox voices. Existing Flite voices can be found here: [http://www.festvox.org/flite/packed/flite-2.0/voices/](http://www.festvox.org/flite/packed/flite-2.0/voices/)
- The voice referenced via an url will be downloaded on the fly
- For each voice additional binaries that contain only that voice are created in ./bin/mimic_FULLVOICENAME, e.g. ./bin/mimic_cmu_us_awb . By default, `-g` is on so it will be bigger than is actually required

####Other options
If no argument or "play" is given, it will attempt to write directly to the audio device (if supported).  if "none"
is given the audio is simply thrown away (used for benchmarking). Explicit options are also available.
  ```
  ./bin/mimic -v doc/alice none
  ```
  will synthesize the file without playing the audio and give a summary of the speed.

An additional set of feature setting options are available, these are *debug* options, Voices are represented as sets of feature values (see `lang/cmu_us_kal/cmu_us_kal.c`) and you can override values on the command line.  This can stop mimic from working if malicious values are set and therefore this facility is not intended to be made available for standard users.  But these are useful for debugging.  Some typical examples are

- Use simple concatenation of diphones without prosodic modification
  ```
  ./bin/mimic --sets join_type=simple_join doc/intro.txt
  ```

- Print sentences as they are said
  ```
  ./bin/mimic -pw doc/alice
  ```

- Make it speak slower
  ```
  ./bin/mimic --setf duration_stretch=1.5 doc/alice
  ```

- Make it speak higher
  ```
  ./bin/mimic --setf int_f0_target_mean=145 doc/alice
  ```

- The talking clock is an example talking clode as discussed on http://festvox.org/ldom it requires a single argument HH:MM.
Under Unix you can call it
  ```
  ./bin/mimic_time `date +%H:%M` 
  ```

##Debugging

The debug flag `-g` is already set when compiling. (This should probably be removed on release build)

######Note
Currently the configure script enables compiler optimizations. These optimizations are the reason for any weird behavior while stepping through the code. (Due to the fact that the compiler has reordered/removed many lines of code.) 

For now to disable optimizations edit the file `mimic/config/config` by hand. Near the top of the file change: `CFLAGS   = -g -O2 -Wall` to read: `CFLAGS   = -g -O0 -Wall`  (You can also put any other debug flags here that you wish)  Keep in mind that this file is auto generated by the `configure`  script and will be overwritten if the script is run. Run `make clean` and then `make` to rebuild with the new flags.

Now, Run the program in the debugger
```
gdb --args ./bin/mimic -t "Hello. Doctor. Name. Continue. Yesterday. Tomorrow."
```

##How to Contribute 
  For those who wish to help contribute to the development of mimic there are a few things to keep in mind. 
  
####Git branching structure
We will be using a branching struture similar to the one described in this article: http://nvie.com/posts/a-successful-git-branching-model/ (a very interesting read)
  
#####In short

- `master` branch is for stable releases, 
  
- `development` branch is where development work is done between releases,
  
- Any feature branch should branch off from `development`, and when complete will be merged back into `development`.
  
- Once enough features are added or a new release is complete those changes in `development` will be merged into `master`, 
  then work can continue on `development` for the next release. 


####Coding Style Requirements
To keep the code in mimic coherent a simple coding style/guide is used. It should be noted that the current codebase as a whole does not meet some of these guidlines,this is a result of coming from the flite codebase. As different parts of the codebase are touched, it is the hope that these inconsistancies will diminish as time goes on.

- **Indentation**

  Each level of indentation is *4 spaces*.

- **Braces**

  Braces always comes on the line following the statement.

  **_Example_**
  ```c
  void cool_function(void)
  {
      int cool;
      for (cool = 0; cool < COOL_LIMIT; cool++)
      {
          [...]
          if (cool == AWESOME)
          {
              [...]
          }
      }
  }
  ```

- **If-statements**

  Always use curly braces.
  
  **_Example_**
  ```c
  if(condition)
  {                             /*always use curly braces even if the 'if' only has one statement*/
      DoJustThisOneThing();        
  }
  
  if(argv[i][2] == 'h' &&      /*split 'if' conditions to multiple lines if the conditions are long */
     argv[i][3] == 'e' &&      /*or if it makes things more readable. */
     argv[i][4] == 'l' && 
     argv[i][5] == 'p')
  {
        /*example taken from args parsing code*/
        /* code */
  }
  else if(condition)
  {
        /* code */
  }
  else
  {
      /* code */
  }
  ```
  
- **Switch-statements**

  Always keep the break statement last in the case, after any code blocks.

  **_Example_**
  ```c
  switch(state)
  {
      case 1:
      {               /* even if the case only has one line, use curly braces (similar reasoning as with if's) */ 
          doA(1);
      } break;
                          /* separate cases with a line */
      case 2:             /* unless it falls into the next one */
      case 3:
      {
          DoThisFirst();
      }                   /* no break, this one also falls through */
      case 4:
      {                   /* notice that curly braces line up with 'case' on line above */
          int b = 2;
          doA(b);
      } break;        /* putting 'break' on this line saves some room and makes it look a little nicer */
  
      case 5:
      {
          /* more code */
      } break;
  
      default:        /* It is nice to always have a default case, even if it does nothing */
      {
          InvalidDefaultCase(); /* or whatever, it depends on what you are trying to do. */
      }
  }
  ```


- **Line length**

  There's no hard limit but if possible keep lines shorter than *80 characters*.

#####Vimrc 

 For those of you who use vim, add this to your vimrc to ensure proper indenting.
 ```vimrc
"####Indentation settings
:filetype plugin indent on
" show existing tab with 4 spaces width
:set tabstop=4
" when indenting with '>', use 4 spaces width
:set shiftwidth=4
" On pressing tab, insert 4 spaces
:set expandtab
" fix indentation problem with types above function name
:set cinoptions+=t0
" fix indentation of { after case
:set cinoptions+==0
" fix indentation of multiline if
:set cinoptions+=(0   "closing ) to let vimrc hylighting work after this line

"see http://vimdoc.sourceforge.net/htmldoc/indent.html#cinoptions-values
"for more indent options
 ```

#####Indent command (currently does not indent switch/cases properly)
```
indent [FILE] -npcs -i4 -bl -Tcst_wave -Tcst_wave_header -Tcst_rateconv \
      -Tcst_voice -Tcst_item -Tcst_features -Tcst_val -Tcst_va -Tcst_viterbi \
      -Tcst_utterance -Tcst_vit_cand_f_t -Tcst_vit_path_f_t -Tcst_vit_path \
      -Tcst_vit_point -Tcst_string -Tcst_lexicon -Tcst_relation \
      -Tcst_voice_struct -Tcst_track -Tcst_viterbi_struct -Tcst_vit_cand \
      -Tcst_tokenstream -Tcst_tokenstream_struct -Tcst_synth_module \
      -Tcst_sts_list -Tcst_lpcres -Tcst_ss -Tcst_regex -Tcst_regstate \
      -Twchar_t -Tcst_phoneset -Tcst_lts_rewrites -Tlexicon_struct \
      -Tcst_filemap -Tcst_lts_rules -Tcst_clunit_db -Tcst_cg_db \
      -Tcst_audio_streaming_info -Tcst_audio_streaming_info_struct -Tcst_cart \
      -Tcst_audiodev -TVocoderSetup -npsl -brs -bli0 -nut
```

##Acknowledgements
see [ACKNOWLEDGEMENTS](https://github.com/MycroftAI/mimic/blob/master/ACKNOWLEDGEMENTS)

##License
See [COPYING](https://github.com/MycroftAI/mimic/blob/master/COPYING)
