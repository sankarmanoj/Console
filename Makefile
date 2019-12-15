
all: console

console: console.cpp
	g++ console.cpp -L/usr/X11/lib -lX11 -g  -o console

c2: c2.cpp
	g++ c2.cpp -L/usr/X11/lib -lX11 -g  -o c2
