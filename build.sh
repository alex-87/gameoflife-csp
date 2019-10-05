#!/bin/sh

##
# MIT License
# 
# Copyright (c) 2019 Alexis LG
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
##

GOL_SRC_DIR="src";
GOL_BIN_DIR="bin";
GOL_BIN_FNAME="gol-gecode";

if [ ! -d "$GOL_BIN_DIR" ]; then
    mkdir "$GOL_BIN_DIR";
fi;

g++ -o3 \
    -std=c++17 \
    -o"$GOL_BIN_DIR/$GOL_BIN_FNAME" \
    `ls ./$GOL_SRC_DIR/*.cpp` \
    -lgecodeflatzinc \
    -lgecodedriver \
    -lgecodesearch \
    -lgecodeminimodel \
    -lgecodeset \
    -lgecodeint \
    -lgecodefloat \
    -lgecodekernel \
    -lgecodesupport
