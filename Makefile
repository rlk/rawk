
FLAGS= \
	-g -Wall \
	$(shell /usr/local/bin/sdl2-config --cflags --libs) \
	-framework OpenGL

rawk : rawk.cpp raw.hpp img.cpp img.hpp
	c++ $(FLAGS) -o rawk rawk.cpp img.cpp

test0 :
	rawk \
		bias 0.5 \
			scale 2.0 \
				input megt00n000hb.img 5632 11520 1 S

test1 :
	rawk \
		bias 0.5 \
			scale 1.0 \
				paste 16896 34560 input megt44s270hb.img 5632 11520 1 S \
				paste 16896 23040 input megt44s180hb.img 5632 11520 1 S \
				paste 16896 11520 input megt44s090hb.img 5632 11520 1 S \
				paste 16896     0 input megt44s000hb.img 5632 11520 1 S \
				paste 11264 34560 input megt00n270hb.img 5632 11520 1 S \
				paste 11264 23040 input megt00n180hb.img 5632 11520 1 S \
				paste 11264 11520 input megt00n090hb.img 5632 11520 1 S \
				paste 11264     0 input megt00n000hb.img 5632 11520 1 S \
				paste  5632 34560 input megt44n270hb.img 5632 11520 1 S \
				paste  5632 23040 input megt44n180hb.img 5632 11520 1 S \
				paste  5632 11520 input megt44n090hb.img 5632 11520 1 S \
				paste  5632     0 input megt44n000hb.img 5632 11520 1 S \
				paste     0 34560 input megt88n270hb.img 5632 11520 1 S \
				paste     0 23040 input megt88n180hb.img 5632 11520 1 S \
				paste     0 11520 input megt88n090hb.img 5632 11520 1 S \
				                  input megt88n000hb.img 5632 11520 1 S
