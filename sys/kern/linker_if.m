#
# Copyright (c) 2000 Doug Rabson
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.
#
# $FreeBSD$
#

#include <sys/linker.h>

INTERFACE linker;

#
# Lookup a symbol in the file's symbol table.  If the symbol is not
# found then return ENOENT, otherwise zero.
#
METHOD int lookup_symbol {
    linker_file_t	file;
    const char*		name;
    c_linker_sym_t*	symp;
};

METHOD int symbol_values {
    linker_file_t	file;
    c_linker_sym_t	sym;
    linker_symval_t*	valp;
};

METHOD int search_symbol {
    linker_file_t	file;
    caddr_t		value;
    c_linker_sym_t*	symp;
    long*		diffp;
};

#
# Unload a file, releasing dependancies and freeing storage.
#
METHOD void unload {
    linker_file_t	file;
};

#
# Load a file, returning the new linker_file_t in *result.  If
# the class does not recognise the file type, zero should be
# returned, without modifying *result.  If the file is
# recognised, the file should be loaded, *result set to the new
# file and zero returned.  If some other error is detected an
# appropriate errno should be returned.
#
STATICMETHOD int load_file {
    linker_class_t	cls;
    const char*		filename;
    linker_file_t*	result;
};
