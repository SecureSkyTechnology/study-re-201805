#include <stdio.h>

void normal(int vi, long vl, float vf, double vd, char *vs)
{
    printf("normal(%d, %ld, %.2f, %.2lf, %s\n", vi, vl, vf, vd, vs);
    return;
}
 
__declspec(naked) void naked(int vi, long vl, float vf, double vd, char *vs)
{
    printf("naked(%d, %ld, %.2f, %.2lf, %s\n", vi, vl, vf, vd, vs);
    // C2490, ""'return' not allowed in function with 'naked' attribute"
    // return;
}

int main(int argc, char *argv[]) {
    normal(1, 2, 3.4, 5.67, "foo");
    naked(1, 2, 3.4, 5.67, "bar"); // program abort X(
    return 0;
}