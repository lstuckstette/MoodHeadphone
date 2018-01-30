all: 
	g++ -Wall -o run src/main.cpp \
		-I ./include \
		-I /usr/lib/arm-linux-gnueabihf/glib-2.0/include \
		-I /usr/include/glib-2.0 \
		-L ./lib \
		-l gobject-2.0 \
		-l gio-2.0 \
		-l glib-2.0 \
		-l wiringPi \
		-l pthread \
		-l glib-2.0 \
		-l ggk \
		-std=c++11
	
clean:
	rm *o

