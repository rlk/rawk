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

#ifndef IMAGE_SOLID_HPP
#define IMAGE_SOLID_HPP

//------------------------------------------------------------------------------

/// Solid color generator

class solid : public image
{
public:
    solid(int height, int width, double value)
        : height(height), width(width), value(value) { }

    virtual double get(int i, int j, int k) const
    {
        return value;
    }

    virtual int get_height() const { return height; }
    virtual int get_width () const { return width;  }

    virtual std::string doc() const
    {
        std::ostringstream out;
        out << "solid " << height << " " << width << " " << value;
        return out.str();
    }

private:
    int    height;
    int    width;
    double value;
};

//------------------------------------------------------------------------------

#endif
