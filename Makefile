FLAGS  = -Wall

CXX = c++
FLAGS += -g

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
rawk : image_bias.hpp
rawk : image_channel.hpp
rawk : image_cubic.hpp
rawk : image_flatten.hpp
rawk : image_gradient.hpp
rawk : image_input.hpp
rawk : image_linear.hpp
rawk : image_nearest.hpp
rawk : image_offset.hpp
rawk : image_output.hpp
rawk : image_paste.hpp
rawk : image_scale.hpp
rawk : image_solid.hpp
rawk : image_sum.hpp
rawk : image_trim.hpp
rawk : raw.hpp
