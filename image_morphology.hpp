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

#ifndef IMAGE_MORPHOLOGY_HPP
#define IMAGE_MORPHOLOGY_HPP

//------------------------------------------------------------------------------

/// Dilation filter

class dilate : public image
{
public:
    /// Apply a morphological dilate with a circular kernel of the given
    /// *radius* to image *L*, wrapped by the given @ref wrap "mode". The dilate
    /// filter gives the maximum value found among all pixels within the kernel
    /// footprint.

    dilate(int radius, int mode, image *L)
        : image(L), radius(radius), mode(mode) { }

    virtual double get(int i, int j, int k) const
    {
        const int h = L->get_height();
        const int w = L->get_width ();

        double v = std::numeric_limits<double>::min();

        for     (int y = -radius; y <= +radius; y++)
            for (int x = -radius; x <= +radius; x++)

                if (x * x + y * y <= radius * radius)
                    v = std::max(v, L->get(wrap(i + y, h, mode & 1),
                                           wrap(j + x, w, mode & 2), k));
        return v;
    }

    virtual void doc(std::ostream& out) const
    {
        out << "dilate " << radius << " " << mode;
    }

protected:
    int radius;
    int mode;
};

//------------------------------------------------------------------------------

/// Erosion filter

class erode : public image
{
public:
    /// Apply a morphological erode with a circular kernel of the given *radius*
    /// to image *L*, wrapped by the given @ref wrap "mode". The erode filter
    /// gives the minimum value found among all pixels within the kernel
    /// footprint.

    erode(int radius, int mode, image *L)
        : image(L), radius(radius), mode(mode) { }

    virtual double get(int i, int j, int k) const
    {
        const int h = L->get_height();
        const int w = L->get_width ();

        double v = std::numeric_limits<double>::max();

        for     (int y = -radius; y <= +radius; y++)
            for (int x = -radius; x <= +radius; x++)

                if (x * x + y * y <= radius * radius)
                    v = std::min(v, L->get(wrap(i + y, h, mode & 1),
                                           wrap(j + x, w, mode & 2), k));
        return v;
    }

    virtual void doc(std::ostream& out) const
    {
        out << "erode " << radius << " " << mode;
    }

protected:
    int radius;
    int mode;
};

//------------------------------------------------------------------------------

#endif
