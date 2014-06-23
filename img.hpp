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

    virtual std::string doc() const { return std::string("ndocumented"); }

    image *gethead() { return head; }
    image *gettail() { return tail; }

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

    virtual int geth() const { return file->geth(); }
    virtual int getw() const { return file->getw(); }
    virtual int getd() const { return file->getd(); }

    virtual std::string doc() const;

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

    virtual std::string doc() const;

    void go() const;

private:
    raw *file;
};

/// Bias filter

class bias : public image
{
public:
    bias(double d, image *H) : image(H), value(d) { }

    virtual double get(int, int, int) const;

    virtual std::string doc() const;

private:
    double value;
};

/// Scale filter

class scale : public image
{
public:
    scale(double d, image *H) : image(H), value(d) { }

    virtual double get(int, int, int) const;

    virtual std::string doc() const;

private:
    double value;
};

/// Overlay operator

class paste : public image
{
public:
    paste(int r, int c, image *H, image *T) : image(H, T), row(r), col(c) { }

    virtual double get(int, int, int) const;

    virtual int geth() const;
    virtual int getw() const;

    virtual std::string doc() const;

private:
    int row;
    int col;
};

/// Test pattern generator

class test : public image
{
public:
    virtual double get(int, int, int) const;

    virtual int geth() const;
    virtual int getw() const;
    virtual int getd() const;

    virtual std::string doc() const;
};

//------------------------------------------------------------------------------

#endif
