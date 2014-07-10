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
    /// Write a raw-formatted data file named *name*. *Depth* is the number of
    /// channels to inclue and *type* is a character giving the output
    /// @ref type "sample type". The image height and width are given by the
    /// *L* image object. Unsigned samples are clamped to the range [0,1] and
    /// signed samples to the range [-1,+1] before being cast to the destination
    /// data type.

    output(std::string name, int depth, char type, image *L)
        : image(L), cache(false), file(0), chars(0)
    {
        const int height = L->get_height();
        const int width  = L->get_width ();

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

    virtual pixel get(int i, int j) const
    {
        if (0 <= i && i < file->get_height() &&
            0 <= j && j < file->get_width ())
        {
            if (cache)
            {
                int d = file->get_depth();

                float r = d > 0 ? file->get(i, j, 0) : 0;
                float g = d > 1 ? file->get(i, j, 1) : 0;
                float b = d > 2 ? file->get(i, j, 2) : 0;
                float a = d > 3 ? file->get(i, j, 3) : 0;

                return pixel(r, g, b, a);
            }
            else
                return L->get(i, j);
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

    /// Process all samples of this image, storing them in the file. Then,
    /// switch into cached mode, sourcing all subsequent sampling from the file
    /// instead of invoking the process. This is useful as an optimization,
    /// especially for processes that feed large-kernel convolutions, as it
    /// eliminates the need to repeatedly compute expensive samples.

    virtual void process()
    {
        int f, g = 8, c = 0;
        int i, h = file->get_height();
        int j, w = file->get_width ();
        int l, d = file->get_depth ();
        pixel p;

        image::process();

        // Divide the set of all scanlines into groups of size g.

        #pragma omp parallel for private(i, j, l, p) schedule(dynamic)
        for (f = 0; f < h; f += g)
        {
            // Process each group in parallel.

            l = std::min(f + g, h);

            for     (i = f; i < l; ++i)
                for (j = 0; j < w; ++j)
                {
                    p = L->get(i, j);

                    if (d > 0) file->put(i, j, 0, p.r);
                    if (d > 1) file->put(i, j, 1, p.g);
                    if (d > 2) file->put(i, j, 2, p.b);
                    if (d > 3) file->put(i, j, 3, p.a);
                }

            // Report a running total of completed scan lines.

            #pragma omp critical
            c += g;

            if (omp_get_thread_num() == 0)
                report(c, h);
        }

        // Finish the report and enable the cache.

        report(h, h);
        cache = true;
    }

private:
    bool cache;
    raw *file;
    int  chars;

    void report(int i, int n)
    {
        std::ostringstream stream;

        stream << "Wrote " << i
               <<   " of " << n
               <<   " to " << file->get_name();

        if (i < n)
            std::cout << std::string(chars, '\b') << stream.str() << std::flush;
        else
            std::cout << std::string(chars, '\b') << stream.str() << std::endl;

        chars = stream.str().size();
    }
};

//------------------------------------------------------------------------------

#endif
