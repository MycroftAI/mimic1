# mimic 0.0.0.9000

* Add PortAudio support. PortAudio is a cross-platform audio I/O library. We
  can support audio output on Mac OS X and Windows.

* Add some unit tests and convert part of the testsuite to unit tests

* Rebuild English cmulex lexicon and letter to sound rules

## Bug fixes

* Workaround for issue #18, where if PulseAudio is running ALSA output was
  stopped mid-word.
