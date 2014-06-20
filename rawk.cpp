#include "GL/GLFundamentals.hpp"
#include "GL/GLDemonstration.hpp"

#include <string>
#include <vector>

#include <stdio.h>
#include <fcntl.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>

#include "rawk.hpp"

//------------------------------------------------------------------------------

/// Base class for all image data sources

class image
{
public:
    image(image *H=0, image *T=0) : head(H), tail(T) { }

    virtual double get(int, int, int) const = 0;

    virtual int geth() const { return head ? head->geth() : 0; }
    virtual int getw() const { return head ? head->getw() : 0; }
    virtual int getd() const { return head ? head->getd() : 0; }

    virtual ~image()
    {
        if (tail) delete tail;
        if (head) delete head;
    }

protected:
    image *head;
    image *tail;
};

/// RAW file reader

class input : public image
{
public:
    input(std::string, int, int, int, char);
   ~input();

    virtual double get(int, int, int) const;

    virtual int geth() const { return file->h; }
    virtual int getw() const { return file->w; }
    virtual int getd() const { return file->d; }

private:
    raw *file;
};

/// RAW file writer with pass-through

class output : public image
{
public:
    output(std::string, char, image *);
   ~output();

    virtual double get(int, int, int) const;

    void go() const;

private:
    raw *file;
};

/// Offset filter

class offset : public image
{
public:
    offset(double d, image *H) : image(H), value(d) { }

    virtual double get(int, int, int) const;

private:
    double value;
};

/// Scale filter

class scale : public image
{
public:
    scale(double d, image *H) : image(H), value(d) { }

    virtual double get(int, int, int) const;

private:
    double value;
};

/// Test pattern generator

class test : public image
{
public:
    virtual double get(int, int, int) const;

    virtual int geth() const;
    virtual int getw() const;
    virtual int getd() const;
};

//------------------------------------------------------------------------------

input::input(std::string a, int h, int w, int d, char t) : file(0)
{
    switch (t)
    {
        case 'b': file = new rawb(a, h, w, d, false); break;
        case 'c': file = new rawc(a, h, w, d, false); break;
        case 'u': file = new rawu(a, h, w, d, false); break;
        case 'U': file = new rawU(a, h, w, d, false); break;
        case 's': file = new raws(a, h, w, d, false); break;
        case 'S': file = new rawS(a, h, w, d, false); break;
        case 'l': file = new rawl(a, h, w, d, false); break;
        case 'L': file = new rawL(a, h, w, d, false); break;
        case 'i': file = new rawi(a, h, w, d, false); break;
        case 'I': file = new rawI(a, h, w, d, false); break;
        case 'f': file = new rawf(a, h, w, d, false); break;
        case 'F': file = new rawF(a, h, w, d, false); break;
        case 'd': file = new rawd(a, h, w, d, false); break;
        case 'D': file = new rawD(a, h, w, d, false); break;
    }
}

input::~input()
{
    delete file;
}

double input::get(int i, int j, int k) const
{
    assert(file);

    return (0 <= i && i < file->h &&
            0 <= j && j < file->w &&
            0 <= k && k < file->d) ? file->get(i, j, k) : 0.0;
}

//------------------------------------------------------------------------------

output::output(std::string a, char t, image *H) : image(H), file(0)
{
    assert(head);

    const int h = head->geth();
    const int w = head->getw();
    const int d = head->getd();

    switch (t)
    {
        case 'b': file = new rawb(a, h, w, d, true); break;
        case 'c': file = new rawc(a, h, w, d, true); break;
        case 'u': file = new rawu(a, h, w, d, true); break;
        case 'U': file = new rawU(a, h, w, d, true); break;
        case 's': file = new raws(a, h, w, d, true); break;
        case 'S': file = new rawS(a, h, w, d, true); break;
        case 'l': file = new rawl(a, h, w, d, true); break;
        case 'L': file = new rawL(a, h, w, d, true); break;
        case 'i': file = new rawi(a, h, w, d, true); break;
        case 'I': file = new rawI(a, h, w, d, true); break;
        case 'f': file = new rawf(a, h, w, d, true); break;
        case 'F': file = new rawF(a, h, w, d, true); break;
        case 'd': file = new rawd(a, h, w, d, true); break;
        case 'D': file = new rawD(a, h, w, d, true); break;
    }
}

output::~output()
{
    delete file;
}

double output::get(int i, int j, int k) const
{
    return head->get(i, j, k);
}

void output::go() const
{
    for         (int i = 0; i < geth(); ++i)
        for     (int j = 0; j < getw(); ++j)
            for (int k = 0; k < getd(); ++k)
                file->put(i, j, k, head->get(i, j, k));
}

//------------------------------------------------------------------------------

double offset::get(int i, int j, int k) const
{
    return head->get(i, j, k) + value;
}

double scale::get(int i, int j, int k) const
{
    return head->get(i, j, k) * value;
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

    void    draw();
    void   wheel(int, int);
    void  motion(int, int);
    void  button(int, bool);
    void     key(int, bool, bool);

private:

    GLuint texture;
    GLuint program;
    GLuint vbuffer;
    GLuint varray;

    GLint  u_offset;
    GLint  u_scale;

    int    point_x;             // Current mouse pointer position x
    int    point_y;             // Current mouse pointer position y

    double image_x;             // Current image view position x
    double image_y;             // Current image view position y
    double image_z;             // Current image view zoom

    double cache_x;             // Cached image view position x
    double cache_y;             // Cached image view position y
    double cache_z;             // Cached image view zoom

    bool   dragging;            // Is a drag in progress?
    int    drag_point_x;        // Mouse pointer position at beginning of drag
    int    drag_point_y;        // Mouse pointer position at beginning of drag
    double drag_image_x;        // Image view position at beginning of drag
    double drag_image_y;        // Image view position at beginning of drag

    image *src;

    void refresh();
    void  center();
};

rawk::rawk(image *src) : demonstration("RAWK", 1280, 720), src(src)
{
    glClearColor(0.1f, 0.1f, 0.1f, 0.0f);

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
            -1.0f, -1.0f, 0.0f, 0.0f,
             1.0f, -1.0f, 1.0f, 0.0f,
            -1.0f,  1.0f, 0.0f, 1.0f,
             1.0f,  1.0f, 1.0f, 1.0f,
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

        glVertexAttribPointer(p, 2, GL_FLOAT, GL_FALSE, 16, (const void *) 0);
        glVertexAttribPointer(t, 2, GL_FLOAT, GL_FALSE, 16, (const void *) 8);

        // Locate the program uniforms.

        u_offset = glGetUniformLocation(program, "offset");
        u_scale  = glGetUniformLocation(program, "scale");
    }

    dragging = false;
    point_x  = 0;
    point_y  = 0;

    center();
    refresh();
}

rawk::~rawk()
{
    glDeleteProgram(program);
    glDeleteBuffers (1, &vbuffer);
    glDeleteTextures(1, &texture);
    glDeleteVertexArrays(1, &varray);
}

static inline int toint(double d)
{
    double f = floor(d);
    double c =  ceil(d);
    return (c - d < d - f) ? int(c) : int(f);
}

void rawk::center()
{
    const int h = src->geth();
    const int w = src->getw();

    if (double(width) / double(height) < double(w) / double(h))
        image_z = double(w) / double(width);
    else
        image_z = double(h) / double(height);

    image_x = w / 2;
    image_y = h / 2;
}

void rawk::refresh()
{
    std::vector<GLfloat> pixel(width * height * 3, 0);

    cache_x = image_x;
    cache_y = image_y;
    cache_z = image_z;

    const int depth = std::min(src->getd(), 3);

    for         (int r = 0; r < height; ++r)
        for     (int c = 0; c < width;  ++c)
            for (int k = 0; k < depth;  ++k)
            {
                int i = toint(cache_y + (r - height / 2) * cache_z);
                int j = toint(cache_x + (c - width  / 2) * cache_z);
                pixel[(r * width + c) * 3 + k] = src->get(i, j, k);
            }

    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height,
                    GL_RGB, GL_FLOAT, &pixel.front());
}

void rawk::draw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    double x = 2.0 * (cache_x - image_x) / cache_z / width;
    double y = 2.0 * (cache_y - image_y) / cache_z / height;
    double z =        cache_z / image_z;

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

    image_z = exp(log(image_z) - dy / 100.0);

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
            case SDL_SCANCODE_SPACE:
                refresh();
                break;

            case SDL_SCANCODE_RETURN:
                center();
                break;
        }
    }
}

//------------------------------------------------------------------------------

int main(int argc, char **argv)
{
    try
    {
        image *src = new offset(0.5,
                                new scale(2.0,
                                          new input("megt00n090hb.img", 5632, 11520, 1, 'S')));
        // image *src = new test();

        rawk app(src);
        app.run(true);

        delete(src);

        return 0;
    }
    catch (std::exception &e)
    {
        fprintf(stderr, "%s\n", e.what());
        return 1;
    }
}
