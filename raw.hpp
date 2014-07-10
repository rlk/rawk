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

#ifndef RAW_HPP
#define RAW_HPP

#include <stdio.h>
#include <fcntl.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>

#include <stdexcept>
#include <string>

extern int errno;

/// The following sample type abbreviations are used consistently throughout.
/// Capitalization implies non-native byte order.
///
///       int8_t ... c
///      uint8_t ... b
///      int16_t ... s S
///     uint16_t ... u U
///      int32_t ... i I
///     uint32_t ... l L
///        float ... f F
///       double ... d D

//------------------------------------------------------------------------------

/// C++-style casts for pointers to samples

typedef  int8_t  * int8_p;
typedef uint8_t  *uint8_p;
typedef  int16_t * int16_p;
typedef uint16_t *uint16_p;
typedef  int32_t * int32_p;
typedef uint32_t *uint32_p;
typedef  float   * float_p;
typedef double   *double_p;

//------------------------------------------------------------------------------

/// RAW image file I/O error

class raw_error : public std::runtime_error
{
public :
    raw_error(const std::string& a,
              const std::string& b) : std::runtime_error(a + ": " + b) { }
};

//------------------------------------------------------------------------------

/// RAW image file

class raw
{
public:
    raw(std::string name, size_t start, size_t height, size_t width, size_t depth, size_t size, bool write) :
        name(name),
        start(start),
        height(height),
        width(width),
        depth(depth),
        size(size),
        file(0),
        buffer(0),
        pixels(0)
    {
        size_t length = start + height * width * depth * size;

        int mode = write ? O_RDWR | O_TRUNC | O_CREAT : O_RDONLY;
        int prot = write ? PROT_READ | PROT_WRITE : PROT_READ;

        if ((file = open(name.c_str(), mode, 0666)) == -1)
            throw raw_error(name, strerror(errno));

        if (write && ftruncate(file, length) == -1)
            throw raw_error(name, strerror(errno));

        if ((buffer = mmap(0, length, prot, MAP_SHARED, file, 0)) == MAP_FAILED)
            throw raw_error(name, strerror(errno));

        pixels = uint8_p(buffer) + start;
    }

    virtual void  put(int, int, int, float) = 0;
    virtual float get(int, int, int) const  = 0;

    std::string get_name()   const { return name;   }
    int         get_height() const { return height; }
    int         get_width()  const { return width;  }
    int         get_depth()  const { return depth;  }

    virtual ~raw()
    {
        size_t length = start + height * width * depth * size;

        if (munmap(buffer, length) == -1)
            throw raw_error(name, strerror(errno));

        if (close(file) == -1)
            throw raw_error(name, strerror(errno));
    }

protected:

    const void *data(int i, int j, int k) const
    {
        return (const uint8_t *) pixels + (((i * width) + j) * depth + k) * size;
    }
    void *data(int i, int j, int k)
    {
        return       (uint8_t *) pixels + (((i * width) + j) * depth + k) * size;
    }

    std::string name;

    size_t start;
    size_t height;
    size_t width;
    size_t depth;
    size_t size;
    int    file;
    void  *buffer;
    void  *pixels;
};

//------------------------------------------------------------------------------

// Clamps prevent under and overflow when casting to normalized sample types.

static inline float uclamp(float d)
{
    if      (d >  1.0) return  1;
    else if (d <  0.0) return  0;
    else               return  d;
}

static inline float clamp(float d)
{
    if      (d >  1.0) return  1;
    else if (d < -1.0) return -1;
    else               return  d;
}

//------------------------------------------------------------------------------

// Byte swappers for all sample types.

static inline void swap(uint8_t *a, uint8_t *b)
{
    uint8_t tt;
    tt = *a;
    *a = *b;
    *b = tt;
}

static inline int16_t swap(int16_t n)
{
    swap(uint8_p(&n) + 0, uint8_p(&n) + 1);
    return n;
}

static inline uint16_t swap(uint16_t n)
{
    swap(uint8_p(&n) + 0, uint8_p(&n) + 1);
    return n;
}

static inline int32_t swap(int32_t n)
{
    swap(uint8_p(&n) + 0, uint8_p(&n) + 3);
    swap(uint8_p(&n) + 1, uint8_p(&n) + 2);
    return n;
}

static inline uint32_t swap(uint32_t n)
{
    swap(uint8_p(&n) + 0, uint8_p(&n) + 3);
    swap(uint8_p(&n) + 1, uint8_p(&n) + 2);
    return n;
}

static inline float swap(float n)
{
    swap(uint8_p(&n) + 0, uint8_p(&n) + 3);
    swap(uint8_p(&n) + 1, uint8_p(&n) + 2);
    return n;
}

static inline double swap(double n)
{
    swap(uint8_p(&n) + 0, uint8_p(&n) + 7);
    swap(uint8_p(&n) + 1, uint8_p(&n) + 6);
    swap(uint8_p(&n) + 2, uint8_p(&n) + 5);
    swap(uint8_p(&n) + 3, uint8_p(&n) + 4);
    return n;
}

//------------------------------------------------------------------------------

/// Signed 8-bit RAW adapter

class rawc : public raw
{
public:
    rawc(std::string a, size_t o, size_t h, size_t w, size_t d, bool m)
        : raw(a, o, h, w, d, sizeof (int8_t), m) { }

    void put(int i, int j, int k, float d)
    {
        *int8_p(data(i, j, k)) = int8_t(clamp(d) * INT8_MAX);
    }
    float get(int i, int j, int k) const
    {
        return float(*int8_p(data(i, j, k))) / INT8_MAX;
    }
};

/// Unsigned 8-bit RAW adapter

class rawb : public raw
{
public:
    rawb(std::string a, size_t o, size_t h, size_t w, size_t d, bool m)
        : raw(a, o, h, w, d, sizeof (uint8_t), m) { }

    void put(int i, int j, int k, float d)
    {
        *uint8_p(data(i, j, k)) = uint8_t(uclamp(d) * UINT8_MAX);
    }
    float get(int i, int j, int k) const
    {
        return float(*uint8_p(data(i, j, k))) / UINT8_MAX;
    }
};

//------------------------------------------------------------------------------

/// Signed 16-bit RAW adapter

class raws : public raw
{
public:
    raws(std::string a, size_t o, size_t h, size_t w, size_t d, bool m)
        : raw(a, o, h, w, d, sizeof (int16_t), m) { }

    void put(int i, int j, int k, float d)
    {
        *int16_p(data(i, j, k)) = int16_t(clamp(d) * INT16_MAX);
    }
    float get(int i, int j, int k) const
    {
        return float(*int16_p(data(i, j, k))) / INT16_MAX;
    }
};

/// Byte-swapped signed 16-bit RAW adapter

class rawS : public raw
{
public:
    rawS(std::string a, size_t o, size_t h, size_t w, size_t d, bool m)
        : raw(a, o, h, w, d, sizeof (int16_t), m) { }

    void put(int i, int j, int k, float d)
    {
        *int16_p(data(i, j, k)) = swap(int16_t(clamp(d) * INT16_MAX));
    }
    float get(int i, int j, int k) const
    {
        return float(swap(*int16_p(data(i, j, k)))) / INT16_MAX;
    }
};

//------------------------------------------------------------------------------

/// Unsigned 16-bit RAW adapter

class rawu : public raw
{
public:
    rawu(std::string a, size_t o, size_t h, size_t w, size_t d, bool m)
        : raw(a, o, h, w, d, sizeof (uint16_t), m) { }

    void put(int i, int j, int k, float d)
    {
        *uint16_p(data(i, j, k)) = uint16_t(uclamp(d) * UINT16_MAX);
    }
    float get(int i, int j, int k) const
    {
        return float(*uint16_p(data(i, j, k))) / UINT16_MAX;
    }
};

/// Byte-swapped unsigned 16-bit RAW adapter

class rawU : public raw
{
public:
    rawU(std::string a, size_t o, size_t h, size_t w, size_t d, bool m)
        : raw(a, o, h, w, d, sizeof (uint16_t), m) { }

    void put(int i, int j, int k, float d)
    {
        *uint16_p(data(i, j, k)) = swap(uint16_t(uclamp(d) * UINT16_MAX));
    }
    float get(int i, int j, int k) const
    {
        return float(swap(*uint16_p(data(i, j, k)))) / UINT16_MAX;
    }
};

//------------------------------------------------------------------------------

/// Signed 32-bit RAW adapter

class rawi : public raw
{
public:
    rawi(std::string a, size_t o, size_t h, size_t w, size_t d, bool m)
        : raw(a, o, h, w, d, sizeof (int32_t), m) { }

    void put(int i, int j, int k, float d)
    {
        *int32_p(data(i, j, k)) = int32_t(clamp(d) * INT32_MAX);
    }
    float get(int i, int j, int k) const
    {
        return float(*int32_p(data(i, j, k))) / INT32_MAX;
    }
};

/// Byte-swapped signed 32-bit RAW adapter

class rawI : public raw
{
public:
    rawI(std::string a, size_t o, size_t h, size_t w, size_t d, bool m)
        : raw(a, o, h, w, d, sizeof (int32_t), m) { }

    void put(int i, int j, int k, float d)
    {
        *int32_p(data(i, j, k)) = swap(int32_t(clamp(d) * INT32_MAX));
    }
    float get(int i, int j, int k) const
    {
        return float(swap(*int32_p(data(i, j, k)))) / INT32_MAX;
    }
};

//------------------------------------------------------------------------------

/// Unigned 32-bit RAW adapter

class rawl : public raw
{
public:
    rawl(std::string a, size_t o, size_t h, size_t w, size_t d, bool m)
        : raw(a, o, h, w, d, sizeof (uint32_t), m) { }

    void put(int i, int j, int k, float d)
    {
        *uint32_p(data(i, j, k)) = uint32_t(uclamp(d) * UINT32_MAX);
    }
    float get(int i, int j, int k) const
    {
        return float(*uint32_p(data(i, j, k))) / UINT32_MAX;
    }
};

/// Byte-swapped unsigned 32-bit RAW adapter

class rawL : public raw
{
public:
    rawL(std::string a, size_t o, size_t h, size_t w, size_t d, bool m)
        : raw(a, o, h, w, d, sizeof (uint32_t), m) { }

    void put(int i, int j, int k, float d)
    {
        *uint32_p(data(i, j, k)) = swap(uint32_t(uclamp(d) * UINT32_MAX));
    }
    float get(int i, int j, int k) const
    {
        return float(swap(*uint32_p(data(i, j, k)))) / UINT32_MAX;
    }
};

//------------------------------------------------------------------------------

/// Single precision floating point RAW adapter

class rawf : public raw
{
public:
    rawf(std::string a, size_t o, size_t h, size_t w, size_t d, bool m)
        : raw(a, o, h, w, d, sizeof (float), m) { }

    void put(int i, int j, int k, float d)
    {
        *float_p(data(i, j, k)) = float(d);
    }
    float get(int i, int j, int k) const
    {
        return float(*float_p(data(i, j, k)));
    }
};

/// Byte-swapped single precision floating point RAW adapter

class rawF : public raw
{
public:
    rawF(std::string a, size_t o, size_t h, size_t w, size_t d, bool m)
        : raw(a, o, h, w, d, sizeof (float), m) { }

    void put(int i, int j, int k, float d)
    {
        *float_p(data(i, j, k)) = swap(float(d));
    }
    float get(int i, int j, int k) const
    {
        return float(swap(*float_p(data(i, j, k))));
    }
};

//------------------------------------------------------------------------------

/// Single precision floating point RAW adapter

class rawd : public raw
{
public:
    rawd(std::string a, size_t o, size_t h, size_t w, size_t d, bool m)
        : raw(a, o, h, w, d, sizeof (double), m) { }

    void put(int i, int j, int k, float d)
    {
        *double_p(data(i, j, k)) = d;
    }
    float get(int i, int j, int k) const
    {
        return *double_p(data(i, j, k));
    }
};

/// Byte-swapped single precision floating point RAW adapter

class rawD : public raw
{
public:
    rawD(std::string a, size_t o, size_t h, size_t w, size_t d, bool m)
        : raw(a, o, h, w, d, sizeof (float), m) { }

    void put(int i, int j, int k, float d)
    {
        *double_p(data(i, j, k)) = swap(double(d));
    }
    float get(int i, int j, int k) const
    {
        return float(swap(*double_p(data(i, j, k))));
    }
};

//------------------------------------------------------------------------------

#endif
