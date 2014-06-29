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

//------------------------------------------------------------------------------

/// Image file writer

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

//------------------------------------------------------------------------------

output::output(std::string name, char t, image *H) : image(H), file(0)
{
    const int h = L->geth();
    const int w = L->getw();
    const int d = L->getd();

    switch (t)
    {
        case 'b': file = new rawb(name, 0, h, w, d, true); break;
        case 'c': file = new rawc(name, 0, h, w, d, true); break;
        case 'u': file = new rawu(name, 0, h, w, d, true); break;
        case 'U': file = new rawU(name, 0, h, w, d, true); break;
        case 's': file = new raws(name, 0, h, w, d, true); break;
        case 'S': file = new rawS(name, 0, h, w, d, true); break;
        case 'l': file = new rawl(name, 0, h, w, d, true); break;
        case 'L': file = new rawL(name, 0, h, w, d, true); break;
        case 'i': file = new rawi(name, 0, h, w, d, true); break;
        case 'I': file = new rawI(name, 0, h, w, d, true); break;
        case 'f': file = new rawf(name, 0, h, w, d, true); break;
        case 'F': file = new rawF(name, 0, h, w, d, true); break;
        case 'd': file = new rawd(name, 0, h, w, d, true); break;
        case 'D': file = new rawD(name, 0, h, w, d, true); break;
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
 	int h = geth();
 	int w = getw();
 	int d = getd();
	int i;
	int j;
	int k;

	#pragma omp parallel for private(j, k)
    for         (i = 0; i < h; ++i)
        for     (j = 0; j < w; ++j)
            for (k = 0; k < d; ++k)
                file->put(i, j, k, L->get(i, j, k));
}

//------------------------------------------------------------------------------

#endif
