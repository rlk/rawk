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

#ifndef IMAGE_INPUT_HPP
#define IMAGE_INPUT_HPP

//------------------------------------------------------------------------------

/// Image file reader
///
/// Read a raw-formatted data file named *name*. *Height*, *width*, and *depth*
/// are integers giving the size and channel count of this input. *Type*
/// is a character giving the sample type.

class input : public image
{
public:
    input(std::string name, int start, int height, int width, int depth, char type)
    {
        switch (type)
        {
            case 'b': file = new rawb(name, start, height, width, depth, false); break;
            case 'c': file = new rawc(name, start, height, width, depth, false); break;
            case 'u': file = new rawu(name, start, height, width, depth, false); break;
            case 'U': file = new rawU(name, start, height, width, depth, false); break;
            case 's': file = new raws(name, start, height, width, depth, false); break;
            case 'S': file = new rawS(name, start, height, width, depth, false); break;
            case 'l': file = new rawl(name, start, height, width, depth, false); break;
            case 'L': file = new rawL(name, start, height, width, depth, false); break;
            case 'i': file = new rawi(name, start, height, width, depth, false); break;
            case 'I': file = new rawI(name, start, height, width, depth, false); break;
            case 'f': file = new rawf(name, start, height, width, depth, false); break;
            case 'F': file = new rawF(name, start, height, width, depth, false); break;
            case 'd': file = new rawd(name, start, height, width, depth, false); break;
            case 'D': file = new rawD(name, start, height, width, depth, false); break;
        }
    }

   ~input()
    {
        delete file;
    }

    virtual double get(int i, int j, int k) const
    {
        return (0 <= i && i < file->get_height() &&
                0 <= j && j < file->get_width () &&
                0 <= k && k < file->get_depth ()) ? file->get(i, j, k) : 0.0;
    }

    virtual int get_height() const { return file->get_height(); }
    virtual int get_width () const { return file->get_width (); }
    virtual int get_depth () const { return file->get_depth (); }

    virtual std::string doc() const
    {
        std::ostringstream out;
        out << "input " << file->get_name  ()
                 << " " << file->get_height()
                 << " " << file->get_width ()
                 << " " << file->get_depth ();
        return out.str();
    }

private:
    raw *file;
};

//------------------------------------------------------------------------------

#endif
