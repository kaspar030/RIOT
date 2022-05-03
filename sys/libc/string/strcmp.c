int strcmp(const char *s1, const char *s2)
{
    char c1, c2;
    int diff;
    while ( (c1 = *s1++) && (c2 = *s2++)) {
        if ((diff = (int)c2 - (int)c1)) {
            break;
        }
    }
    return diff;
}
