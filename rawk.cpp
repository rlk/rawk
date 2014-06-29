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
#include "image_median.hpp"
#include "image_nearest.hpp"
#include "image_offset.hpp"
#include "image_output.hpp"
#include "image_paste.hpp"
#include "image_scale.hpp"
#include "image_solid.hpp"
#include "image_sum.hpp"
#include "image_trim.hpp"

//------------------------------------------------------------------------------

/** @mainpage RAWK : Gigapixel Raw Image Processing Toolkit

RAWK is a command-line tool capable of performing a variety of image processing
operations on raw binary image files. It provides a pannable, zoomable preview
of the result of the process and enables image process parameters to be tuned
interactively regardless of the scale of the input and output.

The speed and capability of RAWK is lies in its reliance upon the raw binary
image format, as such data may be read and written without the overhead of
coding and compression, and data files may be many times larger than available
RAM without serious code complexity.

The simplicity of raw binary makes it the recommended format for the NASA
Planetary Data System, the primary motivator of RAWK. Both Photoshop and
Imagemagick are capable of transforming image into and out of raw format (for
limited file sizes) enabling interoperability with off-the-shelf tools.

An image process is represented by a *tree* of ::image objects. Each object acts
as a source of pixels with a specific height, width, and depth. Each samples its
own input data from zero, one, or two source image objects.

This tree of objects is specified on the command line. The various ::image
objects and their parameters are itemized below. Examples of their use are given
toward the end of this page.

## Interaction

### Basic Usage

On execution, RAWK displays an OpenGL window showing a *cache* of the
result of the image process.

- Click and drag with the left mouse button to pan the cache.
- Rotate the mouse wheel to zoom the cache.
- **Press Space** to update the cache with the modified view configuration.

The window title updates to reflect the current position of the mouse pointer
within the image, as well as the sample value beneath the pointer.

### Exploring the tree

By default, the view displays the root of the image process tree, but image
objects deeper in the tree may also be explored.

- Press Alt-Left to jump to the first child of the current image.
- Press Alt-Right to jump to the second child of the current image.
- Press Alt-Up to jump to the parent of the current image.
- As before, **press Space** to update the cache to display the modified view.

The window title bar changes to reflect the current image.

### Modifying parameters

The parameters of many image objects may be tuned interactively.

- Press Left to decrease the first parameter of the current image.
- Press Right to increase the first parameter of the current image.
- Press Down to decrease the second parameter of the current image.
- Press Up to increase the second parameter of the current image.
- Again, **press Space** to update the cache to display the modified process.

Hold the Shift key to magnify the increase or decrease by a factor of 10. The
window title bar changes to reflect the current parameters.

### Storing views

The Function keys allow up to 12 view configurations to be stored and recalled.
This includes both the position and zoom of the view as well as the current image.

- Hold Function-n and press Enter to store the current view in slot *n*.
- Hold Function-n and press Space to restore the view stored in slot *n*.

Parameter tuning works along side the view storage and recall mechanism:

- Hold Function-n and press Left or Right to decrease and increase the first
  parameter of the image stored in slot *n*.
- Hold Function-n and press Up or Down to decrease and increase the first
  parameter of the image stored in slot *n*.

This capability allows the user to view the output of one image while
manipulating the parameters of another. For example, tweak an ::offset to bring
the results of two separate processes into alignment before summing thing.

## Data Issues

### Sample Type

A variety of sample formats are supported. The following single-character
tags are used to indicate sample type in both the API and on the command line.

Char | Type
---- | ----
`b`  | 8-bit unsigned integer
`c`  | 8-bit signed integer
`u`  | 16-bit unsigned integer
`s`  | 16-bit signed integer
`l`  | 32-bit unsigned integer
`i`  | 32-bit signed integer
`f`  | 32-bit floating point
`d`  | 64-bit floating point

Upper-case indicates non-native (byte-swapped) order data.

Char | Type
---- | ----
`U`  | 16-bit unsigned integer in non-native byte order
`S`  | 16-bit signed integer in non-native byte order
`L`  | 32-bit unsigned integer in non-native byte order
`I`  | 32-bit signed integer in non-native byte order
`F`  | 32-bit floating point in non-native byte order
`D`  | 64-bit floating point in non-native byte order

### Out-of-bounds Sampling

As an image object samples and processes the pixels of its source image,
accesses to of neighboring pixels ofter occur. At the image edges, such neighbor
samples may fall outside the source image. In many cases, an out-of- bounds
sample results in a zero value. However, it is often preferable to treat the
source as a repeating pattern and *wrap* out-of-bounds references to the
opposite edge. Furthermore, it is sometimes necessary to clamp sample positions
to ensure they fall within the source. In cases where the distinction is
important, a *mode* parameter is defined like so:

Mode | Behavior
---- | --------
0    | Clamp all sample positions
1    | Wrap vertically
2    | Wrap horizontally
3    | Wrap both vertically and horizontally

## Image Objects

The image objects and their parameters are itemized here. The command line
argument language that builds the tree of image object is a direct reflection of
the API, so this section documents both. The parse() function makes this one-
to-one mapping explicit.

### File I/O

- ::input *name* *height* *width* *depth* *type*

    Read a raw-formatted data file named *name*. *Height*, *width*, and *depth*
    are integers giving the size and channel count of this input. *Type*
    is a character giving the sample type, as above.

- ::output *name* *type* *source*

    Write a raw-formatted data file named *name*. *Type* is a character giving
    the output sample type, as above. The image height, width, and depth are
    determined by the format of the *source* image object.

### Pixel Sources

- ::solid *height* *width* *value*

    Generate a block of pixels of constant *value* with the given height and
    width and depth of one.

### Basic Filters

- ::bias *value* *source*

    Add *value* to all samples of *source*. This alters the brightness of
    *source*.

- ::channel *index* *source*

    Redirect the first channel of *source* to channel *index*. This is useful
    for building multichannel images out of single channel sources.

- ::gradient *mode* *source*

    Compute the magnitude of the gradient of *source*, wrapped with *mode*. This
    is the value of the greatest rate of change in that image. It's basically an
    edge detection.

- ::median *radius* *mode* *source*

    Find the median over a square of neighboring *source* pixels within
    *radius*, wrapped with *mode*. This eliminates outliers and noise, such as
    erroneous black or white pixels (aka salt and pepper). At high radius, this
    can be a very expensive filter.

- ::medianh *radius* *mode* *source*

    Find the median over a vertical line of neighboring *source* pixels within
    *radius*, wrapped with *mode*. This eliminates erroneous horizontal lines.

- ::medianv *radius* *mode* *source*

    Find the median over a horizontal line of neighboring *source* pixels within
    *radius*, wrapped with *mode*. This eliminates erroneous vertical lines.

- ::offset *rows* *columns* *mode* *source*

    Offset the pixels of *source*, wrapped with *mode*. *Rows* gives the
    vertical distance. *Columns* gives the horizontal distance.

- ::scale *value* *source*

    Multiply *value* by all samples of *source*. This alters the contrast of
    *source*.

- ::trim *height* *width* *source*

    Add or subtroct equal amounts of the top and bottom or left and right edges
    of *source* to give a result with size *height* and *width*. This is akin to
    a crop filter that automatically calculates the row and column to favor the
    center of an image. If *height* and *width* are larger than *source*, the
    result will depend upon how *source* is configured to handle out-of-bounds
    sampling.

### Resampling Filters

- ::nearest *height* *width* *mode* *source*

    Resample *source* to the given *height* and *width* using nearest-neighbor
    sampling. *Mode* gives the wrap mode, documented above.

- ::linear *height* *width* *mode* *source*

    Resample *source* to the given *height* and *width* using linearly-
    interpolated sampling. *Mode* gives the wrap mode, documented above.

- ::cubic *height* *width* *mode* *source*

    Resample *source* to the given *height* and *width* using cubic-interpolated
    sampling. *Mode* gives the wrap mode, documented above.

### Operators

- ::paste *row* *column* *top* *bottom*

    Paste the *top* image over the *bottom* image. *Row* and *column* give the
    position of the top-left corner of the pasted image within the underlying
    image. This is useful for reconstructing tiled or sub-divided image files.
    The width and height of the result expand to accommodate the sizes of both
    source images .

- ::sum *left* *right*

    Add all samples of the *left* image with the *right* image. The width,
    height, and depth of the result are the larger of the widths, heights, and
    depths of the two inputs. While this can be applied as a simple arithmetic
    add operator, it's more often used to merge ::channel routings.

## Examples

The following examples demonstrate procesing of the [Mars Orbiter Laser
Altimeter](http://pds-geosciences.wustl.edu/missions/mgs/megdr.html) data set,
a 46080 &times; 23040 terrain map of the planet Mars distributed in the form
of sixteen raw image tiles.

This first example simply loads and displays one of the tiles. Each tile has a
size of 11520 &times; 5632 with one 16-bit signed integer channel in big-endian
byte order. If you're using a little-endian CPU then the correct type tag is
`S`.

    rawk input megt00n270hb.img 0 5632 11520 1 S

There's not much to see. It's too dark because the pixel values, being signed,
range from -1 to +1. Negative pixel values cannot be displayed unless the image
is biased to make them positive. So...

    rawk bias 0.5 \
            input megt00n270hb.img 0 5632 11520 1 S

The terrain of Mars becomes visible, including Valles Marineris and the north
end of Argyre Planitia. To broaden the scope of the view, paste the tile to the
south atop this tile, offset by 5632 rows and 0 columns to line them up
correctly.

    rawk bias 0.5 \
            paste 5632 0 input megt44s270hb.img 0 5632 11520 1 S \
                         input megt00n270hb.img 0 5632 11520 1 S

That completes the view of Argyre Planitia. This basic technique can be expanded
to view the entire planet.

    rawk bias 0.5 \
            paste 16896 34560 input megt44s270hb.img 0 5632 11520 1 S \
            paste 16896 23040 input megt44s180hb.img 0 5632 11520 1 S \
            paste 16896 11520 input megt44s090hb.img 0 5632 11520 1 S \
            paste 16896     0 input megt44s000hb.img 0 5632 11520 1 S \
            paste 11264 34560 input megt00n270hb.img 0 5632 11520 1 S \
            paste 11264 23040 input megt00n180hb.img 0 5632 11520 1 S \
            paste 11264 11520 input megt00n090hb.img 0 5632 11520 1 S \
            paste 11264     0 input megt00n000hb.img 0 5632 11520 1 S \
            paste  5632 34560 input megt44n270hb.img 0 5632 11520 1 S \
            paste  5632 23040 input megt44n180hb.img 0 5632 11520 1 S \
            paste  5632 11520 input megt44n090hb.img 0 5632 11520 1 S \
            paste  5632     0 input megt44n000hb.img 0 5632 11520 1 S \
            paste     0 34560 input megt88n270hb.img 0 5632 11520 1 S \
            paste     0 23040 input megt88n180hb.img 0 5632 11520 1 S \
            paste     0 11520 input megt88n090hb.img 0 5632 11520 1 S \
                              input megt88n000hb.img 0 5632 11520 1 S

Now the full 46080 &times; 23040 image can be browsed. Note that there is no
significant performance impact associated with loading all sixteen tiles
separately in this fashion. There is no need to pre-preprocess these tiles to a
single contiguous billion-pixel image, so all subsequent Mars terrain
processing operations may act upon the raw data as downloaded direct from NASA.

As an example of an interesting image process, let's compute the gradient of the
Martian terrain. Note that the map wraps around west-to-east, but clamps at
the north and south poles. For this reason, the wrap mode for a planetary
surface map is usually 2. Finally, as the slope of the Martian terrain is quite
gradual, we'll scale the magnitude by 10 to make it visible.

    rawk scale 10.0 \
            gradient 2 \
                paste 16896 34560 input megt44s270hb.img 0 5632 11520 1 S \
                paste 16896 23040 input megt44s180hb.img 0 5632 11520 1 S \
                paste 16896 11520 input megt44s090hb.img 0 5632 11520 1 S \
                paste 16896     0 input megt44s000hb.img 0 5632 11520 1 S \
                paste 11264 34560 input megt00n270hb.img 0 5632 11520 1 S \
                paste 11264 23040 input megt00n180hb.img 0 5632 11520 1 S \
                paste 11264 11520 input megt00n090hb.img 0 5632 11520 1 S \
                paste 11264     0 input megt00n000hb.img 0 5632 11520 1 S \
                paste  5632 34560 input megt44n270hb.img 0 5632 11520 1 S \
                paste  5632 23040 input megt44n180hb.img 0 5632 11520 1 S \
                paste  5632 11520 input megt44n090hb.img 0 5632 11520 1 S \
                paste  5632     0 input megt44n000hb.img 0 5632 11520 1 S \
                paste     0 34560 input megt88n270hb.img 0 5632 11520 1 S \
                paste     0 23040 input megt88n180hb.img 0 5632 11520 1 S \
                paste     0 11520 input megt88n090hb.img 0 5632 11520 1 S \
                                  input megt88n000hb.img 0 5632 11520 1 S

To produce a finished image file of this process at full resolution, introduce
an output node giving the file name. There's no reason to store magnitudes as
signed values, so `b` is a reasonable type tag.

    rawk output megt-gradient.raw b \
            scale 10.0 \
                gradient 2 \
                    paste 16896 34560 input megt44s270hb.img 0 5632 11520 1 S \
                    paste 16896 23040 input megt44s180hb.img 0 5632 11520 1 S \
                    paste 16896 11520 input megt44s090hb.img 0 5632 11520 1 S \
                    paste 16896     0 input megt44s000hb.img 0 5632 11520 1 S \
                    paste 11264 34560 input megt00n270hb.img 0 5632 11520 1 S \
                    paste 11264 23040 input megt00n180hb.img 0 5632 11520 1 S \
                    paste 11264 11520 input megt00n090hb.img 0 5632 11520 1 S \
                    paste 11264     0 input megt00n000hb.img 0 5632 11520 1 S \
                    paste  5632 34560 input megt44n270hb.img 0 5632 11520 1 S \
                    paste  5632 23040 input megt44n180hb.img 0 5632 11520 1 S \
                    paste  5632 11520 input megt44n090hb.img 0 5632 11520 1 S \
                    paste  5632     0 input megt44n000hb.img 0 5632 11520 1 S \
                    paste     0 34560 input megt88n270hb.img 0 5632 11520 1 S \
                    paste     0 23040 input megt88n180hb.img 0 5632 11520 1 S \
                    paste     0 11520 input megt88n090hb.img 0 5632 11520 1 S \
                                      input megt88n000hb.img 0 5632 11520 1 S

This might take a minute or two, depending on the number of processor cores
available. The resulting raw image file is suitable for further conversion using
Photoshop or ImageMagick.
*/

//------------------------------------------------------------------------------

/// View position and zoom

struct state
{
    double x;      ///< Position of the pixel at the center of the view
    double y;      ///< Position of the pixel at the center of the view
    double z;      ///< View zoom factor
};

/// RAWK application

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
};

//------------------------------------------------------------------------------

rawk::rawk(image *p) : demonstration("RAWK", 1280, 720), program(0)
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

                case SDL_SCANCODE_1:
                    init_program("rawk.vert", "rawk_gray.frag");
                    break;
                case SDL_SCANCODE_2:
                    init_program("rawk.vert", "rawk_luma.frag");
                    break;
                case SDL_SCANCODE_3:
                    init_program("rawk.vert", "rawk_rgb.frag");
                    break;
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

        if (op == "median")
        {
            int    r = int(strtol(v[i++], 0, 0));
            int    m = int(strtol(v[i++], 0, 0));
            image *p = parse(i, v);
            return new median(r, m, p);
        }

        if (op == "medianh")
        {
            int    r = int(strtol(v[i++], 0, 0));
            int    m = int(strtol(v[i++], 0, 0));
            image *p = parse(i, v);
            return new medianh(r, m, p);
        }

        if (op == "medianv")
        {
            int    r = int(strtol(v[i++], 0, 0));
            int    m = int(strtol(v[i++], 0, 0));
            image *p = parse(i, v);
            return new medianv(r, m, p);
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
