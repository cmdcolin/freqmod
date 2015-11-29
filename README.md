# freqmod

A realtime synthesizer designed around pipes

## Build

    make

## Requires

    ncurses
    sox

## Usage

    freqmod | play -c 1 -r 44100 -b 16 -q -t raw -e signed-integer -

freqmod outputs raw audio data, hence the specified parameters to play


    freqmod | tee >(sox -q -e signed-integer -b16 -c1 -r44100 -t raw - test.wav) >(sox -q -e signed-integer -b16 -c1 -r44100 -t raw - -d) > /dev/null

play and create test.wav at same time

