#include "regional.h"
using namespace region;
#include <iostream>
#include <fstream>
#define stds std::

int main()
{

    regional reg(25);
    reg.readfile();
    reg.bfs();
    reg.out();
    //reg.readout();
    reg.gograph();
    stds cout << "complished" << stds endl;

    return 0;
}