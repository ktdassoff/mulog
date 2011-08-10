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

#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#ifdef MULOG_WIN32
#include <WinCon.h>
#endif

/* =====================
 * Console color helpers
 * =====================
 */

enum Win32ConsoleColor {
    wbg_black = 0,               wfg_black = 0,
    wbg_white = 16|32|64|128,    wfg_white = 1|2|4|8,
    wbg_grey = 16|32|64,         wfg_grey = 1|2|4,
    wbg_yellow = 32|64|128,      wfg_yellow = 2|4|8,
    wbg_ochre = 32|64,           wfg_ochre = 2|4,
    wbg_cyan = 16|32|128,        wfg_cyan = 1|2|8,
    wbg_teal = 16|32,            wfg_teal = 1|2,
    wbg_magenta = 16|64|128,     wfg_magenta = 1|4|8,
    wbg_violet = 16|64,          wfg_violet = 1|4,
    wbg_blue = 16|128,           wfg_blue = 1|8,
    wbg_dblue = 16,              wfg_dblue = 1,
    wbg_green = 32|128,          wfg_green = 2|128,
    wbg_dgreen = 32,             wfg_dgreen = 2,
    wbg_red = 64|128,            wfg_red = 4|8,
    wbg_dred = 64,               wfg_dred = 4
};
typedef enum Win32ConsoleColor Win32ConsoleColor;

#ifdef MULOG_WIN32
HANDLE mulog_hstdout = 0;
HANDLE mulog_hstderr = 0;
#endif
    
void Win32ConClrSet(FILE *f, Win32ConsoleColor fg, Win32ConsoleColor bg) {
#ifdef MULOG_WIN32
    if(f == stdout) SetConsoleTextAttribute(mulog_hstdout, bg | fg);
    else if(f == stderr) SetConsoleTextAttribute(mulog_hstderr, bg | fg);
#endif
}

void Win32ConClrReset(FILE *f) {
#ifdef MULOG_WIN32
    Win32ConClrSet(f, wfg_grey, wbg_black);
#endif
}

enum UnixConsoleColor {
    u_none = -1,
    u_black = 0, u_red,
    u_green,     u_yellow,
    u_blue,      u_magenta,
    u_cyan,      u_white
};
typedef enum UnixConsoleColor UnixConsoleColor;

void UnixConClrSet(FILE *f, UnixConsoleColor fg, int fgbright, UnixConsoleColor bg, int bgbright) {
#ifdef MULOG_UNIX
    if(fg != u_none) fprintf(f, "\x1B[%d;%dm", (fgbright ? 1 :22), (30+fg));
    if(bg != u_none) fprintf(f, "\x1B[%d;%dm", (bgbright ? 1 : 22), (40 + bg));
#endif
}

void UnixConClrReset(FILE *f) {
#ifdef MULOG_UNIX
    fputs("\x1b[0m", f);
#endif
}

/* =============
 * Instance data
 * =============
 */

enum mulog_flg {
    mulog_f_wdbg = 0x1,
    mulog_f_wclr = 0x2
};
typedef enum mulog_flg mulog_flg;

struct mulog_t {
    mulog_type type;
    mulog_timefmt timefmt;
    mulog_flg flag;
    FILE *fh;
    mulog_ref left;
    mulog_ref right;
};

/* ===================
 * Query/mod functions
 * ===================
 */

mulog_type mulog_get_type(mulog_ref l) {
    if(!l) return mulog_t_dummy;
    return l->type;
}

FILE *mulog_get_file(mulog_ref l) {
    if(!l) return NULL;
    return l->fh;
}
mulog_status mulog_set_file(mulog_ref l, FILE *f) {
    if(!l) return mulog_err_type;
    switch(l->type) {
    case mulog_t_file:
        l->fh = f;
        return mulog_ok;
    default:
        return mulog_err_type;
    }
}

int mulog_get_with_debug(mulog_ref l) {
    if(!l) return -1;
    switch(l->type) {
    case mulog_t_file:
    case mulog_t_con:
        return l->flag & mulog_f_wdbg;
    default:
        return -1;
    }
}
mulog_status mulog_set_with_debug(mulog_ref l, int with_debug) {
    if(!l) return mulog_err_type;
    switch(l->type) {
    case mulog_t_file:
    case mulog_t_con:
        if(with_debug) l->flag |= mulog_f_wdbg;
        else l->flag &= ~mulog_f_wdbg;
        return mulog_ok;
    default:
        return mulog_err_type;
    }
}

int mulog_get_with_color(mulog_ref l) {
    if(!l) return -1;
    switch(l->type) {
    case mulog_t_con:
        return (l->flag & mulog_f_wclr) >> 1;
    default:
        return -1;
    }
}
mulog_status mulog_set_with_color(mulog_ref l, int with_color) {
    if(!l) return mulog_err_type;
    switch(l->type) {
    case mulog_t_con:
        if(with_color) l->flag |= mulog_f_wclr;
        else l->flag &= ~mulog_f_wclr;
        return mulog_ok;
    default:
        return mulog_err_type;
    }
}

mulog_timefmt mulog_get_timefmt(mulog_ref l) {
    if(!l) return mulog_tm_na;
    return l->timefmt;
}
mulog_status mulog_set_timefmt(mulog_ref l, mulog_timefmt timefmt) {
    if(!l) return mulog_err_type;
    switch(l->type) {
    case mulog_t_file:
    case mulog_t_con:
        if(timefmt < mulog_tm_na && timefmt > -1) {
            l->timefmt = timefmt;
            return mulog_ok;
        } else return mulog_err_inval;
    default:
        return mulog_err_type;
    }
}

mulog_ref mulog_get_left(mulog_ref l) {
    if(!l) return NULL;
    return l->left;
}
mulog_ref mulog_get_right(mulog_ref l) {
    if(!l) return NULL;
    return l->right;
}
mulog_status mulog_set_left(mulog_ref l, mulog_ref left) {
    if(!l) return mulog_err_type;
    switch(l->type) {
    case mulog_t_split:
        l->left = left;
        return mulog_ok;
    default:
        return mulog_err_type;
    }
}
mulog_status mulog_set_right(mulog_ref l, mulog_ref right) {
    if(!l) return mulog_err_type;
    switch(l->type) {
    case mulog_t_split:
        l->right = right;
        return mulog_ok;
    default:
        return mulog_err_type;
    }
}

/* ================
 * Destruction func
 * ================
 */

void mulog_destroy(mulog_ref l) {
    if(l) free(l);
}

/* ==============
 * Creation funcs
 * ==============
 */

mulog_status mulog_create_file(mulog_ref *l, FILE *f, mulog_timefmt timefmt, int with_debug) {
    if(timefmt < 0 || timefmt > mulog_tm_na) return mulog_err_inval;
    mulog_ref m = malloc(sizeof(struct mulog_t));
    m->type = mulog_t_file;
    m->fh = f;
    m->timefmt = timefmt;
    m->left = NULL;
    m->right = NULL;
    m->flag = 0;
    if(with_debug) m->flag |= mulog_f_wdbg;
    *l = m;
    return mulog_ok;
}

mulog_status mulog_create_con(mulog_ref *l, mulog_timefmt timefmt, int with_debug, int with_color) {
#ifdef MULOG_WIN32
    if(!mulog_hstdout) mulog_hstdout = GetStdHandle(STD_OUTPUT_HANDLE);
    if(!mulog_hstderr) mulog_hstderr = GetStdHandle(STD_ERROR_HANDLE);
#endif
    
    if(timefmt < 0 || timefmt > mulog_tm_na) return mulog_err_inval;
    mulog_ref m = malloc(sizeof(struct mulog_t));
    m->type = mulog_t_con;
    m->fh = NULL;
    m->timefmt = timefmt;
    m->left = NULL;
    m->right = NULL;
    m->flag = 0;
    if(with_debug) m->flag |= mulog_f_wdbg;
    if(with_color) m->flag |= mulog_f_wclr;
    *l = m;
    return mulog_ok;
}

mulog_status mulog_create_split(mulog_ref *l, mulog_ref left, mulog_ref right) {
    mulog_ref m = malloc(sizeof(struct mulog_t));
    m->type = mulog_t_split;
    m->fh = NULL;
    m->timefmt = mulog_tm_na;
    m->left = left;
    m->right = right;
    m->flag = 0;
    *l = m;
    return mulog_ok;
}

/*
mulog_status mulog_create_dummy(mulog_ref *l) {
    mulog_ref m = malloc(sizeof(struct mulog_t));
    m->type = mulog_t_dummy;
    m->fh = NULL;
    m->timefmt = mulog_tm_na;
    m->left = NULL;
    m->right = NULL;
    m->flag = 0;
    *l = m;
    return mulog_ok;
}
*/
mulog_status mulog_create_dummy(mulog_ref *l) {
    *l = NULL;
    return mulog_ok;
}

/* =========
 * Msg funcs
 * =========
 */

// formating helper func & constants
const char* mulog_tc_long = "%c %Z";
const char* mulog_tc_short = "%x %X %Z";
const char* mulog_tc_fixed = "%Y-%m-%d %H:%M:%S";
const char* mulog_s_err = "ERROR:";
const char* mulog_s_warn = "WARNING:";
const char* mulog_s_info = "INFO:";
const char* mulog_s_dbg = "DEBUG:";
void logstr(FILE *to, mulog_timefmt fmt, const char* msstr, const char* str, va_list va) {
    static char tmstr[256];
    static struct tm *tmtm;
    static time_t ttm;

    time(&ttm);

    switch(fmt) {
    case mulog_tm_long:
        tmtm = localtime(&ttm);
        strftime(tmstr, 256, mulog_tc_long, tmtm);
        break;
    case mulog_tm_short:
        tmtm = localtime(&ttm);
        strftime(tmstr, 256, mulog_tc_short, tmtm);
        break;
    case mulog_tm_fixed:
        tmtm = gmtime(&ttm);
        strftime(tmstr, 256, mulog_tc_fixed, tmtm);
        break;
    default: return;
    }

    fprintf(to, "[%s] %s ", tmstr, msstr);
    vfprintf(to, str, va);
    fputc('\n', to);
}

void mulog_verr(mulog_ref l, const char* str, va_list va) {
    if(!l) return;
    va_list vasc;
    switch(l->type) {
    case mulog_t_file:
        logstr(l->fh, l->timefmt, mulog_s_err, str, va);
        return;
    case mulog_t_con:
        if(l->flag & mulog_f_wclr) UnixConClrSet(stderr, u_red, 1, u_none, 0);
        if(l->flag & mulog_f_wclr) Win32ConClrSet(stderr, wfg_red, wbg_black);
        logstr(stderr, l->timefmt, mulog_s_err, str, va);
        if(l->flag & mulog_f_wclr) Win32ConClrReset(stderr);
        if(l->flag & mulog_f_wclr) UnixConClrReset(stderr);
        return;
    case mulog_t_split:
        va_copy(vasc, va);
        mulog_verr(l->left, str, va);
        mulog_verr(l->right, str, vasc);
        va_end(vasc);
        return;
    default: return;
    }
}
void mulog_err(mulog_ref l, const char* str, ...) {
    va_list va;
    va_start(va, str);
    mulog_verr(l, str, va);
    va_end(va);
}

void mulog_vwarn(mulog_ref l, const char* str, va_list va) {
    if(!l) return;
    va_list vasc;
    switch(l->type) {
    case mulog_t_file:
        logstr(l->fh, l->timefmt, mulog_s_warn, str, va);
        return;
    case mulog_t_con:
        if(l->flag & mulog_f_wclr) UnixConClrSet(stderr, u_magenta, 1, u_none, 0);
        if(l->flag & mulog_f_wclr) Win32ConClrSet(stderr, wfg_magenta, wbg_black);
        logstr(stderr, l->timefmt, mulog_s_warn, str, va);
        if(l->flag & mulog_f_wclr) Win32ConClrReset(stderr);
        if(l->flag & mulog_f_wclr) UnixConClrReset(stderr);
        return;
    case mulog_t_split:
        va_copy(vasc, va);
        mulog_vwarn(l->left, str, va);
        mulog_vwarn(l->right, str, vasc);
        va_end(vasc);
        return;
    default: return;
    }
}
void mulog_warn(mulog_ref l, const char* str, ...) {
    va_list va;
    va_start(va, str);
    mulog_vwarn(l, str, va);
    va_end(va);
}

void mulog_vinfo(mulog_ref l, const char* str, va_list va) {
    if(!l) return;
    va_list vasc;
    switch(l->type) {
    case mulog_t_file:
        logstr(l->fh, l->timefmt, mulog_s_info, str, va);
        return;
    case mulog_t_con:
        if(l->flag & mulog_f_wclr) UnixConClrSet(stdout, u_cyan, 1, u_none, 0);
        if(l->flag & mulog_f_wclr) Win32ConClrSet(stdout, wfg_cyan, wbg_black);
        logstr(stdout, l->timefmt, mulog_s_info, str, va);
        if(l->flag & mulog_f_wclr) Win32ConClrReset(stdout);
        if(l->flag & mulog_f_wclr) UnixConClrReset(stdout);
        return;
    case mulog_t_split:
        va_copy(vasc, va);
        mulog_vinfo(l->left, str, va);
        mulog_vinfo(l->right, str, vasc);
        va_end(vasc);
        return;
    default: return;
    }
}
void mulog_info(mulog_ref l, const char* str, ...) {
    va_list va;
    va_start(va, str);
    mulog_vinfo(l, str, va);
    va_end(va);
}

void mulog_vdbg(mulog_ref l, const char* str, va_list va) {
    if(!l) return;
    va_list vasc;
    switch(l->type) {
    case mulog_t_file:
        if(l->flag & mulog_f_wdbg) logstr(l->fh, l->timefmt, mulog_s_dbg, str, va);
        return;
    case mulog_t_con:
        if(l->flag & mulog_f_wdbg) {
            if(l->flag & mulog_f_wclr) UnixConClrSet(stdout, u_white, 1, u_none, 0);
            if(l->flag & mulog_f_wclr) Win32ConClrSet(stdout, wfg_white, wbg_black);
            logstr(stdout, l->timefmt, mulog_s_dbg, str, va);
            if(l->flag & mulog_f_wclr) Win32ConClrReset(stdout);
            if(l->flag & mulog_f_wclr) UnixConClrReset(stdout);
        }
        return;
    case mulog_t_split:
        va_copy(vasc, va);
        mulog_vdbg(l->left, str, va);
        mulog_vdbg(l->right, str, vasc);
        va_end(vasc);
        return;
    default: return;
    }
}
void mulog_dbg(mulog_ref l, const char* str, ...) {
    va_list va;
    va_start(va, str);
    mulog_vdbg(l, str, va);
    va_end(va);
}

