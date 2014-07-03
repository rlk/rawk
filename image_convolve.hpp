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
    /// Convolve image *L* using a kernel of the given *radius*. While storage
    /// for the kernel is allocated here, the value of the kernel is expected to
    /// be supplied by a subclass.

    convolve(int radius, int mode, image *L)
        : image(L), radius(radius), mode(mode), total(0), kernel((2 * radius + 1)
                                                               * (2 * radius + 1)) { }

    virtual double get(int i, int j, int k) const
    {
        const int h = L->get_height();
        const int w = L->get_width ();

        double t = 0;
        int    z = 0;

        for     (int y = -radius; y <= +radius; y++)
            for (int x = -radius; x <= +radius; x++, z++)

            t += kernel[z] * L->get(wrap(i + y, h, mode & 1),
                                    wrap(j + x, w, mode & 2), k);


        return t / total;
    }

protected:
    int    radius;
    int    mode;
    double total;
    std::vector<double> kernel;
};

//------------------------------------------------------------------------------

/// Gaussian convolution filter

class gaussian : public convolve
{
public:
    /// Convolve image *L* using a Gaussian kernel with standard deviation
    /// *sigma*. *Mode* gives the @ref wrap "wrapping mode". The true radius of
    /// the kernel is 3 sigma, rounded up.

    gaussian(double sigma, int mode, image *L) : convolve(0, mode, L), sigma(sigma)
    {
        update();
    }

    virtual void tweak(int a, int v)
    {
        if (a == 0)
        {
            sigma += v;
            update();
        }
    }

    void update()
    {
        radius = int(ceil(3 * sigma));
        total  = 0;

        kernel.reserve((2 * radius + 1) * (2 * radius + 1));

        int z = 0;

        for     (int y = -radius; y <= +radius; y++)
            for (int x = -radius; x <= +radius; x++, z++)
            {
                double t = exp(-(x * x + y * y) / (2.0 * sigma * sigma));
                kernel[z] = t;
                total    += t;
            }
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
