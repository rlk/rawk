FLAGS  = -Wall

#CXX = c++
#FLAGS += -O2

#CXX = c++
#FLAGS += -g

CXX = /usr/local/bin/g++
FLAGS += -O2 -fopenmp


FLAGS +=$(shell /usr/local/bin/sdl2-config --cflags --libs)
FLAGS +=-framework OpenGL

all : rawk rawtif

rawk : rawk.cpp
	$(CXX) $(FLAGS) -o $@ rawk.cpp

rawtif : rawtif.cpp
	$(CXX) $(FLAGS) -o $@ rawtif.cpp -ltiff

rawk : image.hpp
rawk : image_append.hpp
rawk : image_arithmetic.hpp
rawk : image_bias.hpp
rawk : image_blend.hpp
rawk : image_choose.hpp
rawk : image_convolve.hpp
rawk : image_crop.hpp
rawk : image_function.hpp
rawk : image_gain.hpp
rawk : image_input.hpp
rawk : image_matrix.hpp
rawk : image_median.hpp
rawk : image_morphology.hpp
rawk : image_offset.hpp
rawk : image_output.hpp
rawk : image_paste.hpp
rawk : image_reduce.hpp
rawk : image_resample.hpp
rawk : image_sobel.hpp
rawk : image_solid.hpp
rawk : image_swizzle.hpp
rawk : image_threshold.hpp
rawk : raw.hpp
