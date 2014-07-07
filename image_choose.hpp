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

#ifndef IMAGE_CHOOSE_HPP
#define IMAGE_CHOOSE_HPP

//------------------------------------------------------------------------------

/// Choose operator

class choose : public image
{
public:
    /// Choose image *L* if *which* is 0 or image *R* if *which* is 1. his is
    /// akin to turning a layer on and off. It allows alternatives to be viewed
    /// without the overhead of evaluating both.

    choose(int which, image *L, image *R) : image(L, R), which(which) { }

    virtual double get(int i, int j, int k) const
    {
        return which ? R->get(i, j, k) : L->get(i, j, k);
    }

    virtual void tweak(int a, int v)
    {
        if (a == 0) which += v;
    }

    virtual void doc(std::ostream& out) const
    {
        out << "choose " << which;
    }

private:
    int which;
};

//------------------------------------------------------------------------------

#endif
