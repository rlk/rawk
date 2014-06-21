#include "GL/GLFundamentals.hpp"
#include "GL/GLDemonstration.hpp"

#include "img.hpp"

#include <iostream>
#include <vector>

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

void dump(image *p, int d)
{
    for (int i = 0; i < d; ++i)
        std::cout << "    ";

    std::cout << p->doc() << std::endl;

    if (p->gethead()) dump(p->gethead(), d + 1);
    if (p->gettail()) dump(p->gettail(), d + 1);
}

image *parse(int& i, char **v)
{
    if (v[i])
    {
        std::string op(v[i++]);

        if (op == "offset")
        {
            double d = strtod(v[i++], 0);
            image *p = parse(i, v);
            return new offset(d, p);
        }

        if (op == "scale")
        {
            double d = strtod(v[i++], 0);
            image *p = parse(i, v);
            return new scale(d, p);
        }

        if (op == "paste")
        {
            int    r = int(strtol(v[i++], 0, 0));
            int    c = int(strtol(v[i++], 0, 0));
            image *H = parse(i, v);
            image *T = parse(i, v);
            return new paste(r, c, H, T);
        }

        if (op == "input")
        {
            char  *a = v[i++];
            int    h = int(strtol(v[i++], 0, 0));
            int    w = int(strtol(v[i++], 0, 0));
            int    d = int(strtol(v[i++], 0, 0));
            char   t = v[i++][0];
            return new input(a, h, w, d, t);
        }

        if (op == "output")
        {
            char  *a = v[i++];
            char   t = v[i++][0];
            image *H = parse(i, v);
            return new output(a, t, H);
        }

        if (op == "test")
            return new test();
    }
    throw std::runtime_error("Expected image argument");
    return 0;
}

//------------------------------------------------------------------------------

int main(int argc, char **argv)
{
    try
    {
        int argi = 1;
        image *src = parse(argi, argv);

        dump(src, 0);

        rawk app(src);
        app.run(true);

        delete(src);

        return 0;
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}
