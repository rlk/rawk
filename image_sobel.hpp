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

    virtual double get(int i, int j, int k) const
    {
        const int in = wrap(i - 1, L->get_height(), mode & 1);
        const int is = wrap(i + 1, L->get_height(), mode & 1);
        const int jw = wrap(j - 1, L->get_width (), mode & 2);
        const int je = wrap(j + 1, L->get_width (), mode & 2);

        double d1 = L->get(in, jw, k);
        double d3 = L->get(in, je, k);
        double d4 = L->get(i,  jw, k);
        double d6 = L->get(i,  je, k);
        double d7 = L->get(is, jw, k);
        double d9 = L->get(is, je, k);

        return d3 - d1 + 2.0 * (d6 - d4) + d9 - d7;
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

    virtual double get(int i, int j, int k) const
    {
        const int in = wrap(i - 1, L->get_height(), mode & 1);
        const int is = wrap(i + 1, L->get_height(), mode & 1);
        const int jw = wrap(j - 1, L->get_width (), mode & 2);
        const int je = wrap(j + 1, L->get_width (), mode & 2);

        double d1 = L->get(in, jw, k);
        double d2 = L->get(in, j,  k);
        double d3 = L->get(in, je, k);
        double d7 = L->get(is, jw, k);
        double d8 = L->get(is, j,  k);
        double d9 = L->get(is, je, k);

        return d7 - d1 + 2.0 * (d8 - d2) + d9 - d3;
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
    relief(double dy, double dx, int mode, image *L)
        : image(L), dy(dy), dx(dx), mode(mode) { }

    virtual double get(int i, int j, int k) const
    {
        const int in = wrap(i - 1, L->get_height(), mode & 1);
        const int is = wrap(i + 1, L->get_height(), mode & 1);
        const int jw = wrap(j - 1, L->get_width (), mode & 2);
        const int je = wrap(j + 1, L->get_width (), mode & 2);

        double d1 = L->get(in, jw, k);
        double d2 = L->get(in, j,  k);
        double d3 = L->get(in, je, k);
        double d4 = L->get(i,  jw, k);
        double d6 = L->get(i,  je, k);
        double d7 = L->get(is, jw, k);
        double d8 = L->get(is, j,  k);
        double d9 = L->get(is, je, k);

        double Ly = d7 - d1 + 2.0 * (d8 - d2) + d9 - d3;
        double Lx = d3 - d1 + 2.0 * (d6 - d4) + d9 - d7;

        return 0.5 + Ly * dy + Lx * dx;
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

    virtual double get(int i, int j, int k) const
    {
        const int in = wrap(i - 1, L->get_height(), mode & 1);
        const int is = wrap(i + 1, L->get_height(), mode & 1);
        const int jw = wrap(j - 1, L->get_width (), mode & 2);
        const int je = wrap(j + 1, L->get_width (), mode & 2);

        double d1 = L->get(in, jw, k);
        double d2 = L->get(in, j,  k);
        double d3 = L->get(in, je, k);
        double d4 = L->get(i,  jw, k);
        double d6 = L->get(i,  je, k);
        double d7 = L->get(is, jw, k);
        double d8 = L->get(is, j,  k);
        double d9 = L->get(is, je, k);

        double Lx = d3 - d1 + 2.0 * (d6 - d4) + d9 - d7;
        double Ly = d7 - d1 + 2.0 * (d8 - d2) + d9 - d3;

        return sqrt(Lx * Lx + Ly * Ly);
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
