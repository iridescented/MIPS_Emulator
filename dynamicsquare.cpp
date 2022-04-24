#include <iostream>
#include <Windows.h>

int cache[200];

int mult(int a, int b)
{
    int i = 0, temp = 0;
    for (i = 0; i < b; i++)
        temp += a;
    return temp;
}

int squareOf(int n)
{
    Sleep(10);
    if (n == 0)
        return 0;
    else
    {
        if (!cache[n])
            cache[n] = squareOf(n - 1) + mult(2, n) - 1; //(n-1)^2 + 2n -1
        return (cache[n]);
    }
}
a1 = a1 + a2 - 1;
a2 += 2;

a1 = a1 + a2 - 1;
a1 = a1 + a2 - 1;

int main()
{
    system("cls");
    for (;;)
    {
        int value = 0;
        std::cout << "\nInput value to square: ";
        std::cin >> value;
        std::cout << "\nSquared: " << squareOf(value);
    }
}