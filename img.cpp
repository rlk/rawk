#include <stdio.h>
#include <fcntl.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>

#include <sstream>

#include "img.hpp"

//------------------------------------------------------------------------------

raw::raw(std::string name, size_t h, size_t w, size_t d, size_t s, bool o)
    : name(name), h(h), w(w), d(d), s(s), f(0), p(0)
{
    size_t n = h * w * d * s;

    int mode = o ? O_RDWR | O_TRUNC | O_CREAT : O_RDONLY;
    int prot = o ? PROT_READ | PROT_WRITE : PROT_READ;

    if ((f = open(name.c_str(), mode, 0666)) == -1)
        throw std::runtime_error(strerror(errno));

    if (o && ftruncate(f, n) == -1)
        throw std::runtime_error(strerror(errno));

    if ((p = mmap(0, n, prot, MAP_SHARED, f, 0)) == MAP_FAILED)
        throw std::runtime_error(strerror(errno));
}

raw::~raw()
{
    size_t n = h * w * d * s;

    if (munmap(p, n) == -1)
        throw std::runtime_error(strerror(errno));

    if (close(f) == -1)
        throw std::runtime_error(strerror(errno));
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
    assert(head);

    const int h = head->geth();
    const int w = head->getw();
    const int d = head->getd();

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
    return head->get(i, j, k);
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
                file->put(i, j, k, head->get(i, j, k));
}

//------------------------------------------------------------------------------

double bias::get(int i, int j, int k) const
{
    return head->get(i, j, k) + value;
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
    return head->get(i, j, k) * value;
}

std::string scale::doc() const
{
    std::ostringstream sout;
    sout << "scale " << value;
    return sout.str();
}

//------------------------------------------------------------------------------

double paste::get(int i, int j, int k) const
{
    if (row <= i && i < row + head->geth() &&
        col <= j && j < col + head->getw())
        return head->get(i - row, j - col, k);
    else
        return tail->get(i, j, k);
}

int paste::geth() const
{
    return std::max(head->geth() + row, tail->geth());
}

int paste::getw() const
{
    return std::max(head->getw() + col, tail->getw());
}

std::string paste::doc() const
{
    std::ostringstream sout;
    sout << "paste " << row
              << " " << col;
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

int test::geth() const
{
    return 1024;
}

int test::getw() const
{
    return 1024;
}

int test::getd() const
{
    return 3;
}

std::string test::doc() const
{
    std::ostringstream sout;
    sout << "test";
    return sout.str();
}

//------------------------------------------------------------------------------
