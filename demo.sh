#!/bin/sh
make clean
make
./edge_detector demo$1.jpg
