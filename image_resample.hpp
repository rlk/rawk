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

#ifndef IMAGE_RESAMPLE_HPP
#define IMAGE_RESAMPLE_HPP

//------------------------------------------------------------------------------

/// Linear interpolation

static inline double lerp(double a, double b, double t)
{
    return b * t + a * (1 - t);
}

/// Cubic interpolation

static inline double cerp(double a, double b, double c, double d, double t)
{
    return b + (-a / 2                 + c / 2        ) * t
             + ( a     - 5 * b / 2 + 2 * c     - d / 2) * t * t
             + (-a / 2 + 3 * b / 2 - 3 * c / 2 + d / 2) * t * t * t;
}

//------------------------------------------------------------------------------

/// Resampling filter base class

class resample : public image
{
public:
    resample(int height, int width, int mode, image *L)
        : image(L), height(height), width(width), mode(mode) { }

    virtual int get_height() const { return height; }
    virtual int get_width () const { return width;  }

    virtual void tweak(int a, int v)
    {
        if (a == 0) width  -= v;
        if (a == 1) height -= v;
    }

protected:
    int height;
    int width;
    int mode;
};

//------------------------------------------------------------------------------

/// Nearest value resampling filter

class nearest : public resample
{
public:
    /// Resample *L* to the given *height* and *width* using nearest-neighbor
    /// sampling. The wrap mode for neast sampling is naturally zero.

    nearest(int height, int width, image *L) : resample(height, width, 0, L) { }

    virtual double get(int i, int j, int k) const
    {
        const long long hh = (long long) L->get_height();
        const long long ww = (long long) L->get_width();

        return L->get(int((long long) i * hh / (long long) height),
                      int((long long) j * ww / (long long) width), k);
    }

    virtual void doc(std::ostream& out) const
    {
        out << "nearest " << height << " " << width;
    }
};

//------------------------------------------------------------------------------

/// Linearly-interpolated resampling filter

class linear : public resample
{
public:
    /// Resample *L* to the given *height* and *width* using linearly-
    /// interpolated sampling. *Mode* gives the @ref wrap "wrap mode".

    linear(int height, int width, int mode, image *L)
        : resample(height, width, mode, L) { }

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

    virtual void doc(std::ostream& out) const
    {
        out << "linear " << height << " " << width << " " << mode;
    }
};

//------------------------------------------------------------------------------

/// Cubic-interpolated resampling filter

class cubic : public resample
{
public:
    /// Resample *L* to the given *height* and *width* using cubic-interpolated
    /// sampling. *Mode* gives the @ref wrap "wrap mode".

    cubic(int height, int width, int mode, image *L)
        : resample(height, width, mode, L) { }

    virtual double get(int i, int j, int k) const
    {
        int hh = L->get_height();
        int ww = L->get_width();

        double ii = double(i) * double(hh) / double(height);
        double jj = double(j) * double(ww) / double(width);

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

        return cerp(cerp(aa, ab, ac, ad, t),
                    cerp(ba, bb, bc, bd, t),
                    cerp(ca, cb, cc, cd, t),
                    cerp(da, db, dc, dd, t), s);
    }

    virtual void doc(std::ostream& out) const
    {
        out << "cubic " << height << " " << width << " " << mode;
    }
};

//------------------------------------------------------------------------------

#endif
