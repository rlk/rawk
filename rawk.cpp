#include "GL/GLFundamentals.hpp"
#include "GL/GLDemonstration.hpp"

#include <string>

#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>

extern int errno;

//------------------------------------------------------------------------------

/// The following sample type abbreviations are used consistently throughout:
///     unsigned char .... b
///              char .... c
///     unsigned short ... u
///              short ... s
///     unsigned int ..... l
///              int ..... i
///              float ... f
///              double .. d

class raw
{
public:
    raw(std::string, int, int, int, int);

    virtual void   put(int, int, int, double) = 0;
    virtual double get(int, int, int)         = 0;

    virtual ~raw();

    size_t h;
    size_t w;
    size_t d;

protected:

    size_t index(int i, int j, int k) const { return ((i * w) + j) * d + k; }

    void  *map(size_t, int);
    void unmap(size_t, void *);

    int f;
};

raw::raw(std::string s, int h, int w, int d, int o) : h(h), w(w), d(d), f(0)
{
    if ((f = open(s.c_str(), o, 0666)) == -1)
        throw std::runtime_error(strerror(errno));
}

raw::~raw()
{
    if (close(f) == -1)
        throw std::runtime_error(strerror(errno));
}

void *raw::map(size_t size, int prot)
{
    // If writing, initialize empty storage by extending the file.

    size_t n = h * w * d * size;

    if (prot & PROT_WRITE && ftruncate(f, n) == -1)
        throw std::runtime_error(strerror(errno));

    // Map the contents of the file onto memory.

    void *p = mmap(0, n, prot, MAP_SHARED, f, 0);

    if (p == MAP_FAILED)
        throw std::runtime_error(strerror(errno));

    return p;
}

void raw::unmap(size_t size, void *p)
{
    size_t n = h * w * d * size;

    if (munmap(p, n) == -1)
        throw std::runtime_error(strerror(errno));
}

//------------------------------------------------------------------------------

class rawb : public raw
{
public:
    rawb(std::string, int, int, int, int, int);
   ~rawb();

    void   put(int, int, int, double);
    double get(int, int, int);

private:
    unsigned char *p;
};

class rawc : public raw
{
public:
    rawc(std::string, int, int, int, int, int);
   ~rawc();

    void   put(int, int, int, double);
    double get(int, int, int);

private:
    char *p;
};

class rawu : public raw
{
public:
    rawu(std::string, int, int, int, int, int);
   ~rawu();

    void   put(int, int, int, double);
    double get(int, int, int);

private:
    unsigned short *p;
};

class raws : public raw
{
public:
    raws(std::string, int, int, int, int, int);
   ~raws();

    void   put(int, int, int, double);
    double get(int, int, int);

private:
    short *p;
};

class rawl : public raw
{
public:
    rawl(std::string, int, int, int, int, int);
   ~rawl();

    void   put(int, int, int, double);
    double get(int, int, int);

private:
    unsigned int *p;
};

class rawi : public raw
{
public:
    rawi(std::string, int, int, int, int, int);
   ~rawi();

    void   put(int, int, int, double);
    double get(int, int, int);

private:
    int *p;
};

class rawf : public raw
{
public:
    rawf(std::string, int, int, int, int, int);
   ~rawf();

    void   put(int, int, int, double);
    double get(int, int, int);

private:
    float *p;
};

class rawd : public raw
{
public:
    rawd(std::string, int, int, int, int, int);
   ~rawd();

    void   put(int, int, int, double);
    double get(int, int, int);

private:
    double *p;
};

//------------------------------------------------------------------------------

rawb::rawb(std::string s, int h, int w, int d, int o, int r)
    : raw(s, h, w, d, o), p((unsigned char *) map(sizeof (unsigned char), r))
{
}

rawc::rawc(std::string s, int h, int w, int d, int o, int r)
    : raw(s, h, w, d, o), p((char *) map(sizeof (char), r))
{
}

rawu::rawu(std::string s, int h, int w, int d, int o, int r)
    : raw(s, h, w, d, o), p((unsigned short *) map(sizeof (unsigned short), r))
{
}

raws::raws(std::string s, int h, int w, int d, int o, int r)
    : raw(s, h, w, d, o), p((short *) map(sizeof (short), r))
{
}

rawl::rawl(std::string s, int h, int w, int d, int o, int r)
    : raw(s, h, w, d, o), p((unsigned int *) map(sizeof (unsigned int), r))
{
}

rawi::rawi(std::string s, int h, int w, int d, int o, int r)
    : raw(s, h, w, d, o), p((int *) map(sizeof (int), r))
{
}

rawf::rawf(std::string s, int h, int w, int d, int o, int r)
    : raw(s, h, w, d, o), p((float *) map(sizeof (float), r))
{
}

rawd::rawd(std::string s, int h, int w, int d, int o, int r)
    : raw(s, h, w, d, o), p((double *) map(sizeof (double), r))
{
}

//------------------------------------------------------------------------------

rawb::~rawb()
{
    unmap(sizeof(unsigned char), p);
}

rawc::~rawc()
{
    unmap(sizeof(char), p);
}

rawu::~rawu()
{
    unmap(sizeof(unsigned short), p);
}

raws::~raws()
{
    unmap(sizeof(short), p);
}

rawl::~rawl()
{
    unmap(sizeof(unsigned int), p);
}

rawi::~rawi()
{
    unmap(sizeof(int), p);
}

rawf::~rawf()
{
    unmap(sizeof(float), p);
}

rawd::~rawd()
{
    unmap(sizeof(double), p);
}

//------------------------------------------------------------------------------

void rawb::put(int i, int j, int k, double v)
{
    if      (v >  1.0) p[index(i, j, k)] =  255;
    else if (v <  0.0) p[index(i, j, k)] =    0;
    else               p[index(i, j, k)] = (unsigned char) (v * 255);
}

void rawc::put(int i, int j, int k, double v)
{
    if      (v >  1.0) p[index(i, j, k)] =  127;
    else if (v < -1.0) p[index(i, j, k)] = -127;
    else               p[index(i, j, k)] = (char) (v * 127);
}

void rawu::put(int i, int j, int k, double v)
{
    if      (v >  1.0) p[index(i, j, k)] =  65535;
    else if (v <  0.0) p[index(i, j, k)] =      0;
    else               p[index(i, j, k)] = (unsigned short) (v * 65535);
}

void raws::put(int i, int j, int k, double v)
{
    if      (v >  1.0) p[index(i, j, k)] =  32767;
    else if (v < -1.0) p[index(i, j, k)] = -32767;
    else               p[index(i, j, k)] = (short) (v * 32767);
}

void rawl::put(int i, int j, int k, double v)
{
    if      (v >  1.0) p[index(i, j, k)] =  4294967295;
    else if (v <  0.0) p[index(i, j, k)] =           0;
    else               p[index(i, j, k)] = (unsigned int) (v * 4294967295);
}

void rawi::put(int i, int j, int k, double v)
{
    if      (v >  1.0) p[index(i, j, k)] =  2147483647;
    else if (v < -1.0) p[index(i, j, k)] = -2147483647;
    else               p[index(i, j, k)] = (int) (v * 2147483647);
}

void rawf::put(int i, int j, int k, double v)
{
    p[index(i, j, k)] = float(v);
}

void rawd::put(int i, int j, int k, double v)
{
    p[index(i, j, k)] = v;
}

//------------------------------------------------------------------------------

double rawb::get(int i, int j, int k)
{
    return double(p[index(i, j, k)]) / 255.0;
}

double rawc::get(int i, int j, int k)
{
    return double(p[index(i, j, k)]) / 127.0;
}

double rawu::get(int i, int j, int k)
{
    return double(p[index(i, j, k)]) / 65535.0;
}

double raws::get(int i, int j, int k)
{
    return double(p[index(i, j, k)]) / 32767.0;
}

double rawl::get(int i, int j, int k)
{
    return double(p[index(i, j, k)]) / 4294967295.0;
}

double rawi::get(int i, int j, int k)
{
    return double(p[index(i, j, k)]) / 2147483647.0;
}

double rawf::get(int i, int j, int k)
{
    return double(p[index(i, j, k)]);
}

double rawd::get(int i, int j, int k)
{
    return double(p[index(i, j, k)]);
}

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

/// RAW file reader.

class input : public image
{
public:
    input(std::string, int, int, int, char);
   ~input();

    double get(int i, int j, int k) const { return r->get(i, j, k); }

    int geth() const { return r->h; }
    int getw() const { return r->w; }
    int getd() const { return r->d; }

private:
    raw *r;
};

/// RAW file writer with pass-through.

class output : public image
{
public:
    output(std::string, char, const image *);
   ~output();

    double get(int i, int j, int k) const { return src->get(i, j, k); }

    int geth() const { return r->h; }
    int getw() const { return r->w; }
    int getd() const { return r->d; }

    void go() const;

private:
    raw *r;
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

input::input(std::string s, int h, int w, int d, char t) : r(0)
{
    const int o = O_RDONLY;
    const int p = PROT_READ;

    switch (t)
    {
        case 'b': r = new rawb(s, h, w, d, o, p); break;
        case 'c': r = new rawc(s, h, w, d, o, p); break;
        case 'u': r = new rawu(s, h, w, d, o, p); break;
        case 's': r = new raws(s, h, w, d, o, p); break;
        case 'l': r = new rawl(s, h, w, d, o, p); break;
        case 'i': r = new rawi(s, h, w, d, o, p); break;
        case 'f': r = new rawf(s, h, w, d, o, p); break;
        case 'd': r = new rawd(s, h, w, d, o, p); break;
    }
}

input::~input()
{
    delete r;
}

//------------------------------------------------------------------------------

output::output(std::string s, char t, const image *src) : r(0), src(src)
{
    const int o = O_RDWR | O_TRUNC | O_CREAT;
    const int p = PROT_READ | PROT_WRITE;

    const int h = src->geth();
    const int w = src->getw();
    const int d = src->getd();

    switch (t)
    {
        case 'b': r = new rawb(s, h, w, d, o, p); break;
        case 'c': r = new rawc(s, h, w, d, o, p); break;
        case 'u': r = new rawu(s, h, w, d, o, p); break;
        case 's': r = new raws(s, h, w, d, o, p); break;
        case 'l': r = new rawl(s, h, w, d, o, p); break;
        case 'i': r = new rawi(s, h, w, d, o, p); break;
        case 'f': r = new rawf(s, h, w, d, o, p); break;
        case 'd': r = new rawd(s, h, w, d, o, p); break;
    }
}

output::~output()
{
    delete r;
    delete src;
}

void output::go() const
{
    for         (int i = 0; i < geth(); ++i)
        for     (int j = 0; j < getw(); ++j)
            for (int k = 0; k < getd(); ++k)
                r->put(i, j, k, src->get(i, j, k));
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
        output *o = new output("out.raw", 'u', new test());
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
