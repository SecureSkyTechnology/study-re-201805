#include <stdio.h>

void normal(int vi, long vl, float vf, double vd, char *vs)
{
    printf("normal(%d, %ld, %.2f, %.2lf, %s\n", vi, vl, vf, vd, vs);
    return;
}
 
__declspec(naked) void naked(int vi, long vl, float vf, double vd, char *vs)
{
    /* see : https://docs.microsoft.com/ja-jp/cpp/cpp/considerations-for-writing-prolog-epilog-code */

    /* custom prolog */
    __asm {
        push    ebp
        mov     ebp, esp
        sub     esp, __LOCAL_SIZE
    }

    printf("naked(%d, %ld, %.2f, %.2lf, %s\n", vi, vl, vf, vd, vs);

    /* custom epilog */
    __asm {
        mov     esp, ebp
        pop     ebp
        ret
    }
}

int main(int argc, char *argv[]) {
    normal(1, 2, 3.4, 5.67, "foo");
    naked(1, 2, 3.4, 5.67, "bar");
    return 0;
}
