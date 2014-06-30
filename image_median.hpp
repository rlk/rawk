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
    median(int radius, int mode, image *L)
        : image(L), radius(radius), mode(mode) { }

    virtual double get(int i, int j, int k) const
    {
        const int h = L->get_height();
        const int w = L->get_height();

        std::vector<double> v((2 * radius + 1) * (2 * radius + 1));

        for     (int ii = i - radius; ii <= i + radius; ii++)
            for (int jj = j - radius; jj <= j + radius; jj++)

                v.push_back(L->get(int(wrap(ii, h, mode & 1)),
                                   int(wrap(jj, w, mode & 2)), k));

        std::nth_element(v.begin(), v.begin() + v.size() / 2, v.end());
        return v[v.size() / 2];
    }

    virtual void tweak(int a, int v)
    {
        if (a == 0) radius += v;
    }

    virtual std::string doc() const
    {
        std::ostringstream out;
        out << "median " << radius;
        return out.str();
    }

protected:
    int radius;
    int mode;
};

/// Median filter with a vertical kernel

class medianv : public median
{
public:

    medianv(int radius, int mode, image *L) : median(radius, mode, L) { }

    virtual double get(int i, int j, int k) const
    {
        const int h = L->get_height();

        std::vector<double> v(2 * radius + 1);

        for (int ii = i - radius; ii <= i + radius; ii++)
            v.push_back(L->get(int(wrap(ii, h, mode & 1)), j, k));

        std::nth_element(v.begin(), v.begin() + v.size() / 2, v.end());
        return v[v.size() / 2];
    }

    virtual std::string doc() const
    {
        std::ostringstream out;
        out << "medianv " << radius;
        return out.str();
    }
};

/// Median filter with a horizontal kernel

class medianh : public median
{
public:

    medianh(int radius, int mode, image *L) : median(radius, mode, L) { }

    virtual double get(int i, int j, int k) const
    {
        const int w = L->get_width();

        std::vector<double> v(2 * radius + 1);

        for (int jj = j - radius; jj <= j + radius; jj++)
            v.push_back(L->get(i, int(wrap(jj, w, mode & 1)), k));

        std::nth_element(v.begin(), v.begin() + v.size() / 2, v.end());
        return v[v.size() / 2];
    }

    virtual std::string doc() const
    {
        std::ostringstream out;
        out << "medianh " << radius;
        return out.str();
    }
};

//------------------------------------------------------------------------------

#endif
