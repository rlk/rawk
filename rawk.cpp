#include "GL/GLFundamentals.hpp"
#include "GL/GLDemonstration.hpp"

#include <cstdio>
#include <cstring>
#include <string>

//------------------------------------------------------------------------------

/// The following sample type abbreviations are used consistently throughough:
///     unsigned 8  ... b
///       signed 8  ... c
///     unsigned 16 ... u
///       signed 16 ... s
///     unsigned 32 ... l
///       signed 32 ... i
///        float 32 ... f
///        float 64 ... d

//------------------------------------------------------------------------------

/// Base class for all image data sources.

class image
{
public:
    image() { }

    virtual double get(int, int, int) const = 0;

    virtual int geth() const = 0;
    virtual int getw() const = 0;
    virtual int getd() const = 0;

    virtual ~image() { }
};

/// RAW file writer with pass-through.

class output : public image
{
public:
    output(std::string name, char type, const image *src)
        : name(name), type(type), src(src) { }

    double get(int i, int j, int k) const { return src->get(i, j, k); }

    int geth() const { return src->geth(); }
    int getw() const { return src->getw(); }
    int getd() const { return src->getd(); }

    void go() const;

private:
    std::string name;
    char        type;
    const image *src;
};

/// Test pattern generator.

class test : public image
{
public:
    double get(int, int, int) const;

    int geth() const;
    int getw() const;
    int getd() const;
};

//------------------------------------------------------------------------------

/// Class raw implements a RAW image file templated over the sample type.

template <typename sample> class raw
{
public:

    void write(std::string, const image *);

    static sample fromd(double);
    static double   tod(sample);
};

/// Write the given image to a named RAW image file.

template <typename sample> void raw<sample>::write(std::string name,
                                                   const image *src)
{
    extern int errno;

    const int h = src->geth();
    const int w = src->getw();
    const int d = src->getd();

    sample buffer[w * d];

    FILE *stream;

    if ((stream = fopen(name.c_str(), "wb")) == 0)
        throw(std::runtime_error(strerror(errno)));

    for (int i = 0; i < h; ++i)
    {
        for     (int j = 0; j < w; ++j)
            for (int k = 0; k < d; ++k)
                buffer[d * j + k] = fromd(src->get(i, j, k));

        if (fwrite(buffer, sizeof (sample), w * d, stream) < w * d)
            throw(std::runtime_error(strerror(errno)));
    }

    if (fclose(stream) == EOF)
        throw(std::runtime_error(strerror(errno)));
}

// Specializations for conversion from double to RAW sample types.

template <> unsigned char raw<unsigned char>::fromd(double d)
{
    if      (d >  1.0) return 255;
    else if (d <  0.0) return   0;
    else               return (unsigned char) (d * 255);
}

template <> char raw<char>::fromd(double d)
{
    if      (d >  1.0) return  127;
    else if (d < -1.0) return -127;
    else               return (char) (d * 127);
}

template <> unsigned short raw<unsigned short>::fromd(double d)
{
    if      (d >  1.0) return 65535;
    else if (d <  0.0) return     0;
    else               return (unsigned short) (d * 65535);
}

template <> short raw<short>::fromd(double d)
{
    if      (d >  1.0) return  32767;
    else if (d < -1.0) return -32767;
    else               return (short) (d * 32767);
}

template <> unsigned int raw<unsigned int>::fromd(double d)
{
    if      (d >  1.0) return 4294967295;
    else if (d <  0.0) return          0;
    else               return (unsigned int) (d * 4294967295);
}

template <> int raw<int>::fromd(double d)
{
    if      (d >  1.0) return  2147483647;
    else if (d < -1.0) return -2147483647;
    else               return (int) (d * 2147483647);
}

template <> float raw<float>::fromd(double d)
{
    return float(d);
}

template <> double raw<double>::fromd(double d)
{
    return d;
}

/// Specializations for convertion from RAW sample types to double.

template <> double raw<unsigned char>::tod(unsigned char b)
{
    return double(b) / 255.0;
}

template <> double raw<char>::tod(char c)
{
    return double(c) / 127.0;
}

template <> double raw<unsigned short>::tod(unsigned short u)
{
    return double(u) / 65536.0;
}

template <> double raw<short>::tod(short s)
{
    return double(s) / 32767.0;
}

template <> double raw<unsigned int>::tod(unsigned int u)
{
    return double(u) / 4294967295.0;
}

template <> double raw<int>::tod(int s)
{
    return double(s) / 2147483647.0;
}

template <> double raw<float>::tod(float f)
{
    return double(f);
}

template <> double raw<double>::tod(double d)
{
    return d;
}

//------------------------------------------------------------------------------

void output::go() const
{
    switch (type)
    {
        case 'b': raw<unsigned char>   b; b.write(name, src); break;
        case 'c': raw<         char>   c; c.write(name, src); break;
        case 'u': raw<unsigned short>  u; u.write(name, src); break;
        case 's': raw<         short>  s; s.write(name, src); break;
        case 'l': raw<unsigned int>    i; i.write(name, src); break;
        case 'i': raw<         int>    l; l.write(name, src); break;
        case 'f': raw<         float>  f; f.write(name, src); break;
        case 'd': raw<         double> d; d.write(name, src); break;
    }
}

//------------------------------------------------------------------------------

double test::get(int i, int j, int k) const
{
    if (((i / (1 << (5 + k))) % 2) == ((j / (1 << (5 + k))) % 2))
        return 1.0f;
    else
        return 0.0f;
}

int test::geth() const
{
    return 1024;
}

int test::getw() const
{
    return 1024;
}

int test::getd() const
{
    return 3;
}

//------------------------------------------------------------------------------

class rawk : public gl::demonstration
{
public:

    rawk();
   ~rawk();
};

rawk::rawk() : demonstration("RAWK", 1280, 720)
{
}

rawk::~rawk()
{
}

//------------------------------------------------------------------------------

int main(int argc, char **argv)
{
    try
    {
        output *o = new output("out.raw", 'b', new test());
        o->go();

        rawk app;
        app.run(true);

        delete(o);

        return 0;
    }
    catch (std::exception &e)
    {
        fprintf(stderr, "%s\n", e.what());
        return 1;
    }
}
