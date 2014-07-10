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

#ifndef IMAGE_SOBEL_HPP
#define IMAGE_SOBEL_HPP

//------------------------------------------------------------------------------

/// Horizontal Sobel filter

class sobelx : public image
{
public:
    /// Apply the horizontal 3x3 Sobel filter to image *L*. This will detect
    /// vertical lines in the input. *Mode* gives the @ref wrap "wrap mode".

    sobelx(int mode, image *L) : image(L), mode(mode) { }

    virtual pixel get(int i, int j) const
    {
        const int in = wrap(i - 1, L->get_height(), mode & 1);
        const int is = wrap(i + 1, L->get_height(), mode & 1);
        const int jw = wrap(j - 1, L->get_width (), mode & 2);
        const int je = wrap(j + 1, L->get_width (), mode & 2);

        pixel d1 = L->get(in, jw);
        pixel d3 = L->get(in, je);
        pixel d4 = L->get(i,  jw);
        pixel d6 = L->get(i,  je);
        pixel d7 = L->get(is, jw);
        pixel d9 = L->get(is, je);

        return pixel(d3.r - d1.r + 2.0 * (d6.r - d4.r) + d9.r - d7.r,
                     d3.g - d1.g + 2.0 * (d6.g - d4.g) + d9.g - d7.g,
                     d3.b - d1.b + 2.0 * (d6.b - d4.b) + d9.b - d7.b,
                     d3.a - d1.a + 2.0 * (d6.a - d4.a) + d9.a - d7.a);
    }

    virtual void doc(std::ostream& out) const
    {
        out << "sobelx " << mode;
    }

private:
    int mode;
};

//------------------------------------------------------------------------------

/// Vertical Sobol filter

class sobely : public image
{
public:
    /// Apply the vertical 3x3 Sobel filter to image *L*. This will detect
    /// horizontal lines in the input. *Mode* gives the @ref wrap "wrap mode".

    sobely(int mode, image *L) : image(L), mode(mode) { }

    virtual pixel get(int i, int j) const
    {
        const int in = wrap(i - 1, L->get_height(), mode & 1);
        const int is = wrap(i + 1, L->get_height(), mode & 1);
        const int jw = wrap(j - 1, L->get_width (), mode & 2);
        const int je = wrap(j + 1, L->get_width (), mode & 2);

        pixel d1 = L->get(in, jw);
        pixel d2 = L->get(in, j );
        pixel d3 = L->get(in, je);
        pixel d7 = L->get(is, jw);
        pixel d8 = L->get(is, j );
        pixel d9 = L->get(is, je);

        return pixel(d7.r - d1.r + 2.0 * (d8.r - d2.r) + d9.r - d3.r,
                     d7.g - d1.g + 2.0 * (d8.g - d2.g) + d9.g - d3.g,
                     d7.b - d1.b + 2.0 * (d8.b - d2.b) + d9.b - d3.b,
                     d7.a - d1.a + 2.0 * (d8.a - d2.a) + d9.a - d3.a);
    }

    virtual void doc(std::ostream& out) const
    {
        out << "sobely " << mode;
    }

private:
    int mode;
};

//------------------------------------------------------------------------------

/// Relief shading filter

class relief : public image
{
public:
    /// Show image *L* in shaded relief, with the light coming from direction
    /// (*dy*, *dx*). Higher values for *dx* and *dy* give a more stark relief.
    /// *Mode* gives the @ref "wrap mode".

    relief(double dy, double dx, int mode, image *L)
        : image(L), dy(dy), dx(dx), mode(mode) { }

    virtual pixel get(int i, int j) const
    {
        const int in = wrap(i - 1, L->get_height(), mode & 1);
        const int is = wrap(i + 1, L->get_height(), mode & 1);
        const int jw = wrap(j - 1, L->get_width (), mode & 2);
        const int je = wrap(j + 1, L->get_width (), mode & 2);

        pixel d1 = L->get(in, jw);
        pixel d2 = L->get(in, j );
        pixel d3 = L->get(in, je);
        pixel d4 = L->get(i,  jw);
        pixel d6 = L->get(i,  je);
        pixel d7 = L->get(is, jw);
        pixel d8 = L->get(is, j );
        pixel d9 = L->get(is, je);

        pixel x(d3.r - d1.r + 2.0 * (d6.r - d4.r) + d9.r - d7.r,
                d3.g - d1.g + 2.0 * (d6.g - d4.g) + d9.g - d7.g,
                d3.b - d1.b + 2.0 * (d6.b - d4.b) + d9.b - d7.b,
                d3.a - d1.a + 2.0 * (d6.a - d4.a) + d9.a - d7.a);
        pixel y(d7.r - d1.r + 2.0 * (d8.r - d2.r) + d9.r - d3.r,
                d7.g - d1.g + 2.0 * (d8.g - d2.g) + d9.g - d3.g,
                d7.b - d1.b + 2.0 * (d8.b - d2.b) + d9.b - d3.b,
                d7.a - d1.a + 2.0 * (d8.a - d2.a) + d9.a - d3.a);

        return pixel(0.5 + x.r * dx + y.r * dy,
                     0.5 + x.g * dx + y.g * dy,
                     0.5 + x.b * dx + y.b * dy,
                     0.5 + x.a * dx + y.a * dy);
    }

    virtual void tweak(int a, int v)
    {
        if (a == 0) dx += v;
        if (a == 1) dy += v;
    }

    virtual void doc(std::ostream& out) const
    {
        out << "relief " << dy << " " << dx << " " << mode;
    }

private:
    double dy;
    double dx;
    int mode;
};

//------------------------------------------------------------------------------

/// Gradient filter

class gradient : public image
{
public:
    /// Compute the magnitude of the gradient of image *L*. This is the value of
    /// the greatest rate of change in that image. This behaves as an omni-
    /// directional edge detect. *Mode* gives the @ref wrap "wrap mode".

    gradient(int mode, image *L) : image(L), mode(mode) { }

    virtual pixel get(int i, int j) const
    {
        const int in = wrap(i - 1, L->get_height(), mode & 1);
        const int is = wrap(i + 1, L->get_height(), mode & 1);
        const int jw = wrap(j - 1, L->get_width (), mode & 2);
        const int je = wrap(j + 1, L->get_width (), mode & 2);

        pixel d1 = L->get(in, jw);
        pixel d2 = L->get(in, j );
        pixel d3 = L->get(in, je);
        pixel d4 = L->get(i,  jw);
        pixel d6 = L->get(i,  je);
        pixel d7 = L->get(is, jw);
        pixel d8 = L->get(is, j );
        pixel d9 = L->get(is, je);

        pixel x(d3.r - d1.r + 2.0 * (d6.r - d4.r) + d9.r - d7.r,
                d3.g - d1.g + 2.0 * (d6.g - d4.g) + d9.g - d7.g,
                d3.b - d1.b + 2.0 * (d6.b - d4.b) + d9.b - d7.b,
                d3.a - d1.a + 2.0 * (d6.a - d4.a) + d9.a - d7.a);
        pixel y(d7.r - d1.r + 2.0 * (d8.r - d2.r) + d9.r - d3.r,
                d7.g - d1.g + 2.0 * (d8.g - d2.g) + d9.g - d3.g,
                d7.b - d1.b + 2.0 * (d8.b - d2.b) + d9.b - d3.b,
                d7.a - d1.a + 2.0 * (d8.a - d2.a) + d9.a - d3.a);

        return pixel(sqrtf(x.r * x.r + y.r * y.r),
                     sqrtf(x.g * x.g + y.g * y.g),
                     sqrtf(x.b * x.b + y.b * y.b),
                     sqrtf(x.a * x.a + y.a * y.a));
    }

    virtual void doc(std::ostream& out) const
    {
        out << "gradient " << mode;
    }

private:
    int mode;
};

//------------------------------------------------------------------------------

#endif
