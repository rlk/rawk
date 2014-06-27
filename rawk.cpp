#include "GL/GLFundamentals.hpp"
#include "GL/GLDemonstration.hpp"

#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

#include "raw.hpp"
#include "image.hpp"

#include "image_bias.hpp"
#include "image_channel.hpp"
#include "image_cubic.hpp"
#include "image_flatten.hpp"
#include "image_gradient.hpp"
#include "image_input.hpp"
#include "image_linear.hpp"
#include "image_nearest.hpp"
#include "image_offset.hpp"
#include "image_output.hpp"
#include "image_paste.hpp"
#include "image_scale.hpp"
#include "image_solid.hpp"
#include "image_sum.hpp"
#include "image_trim.hpp"

//------------------------------------------------------------------------------

struct state
{
    double x;
    double y;
    double z;
};

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

    // OpenGL state

    void init_vbuffer();
    void init_program(std::string, std::string);
    void init_texture();

    GLuint varray;
    GLuint vbuffer;
    GLuint program;
    GLuint texture;

    GLint  u_offset;
    GLint  u_scale;
    GLint  u_recolor;

    // Interaction state

    int    selector;
    bool   dragging;

    int    point_x;
    int    point_y;
    int    click_x;
    int    click_y;

    // Current and stored view configurations

    state  curr_state;
    image *curr_image;
    state  mark_state[12];
    image *mark_image[12];

    state cache_state;
    state click_state;

    image *doL(image *);
    image *doR(image *);
    image *doU(image *);
    image *doD(image *);

    void   doL();
    void   doR();
    void   doU();
    void   doD();

    void   refresh();
    void   retitle();

    void showrgb();
    void showrrr();
    void showyyy();
};

//------------------------------------------------------------------------------

rawk::rawk(image *p) : demonstration("RAWK", 1280, 720), program(0)
{
    // Initialize the OpenGL state.

    glClearColor(0.1f, 0.1f, 0.1f, 0.0f);

    init_vbuffer();
    init_program("rawk.vert", "rawk.frag");
    init_texture();

    // Initialize the application state.

    dragging =  false;
    selector = -1;
    point_x  =  0;
    point_y  =  0;

    // Compute the default view state.

    const int h = p->geth();
    const int w = p->getw();

    if (double(width) / double(height) < double(w) / double(h))
        curr_state.z = double(w) / double(width);
    else
        curr_state.z = double(h) / double(height);

    curr_state.x = w * 0.5 - 0.5;
    curr_state.y = h * 0.5 - 0.5;
    curr_image   = p;

    // Copy the default state to all state registers.

    for (int i = 0; i < 12; i++)
    {
        mark_state[i] = curr_state;
        mark_image[i] = curr_image;
    }

    // Initialize the cache.

    refresh();
}

rawk::~rawk()
{
    glDeleteProgram(program);

    glDeleteBuffers     (1, &vbuffer);
    glDeleteTextures    (1, &texture);
    glDeleteVertexArrays(1, &varray);
}

//------------------------------------------------------------------------------

// Initialize a vertex buffer object with a screen-filling rectangle.

void rawk::init_vbuffer()
{
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
}

/// Initialize an OpenGL program object with the named vertex and fragment
/// shaders. Assume the vertex buffer has already been initialized.

void rawk::init_program(std::string vert, std::string frag)
{
    // Release the previous program object, if any.

    if (program) glDeleteProgram(program);

    // Determine the full path to the shader sources.

    if (const char *str = getenv("RAWK_SHADER_PATH"))
    {
        vert = std::string(str) + "/" + vert;
        frag = std::string(str) + "/" + frag;
    }

    // Initialize the GLSL program object.

    if ((program = gl::init_program(vert.c_str(), frag.c_str())))
    {
        glUseProgram(program);

        // Bind the vertex buffer object to the program attributes.

        GLint p = glGetAttribLocation(program, "vPosition");
        GLint t = glGetAttribLocation(program, "vTexCoord");

        glEnableVertexAttribArray(p);
        glEnableVertexAttribArray(t);

        glVertexAttribPointer(p, 2, GL_FLOAT, GL_FALSE, 16, (const void *) 0);
        glVertexAttribPointer(t, 2, GL_FLOAT, GL_FALSE, 16, (const void *) 8);

        // Locate the program uniforms.

        u_offset  = glGetUniformLocation(program, "offset");
        u_scale   = glGetUniformLocation(program, "scale");
        u_recolor = glGetUniformLocation(program, "recolor");

        showrgb();
    }
}

/// Initialize a texture object for use as image cache.

void rawk::init_texture()
{
    glGenTextures(1, &texture);

    glBindTexture  (GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0,
                                   GL_RGB, GL_FLOAT, 0);
}

//------------------------------------------------------------------------------

void rawk::showrgb()
{
    glUniformMatrix3fv(u_recolor, 1, GL_TRUE, gl::mat3(1, 0, 0,
                                                       0, 1, 0,
                                                       0, 0, 1));
}

void rawk::showrrr()
{
    glUniformMatrix3fv(u_recolor, 1, GL_TRUE, gl::mat3(1, 0, 0,
                                                       1, 0, 0,
                                                       1, 0, 0));
}

void rawk::showyyy()
{
    glUniformMatrix3fv(u_recolor, 1, GL_TRUE, gl::mat3(0.299, 0.587, 0.114,
                                                       0.299, 0.587, 0.114,
                                                       0.299, 0.587, 0.114));
}

//------------------------------------------------------------------------------

/// Traverse the node hierarchy or tweak an image parameter left.

image *rawk::doL(image *p)
{
    const SDL_Keymod mod = SDL_GetModState();

    if      (mod == KMOD_NONE)  p->tweak(0, -1);
    else if (mod &  KMOD_SHIFT) p->tweak(0, -10);
    else if (mod &  KMOD_GUI && p->getL()) p = p->getL();

    return p;
}

/// Traverse the node hierarchy or tweak an image parameter right.

image *rawk::doR(image *p)
{
    const SDL_Keymod mod = SDL_GetModState();

    if      (mod == KMOD_NONE)  p->tweak(0, +1);
    else if (mod &  KMOD_SHIFT) p->tweak(0, +10);
    else if (mod &  KMOD_GUI && p->getR()) p = p->getR();

    return p;
}

/// Traverse the node hierarchy or tweak an image parameter up.

image *rawk::doU(image *p)
{
    const SDL_Keymod mod = SDL_GetModState();

    if      (mod == KMOD_NONE)  p->tweak(1, -1);
    else if (mod &  KMOD_SHIFT) p->tweak(1, -10);
    else if (mod &  KMOD_GUI && p->getP()) p = p->getP();

    return p;
}

/// Traverse the node hierarchy or tweak an image parameter down.

image *rawk::doD(image *p)
{
    const SDL_Keymod mod = SDL_GetModState();

    if      (mod == KMOD_NONE)  p->tweak(1, +1);
    else if (mod &  KMOD_SHIFT) p->tweak(1, +10);

    return p;
}

//------------------------------------------------------------------------------

/// Apply left arrow key press to a current or marked image node.

void rawk::doL()
{
    if (selector < 0)
        curr_image = doL(curr_image);
    else
        mark_image[selector] = doL(mark_image[selector]);
}

/// Apply right arrow key press to a current or marked image node.

void rawk::doR()
{
    if (selector < 0)
        curr_image = doR(curr_image);
    else
        mark_image[selector] = doR(mark_image[selector]);
}

/// Apply up arrow key press to a current or marked image node.

void rawk::doU()
{
    if (selector < 0)
        curr_image = doU(curr_image);
    else
        mark_image[selector] = doU(mark_image[selector]);
}

/// Apply down arrow key press to a current or marked image node.

void rawk::doD()
{
    if (selector < 0)
        curr_image = doD(curr_image);
    else
        mark_image[selector] = doD(mark_image[selector]);
}

//------------------------------------------------------------------------------

/// Handle mouse pointer motion.

void rawk::motion(int x, int y)
{
    point_x = x;
    point_y = y;

    if (dragging)
    {
        curr_state.x = click_state.x + (click_x - x) * curr_state.z;
        curr_state.y = click_state.y + (click_y - y) * curr_state.z;
    }
}

/// Handle mouse button press and release.

void rawk::button(int button, bool down)
{
    if (button == SDL_BUTTON_LEFT)
    {
        dragging    = down;
        click_state = curr_state;
        click_x     = point_x;
        click_y     = point_y;
    }
}

/// Handle mouse wheel rotation.

void rawk::wheel(int dx, int dy)
{
    // Compute pointer position relativate to the center of the screen.

    double xx = point_x - 0.5 * width;
    double yy = point_y - 0.5 * height;

    // Compute the image pixel on which the pointer lies.

    double x = curr_state.x + xx * curr_state.z;
    double y = curr_state.y + yy * curr_state.z;

    // Compute a new zoom level.

    curr_state.z = exp(log(curr_state.z) - dy / 100.0);

    // Compute image offsets to ensure the pointer remains over the same pixel.

    curr_state.x = x - xx * curr_state.z;
    curr_state.y = y - yy * curr_state.z;
}

/// Handle a key press or release event.

void rawk::key(int key, bool down, bool repeat)
{
    if (repeat == false)
    {
        if (down)
        {
            switch (key)
            {
                case SDL_SCANCODE_SPACE:
                    if (selector >= 0)
                    {
                        curr_state = mark_state[selector];
                        curr_image = mark_image[selector];
                    }
                    refresh();
                    break;

                case SDL_SCANCODE_RETURN:
                    if (selector >= 0)
                    {
                        mark_state[selector] = curr_state;
                        mark_image[selector] = curr_image;
                    }
                    break;

                case SDL_SCANCODE_1: showrrr(); break;
                case SDL_SCANCODE_2: showyyy(); break;
                case SDL_SCANCODE_3: showrgb(); break;
            }
        }

        switch (key)
        {
            case SDL_SCANCODE_F1:  selector = (down ?  0 : -1); break;
            case SDL_SCANCODE_F2:  selector = (down ?  1 : -1); break;
            case SDL_SCANCODE_F3:  selector = (down ?  2 : -1); break;
            case SDL_SCANCODE_F4:  selector = (down ?  3 : -1); break;
            case SDL_SCANCODE_F5:  selector = (down ?  4 : -1); break;
            case SDL_SCANCODE_F6:  selector = (down ?  5 : -1); break;
            case SDL_SCANCODE_F7:  selector = (down ?  6 : -1); break;
            case SDL_SCANCODE_F8:  selector = (down ?  7 : -1); break;
            case SDL_SCANCODE_F9:  selector = (down ?  8 : -1); break;
            case SDL_SCANCODE_F10: selector = (down ?  9 : -1); break;
            case SDL_SCANCODE_F11: selector = (down ? 10 : -1); break;
            case SDL_SCANCODE_F12: selector = (down ? 11 : -1); break;
        }
    }

    if (down)
    {
        switch (key)
        {
            case SDL_SCANCODE_LEFT:  doL(); break;
            case SDL_SCANCODE_RIGHT: doR(); break;
            case SDL_SCANCODE_UP:    doU(); break;
            case SDL_SCANCODE_DOWN:  doD(); break;
        }
    }
}

//------------------------------------------------------------------------------

static inline int toint(double d)
{
    double f = floor(d);
    double c =  ceil(d);
    return (c - d < d - f) ? int(c) : int(f);
}

/// Write the current view configuration to the window's title bar.

void rawk::retitle()
{
    std::ostringstream stream;

    // Determine the active image.

    image *p = (selector < 0) ? curr_image : mark_image[selector];

    // Document the current image. Parentheses indicate temporary selection.

    if (selector < 0)
        stream        << p->doc();
    else
        stream << "(" << p->doc() << ")";

    // Include the current pointer position.

    int i = int(0.5 + curr_state.y + (point_y - 0.5 * height) * curr_state.z);
    int j = int(0.5 + curr_state.x + (point_x - 0.5 * width)  * curr_state.z);

    stream << " (" << i << ", " << j << ") = ";

    // Include the pixel value at the current pointer position.

    stream << std::setprecision(3);

    for (int k = 0; k < p->getd(); k++)
        stream << (k ? "/" : "") << p->get(i, j, k);

    // Update the window title.

    SDL_SetWindowTitle(window, stream.str().c_str());
}

/// Update the contents of the image cache.

void rawk::refresh()
{
    std::vector<GLfloat> pixel(width * height * 3, 0);

    const int depth = std::min(curr_image->getd(), 3);

    int r;
    int c;
    int k;

    #pragma omp parallel for private(c, k)
    for         (r = 0; r < height; ++r)
        for     (c = 0; c < width;  ++c)
            for (k = 0; k < depth;  ++k)
            {
                int i = toint(curr_state.y + (r - height / 2) * curr_state.z);
                int j = toint(curr_state.x + (c - width  / 2) * curr_state.z);
                pixel[(r * width + c) * 3 + k] = curr_image->get(i, j, k);
            }

    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height,
                    GL_RGB, GL_FLOAT, &pixel.front());

    cache_state = curr_state;
}

/// Render the contents of the image cache to the screen.

void rawk::draw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    double x = 2.0 * (cache_state.x - curr_state.x) / cache_state.z / width;
    double y = 2.0 * (cache_state.y - curr_state.y) / cache_state.z / height;
    double z =        cache_state.z / curr_state.z;

    glUniform1f(u_scale,  z);
    glUniform2f(u_offset, x, y);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    retitle();
}

//------------------------------------------------------------------------------

static output *out = 0;

static image *parse(int& i, char **v)
{
    if (v[i])
    {
        std::string op(v[i++]);

        if (op == "bias")
        {
            double d = strtod(v[i++], 0);
            image *p = parse(i, v);
            return new bias(d, p);
        }

        if (op == "channel")
        {
            int    k = int(strtol(v[i++], 0, 0));
            image *p = parse(i, v);
            return new channel(k, p);
        }

        if (op == "cubic")
        {
            int    h = int(strtol(v[i++], 0, 0));
            int    w = int(strtol(v[i++], 0, 0));
            int    m = int(strtol(v[i++], 0, 0));
            image *p = parse(i, v);
            return new cubic(h, w, m, p);
        }

        if (op == "flatten")
        {
            double d = strtod(v[i++], 0);
            image *p = parse(i, v);
            return new flatten(d, p);
        }

        if (op == "gradient")
        {
            int    m = int(strtol(v[i++], 0, 0));
            image *p = parse(i, v);
            return new gradient(m, p);
        }

        if (op == "input")
        {
            char  *a = v[i++];
            int    o = int(strtol(v[i++], 0, 0));
            int    h = int(strtol(v[i++], 0, 0));
            int    w = int(strtol(v[i++], 0, 0));
            int    d = int(strtol(v[i++], 0, 0));
            char   t = v[i++][0];
            return new input(a, o, h, w, d, t);
        }

        if (op == "linear")
        {
            int    h = int(strtol(v[i++], 0, 0));
            int    w = int(strtol(v[i++], 0, 0));
            int    m = int(strtol(v[i++], 0, 0));
            image *p = parse(i, v);
            return new linear(h, w, m, p);
        }

        if (op == "nearest")
        {
            int    h = int(strtol(v[i++], 0, 0));
            int    w = int(strtol(v[i++], 0, 0));
            image *p = parse(i, v);
            return new nearest(h, w, p);
        }

        if (op == "offset")
        {
            int    r = int(strtol(v[i++], 0, 0));
            int    c = int(strtol(v[i++], 0, 0));
            int    w = int(strtol(v[i++], 0, 0));
            image *p = parse(i, v);
            return new offset(r, c, w, p);
        }

        if (op == "output")
        {
            char  *a = v[i++];
            char   t = v[i++][0];
            image *p = parse(i, v);
            return (out = new output(a, t, p));
        }

        if (op == "paste")
        {
            int    r = int(strtol(v[i++], 0, 0));
            int    c = int(strtol(v[i++], 0, 0));
            image *L = parse(i, v);
            image *R = parse(i, v);
            return new paste(r, c, L, R);
        }

        if (op == "scale")
        {
            double d = strtod(v[i++], 0);
            image *p = parse(i, v);
            return new scale(d, p);
        }

        if (op == "solid")
        {
            int    h = int(strtol(v[i++], 0, 0));
            int    w = int(strtol(v[i++], 0, 0));
            double d = strtod(v[i++], 0);
            return new solid(h, w, d);
        }

        if (op == "sum")
        {
            image *L = parse(i, v);
            image *R = parse(i, v);
            return new sum(L, R);
        }

        if (op == "trim")
        {
            int    h = int(strtol(v[i++], 0, 0));
            int    w = int(strtol(v[i++], 0, 0));
            image *p = parse(i, v);
            return new trim(h, w, p);
        }
    }
    throw std::runtime_error("Expected image argument");
    return 0;
}

int main(int argc, char **argv)
{
    try
    {
        int argi = 1;

        image *in = parse(argi, argv);

        if (out)
            out->go();
        else
        {
            rawk app(in);
            app.run(true);
        }
        delete in;
        return 0;
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}
