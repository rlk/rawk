#ifndef IMG_HPP
#define IMG_HPP

#include "raw.hpp"

#include <string>

//------------------------------------------------------------------------------

/// Base class for all image data sources

class image
{
public:
    image(image *H=0, image *T=0) : head(H), tail(T) { }

    virtual double get(int, int, int) const = 0;

    virtual int geth() const { return head ? head->geth() : 0; }
    virtual int getw() const { return head ? head->getw() : 0; }
    virtual int getd() const { return head ? head->getd() : 0; }

    virtual ~image()
    {
        if (tail) delete tail;
        if (head) delete head;
    }

protected:
    image *head;
    image *tail;
};

/// RAW file reader

class input : public image
{
public:
    input(std::string, int, int, int, char);
   ~input();

    virtual double get(int, int, int) const;

    virtual int geth() const { return file->h; }
    virtual int getw() const { return file->w; }
    virtual int getd() const { return file->d; }

private:
    raw *file;
};

/// RAW file writer with pass-through

class output : public image
{
public:
    output(std::string, char, image *);
   ~output();

    virtual double get(int, int, int) const;

    void go() const;

private:
    raw *file;
};

/// Offset filter

class offset : public image
{
public:
    offset(double d, image *H) : image(H), value(d) { }

    virtual double get(int, int, int) const;

private:
    double value;
};

/// Scale filter

class scale : public image
{
public:
    scale(double d, image *H) : image(H), value(d) { }

    virtual double get(int, int, int) const;

private:
    double value;
};

/// Test pattern generator

class test : public image
{
public:
    virtual double get(int, int, int) const;

    virtual int geth() const;
    virtual int getw() const;
    virtual int getd() const;
};

//------------------------------------------------------------------------------

#endif
