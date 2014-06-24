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

    virtual int geth() const;
    virtual int getw() const;
    virtual int getd() const;

    virtual void tweak(int, int) { }

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
    bias(double d, image *L) : image(L), value(d) { }

    virtual double get(int, int, int) const;
    virtual void tweak(int, int);

    virtual std::string doc() const;

private:
    double value;
};

/// Scale filter

class scale : public image
{
public:
    scale(double d, image *L) : image(L), value(d) { }

    virtual double get(int, int, int) const;

    virtual std::string doc() const;

private:
    double value;
};

/// Offset filter

class offset : public image
{
public:
    offset(int r, int c, int w, image *L) : image(L), row(r), col(c), wrap(w) { }

    virtual double get(int, int, int) const;
    virtual void tweak(int, int);

    virtual std::string doc() const;

private:
    int row;
    int col;
    int wrap;
};

/// Nearest up-sample filter

class nearest : public image
{
public:
    nearest(int h, int w, image *L) : image(L), h(h), w(w) { }

    virtual double get(int, int, int) const;

    virtual int geth() const { return h; }
    virtual int getw() const { return w; }

    virtual std::string doc() const;

private:
    int h;
    int w;
};

/// Channel reassignment filter

class channel : public image
{
public:
    channel(int k, image *L) : image(L), index(k) { }

    virtual double get(int, int, int) const;

    virtual int getd() const { return index + 1; }

    virtual std::string doc() const;

private:
    int index;
};

/// Overlay operator

class paste : public image
{
public:
    paste(int r, int c, image *L, image *R) : image(L, R), row(r), col(c) { }

    virtual double get(int, int, int) const;

    virtual int geth() const;
    virtual int getw() const;

    virtual std::string doc() const;

private:
    int row;
    int col;
};

/// Sum operator

class sum : public image
{
public:
    sum(image *L, image *R) : image(L, R) { }

    virtual double get(int, int, int) const;

    virtual std::string doc() const;
};

/// Gradient filter

class gradient : public image
{
public:
    gradient(int w, image *L) : image(L), wrap(w) { }

    virtual double get(int, int, int) const;

    virtual std::string doc() const;

private:
    int wrap;
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
