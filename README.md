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
