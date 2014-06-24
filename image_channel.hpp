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

#ifndef IMAGE_CHANNEL_HPP
#define IMAGE_CHANNEL_HPP

//------------------------------------------------------------------------------

/// Channel reassignment filter

class channel : public image
{
public:
    channel(int k, image *L) : image(L), index(k) { }

    virtual double get(int, int, int) const;

    virtual int getd() const { return index + 1; }

    virtual std::string doc() const;

private:
    int index;
};

//------------------------------------------------------------------------------

double channel::get(int i, int j, int k) const
{
    if (k == index)
        return L->get(i, j, 0);
    else
        return 0.0;
}

std::string channel::doc() const
{
    std::ostringstream sout;
    sout << "channel " << index;
    return sout.str();
}

//------------------------------------------------------------------------------

#endif
