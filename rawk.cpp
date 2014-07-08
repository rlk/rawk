#include "GL/GLFundamentals.hpp"
#include "GL/GLDemonstration.hpp"

#include <stdexcept>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <limits>

#include <sys/time.h>

#ifdef _OPENMP
#include <omp.h>
#else
int omp_get_thread_num() { return 0; }
#endif

#include "raw.hpp"
#include "image.hpp"

class rawk;

#include "image_append.hpp"
#include "image_arithmetic.hpp"
#include "image_bias.hpp"
#include "image_blend.hpp"
#include "image_choose.hpp"
#include "image_convolve.hpp"
#include "image_crop.hpp"
#include "image_flatten.hpp"
#include "image_function.hpp"
#include "image_gain.hpp"
#include "image_input.hpp"
#include "image_matrix.hpp"
#include "image_median.hpp"
#include "image_morphology.hpp"
#include "image_normalize.hpp"
#include "image_offset.hpp"
#include "image_output.hpp"
#include "image_paste.hpp"
#include "image_reduce.hpp"
#include "image_resample.hpp"
#include "image_sobel.hpp"
#include "image_solid.hpp"
#include "image_swizzle.hpp"
#include "image_threshold.hpp"
#include "image_trim.hpp"

//------------------------------------------------------------------------------

/// Command line parsing error

class parse_error : public std::runtime_error
{
public :
    parse_error(const char *a, const char *b)
        : std::runtime_error(message(a, b)) { }

    std::string message(const char *a, const char *b)
    {
        if (a)
            return "Expected " + std::string(b) + " but got '" + std::string(a) + "'";
        else
            return "Expected " + std::string(b) + " but got nothing";
    }
};

char *parse_string(int &i, char **v)
{
    if (v[i])
        return v[i++];
    throw parse_error(v[i], "a string");
}

double parse_double(int& i, char **v)
{
    if (v[i])
    {
        char *e;
        double d = strtod(v[i], &e);
        if (e[0] == 0)
        {
            i++;
            return d;
        }
    }
    throw parse_error(v[i], "a floating point value");
}

int parse_int(int& i, char **v)
{
    if (v[i])
    {
        char *e;
        long l = strtol(v[i], &e, 0);
        if (e[0] == 0)
        {
            i++;
            return l;
        }
    }
    throw parse_error(v[i], "an integer value");
}

int parse_wrap(int& i, char **v)
{
    if (v[i])
    {
        if (v[i][0] && !v[i][1])
        {
            switch (v[i][0])
            {
                case '0':
                case '1':
                case '2':
                case '3': return v[i++][0] - '0';
            }
        }
    }
    throw parse_error(v[i], "a wrap mode (0123)");
}

char parse_type(int& i, char **v)
{
    if (v[i])
    {
        if (v[i][0] && !v[i][1])
        {
            switch (v[i][0])
            {
                case 'b':
                case 'c':
                case 'u': case 'U':
                case 's': case 'S':
                case 'l': case 'L':
                case 'i': case 'I':
                case 'f': case 'F':
                case 'd': case 'D': return v[i++][0];
            }
        }
    }
    throw parse_error(v[i], "a data type token (ucuslifdUSLIFD)");
}

image *parse_image(int& i, char **v)
{
    if (v[i])
    {
        std::string op(v[i++]);

        if (op == "absolute")
        {
            image *L = parse_image(i, v);
            return new absolute(L);
        }

        if (op == "append")
        {
            image *L = parse_image(i, v);
            image *R = parse_image(i, v);
            return new append(L, R);
        }

        if (op == "bias")
        {
            double d = parse_double(i, v);
            image *L = parse_image(i, v);
            return new bias(d, L);
        }

        if (op == "blend")
        {
            image *L = parse_image(i, v);
            image *R = parse_image(i, v);
            return new blend(L, R);
        }

        if (op == "choose")
        {
            int    n = parse_int(i, v);
            image *L = parse_image(i, v);
            image *R = parse_image(i, v);
            return new choose(n, L, R);
        }

        if (op == "crop")
        {
            int    r = parse_int(i, v);
            int    c = parse_int(i, v);
            int    h = parse_int(i, v);
            int    w = parse_int(i, v);
            image *L = parse_image(i, v);
            return new crop(r, c, h, w, L);
        }

        if (op == "cubic")
        {
            int    h = parse_int(i, v);
            int    w = parse_int(i, v);
            int    m = parse_wrap(i, v);
            image *L = parse_image(i, v);
            return new cubic(h, w, m, L);
        }

        if (op == "difference")
        {
            image *L = parse_image(i, v);
            image *R = parse_image(i, v);
            return new difference(L, R);
        }

        if (op == "dilate")
        {
            int    r = parse_int(i, v);
            int    m = parse_wrap(i, v);
            image *L = parse_image(i, v);
            return new dilate(r, m, L);
        }

        if (op == "erode")
        {
            int    r = parse_int(i, v);
            int    m = parse_wrap(i, v);
            image *L = parse_image(i, v);
            return new erode(r, m, L);
        }

        if (op == "flatten")
        {
            double d = parse_double(i, v);
            image *L = parse_image(i, v);
            return new flatten(d, L);
        }

        if (op == "gain")
        {
            double d = parse_double(i, v);
            image *L = parse_image(i, v);
            return new gain(d, L);
        }

        if (op == "gaussian")
        {
            double d = parse_double(i, v);
            int    m = parse_wrap(i, v);
            image *L = parse_image(i, v);
            return new gaussian(d, m, L);
        }

        if (op == "gaussianh")
        {
            double d = parse_double(i, v);
            int    m = parse_wrap(i, v);
            image *L = parse_image(i, v);
            return new gaussianh(d, m, L);
        }

        if (op == "gaussianv")
        {
            double d = parse_double(i, v);
            int    m = parse_wrap(i, v);
            image *L = parse_image(i, v);
            return new gaussianv(d, m, L);
        }

        if (op == "gradient")
        {
            int    m = parse_wrap(i, v);
            image *L = parse_image(i, v);
            return new gradient(m, L);
        }

        if (op == "input")
        {
            char  *a = parse_string(i, v);
            int    o = parse_int(i, v);
            int    h = parse_int(i, v);
            int    w = parse_int(i, v);
            int    d = parse_int(i, v);
            char   t = parse_type(i, v);
            return new input(a, o, h, w, d, t);
        }

        if (op == "linear")
        {
            int    h = parse_int(i, v);
            int    w = parse_int(i, v);
            int    m = parse_wrap(i, v);
            image *L = parse_image(i, v);
            return new linear(h, w, m, L);
        }

        if (op == "median")
        {
            int    r = parse_int(i, v);
            int    m = parse_wrap(i, v);
            image *L = parse_image(i, v);
            return new median(r, m, L);
        }

        if (op == "medianh")
        {
            int    r = parse_int(i, v);
            int    m = parse_wrap(i, v);
            image *L = parse_image(i, v);
            return new medianh(r, m, L);
        }

        if (op == "medianv")
        {
            int    r = parse_int(i, v);
            int    m = parse_wrap(i, v);
            image *L = parse_image(i, v);
            return new medianv(r, m, L);
        }

        if (op == "multiply")
        {
            image *L = parse_image(i, v);
            image *R = parse_image(i, v);
            return new multiply(L, R);
        }

        if (op == "nearest")
        {
            int    h = parse_int(i, v);
            int    w = parse_int(i, v);
            image *L = parse_image(i, v);
            return new nearest(h, w, L);
        }

        if (op == "normalize")
        {
            image *L = parse_image(i, v);
            return new normalize(L);
        }

        if (op == "offset")
        {
            int    r = parse_int(i, v);
            int    c = parse_int(i, v);
            int    w = parse_int(i, v);
            image *L = parse_image(i, v);
            return new offset(r, c, w, L);
        }

        if (op == "output")
        {
            char  *a = parse_string(i, v);
            char   t = parse_type (i, v);
            image *L = parse_image(i, v);
            return new output(a, t, L);
        }

        if (op == "paste")
        {
            int    r = parse_int(i, v);
            int    c = parse_int(i, v);
            image *L = parse_image(i, v);
            image *R = parse_image(i, v);
            return new paste(r, c, L, R);
        }

        if (op == "reduce")
        {
            image *L = parse_image(i, v);
            return new reduce(L);
        }

        if (op == "rgb2yuv")
        {
            image *L = parse_image(i, v);
            return new rgb2yuv(L);
        }

        if (op == "sobelx")
        {
            int    m = parse_wrap(i, v);
            image *L = parse_image(i, v);
            return new sobelx(m, L);
        }

        if (op == "sobely")
        {
            int    m = parse_wrap(i, v);
            image *L = parse_image(i, v);
            return new sobely(m, L);
        }

        if (op == "solid")
        {
            int    h = parse_int(i, v);
            int    w = parse_int(i, v);
            double d = parse_double(i, v);
            return new solid(h, w, d);
        }

        if (op == "sum")
        {
            image *L = parse_image(i, v);
            image *R = parse_image(i, v);
            return new sum(L, R);
        }

        if (op == "swizzle")
        {
            char  *m = v[i++];
            image *L = parse_image(i, v);
            return new swizzle(m, L);
        }

        if (op == "threshold")
        {
            double d = parse_double(i, v);
            image *L = parse_image(i, v);
            return new threshold(d, L);
        }

        if (op == "trim")
        {
            int    h = parse_int(i, v);
            int    w = parse_int(i, v);
            image *L = parse_image(i, v);
            return new trim(h, w, L);
        }

        if (op == "yuv2rgb")
        {
            image *L = parse_image(i, v);
            return new yuv2rgb(L);
        }

        i--;
    }
    throw parse_error(v[i], "an image process command");
}

//------------------------------------------------------------------------------

/// View position and zoom

struct state
{
    double x;      ///< Position of the pixel at the center of the view
    double y;      ///< Position of the pixel at the center of the view
    double z;      ///< View zoom factor

    void center(image *p, int width, int height)
    {
        const int h = p->get_height();
        const int w = p->get_width();

        if (double(width) / double(height) < double(w) / double(h))
            z = double(w) / double(width);
        else
            z = double(h) / double(height);

        x = w * 0.5 - 0.5;
        y = h * 0.5 - 0.5;
    }
};

/// RAWK application

class rawk : public gl::demonstration
{
public:
    rawk(image *, double, double, double);
   ~rawk();

    void   draw();
    void  wheel(int, int);
    void motion(int, int);
    void button(int, bool);
    void    key(int, bool, bool);

private:

    // OpenGL state

    void init_vbuffer();
    void init_program(std::string, std::string);
    void init_texture();

    GLuint varray;                 ///< GL vertex array object
    GLuint vbuffer;                ///< GL vertex buffer object
    GLuint program;                ///< GL program object
    GLuint texture;                ///< GL texture object

    GLint  u_offset;               ///< Offset uniform location
    GLint  u_scale;                ///< Scale uniform location
    GLint  u_zoom;                 ///< Zoom uniform location

    // Interaction state

    int    selector;               ///< Currently-held Function key (-1 if none)
    bool   dragging;               ///< Mouse drag in progress?

    int    point_x;                ///< Pointer position at last mouse motion
    int    point_y;                ///< Pointer position at last mouse motion
    int    click_x;                ///< Pointer position at last mouse click
    int    click_y;                ///< Pointer position at last mouse click

    // Current and stored view configurations

    image               *root_image;
    state                temp_state;
    state                down_state;
    state                curr_state;
    image               *curr_image;
    std::vector<GLfloat> curr_cache;
    state                mark_state[12];
    image               *mark_image[12];
    std::vector<GLfloat> mark_cache[12];

    image *doL(image *);
    image *doR(image *);
    image *doU(image *);
    image *doD(image *);

    void   doL();
    void   doR();
    void   doU();
    void   doD();

    void refresh();
    void retitle();

    void cache_row(image *, state *, int, int);

    void zerocache(int selector=-1);
    void showcache(int selector=-1);
};

//------------------------------------------------------------------------------

rawk::rawk(image *p, double x, double y, double z)
    : demonstration("RAWK", 1280, 720), program(0), curr_cache(width * height * 3)
{
    // Initialize the OpenGL state.

    glClearColor(0.1f, 0.1f, 0.1f, 0.0f);

    init_vbuffer();
    init_program("rawk.vert", "rawk_rgb.frag");
    init_texture();

    // Initialize the application state.

    dragging =  false;
    selector = -1;
    point_x  =  0;
    point_y  =  0;

    // Initialize the cached and marked view states and images.

    curr_image = p;

    curr_state.center(curr_image, width, height);

    if (x) curr_state.x = x;
    if (y) curr_state.y = y;
    if (z) curr_state.z = z;

    refresh();

    for (int i = 0; i < 12; i++)
    {
        mark_state[i] = curr_state;
        mark_image[i] = curr_image;
        mark_cache[i] = curr_cache;
    }
}

rawk::~rawk()
{
    delete root_image;

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
        u_zoom    = glGetUniformLocation(program, "zoom");
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
        curr_state.x = down_state.x + (click_x - x) * curr_state.z;
        curr_state.y = down_state.y + (click_y - y) * curr_state.z;
    }
}

/// Handle mouse button press and release.

void rawk::button(int button, bool down)
{
    if (button == SDL_BUTTON_LEFT)
    {
        dragging    = down;
        down_state = curr_state;
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
            if (SDL_GetModState() & KMOD_SHIFT)
                switch (key)
                {
                    case SDL_SCANCODE_1:
                        init_program("rawk.vert", "rawk_gray.frag");
                        break;
                    case SDL_SCANCODE_2:
                        init_program("rawk.vert", "rawk_luma.frag");
                        break;
                    case SDL_SCANCODE_3:
                        init_program("rawk.vert", "rawk_rgb.frag");
                        break;
                    case SDL_SCANCODE_4:
                        init_program("rawk.vert", "rawk_relief.frag");
                        break;
                }
            else
                switch (key)
                {
                    case SDL_SCANCODE_SPACE:
                        if (selector >= 0)
                        {
                            curr_state = mark_state[selector];
                            curr_image = mark_image[selector];
                            curr_cache = mark_cache[selector];
                            temp_state = curr_state;
                            showcache();
                        }
                        else refresh();
                        break;

                    case SDL_SCANCODE_RETURN:
                        if (selector >= 0)
                        {
                            mark_state[selector] = curr_state;
                            mark_image[selector] = curr_image;
                            mark_cache[selector] = curr_cache;
                        }
                        break;

                    case SDL_SCANCODE_EQUALS:
                        break;

                    case SDL_SCANCODE_1: curr_state.z =   1; break;
                    case SDL_SCANCODE_2: curr_state.z =   2; break;
                    case SDL_SCANCODE_3: curr_state.z =   4; break;
                    case SDL_SCANCODE_4: curr_state.z =   8; break;
                    case SDL_SCANCODE_5: curr_state.z =  16; break;
                    case SDL_SCANCODE_6: curr_state.z =  32; break;
                    case SDL_SCANCODE_7: curr_state.z =  64; break;
                    case SDL_SCANCODE_8: curr_state.z = 128; break;
                    case SDL_SCANCODE_9: curr_state.z = 256; break;
                }
        }

        switch (key)
        {
            case SDL_SCANCODE_F1:  showcache(selector = (down ?  0 : -1)); break;
            case SDL_SCANCODE_F2:  showcache(selector = (down ?  1 : -1)); break;
            case SDL_SCANCODE_F3:  showcache(selector = (down ?  2 : -1)); break;
            case SDL_SCANCODE_F4:  showcache(selector = (down ?  3 : -1)); break;
            case SDL_SCANCODE_F5:  showcache(selector = (down ?  4 : -1)); break;
            case SDL_SCANCODE_F6:  showcache(selector = (down ?  5 : -1)); break;
            case SDL_SCANCODE_F7:  showcache(selector = (down ?  6 : -1)); break;
            case SDL_SCANCODE_F8:  showcache(selector = (down ?  7 : -1)); break;
            case SDL_SCANCODE_F9:  showcache(selector = (down ?  8 : -1)); break;
            case SDL_SCANCODE_F10: showcache(selector = (down ?  9 : -1)); break;
            case SDL_SCANCODE_F11: showcache(selector = (down ? 10 : -1)); break;
            case SDL_SCANCODE_F12: showcache(selector = (down ? 11 : -1)); break;
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

static double getsecsince(const struct timeval *tv)
{
    struct timeval tw;

    gettimeofday(&tw, 0);

    return double(tw.tv_sec  - tv->tv_sec) +
           double(tw.tv_usec - tv->tv_usec) / 1000000.0;
}

/// Write the current view configuration to the window's title bar.

void rawk::retitle()
{
    std::ostringstream stream;

    // Determine the active image.

    image *p = (selector < 0) ? curr_image : mark_image[selector];

    // Document the current image. Parentheses indicate temporary selection.

    if (selector < 0)
        p->doc(stream);
    else
    {
        stream << "(";
        p->doc(stream);
        stream << ")";
    }

    // Include the current pointer position.

    int i = int(0.5 + curr_state.y + (point_y - 0.5 * height) * curr_state.z);
    int j = int(0.5 + curr_state.x + (point_x - 0.5 * width)  * curr_state.z);

    stream << " (" << i << ", " << j << ") = ";

    // Include the pixel value at the current pointer position.

    stream << std::setprecision(3);

    for (int k = 0; k < p->get_depth(); k++)
        stream << (k ? "/" : "") << p->get(i, j, k);

    // Update the window title.

    SDL_SetWindowTitle(window, stream.str().c_str());
}

void rawk::cache_row(image *p, state *s, int r, int d)
{
    for     (int c = 0; c < width; ++c)
        for (int k = 0; k < d; ++k)
        {
            int i = toint(s->y + (r - height / 2) * s->z);
            int j = toint(s->x + (c - width  / 2) * s->z);
            curr_cache[(r * width + c) * 3 + k] = p->get(i, j, k);
        }
}

/// Update the contents of the image cache.

void rawk::refresh()
{
    if (curr_image)
    {
        temp_state = curr_state;

        struct timeval tv;
        gettimeofday(&tv, 0);

        int r, d = std::min(curr_image->get_depth(), 3);

        zerocache();
        showcache();

        #pragma omp parallel for schedule(dynamic)
        for (r = 0; r < height; ++r)
        {
            cache_row(curr_image, &curr_state, r, d);

            if (omp_get_thread_num() == 0)
            {
                if (getsecsince(&tv) > 0.1)
                {
                    showcache();
                    gettimeofday(&tv, 0);
                }
            }
        }
        showcache();
    }
}

/// Render the contents of the image cache to the screen.

void rawk::draw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    double x = 2.0 * (temp_state.x - curr_state.x) / temp_state.z / width;
    double y = 2.0 * (temp_state.y - curr_state.y) / temp_state.z / height;
    double z =        temp_state.z / curr_state.z;

    glUniform1f(u_zoom, temp_state.z);
    glUniform1f(u_scale,  z);
    glUniform2f(u_offset, x, y);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    retitle();
}

void rawk::zerocache(int selector)
{
    if (selector < 0)
        std::fill(curr_cache.begin(),
                  curr_cache.end(), 0.0f);
    else
        std::fill(mark_cache[selector].begin(),
                  mark_cache[selector].end(), 0.0f);
}

void rawk::showcache(int selector)
{
    if (selector < 0)
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height,
                        GL_RGB, GL_FLOAT, &curr_cache.front());
    else
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height,
                        GL_RGB, GL_FLOAT, &mark_cache[selector].front());
    draw();
    swap();
}

//------------------------------------------------------------------------------

int main(int argc, char **argv)
{
    try
    {
        bool   n = false;
        double x = 0;
        double y = 0;
        double z = 0;

        int c;

        while ((c = getopt(argc, argv, "nx:y:z:")) != -1)
            switch (c)
            {
                case 'n': n = true;              break;
                case 'x': x = strtod(optarg, 0); break;
                case 'y': y = strtod(optarg, 0); break;
                case 'z': z = strtod(optarg, 0); break;
            }

        if (image *p = parse_image(optind, argv))
        {
            if (n)
                p->process();
            else
            {
                rawk app(p, x, y, z);
                app.run(true);
            }
        }
        return 0;
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}
