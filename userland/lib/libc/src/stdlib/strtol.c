#include <stdlib.h>

static int isalpha(int c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

static int isupper(int c)
{
    return (c >= 'A' && c <= 'Z');
}

static int isdigit(int c)
{
    return c >= '0' && c <= '9';
}

static int isspace(int c)
{
    return c == ' ' || c == '\t' || c == '\n';
}

long int strtol(const char *nptr, char **endptr, int base)
{
    int sign = 1;
    long int result = 0;
    int digit;

    while (isspace(*nptr))
        ++nptr;

    if (*nptr == '-')
    {
        sign = -1;
        ++nptr;
    }
    else if (*nptr == '+')
        ++nptr;

    if ((base == 0 || base == 16) && *nptr == '0' &&
        (*(nptr + 1) == 'x' || *(nptr + 1) == 'X'))
    {
        nptr += 2;
        base = 16;
    }
    else if (base == 0 && *nptr == '0')
    {
        ++nptr;
        base = 8;
    }
    else if (base == 0)
        base = 10;

    while (*nptr)
    {
        digit = *nptr;

        if (isdigit(digit))
            digit -= '0';
        else if (isalpha(digit))
            digit -= isupper(digit) ? 'A' - 10 : 'a' - 10;
        else
            break;

        if (digit >= base)
            break;

        result *= base;
        result += digit;

        ++nptr;
    }

    if (endptr)
        *endptr = (char *)nptr;

    return sign * result;
}
