char *strcpy(char *dest, const char *src)
{
    char *d = dest;
    const char *s = src;
    while(*s) *d++ = *s++;
    *d = *s;
    return dest;
}
