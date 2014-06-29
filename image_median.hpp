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
    median(int r, int m, image *L) : image(L), rad(r), mode(m) { }

    virtual double get(int, int, int) const;
    virtual void tweak(int, int);

    virtual std::string doc() const;

protected:
    int rad;
    int mode;
};

/// Median filter with a vertical kernel

class medianv : public median
{
public:

    medianv(int r, int m, image *L) : median(r, m, L) { }

    virtual double get(int, int, int) const;

    virtual std::string doc() const;
};

/// Median filter with a horizontal kernel

class medianh : public median
{
public:

    medianh(int r, int m, image *L) : median(r, m, L) { }

    virtual double get(int, int, int) const;

    virtual std::string doc() const;
};

//------------------------------------------------------------------------------

double median::get(int i, int j, int k) const
{
    const int h = L->geth();
    const int w = L->geth();

    std::vector<double> v((2 * rad + 1) * (2 * rad + 1));

    for     (int ii = i - rad; ii <= i + rad; ii++)
        for (int jj = j - rad; jj <= j + rad; jj++)
            v.push_back(L->get(int(wrap(ii, h, mode & 1)),
                               int(wrap(jj, w, mode & 2)), k));

    std::nth_element(v.begin(), v.begin() + v.size() / 2, v.end());
    return v[v.size() / 2];
}

void median::tweak(int a, int v)
{
    if (a == 0) rad += v;
}

std::string median::doc() const
{
    std::ostringstream sout;
    sout << "median " << rad;
    return sout.str();
}

//------------------------------------------------------------------------------

double medianv::get(int i, int j, int k) const
{
    const int h = L->geth();

    std::vector<double> v(2 * rad + 1);

    for (int ii = i - rad; ii <= i + rad; ii++)
        v.push_back(L->get(int(wrap(ii, h, mode & 1)), j, k));

    std::nth_element(v.begin(), v.begin() + v.size() / 2, v.end());
    return v[v.size() / 2];
}

std::string medianv::doc() const
{
    std::ostringstream sout;
    sout << "medianv " << rad;
    return sout.str();
}

//------------------------------------------------------------------------------

double medianh::get(int i, int j, int k) const
{
    const int w = L->getw();

    std::vector<double> v(2 * rad + 1);

    for (int jj = j - rad; jj <= j + rad; jj++)
        v.push_back(L->get(i, int(wrap(jj, w, mode & 1)), k));

    std::nth_element(v.begin(), v.begin() + v.size() / 2, v.end());
    return v[v.size() / 2];
}

std::string medianh::doc() const
{
    std::ostringstream sout;
    sout << "medianh " << rad;
    return sout.str();
}

//------------------------------------------------------------------------------

#endif
