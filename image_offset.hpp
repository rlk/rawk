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

#ifndef IMAGE_OFFSETS_HPP
#define IMAGE_OFFSETS_HPP

//------------------------------------------------------------------------------

/// Offset filter

class offset : public image
{
public:
    offset(int rows, int columns, int m, image *L)
        : image(L), rows(rows), columns(columns), mode(m) { }

    virtual double get(int i, int j, int k) const
    {
        return L->get(wrap(i - rows,    L->get_height(), mode & 1),
                      wrap(j - columns, L->get_width (), mode & 2), k);
    }

    virtual void tweak(int a, int v)
    {
        if (a == 0) columns += v;
        if (a == 1) rows    += v;
    }

    virtual std::string doc() const
    {
        std::ostringstream out;
        out << "offset " << rows
                   << " " << columns
                   << " " << mode;
        return out.str();
    }

private:
    int rows;
    int columns;
    int mode;
};

//------------------------------------------------------------------------------

#endif
