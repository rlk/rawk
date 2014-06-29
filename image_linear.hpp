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

#ifndef IMAGE_LINEAR_HPP
#define IMAGE_LINEAR_HPP

//------------------------------------------------------------------------------

/// Linearly-interpolated resampling filter

class linear : public image
{
public:
    linear(int h, int w, int m, image *L) : image(L), h(h), w(w), mode(m) { }

    virtual double get(int, int, int) const;
    virtual void tweak(int, int);

    virtual int geth() const { return h; }
    virtual int getw() const { return w; }

    virtual std::string doc() const;

private:
    int h;
    int w;
    int mode;
};

//------------------------------------------------------------------------------

static inline double interp(double a, double b, double t)
{
    return b * t + a * (1 - t);
}

double linear::get(int i, int j, int k) const
{
    int hh = L->geth();
    int ww = L->getw();

    double ii = double(i) * double(hh) / double(h);
    double jj = double(j) * double(ww) / double(w);

    double s = ii - floor(ii);
    double t = jj - floor(jj);

    int ia = wrap(int(floor(ii)), hh, mode & 1);
    int ib = wrap(int( ceil(ii)), hh, mode & 1);
    int ja = wrap(int(floor(jj)), ww, mode & 2);
    int jb = wrap(int( ceil(jj)), ww, mode & 2);

    double aa = L->get(ia, ja, k);
    double ab = L->get(ia, jb, k);
    double ba = L->get(ib, ja, k);
    double bb = L->get(ib, jb, k);

    return interp(interp(aa, ab, t),
                  interp(ba, bb, t), s);
}

void linear::tweak(int a, int v)
{
    if (a == 0) w -= v;
    if (a == 1) h -= v;
}

std::string linear::doc() const
{
    std::ostringstream sout;
    sout << "linear " << h
                << " " << w;
    return sout.str();
}

//------------------------------------------------------------------------------

#endif
