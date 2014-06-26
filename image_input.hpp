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

#ifndef IMAGE_INPUT_HPP
#define IMAGE_INPUT_HPP

//------------------------------------------------------------------------------

/// RAW file reader

class input : public image
{
public:
    input(std::string, int, int, int, int, char);
   ~input();

    virtual double get(int, int, int) const;

    virtual int geth() const { return file->geth(); }
    virtual int getw() const { return file->getw(); }
    virtual int getd() const { return file->getd(); }

    virtual std::string doc() const;

private:
    raw *file;
};

input::input(std::string a, int o, int h, int w, int d, char t) : file(0)
{
    switch (t)
    {
        case 'b': file = new rawb(a, o, h, w, d, false); break;
        case 'c': file = new rawc(a, o, h, w, d, false); break;
        case 'u': file = new rawu(a, o, h, w, d, false); break;
        case 'U': file = new rawU(a, o, h, w, d, false); break;
        case 's': file = new raws(a, o, h, w, d, false); break;
        case 'S': file = new rawS(a, o, h, w, d, false); break;
        case 'l': file = new rawl(a, o, h, w, d, false); break;
        case 'L': file = new rawL(a, o, h, w, d, false); break;
        case 'i': file = new rawi(a, o, h, w, d, false); break;
        case 'I': file = new rawI(a, o, h, w, d, false); break;
        case 'f': file = new rawf(a, o, h, w, d, false); break;
        case 'F': file = new rawF(a, o, h, w, d, false); break;
        case 'd': file = new rawd(a, o, h, w, d, false); break;
        case 'D': file = new rawD(a, o, h, w, d, false); break;
    }
}

input::~input()
{
    delete file;
}

double input::get(int i, int j, int k) const
{
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

#endif
