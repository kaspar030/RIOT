char *strcpy(void *dest, const void *src)
{
    char *d = dest;
    const char *s = src;
    while(*s) *d++ = *s++;
    *d = *s;
    return dest;
}
