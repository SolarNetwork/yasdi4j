#
# Universal Makefile
# Version 1.4
# Copyright (c) 2008 Michael Denk <code@michaeldenk.de>
# 
# Permission is hereby granted, free of charge, to any person
# obtaining a copy of this software and associated documentation
# files (the "Software"), to deal in the Software without
# restriction, including without limitation the rights to use,
# copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following
# conditions:
#
# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
# OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
# HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
# WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
# OTHER DEALINGS IN THE SOFTWARE.
#

#CC =
#CFLAGS =
CPPFLAGS := -I. $(CPPFLAGS)
#DEFS =
LDFLAGS := -L. $(LDFLAGS)
#LDLIBS =

CFLAGS += -pipe
# CFLAGS += -Wall -Wextra -Wfloat-equal
CFLAGS += -Wall -W -Wfloat-equal
CFLAGS += -Wcast-qual
CFLAGS += -Wwrite-strings
# CFLAGS := $(CFLAGS) -Werror
# CFLAGS := $(CFLAGS) -pedantic
# CFLAGS := $(CFLAGS) -ansi
CFLAGS += $(options)

# LDFLAGS += -mwindows

OPT_CFLAGS = -O3 -fomit-frame-pointer

###############################################################################


ifdef WINDIR
EXE_EXT = .exe
endif

SOURCES := $(wildcard *.c)

# well... working for now
EXE_FILTER := ^[[:space:]]*(|int |void )(|WINAPI )(|w|_t)(m|WinM)ain

EXE_SOURCES := $(if $(SOURCES), $(shell grep -w -l -E '$(EXE_FILTER)' *.c))

OBJECTS := $(filter-out $(EXE_SOURCES:.c=.o), $(SOURCES:.c=.o))

# Compile and link separately
# OBJECTS = $(SOURCES:.c=.o)

EXECUTABLES := $(EXE_SOURCES:.c=)

ifdef WINDIR
all: win-release
debug: win-debug
release: win-release
else
all: generic-release
debug: generic-debug
release: generic-release
endif

generic: generic-release

generic-debug: CFLAGS += -Wunreachable-code
generic-debug: CFLAGS += -g
generic-debug: build

generic-release: CFLAGS := $(OPT_CFLAGS) $(CFLAGS)
generic-release: DEFS += -DNDEBUG
generic-release: LDFLAGS += -s
generic-release: build

win: win-release

win-debug: CC = mgcc
win-debug: CPPFLAGS += -I/usr/local/mingw/include -I/usr/local/include
win-debug: LDFLAGS += -L/usr/local/mingw/lib -L/usr/local/lib
win-debug: generic-debug

win-release: CC = mgcc
win-release: CPPFLAGS += -I/usr/local/mingw/include -I/usr/local/include
win-release: LDFLAGS += -L/usr/local/mingw/lib -L/usr/local/lib
win-release: generic-release

win-static: LDFLAGS += -static
win-static: win

build: $(OBJECTS) $(EXECUTABLES)

INCLUDE_MAKEFILES = $(wildcard Makefile.*)
include $(INCLUDE_MAKEFILES)

%.o :: %.c
	$(CC) $(strip $(CPPFLAGS) $(DEFS) $(CFLAGS) -c -o $@ $<)

# Compile and link separately
#% : %.o
#	$(CC) $(strip $(LDFLAGS) -o $@ $< $(LDLIBS))

% :: %.c
	$(CC) $(strip $(CPPFLAGS) $(DEFS) $(CFLAGS) $(LDFLAGS) -o $@ $(filter-out %.h, $^ $(LDLIBS)))

print:
	@echo 'CFLAGS: $(CFLAGS)'
	@echo 'CPPFLAGS: $(CPPFLAGS)'
	@echo 'DEFS: $(DEFS)'
	@echo 'LDFLAGS: $(LDFLAGS)'
	@echo 'LDLIBS: $(LDLIBS)'
	@echo 'EXE_EXT: $(EXE_EXT)'
	@echo 'EXE_FILTER: $(EXE_FILTER)'
	@echo 'SOURCES: $(SOURCES)'
	@echo 'EXE_SOURCES: $(EXE_SOURCES)'
	@echo 'OBJECTS: $(OBJECTS)'
	@echo 'EXECUTABLES: $(EXECUTABLES)'

clean:
	rm -f $(OBJECTS) $(EXECUTABLES:=$(EXE_EXT))

.PHONY: all debug release generic generic-debug generic-release win win-debug win-release win-static build print clean
