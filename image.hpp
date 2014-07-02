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

/// Base class for all image sources, filters, and operators

class image
{
public:
    /// Create a new image object with left child *L* and right child *R*.
    /// The parents *L* and *R* are set to *this*.

    image(image *L=0, image *R=0) : L(L), R(R), P(0)
    {
        if (L) L->setP(this);
        if (R) R->setP(this);
    }

    /// Finalize this image object by deleting any children.

    virtual ~image()
    {
        if (R) delete R;
        if (L) delete L;
    }

    /// Return the value of the sample at row *i*, column *j*, channel *k*.

    virtual double get(int i, int j, int k) const = 0;

    /// Return the height of this image.

    virtual int get_height() const
    {
        if (L && R) return std::max(L->get_height(), R->get_height());
        else if (L) return L->get_height();
        else if (R) return L->get_height();
        else        return 0;
    }

    /// Return the height of this image.

    virtual int get_width() const
    {
        if (L && R) return std::max(L->get_width(), R->get_width());
        else if (L) return L->get_width();
        else if (R) return L->get_width();
        else        return 0;
    }

    /// Return the depth of this image.

    virtual int get_depth() const
    {
        if (L && R) return std::max(L->get_depth(), R->get_depth());
        else if (L) return L->get_depth();
        else if (R) return L->get_depth();
        else        return 0;
    }

    /// Return the left child

    image *getL()
    {
        return L;
    }

    /// Return the right child

    image *getR()
    {
        return R;
    }

    /// Return the parent

    image *getP()
    {
        return P;
    }

    /// Tweak image parameter *a*, changing the value by a factor of *v*.

    virtual void tweak(int a, int v)
    {
    }

    /// Produce a string documenting the function of this object.

    virtual void doc(std::ostream& out) const
    {
        out << "undocumented";
    }

protected:
    image *L;    ///< Left child
    image *R;    ///< Right child
    image *P;    ///< Parent

private:
    void setP(image *p) { P = p; }
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
