#ifndef CTYPE_H
#define CTYPE_H

static inline int isalpha(int c)
{
    return ((((unsigned)c | 32) - 'a') < 26);
}

static inline int isdigit(int c)
{
    return (((unsigned)c - '0') < 10);
}

static inline int isalnum(int c)
{
    return isalpha(c) || isdigit(c);
}

static inline int isspace(int c)
{
    return c == ' ' || (unsigned)c - '\t' < 5;
}

static inline int islower(a)
{
    return (((unsigned)(a) - 'a') < 26);
}

static inline int isupper(a)
{
    return (((unsigned)(a) - 'A') < 26);
}

static inline int isprint(a)
{
    return (((unsigned)(a) - 0x20) < 0x5f);
}

static inline int isgraph(a)
{
    return (((unsigned)(a) - 0x21) < 0x5e);
}

int isblank(int);
int iscntrl(int);
int ispunct(int);
int isxdigit(int);
int tolower(int);
int toupper(int);

#endif /* CTYPE_H */
