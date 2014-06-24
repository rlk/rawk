CXX= /usr/local/bin/g++
#CXX=c++

FLAGS  = -Wall
#FLAGS += -g
FLAGS += -O2 -fopenmp
#FLAGS += -O2
FLAGS +=$(shell /usr/local/bin/sdl2-config --cflags --libs)
FLAGS +=-framework OpenGL

rawk : rawk.cpp raw.hpp img.cpp img.hpp
	$(CXX) $(FLAGS) -o rawk rawk.cpp img.cpp

test0 :
	rawk \
		bias 0.5 \
			scale 2.0 \
				input megt00n000hb.img 5632 11520 1 S

test1 :
	rawk \
		sum \
			nearest 46080 92160 \
				paste 256 0 \
					scale 10.0 \
						gradient 2 \
							paste 16896 34560 input MOLA/megt44s270hb.img 5632 11520 1 S \
							paste 16896 23040 input MOLA/megt44s180hb.img 5632 11520 1 S \
							paste 16896 11520 input MOLA/megt44s090hb.img 5632 11520 1 S \
							paste 16896     0 input MOLA/megt44s000hb.img 5632 11520 1 S \
							paste 11264 34560 input MOLA/megt00n270hb.img 5632 11520 1 S \
							paste 11264 23040 input MOLA/megt00n180hb.img 5632 11520 1 S \
							paste 11264 11520 input MOLA/megt00n090hb.img 5632 11520 1 S \
							paste 11264     0 input MOLA/megt00n000hb.img 5632 11520 1 S \
							paste  5632 34560 input MOLA/megt44n270hb.img 5632 11520 1 S \
							paste  5632 23040 input MOLA/megt44n180hb.img 5632 11520 1 S \
							paste  5632 11520 input MOLA/megt44n090hb.img 5632 11520 1 S \
							paste  5632     0 input MOLA/megt44n000hb.img 5632 11520 1 S \
							paste     0 34560 input MOLA/megt88n270hb.img 5632 11520 1 S \
							paste     0 23040 input MOLA/megt88n180hb.img 5632 11520 1 S \
							paste     0 11520 input MOLA/megt88n090hb.img 5632 11520 1 S \
							                  input MOLA/megt88n000hb.img 5632 11520 1 S \
					solid 23040 46080 0.0 \
			flatten 1.0113 \
					channel 1 \
						offset 0 26 2 \
								paste 38400 84480 input MOC/moc_256_N-90_330.gray 7680 7680 1 b \
								paste 38400 76800 input MOC/moc_256_N-90_300.gray 7680 7680 1 b \
								paste 38400 69120 input MOC/moc_256_N-90_270.gray 7680 7680 1 b \
								paste 38400 61440 input MOC/moc_256_N-90_240.gray 7680 7680 1 b \
								paste 38400 53760 input MOC/moc_256_N-90_210.gray 7680 7680 1 b \
								paste 38400 46080 input MOC/moc_256_N-90_180.gray 7680 7680 1 b \
								paste 38400 38400 input MOC/moc_256_N-90_150.gray 7680 7680 1 b \
								paste 38400 30720 input MOC/moc_256_N-90_120.gray 7680 7680 1 b \
								paste 38400 23040 input MOC/moc_256_N-90_090.gray 7680 7680 1 b \
								paste 38400 15360 input MOC/moc_256_N-90_060.gray 7680 7680 1 b \
								paste 38400  7680 input MOC/moc_256_N-90_030.gray 7680 7680 1 b \
								paste 38400     0 input MOC/moc_256_N-90_000.gray 7680 7680 1 b \
								paste 30720 84480 input MOC/moc_256_N-60_330.gray 7680 7680 1 b \
								paste 30720 76800 input MOC/moc_256_N-60_300.gray 7680 7680 1 b \
								paste 30720 69120 input MOC/moc_256_N-60_270.gray 7680 7680 1 b \
								paste 30720 61440 input MOC/moc_256_N-60_240.gray 7680 7680 1 b \
								paste 30720 53760 input MOC/moc_256_N-60_210.gray 7680 7680 1 b \
								paste 30720 46080 input MOC/moc_256_N-60_180.gray 7680 7680 1 b \
								paste 30720 38400 input MOC/moc_256_N-60_150.gray 7680 7680 1 b \
								paste 30720 30720 input MOC/moc_256_N-60_120.gray 7680 7680 1 b \
								paste 30720 23040 input MOC/moc_256_N-60_090.gray 7680 7680 1 b \
								paste 30720 15360 input MOC/moc_256_N-60_060.gray 7680 7680 1 b \
								paste 30720  7680 input MOC/moc_256_N-60_030.gray 7680 7680 1 b \
								paste 30720     0 input MOC/moc_256_N-60_000.gray 7680 7680 1 b \
								paste 23040 84480 input MOC/moc_256_N-30_330.gray 7680 7680 1 b \
								paste 23040 76800 input MOC/moc_256_N-30_300.gray 7680 7680 1 b \
								paste 23040 69120 input MOC/moc_256_N-30_270.gray 7680 7680 1 b \
								paste 23040 61440 input MOC/moc_256_N-30_240.gray 7680 7680 1 b \
								paste 23040 53760 input MOC/moc_256_N-30_210.gray 7680 7680 1 b \
								paste 23040 46080 input MOC/moc_256_N-30_180.gray 7680 7680 1 b \
								paste 23040 38400 input MOC/moc_256_N-30_150.gray 7680 7680 1 b \
								paste 23040 30720 input MOC/moc_256_N-30_120.gray 7680 7680 1 b \
								paste 23040 23040 input MOC/moc_256_N-30_090.gray 7680 7680 1 b \
								paste 23040 15360 input MOC/moc_256_N-30_060.gray 7680 7680 1 b \
								paste 23040  7680 input MOC/moc_256_N-30_030.gray 7680 7680 1 b \
								paste 23040     0 input MOC/moc_256_N-30_000.gray 7680 7680 1 b \
								paste 15360 84480 input MOC/moc_256_N00_330.gray  7680 7680 1 b \
								paste 15360 76800 input MOC/moc_256_N00_300.gray  7680 7680 1 b \
								paste 15360 69120 input MOC/moc_256_N00_270.gray  7680 7680 1 b \
								paste 15360 61440 input MOC/moc_256_N00_240.gray  7680 7680 1 b \
								paste 15360 53760 input MOC/moc_256_N00_210.gray  7680 7680 1 b \
								paste 15360 46080 input MOC/moc_256_N00_180.gray  7680 7680 1 b \
								paste 15360 38400 input MOC/moc_256_N00_150.gray  7680 7680 1 b \
								paste 15360 30720 input MOC/moc_256_N00_120.gray  7680 7680 1 b \
								paste 15360 23040 input MOC/moc_256_N00_090.gray  7680 7680 1 b \
								paste 15360 15360 input MOC/moc_256_N00_060.gray  7680 7680 1 b \
								paste 15360  7680 input MOC/moc_256_N00_030.gray  7680 7680 1 b \
								paste 15360     0 input MOC/moc_256_N00_000.gray  7680 7680 1 b \
								paste  7680 84480 input MOC/moc_256_N30_330.gray  7680 7680 1 b \
								paste  7680 76800 input MOC/moc_256_N30_300.gray  7680 7680 1 b \
								paste  7680 69120 input MOC/moc_256_N30_270.gray  7680 7680 1 b \
								paste  7680 61440 input MOC/moc_256_N30_240.gray  7680 7680 1 b \
								paste  7680 53760 input MOC/moc_256_N30_210.gray  7680 7680 1 b \
								paste  7680 46080 input MOC/moc_256_N30_180.gray  7680 7680 1 b \
								paste  7680 38400 input MOC/moc_256_N30_150.gray  7680 7680 1 b \
								paste  7680 30720 input MOC/moc_256_N30_120.gray  7680 7680 1 b \
								paste  7680 23040 input MOC/moc_256_N30_090.gray  7680 7680 1 b \
								paste  7680 15360 input MOC/moc_256_N30_060.gray  7680 7680 1 b \
								paste  7680  7680 input MOC/moc_256_N30_030.gray  7680 7680 1 b \
								paste  7680     0 input MOC/moc_256_N30_000.gray  7680 7680 1 b \
								paste     0 84480 input MOC/moc_256_N60_330.gray  7680 7680 1 b \
								paste     0 76800 input MOC/moc_256_N60_300.gray  7680 7680 1 b \
								paste     0 69120 input MOC/moc_256_N60_270.gray  7680 7680 1 b \
								paste     0 61440 input MOC/moc_256_N60_240.gray  7680 7680 1 b \
								paste     0 53760 input MOC/moc_256_N60_210.gray  7680 7680 1 b \
								paste     0 46080 input MOC/moc_256_N60_180.gray  7680 7680 1 b \
								paste     0 38400 input MOC/moc_256_N60_150.gray  7680 7680 1 b \
								paste     0 30720 input MOC/moc_256_N60_120.gray  7680 7680 1 b \
								paste     0 23040 input MOC/moc_256_N60_090.gray  7680 7680 1 b \
								paste     0 15360 input MOC/moc_256_N60_060.gray  7680 7680 1 b \
								paste     0  7680 input MOC/moc_256_N60_030.gray  7680 7680 1 b \
								                  input MOC/moc_256_N60_000.gray  7680 7680 1 b
