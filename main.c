/* Copyright 2011 Kyle Dassoff. All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification, are
 * permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY KYLE DASSOFF ''AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "mulog.h"

int main(int argc, char **argv) {
    mulog_ref mlf, mlfp, mlc, mlcp, mls, dummy;
    mulog_ref all[6];

    FILE *f = fopen("banana.log", "w");
    mulog_create_file(&mlf, f, mulog_tm_long, 1);
    mulog_create_file(&mlfp, f, mulog_tm_short, 0);
    mulog_create_con(&mlc, mulog_tm_fixed, 1, 1);
    mulog_create_con(&mlcp, mulog_tm_long, 0, 0);
    mulog_create_split(&mls, mlfp, mlcp);
    mulog_create_dummy(&dummy);

    all[0] = mlf;
    all[1] = mlfp;
    all[2] = mlc;
    all[3] = mlcp;
    all[4] = mls;
    all[5] = dummy;

    puts("=== Properties ===\n");
    for(int i = 0; i < 6; i++) {
        printf("%d[type]: %d\n", i, mulog_get_type(all[i]));
        printf("%d[file]: %p\n", i, mulog_get_file(all[i]));
        printf("%d[wdbg]: %d\n", i, mulog_get_with_debug(all[i]));
        printf("%d[wclr]: %d\n", i, mulog_get_with_color(all[i]));
        printf("%d[left]: %p\n", i, mulog_get_left(all[i]));
        printf("%d[rite]: %p\n", i, mulog_get_right(all[i]));
        putchar('\n');
        printf("%d[file=] -> %d\n", i, mulog_set_file(all[i], mulog_get_file(all[i])));
        printf("%d[wdbg=] -> %d\n", i, mulog_set_with_debug(all[i], mulog_get_with_debug(all[i])));
        printf("%d[wclr=] -> %d\n", i, mulog_set_with_color(all[i], mulog_get_with_color(all[i])));
        printf("%d[left=] -> %d\n", i, mulog_set_left(all[i], mulog_get_left(all[i])));
        printf("%d[rite=] -> %d\n", i, mulog_set_right(all[i], mulog_get_right(all[i])));
        putchar('\n');
    };

    puts("\n=== Logging ===\n");
    for(int i = 0; i < 6; i++) {
        printf("log %d\n", i);
        mulog_err(all[i], "mulog_err %d", i);
        mulog_warn(all[i], "mulog_warn %d", i);
        mulog_info(all[i], "mulog_info %d", i);
        mulog_dbg(all[i], "mulog_dbg %d", i);
    }

    for(int i = 0; i < 6; i++) {
        mulog_destroy(all[i]);
    }
    return 0;
}
