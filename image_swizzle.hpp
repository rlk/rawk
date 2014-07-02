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
    /// Reorder, replicate, or remove channels of *L*. *Element* is a string of
    /// digits used as channel indices. The height and width of the output match
    /// those of the input, but the depth of the output equals the length of the
    /// element string.
    ///
    /// For example `210` would produce a BGR output from an RGB input. `1`
    /// would isolate the green channel. `000` would route the red channel to
    /// all three channels. `0121` would duplicate the green channel as an alpha
    /// channel.
    ///
    /// Elements indices are base 36, which allows inputs of up to 36 channels:
    /// 0 1 2 3 4 5 6 7 8 9 A B C D E F G H I J K L M N O P Q R S T U V W X Y Z.

    swizzle(std::string element, image *L) : image(L), element(element)
    {
        for (int i = 0; i < element.size(); i++)
        {
            char c = element[i];
            int  k = 0;

            if ('0' <= c && c <= '9') k = c - '0';
            if ('A' <= c && c <= 'Z') k = c - 'A' + 10;

            if (0 <= k && k < L->get_depth())
                index.push_back(k);
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
        out << "swizzle " << element;
    }

private:
    std::string      element;
    std::vector<int> index;
};

//------------------------------------------------------------------------------

#endif
