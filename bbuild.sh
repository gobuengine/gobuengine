#!/bin/bash

if [ "$1" = "run" ]; then
    cd bin
    GSK_RENDERER=cairo ./gobu
else
    meson --reconfigure --wipe build
    cd build
    ninja install
    cd ..
    cd bin
    GSK_RENDERER=cairo ./gobu
fi