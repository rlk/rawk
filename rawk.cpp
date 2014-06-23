#include "GL/GLFundamentals.hpp"
#include "GL/GLDemonstration.hpp"

#include "img.hpp"

#include <iostream>
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

    void refresh();
    void  center(state *, image *);
};

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

    dragging = false;
    selector = 0;
    point_x  = 0;
    point_y  = 0;

    for (int i = 0; i < 12; i++)
        center(mark_state + i, (mark_image[i] = p));

    curr_state = mark_state[0];
    curr_image = mark_image[0];
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

    for         (int r = 0; r < height; ++r)
        for     (int c = 0; c < width;  ++c)
            for (int k = 0; k < depth;  ++k)
            {
                int i = toint(curr_state.y + (r - height / 2) * curr_state.z);
                int j = toint(curr_state.x + (c - width  / 2) * curr_state.z);
                pixel[(r * width + c) * 3 + k] = curr_image->get(i, j, k);
            }

    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height,
                    GL_RGB, GL_FLOAT, &pixel.front());

    cache_state = curr_state;
}

void rawk::draw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    double x = 2.0 * (cache_state.x - curr_state.x) / cache_state.z / width;
    double y = 2.0 * (cache_state.y - curr_state.y) / cache_state.z / height;
    double z =        cache_state.z / curr_state.z;

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
    if (!repeat)
    {
        if (down)
        {
            switch (key)
            {
                case SDL_SCANCODE_SPACE:
                    if (selector)
                    {
                        curr_state = mark_state[selector - 1];
                        curr_image = mark_image[selector - 1];
                    }
                    refresh();
                    break;

                case SDL_SCANCODE_RETURN:
                    if (selector)
                    {
                        mark_state[selector - 1] = curr_state;
                        mark_image[selector - 1] = curr_image;
                    }
                    break;
            }
        }

        switch (key)
        {
            case SDL_SCANCODE_F1:  selector = down ?  1 : 0; break;
            case SDL_SCANCODE_F2:  selector = down ?  2 : 0; break;
            case SDL_SCANCODE_F3:  selector = down ?  3 : 0; break;
            case SDL_SCANCODE_F4:  selector = down ?  4 : 0; break;
            case SDL_SCANCODE_F5:  selector = down ?  5 : 0; break;
            case SDL_SCANCODE_F6:  selector = down ?  6 : 0; break;
            case SDL_SCANCODE_F7:  selector = down ?  7 : 0; break;
            case SDL_SCANCODE_F8:  selector = down ?  8 : 0; break;
            case SDL_SCANCODE_F9:  selector = down ?  9 : 0; break;
            case SDL_SCANCODE_F10: selector = down ? 10 : 0; break;
            case SDL_SCANCODE_F11: selector = down ? 11 : 0; break;
            case SDL_SCANCODE_F12: selector = down ? 12 : 0; break;
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
