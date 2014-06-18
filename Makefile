
FLAGS= \
	-g -Wall \
	$(shell /usr/local/bin/sdl2-config --cflags --libs) \
	-framework OpenGL

rawk : rawk.cpp
	c++ $(FLAGS) -o rawk rawk.cpp
