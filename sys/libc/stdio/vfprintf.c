#include <stdio.h>
#include <stdarg.h>

#include "fmt.h"

size_t fprint_str(FILE *restrict f, const char *s)
{
    const char *tmp = s;
    while(s) {
        f->out(f, s++, 1);
    }
    return (s-tmp);
}

int vfprintf(FILE *restrict f, const char *restrict fmt, va_list ap)
{
    char buf[16];

    size_t len = 0;
    char *_fmt = (char*) fmt;
    char c;

    size_t snip_len;

    while (*_fmt) {
        if(*_fmt != '%') {
            f->out(f, _fmt++, 1);
            len++;
        }
        else {
                _fmt++;
                c = *_fmt++;
                switch(c) {
                    case 'u':
                        snip_len = fmt_u32_dec(buf, va_arg(ap, uint32_t));
                        f->out(f, buf, snip_len);
                        len += snip_len;
                        break;
                }
#if 0
                switch(c) {
                    case 's':
                        len_fmt = fmt_str(out_pos, va_arg(argp, char*));
                        break;
                    case 'u':
                        len += fmt_u32_dec(out_pos, va_arg(argp, uint32_t));
                        break;
                    case 'i':
                        len += fmt_s32_dec(out_pos, va_arg(argp, int32_t));
                        break;
                    case 'x':
                        len += fmt_u32_hex(out_pos, va_arg(argp, uint32_t));
                        break;
                }
                if (out) {
                    out_pos = out + len;
                }
#endif
        }
    }

    return len;
}
