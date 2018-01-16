all: 
	g++ -Wall -o run src/main.cpp \
		-I ./include \
		-L ./lib \
		-l wiringPi \
		-l pthread \
		-l glib-2.0 \
		-l ggk
	
clean:
	rm *o

