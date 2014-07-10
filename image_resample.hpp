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

static inline pixel lerpv(pixel a, pixel b, double t)
{
    return pixel(lerp(a.r, b.r, t),
                 lerp(a.g, b.g, t),
                 lerp(a.b, b.b, t),
                 lerp(a.a, b.a, t));
}

/// Cubic interpolation

static inline double cerp(double a, double b, double c, double d, double t)
{
    return b + (-a / 2                 + c / 2        ) * t
             + ( a     - 5 * b / 2 + 2 * c     - d / 2) * t * t
             + (-a / 2 + 3 * b / 2 - 3 * c / 2 + d / 2) * t * t * t;
}

static inline pixel cerpv(pixel a, pixel b, pixel c, pixel d, double t)
{
    return pixel(cerp(a.r, b.r, c.r, d.r, t),
                 cerp(a.g, b.g, c.g, d.g, t),
                 cerp(a.b, b.b, c.b, d.b, t),
                 cerp(a.a, b.a, c.a, d.a, t));
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

    virtual pixel get(int i, int j) const
    {
        const long long hh = (long long) L->get_height();
        const long long ww = (long long) L->get_width();

        return L->get(int((long long) i * hh / (long long) height),
                      int((long long) j * ww / (long long) width));
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

    virtual pixel get(int i, int j) const
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

        pixel aa = L->get(ia, ja);
        pixel ab = L->get(ia, jb);
        pixel ba = L->get(ib, ja);
        pixel bb = L->get(ib, jb);

        return lerpv(lerpv(aa, ab, t),
                     lerpv(ba, bb, t), s);
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

    virtual pixel get(int i, int j) const
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

        pixel aa = L->get(ia, ja);
        pixel ab = L->get(ia, jb);
        pixel ac = L->get(ia, jc);
        pixel ad = L->get(ia, jd);
        pixel ba = L->get(ib, ja);
        pixel bb = L->get(ib, jb);
        pixel bc = L->get(ib, jc);
        pixel bd = L->get(ib, jd);
        pixel ca = L->get(ic, ja);
        pixel cb = L->get(ic, jb);
        pixel cc = L->get(ic, jc);
        pixel cd = L->get(ic, jd);
        pixel da = L->get(id, ja);
        pixel db = L->get(id, jb);
        pixel dc = L->get(id, jc);
        pixel dd = L->get(id, jd);

        return cerpv(cerpv(aa, ab, ac, ad, t),
                     cerpv(ba, bb, bc, bd, t),
                     cerpv(ca, cb, cc, cd, t),
                     cerpv(da, db, dc, dd, t), s);
    }

    virtual void doc(std::ostream& out) const
    {
        out << "cubic " << height << " " << width << " " << mode;
    }
};

//------------------------------------------------------------------------------

#endif
