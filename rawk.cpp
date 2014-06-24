#include "GL/GLFundamentals.hpp"
#include "GL/GLDemonstration.hpp"

#include "img.hpp"

#include <stdexcept>
#include <iostream>
#include <sstream>
#include <vector>

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

    std::string title;

    GLuint texture;
    GLuint program;
    GLuint vbuffer;
    GLuint varray;

    GLint  u_offset;
    GLint  u_scale;

    int    selector;
    bool   dragging;

    int    point_x;
    int    point_y;
    int    click_x;
    int    click_y;

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

    image *retitle(image *, bool=false);
    void   refresh();
    void    select(int);
    void    center(state *, image *);
};

//------------------------------------------------------------------------------

rawk::rawk(image *p) : demonstration("RAWK", 1280, 720)
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

    dragging =  false;
    selector = -1;
    point_x  =  0;
    point_y  =  0;

    for (int i = 0; i < 12; i++)
        center(mark_state + i, (mark_image[i] = p));

    curr_state = mark_state[0];
    curr_image = mark_image[0];
    select(-1);
    refresh();
}

rawk::~rawk()
{
    glDeleteProgram(program);
    glDeleteBuffers (1, &vbuffer);
    glDeleteTextures(1, &texture);
    glDeleteVertexArrays(1, &varray);
}

//------------------------------------------------------------------------------

static inline int toint(double d)
{
    double f = floor(d);
    double c =  ceil(d);
    return (c - d < d - f) ? int(c) : int(f);
}

image *rawk::retitle(image *p, bool temporary)
{
    std::ostringstream stream;

    // Document the current image. Parentheses indicate temporary selection.

    if (temporary)
        stream << "(" << p->doc() << ")";
    else
        stream <<        p->doc();

    // Include the size of this image.

    stream << " ... " << p->geth()
           <<   " x " << p->getw()
           <<   " x " << p->getd();

    // Document the head and tail for ease of navigation.

    stream <<  " ... [ "
           << (p->getL() ? p->getL()->doc() : "NULL")
           << " / "
           << (p->getR() ? p->getR()->doc() : "NULL")
           << " ]";

    title = stream.str();

    return p;
}

void rawk::select(int i)
{
    selector = i;

    if (selector < 0)
        retitle(curr_image, false);
    else
        retitle(mark_image[i], true);
}

void rawk::center(state *s, image *p)
{
    const int h = p->geth();
    const int w = p->getw();

    if (double(width) / double(height) < double(w) / double(h))
        s->z = double(w) / double(width);
    else
        s->z = double(h) / double(height);

    s->x = w / 2;
    s->y = h / 2;
}

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

//------------------------------------------------------------------------------

image *rawk::doL(image *p)
{
    const SDL_Keymod mod = SDL_GetModState();

    if      (mod == KMOD_NONE)  p->tweak(0, -1);
    else if (mod &  KMOD_SHIFT) p->tweak(0, -10);
    else if (mod &  KMOD_GUI && p->getL()) p = p->getL();

    return retitle(p);
}

image *rawk::doR(image *p)
{
    const SDL_Keymod mod = SDL_GetModState();

    if      (mod == KMOD_NONE)  p->tweak(0, +1);
    else if (mod &  KMOD_SHIFT) p->tweak(0, +10);
    else if (mod &  KMOD_GUI && p->getR()) p = p->getR();

    return retitle(p);
}

image *rawk::doU(image *p)
{
    const SDL_Keymod mod = SDL_GetModState();

    if      (mod == KMOD_NONE)  p->tweak(1, -1);
    else if (mod &  KMOD_SHIFT) p->tweak(1, -10);
    else if (mod &  KMOD_GUI && p->getP()) p = p->getP();

    return retitle(p);
}

image *rawk::doD(image *p)
{
    const SDL_Keymod mod = SDL_GetModState();

    if      (mod == KMOD_NONE)  p->tweak(1, +1);
    else if (mod &  KMOD_SHIFT) p->tweak(1, +10);

    return retitle(p);
}

//------------------------------------------------------------------------------

void rawk::doL()
{
    if (selector < 0)
        curr_image = doL(curr_image);
    else
        mark_image[selector] = doL(mark_image[selector]);
}

void rawk::doR()
{
    if (selector < 0)
        curr_image = doR(curr_image);
    else
        mark_image[selector] = doR(mark_image[selector]);
}

void rawk::doU()
{
    if (selector < 0)
        curr_image = doU(curr_image);
    else
        mark_image[selector] = doU(mark_image[selector]);
}

void rawk::doD()
{
    if (selector < 0)
        curr_image = doD(curr_image);
    else
        mark_image[selector] = doD(mark_image[selector]);
}

//------------------------------------------------------------------------------

void rawk::draw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    double x = 2.0 * (cache_state.x - curr_state.x) / cache_state.z / width;
    double y = 2.0 * (cache_state.y - curr_state.y) / cache_state.z / height;
    double z =        cache_state.z / curr_state.z;

    glUniform1f(u_scale,  z);
    glUniform2f(u_offset, x, y);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    std::ostringstream stream;

    int px = int(curr_state.x + (point_x - 0.5 * width)  * curr_state.z);
    int py = int(curr_state.y + (point_y - 0.5 * height) * curr_state.z);

    stream << title << " (" << px << ", " << py << ")";

    SDL_SetWindowTitle(window, stream.str().c_str());
}

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

void rawk::button(int button, bool down)
{
    if (button == SDL_BUTTON_LEFT)
    {
        dragging   = down;
        click_state = curr_state;
        click_x    = point_x;
        click_y    = point_y;
    }
}

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
            }
        }

        switch (key)
        {
            case SDL_SCANCODE_F1:  select(down ?  0 : -1); break;
            case SDL_SCANCODE_F2:  select(down ?  1 : -1); break;
            case SDL_SCANCODE_F3:  select(down ?  2 : -1); break;
            case SDL_SCANCODE_F4:  select(down ?  3 : -1); break;
            case SDL_SCANCODE_F5:  select(down ?  4 : -1); break;
            case SDL_SCANCODE_F6:  select(down ?  5 : -1); break;
            case SDL_SCANCODE_F7:  select(down ?  6 : -1); break;
            case SDL_SCANCODE_F8:  select(down ?  7 : -1); break;
            case SDL_SCANCODE_F9:  select(down ?  8 : -1); break;
            case SDL_SCANCODE_F10: select(down ?  9 : -1); break;
            case SDL_SCANCODE_F11: select(down ? 10 : -1); break;
            case SDL_SCANCODE_F12: select(down ? 11 : -1); break;
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

void dump(image *p, int d)
{
    for (int i = 0; i < d; ++i)
        std::cout << "    ";

    std::cout << p->doc() << std::endl;

    if (p->getL()) dump(p->getL(), d + 1);
    if (p->getR()) dump(p->getR(), d + 1);
}

image *parse(int& i, char **v)
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

        if (op == "scale")
        {
            double d = strtod(v[i++], 0);
            image *p = parse(i, v);
            return new scale(d, p);
        }

        if (op == "offset")
        {
            int    r = int(strtol(v[i++], 0, 0));
            int    c = int(strtol(v[i++], 0, 0));
            int    w = int(strtol(v[i++], 0, 0));
            image *p = parse(i, v);
            return new offset(r, c, w, p);
        }

        if (op == "gradient")
        {
            int    w = int(strtol(v[i++], 0, 0));
            image *p = parse(i, v);
            return new gradient(w, p);
        }

        if (op == "nearest")
        {
            int    h = int(strtol(v[i++], 0, 0));
            int    w = int(strtol(v[i++], 0, 0));
            image *p = parse(i, v);
            return new nearest(h, w, p);
        }

        if (op == "trim")
        {
            int    h = int(strtol(v[i++], 0, 0));
            int    w = int(strtol(v[i++], 0, 0));
            image *p = parse(i, v);
            return new trim(h, w, p);
        }

        if (op == "channel")
        {
            int    k = int(strtol(v[i++], 0, 0));
            image *p = parse(i, v);
            return new channel(k, p);
        }

        if (op == "paste")
        {
            int    r = int(strtol(v[i++], 0, 0));
            int    c = int(strtol(v[i++], 0, 0));
            image *L = parse(i, v);
            image *R = parse(i, v);
            return new paste(r, c, L, R);
        }

        if (op == "sum")
        {
            image *L = parse(i, v);
            image *R = parse(i, v);
            return new sum(L, R);
        }

        if (op == "solid")
        {
            int    h = int(strtol(v[i++], 0, 0));
            int    w = int(strtol(v[i++], 0, 0));
            double d = strtod(v[i++], 0);
            return new solid(h, w, d);
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
            image *p = parse(i, v);
            return new output(a, t, p);
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

        rawk app(parse(argi, argv));
        app.run(true);

        return 0;
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}
