#include <stdio.h>
#include <string.h>
#include <stdarg.h>

class foo {
protected:
    int  mi;
    long ml;
    float mf;
    double md;
    char* ms;
public:
    foo(int ai, long al, float af, double ad, char* as);
    int fi1(int a);
    int fi2(int a, int b);
    int fi3(int a, int b, int c);
    int fi4(int a, int b, int c, int d);
    int fi5(int a, int b, int c, int d, int e);
    int fi6(int a, int b, int c, int d, int e, int f);
    long fl1(long a);
    long fl2(long a, long b);
    long fl3(long a, long b, long c);
    long fl4(long a, long b, long c, long d);
    long fl5(long a, long b, long c, long d, long e);
    long fl6(long a, long b, long c, long d, long e, long f);
    float ff1(float a);
    float ff2(float a, float b);
    float ff3(float a, float b, float c);
    float ff4(float a, float b, float c, float d);
    float ff5(float a, float b, float c, float d, float e);
    float ff6(float a, float b, float c, float d, float e, float f);
    double fd1(double a);
    double fd2(double a, double b);
    double fd3(double a, double b, double c);
    double fd4(double a, double b, double c, double d);
    double fd5(double a, double b, double c, double d, double e);
    double fd6(double a, double b, double c, double d, double e, double f);
    size_t fp1(char *a);
    size_t fp2(char *a, char *b);
    size_t fp3(char *a, char *b, char *c);
    size_t fp4(char *a, char *b, char *c, char *d);
    size_t fp5(char *a, char *b, char *c, char *d, char *e);
    size_t fp6(char *a, char *b, char *c, char *d, char *e, char *f);
    int sumi(int count, ...);
    long suml(int count, ...);
    float sumf(int count, ...);
    double sumd(int count, ...);
    size_t sums(int count, ...);
};

#define HELLO_FOO (printf("Hello, mi=%d, ml=%ld, mf=%.2f, md=%.2lf, ms=[%s]\n", this->mi, this->ml, this->mf, this->md, this->ms))

foo::foo(int ai, long al, float af, double ad, char* as)
{
    this->mi = ai;
    this->ml = al;
    this->mf = af;
    this->md = ad;
    this->ms = as;
    HELLO_FOO;
}

/* integar argument functions */
int foo::fi1(int a)
{
    HELLO_FOO;
    return a;
}
int foo::fi2(int a, int b)
{
    HELLO_FOO;
    return a + b;
}
int foo::fi3(int a, int b, int c)
{
    HELLO_FOO;
    return a + b + c;
}
int foo::fi4(int a, int b, int c, int d)
{
    HELLO_FOO;
    return a + b + c + d;
}
int foo::fi5(int a, int b, int c, int d, int e)
{
    HELLO_FOO;
    return a + b + c + d + e;
}
int foo::fi6(int a, int b, int c, int d, int e, int f)
{
    HELLO_FOO;
    return a + b + c + d + e + f;
}

/* long argument functions */
long foo::fl1(long a)
{
    HELLO_FOO;
    return a;
}
long foo::fl2(long a, long b)
{
    HELLO_FOO;
    return a + b;
}
long foo::fl3(long a, long b, long c)
{
    HELLO_FOO;
    return a + b + c;
}
long foo::fl4(long a, long b, long c, long d)
{
    HELLO_FOO;
    return a + b + c + d;
}
long foo::fl5(long a, long b, long c, long d, long e)
{
    HELLO_FOO;
    return a + b + c + d + e;
}
long foo::fl6(long a, long b, long c, long d, long e, long f)
{
    HELLO_FOO;
    return a + b + c + d + e + f;
}

/* float argument functions */
float foo::ff1(float a)
{
    HELLO_FOO;
    return a;
}
float foo::ff2(float a, float b)
{
    HELLO_FOO;
    return a + b;
}
float foo::ff3(float a, float b, float c)
{
    HELLO_FOO;
    return a + b + c;
}
float foo::ff4(float a, float b, float c, float d)
{
    HELLO_FOO;
    return a + b + c + d;
}
float foo::ff5(float a, float b, float c, float d, float e)
{
    HELLO_FOO;
    return a + b + c + d + e;
}
float foo::ff6(float a, float b, float c, float d, float e, float f)
{
    HELLO_FOO;
    return a + b + c + d + e + f;
}

/* double argument functions */
double foo::fd1(double a)
{
    HELLO_FOO;
    return a;
}
double foo::fd2(double a, double b)
{
    HELLO_FOO;
    return a + b;
}
double foo::fd3(double a, double b, double c)
{
    HELLO_FOO;
    return a + b + c;
}
double foo::fd4(double a, double b, double c, double d)
{
    HELLO_FOO;
    return a + b + c + d;
}
double foo::fd5(double a, double b, double c, double d, double e)
{
    HELLO_FOO;
    return a + b + c + d + e;
}
double foo::fd6(double a, double b, double c, double d, double e, double f)
{
    HELLO_FOO;
    return a + b + c + d + e + f;
}

/* pointer argument functions */
size_t foo::fp1(char *a)
{
    HELLO_FOO;
    return strlen(a);
}
size_t foo::fp2(char *a, char *b)
{
    HELLO_FOO;
    return strlen(a) + strlen(b);
}
size_t foo::fp3(char *a, char *b, char *c)
{
    HELLO_FOO;
    return strlen(a) + strlen(b) + strlen(c);
}
size_t foo::fp4(char *a, char *b, char *c, char *d)
{
    HELLO_FOO;
    return strlen(a) + strlen(b) + strlen(c) + strlen(d);
}
size_t foo::fp5(char *a, char *b, char *c, char *d, char *e)
{
    HELLO_FOO;
    return strlen(a) + strlen(b) + strlen(c) + strlen(d) + strlen(e);
}
size_t foo::fp6(char *a, char *b, char *c, char *d, char *e, char *f)
{
    HELLO_FOO;
    return strlen(a) + strlen(b) + strlen(c) + strlen(d) + strlen(e) + strlen(f);
}

/* variable argument functions */
int foo::sumi(int count, ...)
{
    va_list vargs;
    int sum = 0;
    va_start(vargs, count);
    for (int i = 0; i < count; i++) {
        sum += va_arg(vargs, int);
    }
    va_end(vargs);
    HELLO_FOO;
    return sum;
}

long foo::suml(int count, ...)
{
    va_list vargs;
    long sum = 0;
    va_start(vargs, count);
    for (int i = 0; i < count; i++) {
        sum += va_arg(vargs, long);
    }
    va_end(vargs);
    HELLO_FOO;
    return sum;
}

float foo::sumf(int count, ...)
{
    va_list vargs;
    float sum = 0.0;
    va_start(vargs, count);
    for (int i = 0; i < count; i++) {
        sum += va_arg(vargs, float);
    }
    va_end(vargs);
    HELLO_FOO;
    return sum;
}

double foo::sumd(int count, ...)
{
    va_list vargs;
    double sum = 0.0;
    va_start(vargs, count);
    for (int i = 0; i < count; i++) {
        sum += va_arg(vargs, double);
    }
    va_end(vargs);
    HELLO_FOO;
    return sum;
}

size_t foo::sums(int count, ...)
{
    va_list vargs;
    size_t sum = 0;
    va_start(vargs, count);
    for (int i = 0; i < count; i++) {
        char* s = va_arg(vargs, char*);
        sum += strlen(s);
    }
    va_end(vargs);
    HELLO_FOO;
    return sum;
}

int __cdecl main(int argc, char *argv[])
{
    foo *f = new foo(1, 2, 3.4, 5.67, "foo");
    printf("fi1() : %d\n", f->fi1(1));
    printf("fi2() : %d\n", f->fi2(1, 2));
    printf("fi3() : %d\n", f->fi3(1, 2, 3));
    printf("fi4() : %d\n", f->fi4(1, 2, 3, 4));
    printf("fi5() : %d\n", f->fi5(1, 2, 3, 4, 5));
    printf("fi6() : %d\n", f->fi6(1, 2, 3, 4, 5, 6));

    printf("fl1() : %ld\n", f->fl1(1));
    printf("fl2() : %ld\n", f->fl2(1, 2));
    printf("fl3() : %ld\n", f->fl3(1, 2, 3));
    printf("fl4() : %ld\n", f->fl4(1, 2, 3, 4));
    printf("fl5() : %ld\n", f->fl5(1, 2, 3, 4, 5));
    printf("fl6() : %ld\n", f->fl6(1, 2, 3, 4, 5, 6));

    printf("ff1() : %.2f\n", f->ff1(1.0));
    printf("ff2() : %.2f\n", f->ff2(1.0, 2.0));
    printf("ff3() : %.2f\n", f->ff3(1.0, 2.0, 3.0));
    printf("ff4() : %.2f\n", f->ff4(1.0, 2.0, 3.0, 4.0));
    printf("ff5() : %.2f\n", f->ff5(1.0, 2.0, 3.0, 4.0, 5.0));
    printf("ff6() : %.2f\n", f->ff6(1.0, 2.0, 3.0, 4.0, 5.0, 6.0));

    printf("fd1() : %.2lf\n", f->fd1(1.0));
    printf("fd2() : %.2lf\n", f->fd2(1.0, 2.0));
    printf("fd3() : %.2lf\n", f->fd3(1.0, 2.0, 3.0));
    printf("fd4() : %.2lf\n", f->fd4(1.0, 2.0, 3.0, 4.0));
    printf("fd5() : %.2lf\n", f->fd5(1.0, 2.0, 3.0, 4.0, 5.0));
    printf("fd6() : %.2lf\n", f->fd6(1.0, 2.0, 3.0, 4.0, 5.0, 6.0));

    printf("fp1() : %zd\n", f->fp1("a"));
    printf("fp2() : %zd\n", f->fp2("a", "bb"));
    printf("fp3() : %zd\n", f->fp3("a", "bb", "ccc"));
    printf("fp4() : %zd\n", f->fp4("a", "bb", "ccc", "dddd"));
    printf("fp5() : %zd\n", f->fp5("a", "bb", "ccc", "dddd", "eeeee"));
    printf("fp6() : %zd\n", f->fp6("a", "bb", "ccc", "dddd", "eeeee", "ffffff"));

    printf("sumi() : %d\n",     f->sumi(10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10));
    printf("suml() : %ld\n",    f->suml(10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10));
    printf("sumf() : %.2f\n",   f->sumf(10, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0));
    printf("sumd() : %.2lf\n",  f->sumd(10, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0));
    printf("sums() : %zd\n",    f->sums(10, "1", "12", "123", "1234", "12345", "123456", "1234567", "12345678", "123456789", "1234567890"));

    return 0;
}