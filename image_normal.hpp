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

#ifndef IMAGE_NORMALIZE_HPP
#define IMAGE_NORMALIZE_HPP

//------------------------------------------------------------------------------

/// Vector renormalization filter

class normalize : public image
{
public:
    /// Renormalize a normal map.

    normalize(image *L) : image(L) { }

    virtual pixel get(int i, int j) const
    {
        pixel p = L->get(i, j);

        float k = sqrtf(p.r * p.r + p.g * p.g + p.b * p.b);

        return pixel(p.r / k, p.g / k, p.b / k, p.a);
    }

    virtual void doc(std::ostream& out) const
    {
        out << "normalize";
    }
};

//------------------------------------------------------------------------------

/// Normal map generator

class normalmap : public image
{
public:
    /// Generate an object-space normal map for the sphere with height map *L*
    /// and the given *radius*. *L* is presumed to cover the full sphere with
    /// equirectangular projection.

    normalmap(double radius, image *L) : image(L), radius(radius) { }

    virtual pixel get(int i, int j) const
    {
#if 0
        const int h = L->get_height();
        const int w = L->get_width ();

        const int in = wrap(i - 1, h, mode & 1);
        const int is = wrap(i + 1, h, mode & 1);
        const int jw = wrap(j - 1, w, mode & 2);
        const int je = wrap(j + 1, w, mode & 2);

        double dn = L->get(in, j,  k);
        double ds = L->get(is, j,  k);
        double dw = L->get(i, jw,  k);
        double de = L->get(i, je,  k);
#endif
        return L->get(i, j);
    }

    virtual void tweak(int a, int v)
    {
        if (a == 0) radius += v;
    }

    virtual void doc(std::ostream& out) const
    {
        out << "normalmap " << radius;
    }

private:
    double radius;
};

//------------------------------------------------------------------------------

#endif
