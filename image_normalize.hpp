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

#ifndef IMAGE_NORMALIZE_HPP
#define IMAGE_NORMALIZE_HPP

//------------------------------------------------------------------------------

/// Vector renormalization filter

class normalize : public image
{
public:
    /// Renormalize a normal map. This filter has an extremely limited usage...
    /// A normal map encodes a unit-length vector N in an unsigned pixel P where
    /// P = (N + 1) / 2, effectively remapping from the range [-1,+1] to the
    /// range [0,1]. Most filters ignore the unit-length invariant so the vector
    /// length drifts as a normal map is operated upon (resampled, scaled, etc).
    /// This filter corrects this drift. It convert a pixel to a vector,
    /// normalizes it, and converts it back to a pixel. This should be the last
    /// filter applied to any normal map process, and is not relevant to any
    /// other type of process.

    normalize(image *L) : image(L) { }

    virtual double get(int i, int j, int k) const
    {
        // This function is very wasteful of CPU resources, as it touches every
        // source channel for every sample. But it works for those limited cases
        // where it's needed.

        const int d = get_depth();
        double    r = 0;
        double    v = 0;

        for (int l = 0; l < d; l++)
        {
            double t = 2.0 * L->get(i, j, l) - 1.0;
            if (l == k)
                v  = t;
            r += t * t;
        }
        return (v / sqrt(r) + 1.0) / 2.0;
    }

    virtual void doc(std::ostream& out) const
    {
        out << "normalize";
    }
};

//------------------------------------------------------------------------------

#endif
