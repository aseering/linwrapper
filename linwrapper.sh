#!/bin/sh
export LD_PRELOAD="$PWD/linwrapper.so"
exec "$@"
