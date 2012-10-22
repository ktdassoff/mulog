# Copyright 2011 Kyle Dassoff. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification, are
# permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice, this list of
# conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice, this list
# of conditions and the following disclaimer in the documentation and/or other materials
# provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY KYLE DASSOFF ''AS IS'' AND ANY EXPRESS OR IMPLIED
# WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
# FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
# ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
# ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

DEFINES=-DMULOG_UNIX
CC=gcc
CFLAGS=-pipe -std=c99 $(DEFINES) -I/usr/include/qt4

DBGCFLAGS=$(CFLAGS) -g
RELCFLAGS=$(CFLAGS) -O2

.PHONY: clean help

all: mulog.o mulog_d.o libmulog.so libmulog_d.so testmulog testmulog_d

mulog.o: mulog.c mulog.h
	$(CC) -c $(RELCFLAGS) -o $@ $<

mulog_d.o: mulog.c mulog.h
	$(CC) -c $(DBGCFLAGS) -o $@ $<

libmulog.so: mulog.c mulog.h
	$(CC) -shared -fPIC $(RELCFLAGS) -o $@ $<

libmulog_d.so: mulog.c mulog.h
	$(CC) -shared -fPIC $(DBGCFLAGS) -o $@ $<

testmulog: mulog.o main.c
	$(CC) $(RELCFLAGS) -o $@ $^

testmulog_d: mulog_d.o main.c
	$(CC) $(DBGCFLAGS) -o $@ $^

clean:
	rm -rf *.so *.o testmulog*

help:
	@echo "MuLog Unix Makefile"
	@echo "Targets:"
	@echo "mulog.o, obj          -- release/optimized object file for static linking"
	@echo "mulog_d.o, obj_dbg    -- debug object file for static linking"
	@echo "libmulog.so           -- release/optimized shared library"
	@echo "libmulog_d.so         -- debug shared library"
	@echo "testmulog             -- build test command with debug object"
	@echo "testmulog_d           -- build test command with release/optimized object"
	@echo "all                   -- builds all above targets"
	@echo "clean                 -- remove *.o, *.so, testmulog* files"

