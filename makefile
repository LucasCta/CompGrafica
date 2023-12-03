all: rgb main

rgb:
	g++ -c RgbImage.cpp -o rgb.o 

main:
	g++ main.cpp rgb.o -o dog -lglut -lGLU -lGL
