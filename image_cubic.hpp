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

#ifndef IMAGE_CUBIC_HPP
#define IMAGE_CUBIC_HPP

//------------------------------------------------------------------------------

/// Nearest filter

class cubic : public image
{
public:
    cubic(int h, int w, int m, image *L) : image(L), h(h), w(w), mode(m) { }

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

static inline double interp(double a, double b, double c, double d, double t)
{
    return b + (-a / 2                 + c / 2        ) * t
             + ( a     - 5 * b / 2 + 2 * c     - d / 2) * t * t
             + (-a / 2 + 3 * b / 2 - 3 * c / 2 + d / 2) * t * t * t;
}

double cubic::get(int i, int j, int k) const
{
    int hh = L->geth();
    int ww = L->getw();

    double ii = double(i) * double(hh) / double(h);
    double jj = double(j) * double(ww) / double(w);

    double s = ii - floor(ii);
    double t = jj - floor(jj);

    int ib = wrap(int(floor(ii)), hh, mode & 1);
    int ic = wrap(int( ceil(ii)), hh, mode & 1);
    int jb = wrap(int(floor(jj)), ww, mode & 2);
    int jc = wrap(int( ceil(jj)), ww, mode & 2);

    int ia = wrap(ib - 1, hh, mode & 1);
    int id = wrap(ic + 1, hh, mode & 1);
    int ja = wrap(jb - 1, ww, mode & 2);
    int jd = wrap(jc + 1, ww, mode & 2);

    double aa = L->get(ia, ja, k);
    double ab = L->get(ia, jb, k);
    double ac = L->get(ia, jc, k);
    double ad = L->get(ia, jd, k);
    double ba = L->get(ib, ja, k);
    double bb = L->get(ib, jb, k);
    double bc = L->get(ib, jc, k);
    double bd = L->get(ib, jd, k);
    double ca = L->get(ic, ja, k);
    double cb = L->get(ic, jb, k);
    double cc = L->get(ic, jc, k);
    double cd = L->get(ic, jd, k);
    double da = L->get(id, ja, k);
    double db = L->get(id, jb, k);
    double dc = L->get(id, jc, k);
    double dd = L->get(id, jd, k);

    return interp(interp(aa, ab, ac, ad, t),
                  interp(ba, bb, bc, bd, t),
                  interp(ca, cb, cc, cd, t),
                  interp(da, db, dc, dd, t), s);
}

void cubic::tweak(int a, int v)
{
    if (a == 0) w -= v;
    if (a == 1) h -= v;
}

std::string cubic::doc() const
{
    std::ostringstream sout;
    sout << "cubic " << h
              << " " << w;
    return sout.str();
}

//------------------------------------------------------------------------------

#endif
