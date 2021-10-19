#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "fmt.h"

enum {
    FLAG_ZERO_PAD               = 0x1,
    FLAG_ALT                    = 0x2,
    FLAG_LEFT_ADJUST            = 0x4,
    FLAG_BLANK_BEFORE_POSITIVE  = 0x8,
    FLAG_ALWAYS_SIGN            = 0x10,
    FLAG_PRECISION              = 0x20,
    FLAG_WIDTH                  = 0x40,
    FLAG_LONG                   = 0x80,
    FLAG_LONG_LONG              = 0x100,
};

size_t fprint_str(FILE *restrict f, const char *s)
{
    const char *tmp = s;

    while (s) {
        f->out(f, s++, 1);
    }
    return (s - tmp);
}

static size_t _fpad(FILE *restrict f, int width, int len, char pad_char)
{
    if (width >= 0) {
        if (len < width) {
            unsigned n = width - len;
            while (n--) {
                f->out(f, &pad_char, 1);
            }
            return width - len;
        }
    }
    return 0;
}

int vfprintf(FILE *restrict f, const char *restrict fmt, va_list ap)
{
    char buf[16];
    const char *prefix;

    size_t len = 0;
    char c;
    char *s;

    while ((c = *fmt++)) {
        if (c != '%') {
            f->out(f, &c, 1);
            len++;
        }
        else {
            unsigned flags = 0;
            unsigned width = 0;
            unsigned precision = 0;
            prefix = NULL;

            size_t (*int_conv_func)(uint8_t *, uint32_t) = NULL;

            /* flags */
            while ((c = *fmt++)) {
                switch (c) {
                    case '0':
                        flags |= FLAG_ZERO_PAD;
                        continue;
                    case '#':
                        flags |= FLAG_ALT;
                        continue;
                    case '-':
                        flags |= FLAG_LEFT_ADJUST;
                        continue;
                    case ' ':
                        flags |= FLAG_BLANK_BEFORE_POSITIVE;
                        continue;
                    case '+':
                        flags |= FLAG_ALWAYS_SIGN;
                        continue;
                    default:
                        fmt--;
                }
                break;
            }

            while ((c = *fmt++)) {
                switch (c) {
                    case 'l':
                        flags |= flags & FLAG_LONG ? FLAG_LONG_LONG : FLAG_LONG;
                        break;
                    case '.':
                        flags |= FLAG_PRECISION;
                        break;
                    case 's':
                        s = va_arg(ap, char *);
                        unsigned slen = flags & FLAG_PRECISION ? precision : strlen(s);
                        if (!(flags & FLAG_LEFT_ADJUST)) {
                            len += _fpad(f, width, slen, ' ');
                        }
                        len += f->out(f, s, slen);
                        if (flags & FLAG_LEFT_ADJUST) {
                            len += _fpad(f, width, slen, ' ');
                        }
                        goto done;
                    case 'd':
                    case 'i':
                        int_conv_func = (void *)fmt_s32_dec;
                        goto done;
                    case 'u':
                        int_conv_func = (void *)fmt_u32_dec;
                        goto done;
                    case 'p':
                        prefix = "0x";
                    /* fall through */
                    case 'x':
                    case 'X':
                        int_conv_func = (void *)fmt_u32_hex;
                        goto done;
                    default:
                        if (isdigit(c)) {
                            uint32_t val = scn_u32_dec(fmt - 1, strlen(fmt - 1));
                            if (flags & FLAG_PRECISION) {
                                precision = val;
                            }
                            else {
                                width = val;
                            }
                            while (isdigit(*fmt)) {
                                fmt++;
                            }
                        }
                }
            }
done:
            if (int_conv_func) {
                size_t clen;
                if (0) { }
                else if (flags & FLAG_LONG_LONG) {
                    int64_t value = (int64_t)va_arg(ap, long long int);
                    clen = fmt_s64_dec((void *)buf, value);
                }
                else {
                    int32_t value;
                    if (flags & FLAG_LONG) {
                        value = (int32_t)va_arg(ap, long int);
                    }
                    else {
                        value = (int)va_arg(ap, int);
                    }
                    clen = int_conv_func((void *)buf, value);
                }
                size_t prefix_len = prefix ? strlen(prefix) : 0;
                if (!(flags & FLAG_LEFT_ADJUST)) {
                    len += _fpad(f, width, clen + prefix_len, ' ');
                }
                if (prefix_len) {
                    len += f->out(f, prefix, prefix_len);
                }
                len += f->out(f, buf, clen);
                if (flags & FLAG_LEFT_ADJUST) {
                    len += _fpad(f, width, clen + prefix_len, ' ');
                }
            }
        }
    }

    return len;
}
