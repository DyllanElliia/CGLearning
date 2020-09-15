//regional.h

#pragma once

namespace region
{
constexpr int dir[8][2]{
    {-1, -1},
    {-1, 0},
    {-1, 1},
    {0, -1},
    {0, 1},
    {1, -1},
    {1, 0},
    {1, 1}};

constexpr size_t H = 2012;
constexpr size_t L = 1881;

class regional
{
public:
    struct pos
    {
        int _x, _y;
        pos(const int a, const int b) : _x(a), _y(b) {}
        bool operator<(const pos &p) const
        {
            if (_x == p._x)
                return _y < p._y;
            return _x < p._x;
        }
    };

public:
    regional(const size_t delt);

    void readfile();

    void bfs();

    void out();

    void readout();

    void gograph() const;

private:
    short _img[2012 + 1][1881 + 1];

    bool reset[H + 1][L + 1];

    size_t _delt;
};

} // namespace region