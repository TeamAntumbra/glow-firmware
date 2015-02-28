#!/bin/sh

set -e

bsdtar -s '/^lufa-LUFA-140302/lufa/' -xf deps/lufa/LUFA-140302.zip

make
tar -czf BUILD.tar.gz loader.hex main.hex main.bin combined.hex default.eep combined.eep README.org
