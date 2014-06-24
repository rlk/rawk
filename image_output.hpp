// RAWK Copyright (C) 2014 Robert Kooima
//
// This program is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful, but WITH-
// OUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
// more details.

#ifndef IMAGE_OUTPUT_HPP
#define IMAGE_OUTPUT_HPP

#include "image.hpp"
#include "raw.hpp"

//------------------------------------------------------------------------------

/// RAW file writer with pass-through

class output : public image
{
public:
    output(std::string, char, image *);
   ~output();

    virtual double get(int, int, int) const;

    virtual std::string doc() const;

    void go() const;

private:
    raw *file;
};

output::output(std::string name, char t, image *H) : image(H), file(0)
{
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

#endif
