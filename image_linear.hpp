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

static inline double lerp(double a, double b, double t)
{
    return b * t + a * (1 - t);
}

//------------------------------------------------------------------------------

/// Linearly-interpolated resampling filter

class linear : public image
{
public:
    linear(int height, int width, int m, image *L)
        : image(L), height(height), width(width), mode(m) { }

    virtual double get(int i, int j, int k) const
    {
        int hh = L->get_height();
        int ww = L->get_width();

        double ii = double(i) * double(hh) / double(height);
        double jj = double(j) * double(ww) / double(width);

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

        return lerp(lerp(aa, ab, t),
                    lerp(ba, bb, t), s);
    }

    virtual int get_height() const { return height; }
    virtual int get_width () const { return width;  }

    virtual void tweak(int a, int v)
    {
        if (a == 0) width -= v;
        if (a == 1) height -= v;
    }

    virtual void doc(std::ostream& out) const
    {
        out << "linear " << height << " " << width;
    }

private:
    int height;
    int width;
    int mode;
};

//------------------------------------------------------------------------------

#endif
