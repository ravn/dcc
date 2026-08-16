/* tplng.c - printf long-only runtime entry coverage (_pflng and friends).
 * Confirms %ld/%lu/%lx support is shared identically across the whole printf
 * family (sprintf/fprintf/vprintf/vsprintf/vfprintf), not just printf(). */
#include <stdio.h>
#include <stdarg.h>

static void call_vprintf(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
}

static void call_vfprintf(FILE *fp, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vfprintf(fp, fmt, ap);
    va_end(ap);
}

static void call_vsprintf(char *buf, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vsprintf(buf, fmt, ap);
    va_end(ap);
}

static void call_vsnprintf(char *buf, size_t n, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, n, fmt, ap);
    va_end(ap);
}

int main(void)
{
    char buf[40];
    int r;
    long sl;
    unsigned long ul;

    sl = -123456L;
    ul = 0x89ABCDEFUL;

    printf("tplng start\n");
    printf("sl=%ld\n", sl);
    printf("ul=%lu\n", ul);
    printf("hex=%lx\n", ul);
    printf("wide=[%12ld]\n", sl);
    printf("mix=%d %ld %s\n", 42, 1234567L, "ok");

    sprintf(buf, "sp=%ld", sl);
    printf("%s\n", buf);

    fprintf(stdout, "fp=%lu\n", ul);

    call_vprintf("vp=%lx\n", ul);

    call_vsprintf(buf, "vs=%ld", sl);
    printf("%s\n", buf);

    call_vfprintf(stdout, "vf=%lu\n", ul);

    r = snprintf(buf, sizeof(buf), "sn=%ld", sl);
    printf("%s r=%d\n", buf, r);

    r = snprintf(buf, 4, "sn=%ld", sl);
    printf("%s r=%d\n", buf, r);

    call_vsnprintf(buf, sizeof(buf), "vn=%lu", ul);
    printf("%s\n", buf);

    printf("tplng ok\n");
    return 0;
}
