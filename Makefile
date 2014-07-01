FLAGS  = -Wall

CXX = c++
FLAGS += -O2

# ifeq ($(DEBUG), 1)
#	CXX = c++
#	FLAGS += -g
# else
# 	CXX = /usr/local/bin/g++
# 	FLAGS += -O2 -fopenmp
# endif

FLAGS +=$(shell /usr/local/bin/sdl2-config --cflags --libs)
FLAGS +=-framework OpenGL

rawk : rawk.cpp
	$(CXX) $(FLAGS) -o rawk rawk.cpp

rawk : image.hpp
rawk : image_append.hpp
rawk : image_arithmetic.hpp
rawk : image_bias.hpp
rawk : image_blend.hpp
rawk : image_convolve.hpp
rawk : image_crop.hpp
rawk : image_flatten.hpp
rawk : image_gradient.hpp
rawk : image_input.hpp
rawk : image_matrix.hpp
rawk : image_median.hpp
rawk : image_morphology.hpp
rawk : image_normalize.hpp
rawk : image_offset.hpp
rawk : image_output.hpp
rawk : image_paste.hpp
rawk : image_reduce.hpp
rawk : image_resample.hpp
rawk : image_scale.hpp
rawk : image_solid.hpp
rawk : image_swizzle.hpp
rawk : image_threshold.hpp
rawk : image_trim.hpp
rawk : raw.hpp
