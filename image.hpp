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

#ifndef IMAGE_HPP
#define IMAGE_HPP

//------------------------------------------------------------------------------

/// Base class for all image data sources

class image
{
public:
    image(image *L=0, image *R=0) : L(L), R(R), P(0)
    {
        if (L) L->setP(this);
        if (R) R->setP(this);
    }

    virtual double get(int, int, int) const = 0;
    virtual void tweak(int, int) { }

    virtual int geth() const
    {
        if (L && R) return std::max(L->geth(), R->geth());
        else if (L) return L->geth();
        else if (R) return L->geth();
        else        return 0;
    }
    virtual int getw() const
    {
        if (L && R) return std::max(L->getw(), R->getw());
        else if (L) return L->getw();
        else if (R) return L->getw();
        else        return 0;
    }
    virtual int getd() const
    {
        if (L && R) return std::max(L->getd(), R->getd());
        else if (L) return L->getd();
        else if (R) return L->getd();
        else        return 0;
    }

    virtual ~image()
    {
        if (R) delete R;
        if (L) delete L;
    }

    virtual std::string doc() const
    {
        return std::string("undocumented");
    }

    image *getL() { return L; }
    image *getR() { return R; }
    image *getP() { return P; }

    void setP(image *p) { P = p; }

protected:
    image *L;
    image *R;
    image *P;
};

//------------------------------------------------------------------------------

static inline int mod(int a, int n)
{
    return (a % n < 0) ? (a % n + n) : (a % n);
}

static inline int wrap(int i, int n, bool w)
{
    if      (i <     0) return w ? mod(i, n) :     0;
    else if (i > n - 1) return w ? mod(i, n) : n - 1;
    else                return i;
}

//------------------------------------------------------------------------------

#endif
