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

@section manual Manual

RAWK is a command-line tool capable of performing a variety of image processing
operations on very large raw binary image files. It provides a pannable,
zoomable preview of the result of the process and enables image process
parameters to be tuned interactively regardless of the scale of the input and
output.

The speed and capability of RAWK lies in its reliance upon the raw binary image
format, as such images may be read and written without the overhead of coding
and compression, and may be many times larger than available RAM without added
algorithmic complexity.

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

@subsection image_objects Image Objects

There is a direct mapping between an image object's constructor and its command
line arguments, so see the documentation of each individual object for usage.

@subsubsection image_sources Image Sources

::input ---
::solid

@subsubsection image_filters Image Filters

::bias ---
::crop ---
::cubic ---
::dilate ---
::erode ---
::flatten ---
::gain ---
::gaussian ---
::gradient ---
::linear ---
::median ---
::medianh ---
::medianv ---
::nearest ---
::normalize ---
::offset ---
::output ---
::reduce ---
::rgb2yuv ---
::swizzle ---
::threshold ---
::trim ---
::yuv2rgb

@subsubsection image_operators Image Operators

::append ---
::blend ---
::difference ---
::paste ---
::sum

@subsection interaction Interaction

On execution, RAWK displays an OpenGL window showing a *cache* of the
result of the image process.

- Click and drag with the left mouse button to pan the cache.
- Rotate the mouse wheel to zoom the cache.
- **Press Space** to update the cache with the modified view configuration.

The window title updates to reflect the current position of the mouse pointer
within the image, as well as the sample value beneath the pointer.

@subsubsection exploring_the_tree Exploring the tree

By default, the view displays the root of the image process tree, but image
objects deeper in the tree may also be explored.

- Press Alt-Left to jump to the first child of the current image.
- Press Alt-Right to jump to the second child of the current image.
- Press Alt-Up to jump to the parent of the current image.
- **Press Space** to update the cache to display result of the current image object.

The window title bar changes to reflect the current image.

@subsubsection modifying_parameters Modifying parameters

The parameters of many image objects may be tuned interactively.

- Press Left or Right to decrease or increase the first parameter of the current image.
- Press Down or Up to decrease or increase the second parameter of the current image.
- Again, **press Space** to update the cache to display the modified process.

Hold the Shift key to magnify the increase or decrease by a factor of 10. The
window title bar changes to reflect the current parameters.

@subsubsection storing_views Storing Views

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

@subsubsection display_modes Display Modes

A variety of different data types are supported, but their interpretation must
be selected manually.

- Press 1 to display one channel as a grayscale image.
- Press 2 to display the luminance of an RGB image.
- Press 3 to display three channels as an RGB image.
- Press 4 to display one channel as a relief-shaded height map.

@subsection data Data Issues

@subsubsection type Sample Type

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

Upper-case indicates non-native (byte-swapped) data order.

Char | Type
---- | ----
`U`  | 16-bit unsigned integer in non-native byte order
`S`  | 16-bit signed integer in non-native byte order
`L`  | 32-bit unsigned integer in non-native byte order
`I`  | 32-bit signed integer in non-native byte order
`F`  | 32-bit floating point in non-native byte order
`D`  | 64-bit floating point in non-native byte order

@subsubsection wrap Out-of-bounds Sampling

As an image object samples and processes the pixels of its source image,
accesses to neighboring pixels often occur. At the image edges, such neighbor
samples may fall outside the source image. Generally, an out-of-bounds sample
results in a zero value. However, it is often preferable to treat the source as
a repeating pattern and *wrap* out-of-bounds references to the opposite edge. In
other cases, it is sometimes necessary to clamp sample positions to ensure they
fall within the source. In circumstances where this distinction is important, a
*mode* parameter is defined like so:

Mode | Behavior
---- | --------
0    | Clamp all sample positions
1    | Wrap vertically
2    | Wrap horizontally
3    | Wrap both vertically and horizontally

@section examples Examples

@subsection pds Planetary Data System

The following examples demonstrate processing of the [Mars Orbiter Laser
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

    rawk gain 10.0 \
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
            gain 10.0 \
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
available.

*/
#endif