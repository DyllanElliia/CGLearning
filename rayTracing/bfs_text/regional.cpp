#include "regional.h"
#include <iostream>
#include <fstream>
#include <queue>
#include <set>
using namespace std;
using namespace region;

regional::regional(const size_t delt)
    : _delt(delt)
{
    memset(reset, false, sizeof reset);//初始化
}

void regional::readfile()
{
    ifstream infile;
    infile.open("image.txt");
    if (!infile.is_open())
        cerr << "open failed" << endl;
    int x, y;
    infile >> x >> y;
    for (int i = 1; i <= x; ++i)
        for (int j = 1; j <= y; ++j)
            infile >> _img[i][j];
    infile.close();
}

void regional::bfs()
{
    queue<pos> Qcurrent;    //创建队列 Qcurrent
    set<pos> Qnew;          //创建有序集合 Qnew
    Qnew.insert(pos(1, 1)); //从左上角开始生长
    while (Qnew.size())
    {
        Qcurrent.push(*Qnew.begin());
        Qnew.erase(Qnew.begin());
        if (reset[Qcurrent.front()._x][Qcurrent.front()._y]) //该种子点已经访问过
        {
            Qcurrent.pop();
            continue;
        }
        while (Qcurrent.size())
        {
            pos seed = Qcurrent.front();
            reset[seed._x][seed._y] = true;
            Qcurrent.pop();
            for (int trans = 0; trans < 8; ++trans)
            {
                pos p(seed._x + dir[trans][0], seed._y + dir[trans][1]);
                if (p._x > 0 && p._x <= H && p._y > 0 && p._y <= L && !reset[p._x][p._y])
                    if (abs(_img[p._x][p._y] - _img[seed._x][seed._y]) < _delt)
                    {
                        _img[p._x][p._y] = _img[seed._x][seed._y];
                        reset[p._x][p._y] = true;
                        Qcurrent.push(p);
                    }
                    else
                        Qnew.insert(p);
            }
        }
    }
}

void regional::out()
{
    ofstream outfile;
    outfile.open("outall.txt");
    if (!outfile.is_open())
        cerr << "open failed" << endl;

    for (int i = 1; i <= H; ++i)
    {
        for (int j = 1; j <= L; ++j)
            outfile << _img[i][j] << " ";
        outfile << endl;
    }

    outfile.close();
}

void regional::readout()
{
    ifstream infile("outall.txt");
    if (!infile.is_open())
    {
        cerr << "error open" << endl;
    }

    for (int i = 1; i <= H; ++i)
        for (int j = 1; j <= L; ++j)
            infile >> _img[i][j];

    infile.close();
}

void regional::gograph() const
{
    ofstream file;

    auto left = [&](int cnt) {
        for (int i = (cnt - 1) * 700 + 1; i <= 700 * cnt; ++i)
            for (int j = 1; j <= 1000; ++j)
                file << (int)((0.2 + float(_img[i][j] % 10) / 10) * _img[i][j])
                     << " " << (int)((0.5 + float(_img[i][j] % 10) / 10) * _img[i][j])
                     << " " << (int)((0.7 + float(_img[i][j] % 10) / 10) * _img[i][j]) << endl;
    };

    auto right = [&](int cnt) {
        for (int i = (cnt - 1) * 700 + 1; i <= 700 * cnt; ++i)
            for (int j = L - 1000 + 1; j <= L; ++j)
                file << (int)((0.2 + float(_img[i][j] % 10) / 10) * _img[i][j])
                     << " " << (int)((0.5 + float(_img[i][j] % 10) / 10) * _img[i][j])
                     << " " << (int)((0.7 + float(_img[i][j] % 10) / 10) * _img[i][j]) << endl;
    };

    file.open("slip1'.ppm");
    file << "P3" << endl;
    file << 1000 << " " << 700 << endl;
    file << "255" << endl;
    left(1);
    file.close();

    file.open("slip2'.ppm");
    file << "P3" << endl;
    file << 1000 << " " << 700 << endl;
    file << "255" << endl;
    right(1);
    file.close();

    file.open("slip3'.ppm");
    file << "P3" << endl;
    file << 1000 << " " << 700 << endl;
    file << "255" << endl;
    left(2);
    file.close();

    file.open("slip4'.ppm");
    file << "P3" << endl;
    file << 1000 << " " << 700 << endl;
    file << "255" << endl;
    right(2);
    file.close();

    file.open("slip5'.ppm");
    file << "P3" << endl;
    file << 1000 << " " << 700 << endl;
    file << "255" << endl;
    left(3);
    file.close();

    file.open("slip6'.ppm");
    file << "P3" << endl;
    file << 1000 << " " << 700 << endl;
    file << "255" << endl;
    right(3);
    file.close();
}