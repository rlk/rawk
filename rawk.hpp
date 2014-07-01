// RAWK Copyright (C) 2014 Robert Kooima
//
// This program is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful, but WITH-
// OUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
// more details.

#ifndef RAWK_HPP
#define RAWK_HPP

/** @mainpage RAWK : Gigapixel Raw Image Processing Toolkit

RAWK is a command-line tool capable of performing a variety of image processing
operations on very large raw binary image files. It provides a pannable,
zoomable preview of the result of the process and enables image process
parameters to be tuned interactively regardless of the scale of the input and
output.

The speed and capability of RAWK is lies in its reliance upon the raw binary
image format, as such images may be read and written without the overhead of
coding and compression, and may be many times larger than available RAM without
added code complexity.

The simplicity of raw binary makes it the recommended format for the NASA
Planetary Data System, the primary motivator of RAWK. Both Photoshop and
Imagemagick are capable of transforming image into and out of raw format (for
limited file sizes) enabling interoperability with off-the-shelf tools.

The RAWK tool emphasizes the application of several image processes in a single
command. An image process is represented by a *tree* of ::image objects. Each
object acts as a source of pixels with a specific height, width, and depth. Each
samples its own input data from zero, one, or two source image objects.

This tree of objects is specified on the command line. The various ::image
objects and their parameters are itemized below. Real-world examples of their use are given
toward the end of this page.

## Introductory Example

Here is a straightforward RAWK process. It reads an image, resizes it, and
writes it to a new file.

    ./rawk output result.rgb b cubic 100 100 0 input mandrill.rgb 0 512 512 3 b

There are three stages. The first is an ::output object that stores its input
in a file named "result.rgb" with 8-bit unsigned sample type.

    ./rawk output result.rgb b ...

The second provides that input, a ::cubic resampling to 100 by 100 with no pixel
wrap-around.

    ... cubic 100 100 0 ...

The third provides data to the resampler by reading ::input from the file
system. Because it's raw data, the image offset, height, width, channel count,
and data type must be specified manually.

    ... input mandrill.rgb 0 512 512 3 b

This example is trivial, but the scalability of these building blocks is
limited only by available disk space.

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
- **Press Space** to update the cache to display result of the current image object.

The window title bar changes to reflect the current image.

### Modifying parameters

The parameters of many image objects may be tuned interactively.

- Press Left or Right to decrease or increase the first parameter of the current image.
- Press Down or Up to decrease or increase the second parameter of the current image.
- Again, **press Space** to update the cache to display the modified process.

Hold the Shift key to magnify the increase or decrease by a factor of 10. The
window title bar changes to reflect the current parameters.

### Storing Views

The Function keys allow up to 12 view configurations to be stored and recalled.
This includes both the position and zoom of the view as well as the current image.

- Hold Function-n and press Enter to store the current view in slot *n*.
- Hold Function-n and press Space to recall the view stored in slot *n*.

Parameter tuning works along side the view storage and recall mechanism:

- Hold Function-n and press Left or Right to decrease and increase the first
  parameter of the image stored in slot *n*.
- Hold Function-n and press Down or Up to decrease and increase the first
  parameter of the image stored in slot *n*.

This capability allows the user to view the output of one image while
manipulating the parameters of another. For example, tweak an ::offset to bring
the results of two separate processes into alignment before summing thing.

### Display Modes

A variety of different data types are supported, but their interpretation must
be selected manually.

- Press 1 to display one channel as a grayscale image.
- Press 2 to display the luminance of an RGB image.
- Press 3 to display three channels as an RGB image.

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

- ::gain *value* *source*

    Multiply *value* by all samples of *source*. This alters the contrast of
    *source*.

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

- ::append *first *second*

    Append the channels of the *second* image to those of the *first* image.
    This can be used to assemble a multichannel image from grayscale inputs, or
    to attach an alpha channel for use by ::blend. The width, height, and depth
    of the result are the larger of the widths, heights, and depths of the two
    inputs.

- ::blend *first* *second*

    Blend the *first* image with the *second* image using the alpha channel of
    the *first*. The alpha channel is taken to be the last channel, regardless
    of actual channel count. The width, height, and depth of the result are the
    larger of the widths, heights, and depths of the two inputs, with the
    last channel of the *first* image discarded.

- ::paste *row* *column* *top* *bottom*

    Paste the *top* image over the *bottom* image. *Row* and *column* give the
    position of the top-left corner of the pasted image within the underlying
    image. This is useful for reconstructing tiled or sub-divided image files.
    The width and height of the result expand to accommodate the sizes of both
    source images .

- ::sum *first* *second*

    Add all samples of the *first* image with the *second* image. The width,
    height, and depth of the result are the larger of the widths, heights, and
    depths of the two inputs.

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
#endif