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

#ifndef IMAGE_SWIZZLE_HPP
#define IMAGE_SWIZZLE_HPP

//------------------------------------------------------------------------------

/// Swizzle filter

class swizzle : public image
{
public:
    swizzle(std::string mode, image *L) : image(L), mode(mode)
    {
        for (int i = 0; i < mode.size(); i++)
        {
            int s = mode[i] - '0';

            if (0 <= s && s < L->get_depth())
                index.push_back(s);
            else
                throw std::runtime_error("Swizzle index out of range");
        }
    }

    virtual double get(int i, int j, int k) const
    {
        return L->get(i, j, index[k]);
    }

    virtual int get_depth() const
    {
        return index.size();
    }

    virtual void doc(std::ostream& out) const
    {
        out << "swizzle " << mode;
    }

private:
    std::string      mode;
    std::vector<int> index;
};

//------------------------------------------------------------------------------

#endif
