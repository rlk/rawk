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

#ifndef IMAGE_CONVOLVE_HPP
#define IMAGE_CONVOLVE_HPP

//------------------------------------------------------------------------------

/// Convolution filter

class convolve : public image
{
public:
    /// Convolve image *L* using a kernel of the given *yradius* and *xradius*.
    /// The kernel is expected to be supplied by a subclass.

    convolve(int yradius, int xradius, int mode, image *L)
        : image(L), yradius(yradius), xradius(xradius), mode(mode) { }

    virtual pixel get(int i, int j) const
    {
        const int h = L->get_height();
        const int w = L->get_width ();

        double t = 0;
        pixel  p;

        for     (int y = -yradius; y <= yradius; y++)
            for (int x = -xradius; x <= xradius; x++)
                if (double s = kernel(y, x))
                {
                    pixel q = L->get(wrap(i + y, h, mode & 1),
                                     wrap(j + x, w, mode & 2));
                    t   += s;
                    p.r += s * q.r;
                    p.g += s * q.g;
                    p.b += s * q.b;
                    p.a += s * q.a;
                }

        return pixel(p.r / t, p.g / t, p.b / t, p.a / t);
    }

protected:
    virtual double kernel(int, int) const = 0;

    int yradius;
    int xradius;
    int mode;
};

//------------------------------------------------------------------------------

/// Gaussian convolution filter

class gaussian : public convolve
{
public:
    /// Convolve image *L* using a Gaussian kernel with standard deviation
    /// *sigma*. *Mode* gives the @ref wrap "wrapping mode". The true radius of
    /// the kernel is 3 sigma, rounded up.

    gaussian(double sigma, int mode, image *L)
        : convolve(int(ceil(sigma * 3)),
                   int(ceil(sigma * 3)), mode, L), sigma(sigma) { }

    virtual void tweak(int a, int v)
    {
        if (a == 0)
        {
            sigma  += v;
            xradius = int(ceil(sigma * 3));
            yradius = int(ceil(sigma * 3));
        }
    }

    virtual double kernel(int y, int x) const
    {
        return sigma ? exp(-0.5 * (x * x + y * y) / (sigma * sigma)) : 1.0;
    }

    virtual void doc(std::ostream& out) const
    {
        out << "gaussian " << sigma << " " << mode;
    }

private:
    double sigma;
};

//------------------------------------------------------------------------------

/// Vertical Gaussian convolution filter

class gaussianv : public convolve
{
public:
    /// Convolve image *L* using a vertical 1D Gaussian kernel with standard
    /// deviation *sigma*. *Mode* gives the @ref wrap "wrapping mode". The true
    /// radius of the kernel is 3 sigma, rounded up.

    gaussianv(double sigma, int mode, image *L)
        : convolve(int(ceil(sigma * 3)), 0, mode, L), sigma(sigma) { }

    virtual void tweak(int a, int v)
    {
        if (a == 0)
        {
            sigma  += v;
            yradius = int(ceil(sigma * 3));
        }
    }

    virtual double kernel(int y, int x) const
    {
        return sigma ? exp(-0.5 * (y * y) / (sigma * sigma)) : 1.0;
    }

    virtual void doc(std::ostream& out) const
    {
        out << "gaussianv " << sigma << " " << mode;
    }

private:
    double sigma;
};

//------------------------------------------------------------------------------

/// Horizontal Gaussian convolution filter

class gaussianh : public convolve
{
public:
    /// Convolve image *L* using a horizontal 1D Gaussian kernel with standard
    /// deviation *sigma*. *Mode* gives the @ref wrap "wrapping mode". The true
    /// radius of the kernel is 3 sigma, rounded up.

    gaussianh(double sigma, int mode, image *L)
        : convolve(0, int(ceil(sigma * 3)), mode, L), sigma(sigma) { }

    virtual void tweak(int a, int v)
    {
        if (a == 0)
        {
            sigma  += v;
            xradius = int(ceil(sigma * 3));
        }
    }

    virtual double kernel(int y, int x) const
    {
        return sigma ? exp(-0.5 * (x * x) / (sigma * sigma)) : 1.0;
    }

    virtual void doc(std::ostream& out) const
    {
        out << "gaussianh " << sigma << " " << mode;
    }

private:
    double sigma;
};

//------------------------------------------------------------------------------

#endif
