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

#ifndef IMAGE_MEDIAN_HPP
#define IMAGE_MEDIAN_HPP

//------------------------------------------------------------------------------

/// Median filter

class median : public image
{
public:
    /// Find the median over a square of neighboring pixels within *radius*,
    /// wrapped with the given @ref wrap "wrapping mode". This eliminates
    /// outliers and noise such as erroneous black or white pixels (aka salt and
    /// pepper). At high radius, this can be a very expensive filter.

    median(int radius, int mode, image *L)
        : image(L), radius(radius), mode(mode) { }

    virtual double get(int i, int j, int k) const
    {
        const int h = L->get_height();
        const int w = L->get_width ();

        std::vector<double> v((2 * radius + 1) * (2 * radius + 1));

        int z = 0;

        for     (int y = -radius; y <= radius; y++)
            for (int x = -radius; x <= radius; x++)
                if (x * x + y * y <= radius * radius)
                    v[z++] = L->get(wrap(i + y, h, mode & 1),
                                    wrap(j + x, w, mode & 2), k);

        std::nth_element(v.begin(), v.begin() + z / 2, v.begin() + z);
        return v[z / 2];
    }

    virtual void tweak(int a, int v)
    {
        if (a == 0)
        {
            radius += v;
        }
    }

    virtual void doc(std::ostream& out) const
    {
        out << "median " << radius << " " << mode;
    }

protected:
    int radius;
    int mode;
};

//------------------------------------------------------------------------------

/// Median filter with a vertical kernel

class medianv : public median
{
public:
    /// Find the median over a vertical line of neighboring pixels within
    /// *radius*, wrapped with the given @ref wrap "wrapping mode".

    medianv(int radius, int mode, image *L) : median(radius, mode, L) { }

    virtual double get(int i, int j, int k) const
    {
        const int h = L->get_height();

        std::vector<double> v(2 * radius + 1);

        int z = 0;

        for (int y = -radius; y <= radius; y++, z++)
            v[z] = L->get(wrap(i + y, h, mode & 1), j, k);

        std::nth_element(v.begin(), v.begin() + z / 2, v.begin() + z);
        return v[z / 2];
    }

    virtual void doc(std::ostream& out) const
    {
        out << "medianv " << radius;
    }
};

//------------------------------------------------------------------------------

/// Median filter with a horizontal kernel


class medianh : public median
{
public:
    /// Find the median over a horizontal line of neighboring pixels within
    /// *radius*, wrapped with the given @ref wrap "wrapping mode".

    medianh(int radius, int mode, image *L) : median(radius, mode, L) { }

    virtual double get(int i, int j, int k) const
    {
        const int w = L->get_width();

        std::vector<double> v(2 * radius + 1);

        int z = 0;

        for (int x = -radius; x <= radius; x++, z++)
            v[z] = L->get(i, wrap(j + x, w, mode & 2), k);

        std::nth_element(v.begin(), v.begin() + z / 2, v.begin() + z);
        return v[z / 2];
    }

    virtual void doc(std::ostream& out) const
    {
        out << "medianh " << radius;
    }
};

//------------------------------------------------------------------------------

#endif
