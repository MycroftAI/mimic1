# Mimic - The Mycroft TTS Engine

[![Stories in Ready](https://badge.waffle.io/MycroftAI/mimic.png?label=ready&title=Ready)](https://waffle.io/MycroftAI/mimic)
[![Build Status](https://travis-ci.org/MycroftAI/mimic.svg?branch=development)](https://travis-ci.org/MycroftAI/mimic)
[![codecov.io](https://codecov.io/github/MycroftAI/mimic/coverage.svg?branch=development)](https://codecov.io/github/MycroftAI/mimic?branch=development)
[![Coverity Scan](https://img.shields.io/coverity/scan/8420.svg)](https://scan.coverity.com/projects/mycroftai-mimic?tab=overview)

Mimic is a fast, lightweight Text-to-speech engine developed by [Mycroft A.I.](https://mycroft.ai/) and [VocaliD](https://vocalid.co/), based on Carnegie Mellon University’s [Flite (Festival-Lite)](http://www.festvox.org/flite) software. Mimic takes in text and reads it out loud to create a high quality voice. 

###### Official project site: [mimic.mycroft.ai](https://mimic.mycroft.ai/)


## Supported platforms

- Linux (ARM & Intel architectures)
- Mac OS X
- Windows

**Untested**
- Android

**Future**
- iOS

## Requirements

This is the list of requirements. Below there is the commands needed on the most
popular distributions and supported OS.

- A good C compiler:
  * Linux or Mac OSX: _Recommended:_ gcc or clang
  * Windows: _Recommended:_ GCC under [Cygwin](https://cygwin.com/) or [mingw32](http://www.mingw.org/)
- GNU make, automake and libtool
- pkg-config
- Optionally, PCRE2 library and headers (they are compiled otherwise)
- An audio engine:
  * Linux: ALSA/PortAudio/PulseAudio (_Recommended:_ ALSA)
  * Mac OSX: PortAudio
  * Windows: PortAudio

### Linux

##### On Debian/Ubuntu
```
$ sudo apt-get install gcc make pkg-config automake libtool libasound2-dev
```

##### On Fedora
```
$ sudo dnf install gcc make pkgconfig automake libtool alsa-lib-devel
```

##### On Arch
```
$ sudo pacman -S --needed install gcc make pkg-config automake libtool alsa-lib
```


### Mac OSX

- Install *Brew*
  ```
  $ /usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
  ```

- Install *pkg-config*, *automake*, *libtool*, *pcre2* and *PortAudio*
  ```
  $ brew install pkg-config automake libtool portaudio pcre2
  ```

### Windows

#### Cross compiling:

The fastest and most straightforward way to build mimic for windows is by
cross-compilation from linux. This requires some additional packages to be
installed.

On Ubuntu 16.04 (xenial):
```
sudo apt-get install gcc make pkg-config automake libtool libpcre2-dev wine binutils-mingw-w64-i686 mingw-w64-i686-dev gcc-mingw-w64-i686

```

On Ubuntu 14.04 (trusty):

```
sudo apt-get install gcc make pkg-config automake libtool mingw32 mingw32-runtime wine
```


#### Native Windows building

- Audio device and audio libraries are optional, as mimic can write its output to a waveform file.
- Some of the source files are quite large, that some C compilers might choke on these. So, *gcc* is recommended.
- Visual C++ 6.0 is known to fail on the large diphone database files
- The build process is **MUCH** slower on Windows.


## Build

### On a native build (not cross-compilation)

- Clone the repository
  ```
  $ git clone https://github.com/MycroftAI/mimic1.git
  ```
  
- Navigate to mimic directory
  ```
  $ cd mimic1
  ```

- Build and install missing dependencies (pcre2)
  ```
  $ ./dependencies.sh --prefix="/usr/local"
  ```

- Generate mimic build scripts
  ```
  $ ./autogen.sh
  ```
  
- Configure.

  ```
  $ ./configure --prefix="/usr/local"
  ```
  
- Build
  ```
  $ make
  ```
  
- Check
  ```
  $ make check
  ```

### Cross compilation:

- Run the windows build script:

```
./run_testsuite.sh winbuild
```

- Test it: The directory `install` will contain `bin/mimic.exe` file

```
wine ./mimic.exe -t "hello world" 
```

- Distribute it

You can distribute the compiled mimic by adding to a zip file everything in the
`install/winbuild/bin` directory.


## Usage

By default mimic will play the text using an audio device. Alternatively it can
output the wave file in RIFF format (often called `.wav`).

#### Read text

- To an audio device
  ```
  $ ./mimic -t TEXT
  ```
  
  **_Example_**
  ```
  $ ./mimic -t "Hello. Doctor. Name. Continue. Yesterday. Tomorrow."
  ```

- To an audio file
  ```
  $ ./mimic -t TEXT -o WAVEFILE
  ```
  
  **_Example_**
  ```
  $ ./mimic -t "Hello. Doctor. Name. Continue. Yesterday. Tomorrow." -o hello.wav
  ```

#### Read text from file

- To an audio device
  ```
  $ ./mimic -f TEXTFILE
  ```
  
  **_Example_**
  ```
  $ ./mimic -f doc/alice
  ```

- To an audio file
  ```
  $ ./mimic -f TEXTFILE -o WAVEFILE`
  ```
  **_Example_**
  ```
  $ ./mimic -f doc/alice -o hello.wav
  ```

#### Change voice

- List available internal voices
  ```
  $ ./mimic -lv
  ```

- Use an internal voice
  ```
  $ ./mimic -t TEXT -voice VOICE
  ```
  
  **_Example_**
  ```
  $ ./mimic -t "Hello" -voice slt
  ```

- Use an external voice file
  ```
  $ ./mimic -t TEXT -voice VOICEFILE
  ```
  
  **_Example_**
  ```
  $ ./mimic -t "Hello" -voice voices/cmu_us_slt.flitevox
  ```

- Use an external voice url
  ```
  $ ./mimic -t TEXT -voice VOICEURL
  ```
  
  **_Example_**
  ```
  $ ./mimic -t "Hello" -voice http://www.festvox.org/flite/packed/flite-2.0/voices/cmu_us_ksp.flitevox
  ```

###### Notes

- mimic offers several voices that can use different speech modelling techniques
  (diphone, clustergen, hts). Voices can differ a lot on size, naturalness and
  intelligibility.

  * Diphone voices are less computationally expensive and quite intelligible but
    they lack naturalness (sound more robotic). e.g. `./mimic -t "Hello world" -voice kal16`

  * clustergen voices can sound more natural and intelligible at the expense of
    size and computational requirements. e.g.:  e.g. `./mimic -t "Hello world" -voice slt`, `./mimic -t "Hello world" -voice ap`

  * hts voices usually may sound a bit more synthetic than clustergen voices,
    but have much smaller size. e.g.:  e.g. `./mimic -t "Hello world" -voice slt_hts`

- Voices can be compiled (built-in) into mimic or loaded from a `.flitevox` file.
  The only exception are hts voices. hts voices combine both a compiled function with
  a voice data file `.htsvoice`. Mimic will look for the `.htsvoice` file when
  the hts voice is loaded, looking into the current working directory, 
  the "voices" subdirectory and the `$prefix/share/mimic/voices` directory if it
  exists.

- Voice names are identified as loadable files if the name includes a "`/`" 
  (slash) otherwise they are treated as internal compiled-in voices.

- The `voices/` directory contains several flitevox voices. Existing Flite voices
  can be found here: [http://www.festvox.org/flite/packed/flite-2.0/voices/](http://www.festvox.org/flite/packed/flite-2.0/voices/)

- The voice referenced via an url will be downloaded on the fly.

#### Other options

Voices accept additional *debug* options. specified as `--setf feature=value` in the
command line. Wrong values can prevent mimic from working. Some speech modelling
techniques may not implement support for changing these features so at some point
some voices may not provide support for these options. Here are some examples:

- Use simple concatenation of diphones without prosodic modification
  ```
  ./mimic --sets join_type=simple_join doc/intro.txt
  ```

- Print sentences as they are said
  ```
  ./mimic -pw doc/alice
  ```

- Make it speak slower
  ```
  ./mimic --setf duration_stretch=1.5 doc/alice
  ```

- Make it speak faster
  ```
  ./mimic --setf duration_stretch=0.8 doc/alice
  ```

- Make it speak higher
  ```
  ./mimic --setf int_f0_target_mean=145 doc/alice
  ```

See `lang/cmu_us_kal/cmu_us_kal.c`) to see some other features and values.


#### Say the hour

- The talking clock requires a single argument HH:MM. Under Unix you can call it
  ```
  ./mimic_time `date +%H:%M` 
  ```

#### Benchmarking

- For benchmarking, "none" can be used to discard the generated audio and give a summary of the speed:
  ```
  ./mimic -f doc/alice none
  ```

## How to Contribute 

For those who wish to help contribute to the development of mimic there are a few things to keep in mind. 
  
#### Git branching structure
We will be using a branching struture similar to the one described in [this article](http://nvie.com/posts/a-successful-git-branching-model/)
  
##### In short

- `master` branch is for stable releases, 
  
- `development` branch is where development work is done between releases,
  
- Any feature branch should branch off from `development`, and when complete will be merged back into `development`.
  
- Once enough features are added or a new release is complete those changes in `development` will be merged into `master`, 
  then work can continue on `development` for the next release. 


#### Coding Style Requirements
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

##### Vimrc 

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

##### Indent command (currently does not indent switch/cases properly)

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

## Acknowledgements
see [ACKNOWLEDGEMENTS](https://github.com/MycroftAI/mimic/blob/master/ACKNOWLEDGEMENTS)

## License
See [COPYING](https://github.com/MycroftAI/mimic/blob/master/COPYING)

