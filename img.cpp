#include <math.h>
#include <stdio.h>
#include <fcntl.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>

#include <stdexcept>
#include <sstream>

#include "img.hpp"

//------------------------------------------------------------------------------

class raw_error : public std::runtime_error
{
public :
    raw_error(const std::string& a,
              const std::string& b) : std::runtime_error(a + ": " + b) { }
};

raw::raw(std::string name, size_t h, size_t w, size_t d, size_t s, bool o)
    : name(name), h(h), w(w), d(d), s(s), f(0), p(0)
{
    size_t n = h * w * d * s;

    int mode = o ? O_RDWR | O_TRUNC | O_CREAT : O_RDONLY;
    int prot = o ? PROT_READ | PROT_WRITE : PROT_READ;

    if ((f = open(name.c_str(), mode, 0666)) == -1)
        throw raw_error(name, strerror(errno));

    if (o && ftruncate(f, n) == -1)
        throw raw_error(name, strerror(errno));

    if ((p = mmap(0, n, prot, MAP_SHARED, f, 0)) == MAP_FAILED)
        throw raw_error(name, strerror(errno));
}

raw::~raw()
{
    size_t n = h * w * d * s;

    if (munmap(p, n) == -1)
        throw raw_error(name, strerror(errno));

    if (close(f) == -1)
        throw raw_error(name, strerror(errno));
}

//------------------------------------------------------------------------------

image::image(image *L, image *R) : L(L), R(R), P(0)
{
    if (L) L->setP(this);
    if (R) R->setP(this);
}

int image::geth() const
{
    if (L && R) return std::max(L->geth(), R->geth());
    else if (L) return L->geth();
    else if (R) return L->geth();
    else        return 0;
}

int image::getw() const
{
    if (L && R) return std::max(L->getw(), R->getw());
    else if (L) return L->getw();
    else if (R) return L->getw();
    else        return 0;
}

int image::getd() const
{
    if (L && R) return std::max(L->getd(), R->getd());
    else if (L) return L->getd();
    else if (R) return L->getd();
    else        return 0;
}

//------------------------------------------------------------------------------

input::input(std::string a, int h, int w, int d, char t) : file(0)
{
    switch (t)
    {
        case 'b': file = new rawb(a, h, w, d, false); break;
        case 'c': file = new rawc(a, h, w, d, false); break;
        case 'u': file = new rawu(a, h, w, d, false); break;
        case 'U': file = new rawU(a, h, w, d, false); break;
        case 's': file = new raws(a, h, w, d, false); break;
        case 'S': file = new rawS(a, h, w, d, false); break;
        case 'l': file = new rawl(a, h, w, d, false); break;
        case 'L': file = new rawL(a, h, w, d, false); break;
        case 'i': file = new rawi(a, h, w, d, false); break;
        case 'I': file = new rawI(a, h, w, d, false); break;
        case 'f': file = new rawf(a, h, w, d, false); break;
        case 'F': file = new rawF(a, h, w, d, false); break;
        case 'd': file = new rawd(a, h, w, d, false); break;
        case 'D': file = new rawD(a, h, w, d, false); break;
    }
}

input::~input()
{
    delete file;
}

double input::get(int i, int j, int k) const
{
    assert(file);

    return (0 <= i && i < file->geth() &&
            0 <= j && j < file->getw() &&
            0 <= k && k < file->getd()) ? file->get(i, j, k) : 0.0;
}

std::string input::doc() const
{
    std::ostringstream sout;
    sout << "input " << file->getname()
              << " " << file->geth()
              << " " << file->getw()
              << " " << file->getd();
    return sout.str();
}

//------------------------------------------------------------------------------

output::output(std::string name, char t, image *H) : image(H), file(0)
{
    assert(L);

    const int h = L->geth();
    const int w = L->getw();
    const int d = L->getd();

    switch (t)
    {
        case 'b': file = new rawb(name, h, w, d, true); break;
        case 'c': file = new rawc(name, h, w, d, true); break;
        case 'u': file = new rawu(name, h, w, d, true); break;
        case 'U': file = new rawU(name, h, w, d, true); break;
        case 's': file = new raws(name, h, w, d, true); break;
        case 'S': file = new rawS(name, h, w, d, true); break;
        case 'l': file = new rawl(name, h, w, d, true); break;
        case 'L': file = new rawL(name, h, w, d, true); break;
        case 'i': file = new rawi(name, h, w, d, true); break;
        case 'I': file = new rawI(name, h, w, d, true); break;
        case 'f': file = new rawf(name, h, w, d, true); break;
        case 'F': file = new rawF(name, h, w, d, true); break;
        case 'd': file = new rawd(name, h, w, d, true); break;
        case 'D': file = new rawD(name, h, w, d, true); break;
    }
}

output::~output()
{
    delete file;
}

double output::get(int i, int j, int k) const
{
    return L->get(i, j, k);
}

std::string output::doc() const
{
    std::ostringstream sout;
    sout << "output " << file->getname()
               << " " << file->geth()
               << " " << file->getw()
               << " " << file->getd();
    return sout.str();
}

void output::go() const
{
    for         (int i = 0; i < geth(); ++i)
        for     (int j = 0; j < getw(); ++j)
            for (int k = 0; k < getd(); ++k)
                file->put(i, j, k, L->get(i, j, k));
}

//------------------------------------------------------------------------------

double bias::get(int i, int j, int k) const
{
    return L->get(i, j, k) + value;
}

void bias::tweak(int a, int v)
{
    if (a == 0) value += 0.1 * v;
}

std::string bias::doc() const
{
    std::ostringstream sout;
    sout << "bias " << value;
    return sout.str();
}

//------------------------------------------------------------------------------

double scale::get(int i, int j, int k) const
{
    return L->get(i, j, k) * value;
}

std::string scale::doc() const
{
    std::ostringstream sout;
    sout << "scale " << value;
    return sout.str();
}

//------------------------------------------------------------------------------

double offset::get(int i, int j, int k) const
{
    const int h = L->geth();
    const int w = L->getw();
    const int d = L->getd();

    if (0 <= i && i < h && 0 <= j && j < w && 0 <= k && k < d)
        return L->get(::wrap(i - row, h, wrap & 1),
                      ::wrap(j - col, w, wrap & 2), k);
    else
        return 0.0;
}

void offset::tweak(int a, int v)
{
    if (a == 0) col += v;
    if (a == 1) row += v;
}

std::string offset::doc() const
{
    std::ostringstream sout;
    sout << "offset " << row
               << " " << col
               << " " << wrap;
    return sout.str();
}

//------------------------------------------------------------------------------

double nearest::get(int i, int j, int k) const
{
    const long long hh = (long long) L->geth();
    const long long ww = (long long) L->getw();

    return L->get(int((long long) i * hh / (long long) h),
                  int((long long) j * ww / (long long) w), k);
}

void nearest::tweak(int a, int v)
{
    if (a == 0) w -= v;
    if (a == 1) h -= v;
}

std::string nearest::doc() const
{
    std::ostringstream sout;
    sout << "nearest " << h
                << " " << w;
    return sout.str();
}

//------------------------------------------------------------------------------

double flatten::get(int i, int j, int k) const
{
    const int h = L->geth() / 2;

    double lat = M_PI_2 * double(h - i) / h;

    double y = sin(lat) * value;
    double x = cos(lat);
    double r = sqrt(x * x + y * y);

    x = x / r;
    y = y / r;

    lat = asin(y);

    i = -h * (lat - M_PI_2) / M_PI_2;

    return L->get(i, j, k);
}

void flatten::tweak(int a, int v)
{
    if (a == 0) value += 0.0001 * v;
}

std::string flatten::doc() const
{
    std::ostringstream sout;
    sout << "flatten " << value;
    return sout.str();
}

//------------------------------------------------------------------------------

double trim::get(int i, int j, int k) const
{
    const int hh = L->geth();
    const int ww = L->getw();

    if (hh > h) i += (hh - h) / 2;
    if (h > hh) i -= (h - hh) / 2;
    if (ww > w) j += (ww - w) / 2;
    if (w > ww) j -= (w - ww) / 2;

    return L->get(i, j, k);
}

void trim::tweak(int a, int v)
{
    if (a == 0) w += v;
    if (a == 1) h += v;
}

std::string trim::doc() const
{
    std::ostringstream sout;
    sout << "trim " << h
             << " " << w;
    return sout.str();
}

//------------------------------------------------------------------------------

double channel::get(int i, int j, int k) const
{
    if (k == index)
        return L->get(i, j, 0);
    else
        return 0.0;
}

std::string channel::doc() const
{
    std::ostringstream sout;
    sout << "channel " << index;
    return sout.str();
}

//------------------------------------------------------------------------------

double paste::get(int i, int j, int k) const
{
    if (row <= i && i < row + L->geth() &&
        col <= j && j < col + L->getw())
        return L->get(i - row, j - col, k);
    else
        return R->get(i, j, k);
}

int paste::geth() const
{
    return std::max(L->geth() + row, R->geth());
}

int paste::getw() const
{
    return std::max(L->getw() + col, R->getw());
}

std::string paste::doc() const
{
    std::ostringstream sout;
    sout << "paste " << row
              << " " << col;
    return sout.str();
}

//------------------------------------------------------------------------------

double sum::get(int i, int j, int k) const
{
    return L->get(i, j, k)
         + R->get(i, j, k);
}

std::string sum::doc() const
{
    std::ostringstream sout;
    sout << "sum";
    return sout.str();
}

//------------------------------------------------------------------------------

double gradient::get(int ic, int jc, int k) const
{
    const int h = L->geth();
    const int w = L->getw();
    const int d = L->getd();

    if (0 <= ic && ic < h && 0 <= jc && jc < w && 0 <= k && k < d)
    {
        const int in = ::wrap(ic - 1, L->geth(), wrap & 1);
        const int is = ::wrap(ic + 1, L->geth(), wrap & 1);
        const int jw = ::wrap(jc - 1, L->getw(), wrap & 2);
        const int je = ::wrap(jc + 1, L->getw(), wrap & 2);

        double d1 = L->get(in, jw, k);
        double d2 = L->get(in, jc, k);
        double d3 = L->get(in, je, k);
        double d4 = L->get(ic, jw, k);

        double d6 = L->get(ic, je, k);
        double d7 = L->get(is, jw, k);
        double d8 = L->get(is, jc, k);
        double d9 = L->get(is, je, k);

        double Lx = d3 - d1 + 2.0 * (d6 - d4) + d9 - d7;
        double Ly = d1 - d7 + 2.0 * (d2 - d8) + d3 - d9;

        return sqrt(Lx * Lx + Ly * Ly);
    }
    return 0.0;
}

std::string gradient::doc() const
{
    std::ostringstream sout;
    sout << "gradient " << wrap;
    return sout.str();
}

//------------------------------------------------------------------------------

double solid::get(int i, int j, int k) const
{
    return value;
}

std::string solid::doc() const
{
    std::ostringstream sout;
    sout << "solid " << h <<
                 " " << w <<
                 " " << value;
    return sout.str();
}

//------------------------------------------------------------------------------

double test::get(int i, int j, int k) const
{
    if (0 <= i && i < geth() && 0 <= j && j < getw() && 0 <= k && k < getd())
    {
        if (((i / (1 << (5 + k))) % 2) == ((j / (1 << (5 + k))) % 2))
            return 1.0;
        else
            return 0.0;
    }
    return 0.0;
}

std::string test::doc() const
{
    std::ostringstream sout;
    sout << "test";
    return sout.str();
}

//------------------------------------------------------------------------------
