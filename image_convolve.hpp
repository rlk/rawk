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
    /// Convolve image *L* using a kernel of the given *xradius* and *yradius*.
    /// The kernel is expected to be supplied by a subclass.

    convolve(int xradius, int yradius, int mode, image *L)
        : image(L), xradius(xradius), yradius(yradius), mode(mode) { }

    virtual double get(int i, int j, int k) const
    {
        const int h = L->get_height();
        const int w = L->get_width ();

        double s = 0;
        double t = 0;
        double T = 0;

        for     (int y = -xradius; y <= xradius; y++)
            for (int x = -yradius; x <= yradius;  x++)
                if ((s = kernel(y, x)))
                {
                    T += s;
                    t += s * L->get(wrap(i + y, h, mode & 1),
                                    wrap(j + x, w, mode & 2), k);
                }

        return t / T;
    }

protected:
    virtual double kernel(int, int) const = 0;

    int    xradius;
    int    yradius;
    int    mode;
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

    virtual double kernel(int x, int y) const
    {
        return sigma ? exp(-(x * x + y * y) / (2.0 * sigma * sigma)) : 1.0;
    }

    virtual void doc(std::ostream& out) const
    {
        out << "gaussian " << sigma << " " << mode;
    }

private:
    double sigma;
};


//------------------------------------------------------------------------------

#endif
