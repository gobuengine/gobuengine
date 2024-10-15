#!/bin/bash

#GSK_RENDERER=cairo ./gobu

if [ "$1" = "run" ]; then
    cd bin
    ./gobu
else
    meson --reconfigure --wipe build
    cd build
    ninja install
    cd ..
    cd bin
    ./gobu
fi