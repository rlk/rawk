#include "GL/GLFundamentals.hpp"
#include "GL/GLDemonstration.hpp"

#include <string>
#include <vector>

#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>

extern int errno;

//------------------------------------------------------------------------------

/// The following sample type abbreviations are used consistently throughout.
/// Capitalization implies non-native byte order.
///
///     unsigned char .... b
///              char .... c
///     unsigned short ... u U
///              short ... s S
///     unsigned int ..... l L
///              int ..... i I
///              float ... f F
///              double .. d D

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

    size_t index(int, int, int) const;
    bool   valid(int, int, int) const;

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

size_t raw::index(int i, int j, int k) const
{
    return ((i * w) + j) * d + k;
}

bool raw::valid(int i, int j, int k) const
{
    return (0 <= i && i < h
         && 0 <= j && j < w
         && 0 <= k && k < d);
}

void *raw::map(size_t size, int prot)
{
    size_t n = h * w * d * size;
    void *p;

    // If writing, initialize empty storage by extending the file.

    if (prot & PROT_WRITE && ftruncate(f, n) == -1)
        throw std::runtime_error(strerror(errno));

    // Map the contents of the file onto memory.

    if ((p = mmap(0, n, prot, MAP_SHARED, f, 0)) == MAP_FAILED)
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
    if (valid(i, j, k))
    {
        if      (v >  1.0) p[index(i, j, k)] =  255;
        else if (v <  0.0) p[index(i, j, k)] =    0;
        else               p[index(i, j, k)] = (unsigned char) (v * 255);
    }
}

void rawc::put(int i, int j, int k, double v)
{
    if (valid(i, j, k))
    {
        if      (v >  1.0) p[index(i, j, k)] =  127;
        else if (v < -1.0) p[index(i, j, k)] = -127;
        else               p[index(i, j, k)] = (char) (v * 127);
    }
}

void rawu::put(int i, int j, int k, double v)
{
    if (valid(i, j, k))
    {
        if      (v >  1.0) p[index(i, j, k)] =  65535;
        else if (v <  0.0) p[index(i, j, k)] =      0;
        else               p[index(i, j, k)] = (unsigned short) (v * 65535);
    }
}

void raws::put(int i, int j, int k, double v)
{
    if (valid(i, j, k))
    {
        if      (v >  1.0) p[index(i, j, k)] =  32767;
        else if (v < -1.0) p[index(i, j, k)] = -32767;
        else               p[index(i, j, k)] = (short) (v * 32767);
    }
}

void rawl::put(int i, int j, int k, double v)
{
    if (valid(i, j, k))
    {
        if      (v >  1.0) p[index(i, j, k)] =  4294967295;
        else if (v <  0.0) p[index(i, j, k)] =           0;
        else               p[index(i, j, k)] = (unsigned int) (v * 4294967295);
    }
}

void rawi::put(int i, int j, int k, double v)
{
    if (valid(i, j, k))
    {
        if      (v >  1.0) p[index(i, j, k)] =  2147483647;
        else if (v < -1.0) p[index(i, j, k)] = -2147483647;
        else               p[index(i, j, k)] = (int) (v * 2147483647);
    }
}

void rawf::put(int i, int j, int k, double v)
{
    if (valid(i, j, k)) p[index(i, j, k)] = float(v);
}

void rawd::put(int i, int j, int k, double v)
{
    if (valid(i, j, k)) p[index(i, j, k)] = v;
}

//------------------------------------------------------------------------------

double rawb::get(int i, int j, int k)
{
    return valid(i, j, k) ? double(p[index(i, j, k)]) / 255.0 : 0.0;
}

double rawc::get(int i, int j, int k)
{
    return valid(i, j, k) ? double(p[index(i, j, k)]) / 127.0 : 0.0;
}

double rawu::get(int i, int j, int k)
{
    return valid(i, j, k) ? double(p[index(i, j, k)]) / 65535.0 : 0.0;
}

double raws::get(int i, int j, int k)
{
    return valid(i, j, k) ? double(p[index(i, j, k)]) / 32767.0 : 0.0;
}

double rawl::get(int i, int j, int k)
{
    return valid(i, j, k) ? double(p[index(i, j, k)]) / 4294967295.0 : 0.0;
}

double rawi::get(int i, int j, int k)
{
    return valid(i, j, k) ? double(p[index(i, j, k)]) / 2147483647.0 : 0.0;
}

double rawf::get(int i, int j, int k)
{
    return valid(i, j, k) ? double(p[index(i, j, k)]) : 0.0;
}

double rawd::get(int i, int j, int k)
{
    return valid(i, j, k) ? double(p[index(i, j, k)]) : 0.0;
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
    if (0 <= i && i < geth() && 0 <= j && j < getw() && 0 <= k && k < getd())
    {
        if (((i / (1 << (5 + k))) % 2) == ((j / (1 << (5 + k))) % 2))
            return 1.0;
        else
            return 0.0;
    }
    return 0.0;
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
    rawk(image *);
   ~rawk();

   void   draw();
   void  wheel(int, int);
   void motion(int, int);
   void button(int, bool);
   void    key(int, bool, bool);

private:

    GLuint texture;
    GLuint program;
    GLuint vbuffer;
    GLuint varray;

    GLint  u_offset;
    GLint  u_scale;

    int    point_x;       // Current mouse pointer position x
    int    point_y;       // Current mouse pointer position y

    int    image_x;       // Current image view position x
    int    image_y;       // Current image view position y
    double image_z;       // Current image view zoom

    int    cache_x;       // Cached image view position x
    int    cache_y;       // Cached image view position y
    double cache_z;       // Cached image view zoom

    bool   dragging;      // Is a drag in progress?
    int    drag_point_x;  // Mouse pointer position at beginning of drag
    int    drag_point_y;  // Mouse pointer position at beginning of drag
    int    drag_image_x;  // Image view position at beginning of drag
    int    drag_image_y;  // Image view position at beginning of drag

    image *src;

    void refresh();
};

rawk::rawk(image *src) : demonstration("RAWK", 640, 360), src(src)
{
    glClearColor(0.2f, 0.2f, 0.2f, 0.0f);

    // Initialize a texture object for the image cache.

    glGenTextures(1, &texture);

    glBindTexture  (GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0,
                                   GL_RGB, GL_FLOAT, 0);

    // Initialize the GLSL program.

    if ((program = gl::init_program("rawk.vert", "rawk.frag")))
    {
        glUseProgram(program);

        // Initialize a vertex buffer object to fill the screen.

        static const GLfloat rect[] = {
            -1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
            -1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 1.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        };

        glGenVertexArrays(1, &varray);
        glBindVertexArray(    varray);

        glGenBuffers(1, &vbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vbuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(rect), rect, GL_STATIC_DRAW);

        // Bind the vertex buffer object to the program attributes.

        GLint p = glGetAttribLocation(program, "vPosition");
        GLint t = glGetAttribLocation(program, "vTexCoord");

        glEnableVertexAttribArray(p);
        glEnableVertexAttribArray(t);

        glVertexAttribPointer(p, 4, GL_FLOAT, GL_FALSE, 24, (const void *)  0);
        glVertexAttribPointer(t, 2, GL_FLOAT, GL_FALSE, 24, (const void *) 16);

        // Locate the program uniforms.

        u_offset = glGetUniformLocation(program, "offset");
        u_scale  = glGetUniformLocation(program, "scale");
    }

    // Set the initial view on the image.

    const int h = src->geth();
    const int w = src->getw();

    if (double(width) / double(height) < double(w) / double(h))
        image_z = double(w) / double(width);
    else
        image_z = double(h) / double(height);

    image_x = w / 2;
    image_y = h / 2;

    point_x = 0;
    point_y = 0;

    dragging = false;

    refresh();
}

rawk::~rawk()
{
    glDeleteProgram(program);
    glDeleteBuffers (1, &vbuffer);
    glDeleteTextures(1, &texture);
    glDeleteVertexArrays(1, &varray);
}

void rawk::draw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // double z =        image_z / cache_z;
    double z =        cache_z / image_z;
    double x = 2 * double(cache_x - image_x) / cache_z / (width  / 1);
    double y = 2 * double(cache_y - image_y) / cache_z / (height / 1);

    printf("%d %d %f ... %d %d %f ... %f %f %f\n",
            image_x, image_y, image_z,
            cache_x, cache_y, cache_z, x, y, z);

    glUniform1f(u_scale,  z);
    glUniform2f(u_offset, x, y);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void rawk::motion(int x, int y)
{
    point_x = x;
    point_y = y;

    if (dragging)
    {
        image_x = drag_image_x + (drag_point_x - x) * image_z;
        image_y = drag_image_y + (y - drag_point_y) * image_z;
    }
}

void rawk::button(int button, bool down)
{
    if (button == SDL_BUTTON_LEFT)
    {
        dragging     = down;
        drag_image_x = image_x;
        drag_image_y = image_y;
        drag_point_x = point_x;
        drag_point_y = point_y;
    }
}

void rawk::wheel(int dx, int dy)
{
    // Compute pointer position relativate to the center of the screen.

    double xx = point_x - 0.5 * width;
    double yy = point_y - 0.5 * height;

    // Compute the image pixel on which the pointer lies.

    double x = image_x + xx * image_z;
    double y = image_y - yy * image_z;

    // Compute a new zoom level.

    image_z = exp(log(image_z) + dy / 100.0);

    // Compute image offsets to ensure the pointer remains over the same pixel.

    image_x = x - xx * image_z;
    image_y = y + yy * image_z;
}

void rawk::key(int key, bool down, bool repeat)
{
    if (down && !repeat)
    {
        switch (key)
        {
            case SDL_SCANCODE_SPACE: refresh(); break;
            case SDL_SCANCODE_RETURN:
                cache_x = image_x = 0;
                cache_y = image_y = 0;
                cache_z = image_z = 2;
                break;
        }
    }
}

//------------------------------------------------------------------------------

void rawk::refresh()
{
    std::vector<GLfloat> pixel(width * height * 3, 0);

    cache_x = image_x;
    cache_y = image_y;
    cache_z = image_z;

    const int depth = std::min(src->getd(), 3);

    for         (int i = 0; i < height; ++i)
        for     (int j = 0; j < width;  ++j)
            for (int k = 0; k < depth;  ++k)

                pixel[(i * width + j) * 3 + k]
                    = src->get(int(floor(cache_y + (i - height / 2) * cache_z)),
                               int(floor(cache_x + (j - width  / 2) * cache_z)), k);

    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height,
                    GL_RGB, GL_FLOAT, &pixel.front());
}

//------------------------------------------------------------------------------

int main(int argc, char **argv)
{
    try
    {
        output *o = new output("out.raw", 'u', new test());

        rawk app(o);
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
