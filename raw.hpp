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

// C++-style casts for pointers to samples.

typedef  int8_t  * int8_p;
typedef uint8_t  *uint8_p;
typedef  int16_t * int16_p;
typedef uint16_t *uint16_p;
typedef  int32_t * int32_p;
typedef uint32_t *uint32_p;
typedef  float   * float_p;
typedef double   *double_p;

//------------------------------------------------------------------------------

class raw_error : public std::runtime_error
{
public :
    raw_error(const std::string& a,
              const std::string& b) : std::runtime_error(a + ": " + b) { }
};

//------------------------------------------------------------------------------

class raw
{
public:
    raw(std::string name, size_t o, size_t h, size_t w, size_t d, size_t s, bool m)
        : name(name), h(h), w(w), d(d), s(s), f(0), p(0)
    {
        size_t n = h * w * d * s;

        int mode = m ? O_RDWR | O_TRUNC | O_CREAT : O_RDONLY;
        int prot = m ? PROT_READ | PROT_WRITE : PROT_READ;

        if ((f = open(name.c_str(), mode, 0666)) == -1)
            throw raw_error(name, strerror(errno));

        if (m && ftruncate(f, n) == -1)
            throw raw_error(name, strerror(errno));

        if ((q = mmap(0, n + o, prot, MAP_SHARED, f, 0)) == MAP_FAILED)
            throw raw_error(name, strerror(errno));

        p = uint8_p(q) + o;
    }

    virtual void   put(int, int, int, double) = 0;
    virtual double get(int, int, int) const   = 0;

    std::string getname() const { return name; }
    int         geth()    const { return h;    }
    int         getw()    const { return w;    }
    int         getd()    const { return d;    }

    virtual ~raw()
    {
        size_t n = h * w * d * s;

        if (munmap(q, n) == -1)
            throw raw_error(name, strerror(errno));

        if (close(f) == -1)
            throw raw_error(name, strerror(errno));
    }

protected:

    const void *data(int i, int j, int k) const
    {
        return (const uint8_t *) p + (((i * w) + j) * d + k) * s;
    }
    void *data(int i, int j, int k)
    {
        return       (uint8_t *) p + (((i * w) + j) * d + k) * s;
    }

    std::string name;

    size_t h;     // Image height
    size_t w;     // Image width
    size_t d;     // Image depth
    size_t s;     // Image sample size
    size_t o;     // Offset of first data
    int    f;     // File descriptor
    void  *p;     // Pointer to beginning of data
    void  *q;     // Pointer to beginning of file
};

//------------------------------------------------------------------------------

// Clamps prevent under and overflow when casting to normalized sample types.

static inline double uclamp(double d)
{
    if      (d >  1.0) return  1;
    else if (d <  0.0) return  0;
    else               return  d;
}

static inline double clamp(double d)
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

    void put(int i, int j, int k, double d)
    {
        *int8_p(data(i, j, k)) = int8_t(clamp(d) * INT8_MAX);
    }
    double get(int i, int j, int k) const
    {
        return double(*int8_p(data(i, j, k))) / INT8_MAX;
    }
};

/// Unsigned 8-bit RAW adapter

class rawb : public raw
{
public:
    rawb(std::string a, size_t o, size_t h, size_t w, size_t d, bool m)
        : raw(a, o, h, w, d, sizeof (uint8_t), m) { }

    void put(int i, int j, int k, double d)
    {
        *uint8_p(data(i, j, k)) = uint8_t(uclamp(d) * UINT8_MAX);
    }
    double get(int i, int j, int k) const
    {
        return double(*uint8_p(data(i, j, k))) / UINT8_MAX;
    }
};

//------------------------------------------------------------------------------

/// Signed 16-bit RAW adapter

class raws : public raw
{
public:
    raws(std::string a, size_t o, size_t h, size_t w, size_t d, bool m)
        : raw(a, o, h, w, d, sizeof (int16_t), m) { }

    void put(int i, int j, int k, double d)
    {
        *int16_p(data(i, j, k)) = int16_t(clamp(d) * INT16_MAX);
    }
    double get(int i, int j, int k) const
    {
        return double(*int16_p(data(i, j, k))) / INT16_MAX;
    }
};

/// Byte-swapped signed 16-bit RAW adapter

class rawS : public raw
{
public:
    rawS(std::string a, size_t o, size_t h, size_t w, size_t d, bool m)
        : raw(a, o, h, w, d, sizeof (int16_t), m) { }

    void put(int i, int j, int k, double d)
    {
        *int16_p(data(i, j, k)) = swap(int16_t(clamp(d) * INT16_MAX));
    }
    double get(int i, int j, int k) const
    {
        return double(swap(*int16_p(data(i, j, k)))) / INT16_MAX;
    }
};

//------------------------------------------------------------------------------

/// Unsigned 16-bit RAW adapter

class rawu : public raw
{
public:
    rawu(std::string a, size_t o, size_t h, size_t w, size_t d, bool m)
        : raw(a, o, h, w, d, sizeof (uint16_t), m) { }

    void put(int i, int j, int k, double d)
    {
        *uint16_p(data(i, j, k)) = uint16_t(uclamp(d) * UINT16_MAX);
    }
    double get(int i, int j, int k) const
    {
        return double(*uint16_p(data(i, j, k))) / UINT16_MAX;
    }
};

/// Byte-swapped unsigned 16-bit RAW adapter

class rawU : public raw
{
public:
    rawU(std::string a, size_t o, size_t h, size_t w, size_t d, bool m)
        : raw(a, o, h, w, d, sizeof (uint16_t), m) { }

    void put(int i, int j, int k, double d)
    {
        *uint16_p(data(i, j, k)) = swap(uint16_t(uclamp(d) * UINT16_MAX));
    }
    double get(int i, int j, int k) const
    {
        return double(swap(*uint16_p(data(i, j, k)))) / UINT16_MAX;
    }
};

//------------------------------------------------------------------------------

/// Signed 32-bit RAW adapter

class rawi : public raw
{
public:
    rawi(std::string a, size_t o, size_t h, size_t w, size_t d, bool m)
        : raw(a, o, h, w, d, sizeof (int32_t), m) { }

    void put(int i, int j, int k, double d)
    {
        *int32_p(data(i, j, k)) = int32_t(clamp(d) * INT32_MAX);
    }
    double get(int i, int j, int k) const
    {
        return double(*int32_p(data(i, j, k))) / INT32_MAX;
    }
};

/// Byte-swapped signed 32-bit RAW adapter

class rawI : public raw
{
public:
    rawI(std::string a, size_t o, size_t h, size_t w, size_t d, bool m)
        : raw(a, o, h, w, d, sizeof (int32_t), m) { }

    void put(int i, int j, int k, double d)
    {
        *int32_p(data(i, j, k)) = swap(int32_t(clamp(d) * INT32_MAX));
    }
    double get(int i, int j, int k) const
    {
        return double(swap(*int32_p(data(i, j, k)))) / INT32_MAX;
    }
};

//------------------------------------------------------------------------------

/// Unigned 32-bit RAW adapter

class rawl : public raw
{
public:
    rawl(std::string a, size_t o, size_t h, size_t w, size_t d, bool m)
        : raw(a, o, h, w, d, sizeof (uint32_t), m) { }

    void put(int i, int j, int k, double d)
    {
        *uint32_p(data(i, j, k)) = uint32_t(uclamp(d) * UINT32_MAX);
    }
    double get(int i, int j, int k) const
    {
        return double(*uint32_p(data(i, j, k))) / UINT32_MAX;
    }
};

/// Byte-swapped unsigned 32-bit RAW adapter

class rawL : public raw
{
public:
    rawL(std::string a, size_t o, size_t h, size_t w, size_t d, bool m)
        : raw(a, o, h, w, d, sizeof (uint32_t), m) { }

    void put(int i, int j, int k, double d)
    {
        *uint32_p(data(i, j, k)) = swap(uint32_t(uclamp(d) * UINT32_MAX));
    }
    double get(int i, int j, int k) const
    {
        return double(swap(*uint32_p(data(i, j, k)))) / UINT32_MAX;
    }
};

//------------------------------------------------------------------------------

/// Single precision floating point RAW adapter

class rawf : public raw
{
public:
    rawf(std::string a, size_t o, size_t h, size_t w, size_t d, bool m)
        : raw(a, o, h, w, d, sizeof (float), m) { }

    void put(int i, int j, int k, double d)
    {
        *float_p(data(i, j, k)) = float(d);
    }
    double get(int i, int j, int k) const
    {
        return double(*float_p(data(i, j, k)));
    }
};

/// Byte-swapped single precision floating point RAW adapter

class rawF : public raw
{
public:
    rawF(std::string a, size_t o, size_t h, size_t w, size_t d, bool m)
        : raw(a, o, h, w, d, sizeof (float), m) { }

    void put(int i, int j, int k, double d)
    {
        *float_p(data(i, j, k)) = swap(float(d));
    }
    double get(int i, int j, int k) const
    {
        return double(swap(*float_p(data(i, j, k))));
    }
};

//------------------------------------------------------------------------------

/// Single precision floating point RAW adapter

class rawd : public raw
{
public:
    rawd(std::string a, size_t o, size_t h, size_t w, size_t d, bool m)
        : raw(a, o, h, w, d, sizeof (double), m) { }

    void put(int i, int j, int k, double d)
    {
        *double_p(data(i, j, k)) = d;
    }
    double get(int i, int j, int k) const
    {
        return *double_p(data(i, j, k));
    }
};

/// Byte-swapped single precision floating point RAW adapter

class rawD : public raw
{
public:
    rawD(std::string a, size_t o, size_t h, size_t w, size_t d, bool m)
        : raw(a, o, h, w, d, sizeof (double), m) { }

    void put(int i, int j, int k, double d)
    {
        *double_p(data(i, j, k)) = swap(d);
    }
    double get(int i, int j, int k) const
    {
        return swap(*double_p(data(i, j, k)));
    }
};

//------------------------------------------------------------------------------

#endif
