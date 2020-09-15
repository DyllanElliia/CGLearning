#include <iostream>
int main()
{
    bool a(int a);
    if(a(0))
        std::cout << int(114514);
    system("pause");
}
bool a(int a)
{
    if (a == 0)
        return 1;
    return 0;
}