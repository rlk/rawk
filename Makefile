
FLAGS= \
	-g -Wall \
	$(shell /usr/local/bin/sdl2-config --cflags --libs) \
	-framework OpenGL

rawk : rawk.cpp raw.hpp img.cpp img.hpp
	c++ $(FLAGS) -o rawk rawk.cpp img.cpp
