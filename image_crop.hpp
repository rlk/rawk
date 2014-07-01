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

#ifndef IMAGE_CROP_HPP
#define IMAGE_CROP_HPP

//------------------------------------------------------------------------------

/// Crop filter

class crop : public image
{
public:
    crop(int row, int column, int height, int width, image *L)
        : image(L), row(row), column(column), height(height), width(width) { }

    virtual double get(int i, int j, int k) const
    {
        if (0 <= i && i < height &&
            0 <= j && j < width)

            return L->get(i + row, j + column, k);
        else
            return 0.0;
    }

    virtual int get_height() const { return height; }
    virtual int get_width () const { return width;  }

    virtual void doc(std::ostream& out) const
    {
        out << "crop " << row << " " << column << " " << height << " " << width;
    }

private:
    int row;
    int column;
    int height;
    int width;
};

//------------------------------------------------------------------------------

#endif
