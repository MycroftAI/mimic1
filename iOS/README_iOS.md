# Build System for iOS and Universal Static Library

The build system added to the /iOS directory will allow the creation of a basic mimic static library for inclusion in an iOS project in Xcode that allows one to write C and Objective-C code against the public api.

There are two bash scripts for working with iOS library binaries for specific architectures and a universal library.

clean_mimic_iOS.sh will remove any library and supporting files created by a previous build.

build_mimic_iOS.sh will create the libraries and supporting files.

## Preparing For Building on OSX (first time)

#### You must modify the Makefile in mimic/main/Makefile
If implementing for the first time in new original source not prepared for OSX or iOS
For the prefix configure flag to have effect, and install the specific binaries to discrete directories, the cp -pd arg flags in two cases near the bottom of the file must be changed for OSX
These changes are added and commented out in this version of the code and can be toggled by commenting/uncommenting appropriately near line 135
The -d flag is interpreted differently in OSX - needs to be -R, so cp -pd must be changed to cp -pR  
see: http://stackoverflow.com/questions/23001775/error-installing-flite-on-mac-osx

## Building

Use a command line tool, such as Terminal, or another appropriate tool if you prefer.

#### To clean any previous builds: 
From the /iOS directory run ./clean_mimic_iOS.sh 

#### To run a build:
From the /iOS directory run ./build_mimic_iOS.sh

The build script runs .configure with the required flags and invokes the native clang C compiler via make to create a binary for each architecture necessary to support iOS development on native device architectures as well as the iOS simulator that runs on OSX.

During the install phase of the build, files are output to 5 discrete directories for each architecture. These directories are under /iOS and named 'install_[arch-name]'.
After the install the binary lib files (libmimic.a) are all linked together into a fat binary of the same name (libmimic.a) and placed into an output directory named 'output-iOS'.
The include directory of each separate architecture has header files, and one set of these is copied into output-iOS as well.

The libmimic.a library file is now ready to be added to an iOS project in Xcode as a static library.
The header files also need to be added as part of that static library in the iOS Xcode project.
The simplest approach is to create an Objective-C iOS Xcode project and add the static library and headers. Once done you can use C or Objective-C to utilize any of the properties and methods exposed by the header files. Details or step by step instructions are beyond the scope of this document, but this a fairly standard practice in iOS development and many examples exist online.

## Limitations

Mimic and/or flite has dependencies that this build and these basic instructions do not address. These include resources in lang, voice, and other directories. Resources such as voice files would need to be added to an Xcode project.
Also, as far as I know, you cannot execute code against the mimic main method as described in mimic and/or flite documentation in the iOS environment utilizing the static library. You are limited to using the public api, and/or modifiying that api to the extent necessary to expose methods or leverage functionality that you might be able to use by running mimic using the command line and arguments (e.g. ./bin/mimic -lv). One might be tempted to think you could use the simple executable created by a mimic build and write code in an iOS project that leveraged that functionality, but it would appear that this is not feasible.

Currently on the Mac, many features that work out of the box on Unix systems do not work on the current version of mimic (or flite) that is built using the command line on OSX (presumably true for iOS as well). For example, the system cannot write audio ouput to native libraries or hardware. However, on a mimic executable build for OSX, you can synthesize custom text input to speech using a default voice that is output as a .WAV file to the system and it will play in an audio player such as iTunes. Some, perhaps many, internal functions do seem to work as expected. 

## Test build for Mac OSX
You can build on OSX using the existing configure and make infrastructure by running ./configure with appropriate flags, as shown in sections below, and then running make.
Output will be in directories with names like '/build/x86_64-darwin14.5.0' or 'build/arm-darwin'.

## -arch i386
make clean
./configure --disable-shared --host=arm-apple-darwin CFLAGS="-isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.11.sdk -arch i386" LDFLAGS="-L." CC="/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc"
make
## -arch x86_64
make clean
./configure --disable-shared --host=arm-apple-darwin CFLAGS="-isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.11.sdk -arch x86_64" LDFLAGS="-L." CC="/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc"
make

