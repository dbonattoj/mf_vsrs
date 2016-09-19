#!/bin/sh
make &&
lldb-3.8 -O "break set -E c++" -- dist/mf_vsrs $1
