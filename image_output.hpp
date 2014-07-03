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

#ifndef IMAGE_OUTPUT_HPP
#define IMAGE_OUTPUT_HPP

//------------------------------------------------------------------------------

/// Image file writer

class output : public image
{
public:
    /// Write a raw-formatted data file named *name*. *Type* is a character
    /// giving the output @ref type "sample type". The image height, width, and
    /// depth are given by the *L* image object. Unsigned samples are clamped to
    /// the range [0,1] and signed samples to the range [-1,+1] before being
    /// cast to the destination data type.

    output(std::string name, char type, image *L) : image(L), cache(false), file(0)
    {
        const int height = L->get_height();
        const int width  = L->get_width ();
        const int depth  = L->get_depth ();

        switch (type)
        {
            case 'b': file = new rawb(name, 0, height, width, depth, true); break;
            case 'c': file = new rawc(name, 0, height, width, depth, true); break;
            case 'u': file = new rawu(name, 0, height, width, depth, true); break;
            case 'U': file = new rawU(name, 0, height, width, depth, true); break;
            case 's': file = new raws(name, 0, height, width, depth, true); break;
            case 'S': file = new rawS(name, 0, height, width, depth, true); break;
            case 'l': file = new rawl(name, 0, height, width, depth, true); break;
            case 'L': file = new rawL(name, 0, height, width, depth, true); break;
            case 'i': file = new rawi(name, 0, height, width, depth, true); break;
            case 'I': file = new rawI(name, 0, height, width, depth, true); break;
            case 'f': file = new rawf(name, 0, height, width, depth, true); break;
            case 'F': file = new rawF(name, 0, height, width, depth, true); break;
            case 'd': file = new rawd(name, 0, height, width, depth, true); break;
            case 'D': file = new rawD(name, 0, height, width, depth, true); break;
        }
    }

   ~output()
    {
        delete file;
    }

    virtual double get(int i, int j, int k) const
    {
        if (0 <= i && i < file->get_height() &&
            0 <= j && j < file->get_width () &&
            0 <= k && k < file->get_depth ())
        {
            if (cache)
                return file->get(i, j, k);
            else
                return    L->get(i, j, k);
        }
        return 0.0;
    }

    virtual void doc(std::ostream& out) const
    {
        out << "output " << file->get_name  ()
                  << " " << file->get_height()
                  << " " << file->get_width ()
                  << " " << file->get_depth ();
    }

    void process(int i) const
    {
     	int j, w = get_width();
     	int k, d = get_depth();

        for     (j = 0; j < w; ++j)
            for (k = 0; k < d; ++k)
                file->put(i, j, k, L->get(i, j, k));
    }

    void set_cache(bool c)
    {
        cache = c;
    }

private:
    bool cache;
    raw *file;
};

//------------------------------------------------------------------------------

#endif
