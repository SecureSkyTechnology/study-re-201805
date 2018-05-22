int __declspec(dllexport) foo(int a, int b)
{
    return a + b;
}
int __declspec(dllexport) bar(int a, int b)
{
    return a * b;
}
int baz(int a, int b)
{
    return a - b;
}