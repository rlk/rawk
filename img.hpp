#ifndef IMG_HPP
#define IMG_HPP

#include "raw.hpp"

#include <string>

//------------------------------------------------------------------------------













/// Test pattern generator

class test : public image
{
public:
    virtual double get(int, int, int) const;

    virtual int geth() const { return 1024; }
    virtual int getw() const { return 1024; }
    virtual int getd() const { return    3; }

    virtual std::string doc() const;
};

//------------------------------------------------------------------------------

#endif
