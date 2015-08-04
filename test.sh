#!/bin/sh
LD_LIBRARY_PATH=jni java -Djava.library.path=jni -cp build Test yasdi_linux.ini
