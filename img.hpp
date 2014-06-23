#ifndef IMG_HPP
#define IMG_HPP

#include "raw.hpp"

#include <string>

//------------------------------------------------------------------------------

/// Base class for all image data sources

class image
{
public:
    image(image *L=0, image *R=0);

    virtual double get(int, int, int) const = 0;

    virtual int geth() const { return L ? L->geth() : 0; }
    virtual int getw() const { return L ? L->getw() : 0; }
    virtual int getd() const { return L ? L->getd() : 0; }

    virtual ~image()
    {
        if (R) delete R;
        if (L) delete L;
    }

    virtual std::string doc() const { return std::string("ndocumented"); }

    image *getL() { return L; }
    image *getR() { return R; }
    image *getP() { return P; }

    void setP(image *p) { P = p; }

protected:
    image *L;
    image *R;
    image *P;
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
