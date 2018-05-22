#include <stdio.h>
#include <string.h>
#include <stdarg.h>

/* integar argument functions */
int fi1(int a)
{
    return a;
}
int fi2(int a, int b)
{
    return a + b;
}
int fi3(int a, int b, int c)
{
    return a + b + c;
}
int fi4(int a, int b, int c, int d)
{
    return a + b + c + d;
}
int fi5(int a, int b, int c, int d, int e)
{
    return a + b + c + d + e;
}
int fi6(int a, int b, int c, int d, int e, int f)
{
    return a + b + c + d + e + f;
}

/* long argument functions */
long fl1(long a)
{
    return a;
}
long fl2(long a, long b)
{
    return a + b;
}
long fl3(long a, long b, long c)
{
    return a + b + c;
}
long fl4(long a, long b, long c, long d)
{
    return a + b + c + d;
}
long fl5(long a, long b, long c, long d, long e)
{
    return a + b + c + d + e;
}
long fl6(long a, long b, long c, long d, long e, long f)
{
    return a + b + c + d + e + f;
}

/* float argument functions */
float ff1(float a)
{
    return a;
}
float ff2(float a, float b)
{
    return a + b;
}
float ff3(float a, float b, float c)
{
    return a + b + c;
}
float ff4(float a, float b, float c, float d)
{
    return a + b + c + d;
}
float ff5(float a, float b, float c, float d, float e)
{
    return a + b + c + d + e;
}
float ff6(float a, float b, float c, float d, float e, float f)
{
    return a + b + c + d + e + f;
}

/* double argument functions */
double fd1(double a)
{
    return a;
}
double fd2(double a, double b)
{
    return a + b;
}
double fd3(double a, double b, double c)
{
    return a + b + c;
}
double fd4(double a, double b, double c, double d)
{
    return a + b + c + d;
}
double fd5(double a, double b, double c, double d, double e)
{
    return a + b + c + d + e;
}
double fd6(double a, double b, double c, double d, double e, double f)
{
    return a + b + c + d + e + f;
}

/* pointer argument functions */
size_t fp1(char *a)
{
    return strlen(a);
}
size_t fp2(char *a, char *b)
{
    return strlen(a) + strlen(b);
}
size_t fp3(char *a, char *b, char *c)
{
    return strlen(a) + strlen(b) + strlen(c);
}
size_t fp4(char *a, char *b, char *c, char *d)
{
    return strlen(a) + strlen(b) + strlen(c) + strlen(d);
}
size_t fp5(char *a, char *b, char *c, char *d, char *e)
{
    return strlen(a) + strlen(b) + strlen(c) + strlen(d) + strlen(e);
}
size_t fp6(char *a, char *b, char *c, char *d, char *e, char *f)
{
    return strlen(a) + strlen(b) + strlen(c) + strlen(d) + strlen(e) + strlen(f);
}

/* variable argument functions */
int sumi(int count, ...)
{
    va_list vargs;
    int sum = 0;
    va_start(vargs, count);
    for (int i = 0; i < count; i++) {
        sum += va_arg(vargs, int);
    }
    va_end(vargs);
    return sum;
}

long suml(int count, ...)
{
    va_list vargs;
    long sum = 0;
    va_start(vargs, count);
    for (int i = 0; i < count; i++) {
        sum += va_arg(vargs, long);
    }
    va_end(vargs);
    return sum;
}

float sumf(int count, ...)
{
    va_list vargs;
    float sum = 0.0;
    va_start(vargs, count);
    for (int i = 0; i < count; i++) {
        sum += va_arg(vargs, float);
    }
    va_end(vargs);
    return sum;
}

double sumd(int count, ...)
{
    va_list vargs;
    double sum = 0.0;
    va_start(vargs, count);
    for (int i = 0; i < count; i++) {
        sum += va_arg(vargs, double);
    }
    va_end(vargs);
    return sum;
}

size_t sums(int count, ...)
{
    va_list vargs;
    size_t sum = 0;
    va_start(vargs, count);
    for (int i = 0; i < count; i++) {
        char* s = va_arg(vargs, char*);
        sum += strlen(s);
    }
    va_end(vargs);
    return sum;
}

int __cdecl main(int argc, char *argv[])
{
    printf("fi1() : %d\n", fi1(1));
    printf("fi2() : %d\n", fi2(1, 2));
    printf("fi3() : %d\n", fi3(1, 2, 3));
    printf("fi4() : %d\n", fi4(1, 2, 3, 4));
    printf("fi5() : %d\n", fi5(1, 2, 3, 4, 5));
    printf("fi6() : %d\n", fi6(1, 2, 3, 4, 5, 6));

    printf("fl1() : %ld\n", fl1(1));
    printf("fl2() : %ld\n", fl2(1, 2));
    printf("fl3() : %ld\n", fl3(1, 2, 3));
    printf("fl4() : %ld\n", fl4(1, 2, 3, 4));
    printf("fl5() : %ld\n", fl5(1, 2, 3, 4, 5));
    printf("fl6() : %ld\n", fl6(1, 2, 3, 4, 5, 6));

    printf("ff1() : %.2f\n", ff1(1.0));
    printf("ff2() : %.2f\n", ff2(1.0, 2.0));
    printf("ff3() : %.2f\n", ff3(1.0, 2.0, 3.0));
    printf("ff4() : %.2f\n", ff4(1.0, 2.0, 3.0, 4.0));
    printf("ff5() : %.2f\n", ff5(1.0, 2.0, 3.0, 4.0, 5.0));
    printf("ff6() : %.2f\n", ff6(1.0, 2.0, 3.0, 4.0, 5.0, 6.0));

    printf("fd1() : %.2lf\n", fd1(1.0));
    printf("fd2() : %.2lf\n", fd2(1.0, 2.0));
    printf("fd3() : %.2lf\n", fd3(1.0, 2.0, 3.0));
    printf("fd4() : %.2lf\n", fd4(1.0, 2.0, 3.0, 4.0));
    printf("fd5() : %.2lf\n", fd5(1.0, 2.0, 3.0, 4.0, 5.0));
    printf("fd6() : %.2lf\n", fd6(1.0, 2.0, 3.0, 4.0, 5.0, 6.0));

    printf("fp1() : %zd\n", fp1("a"));
    printf("fp2() : %zd\n", fp2("a", "bb"));
    printf("fp3() : %zd\n", fp3("a", "bb", "ccc"));
    printf("fp4() : %zd\n", fp4("a", "bb", "ccc", "dddd"));
    printf("fp5() : %zd\n", fp5("a", "bb", "ccc", "dddd", "eeeee"));
    printf("fp6() : %zd\n", fp6("a", "bb", "ccc", "dddd", "eeeee", "ffffff"));

    printf("sumi() : %d\n",     sumi(10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10));
    printf("suml() : %ld\n",    suml(10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10));
    printf("sumf() : %.2f\n",   sumf(10, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0));
    printf("sumd() : %.2lf\n",  sumd(10, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0));
    printf("sums() : %zd\n",    sums(10, "1", "12", "123", "1234", "12345", "123456", "1234567", "12345678", "123456789", "1234567890"));

    return 0;
}