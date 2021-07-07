link = -I/usr/local/include/opencv4
pkglib = `pkg-config opencv4 --static --cflags --libs`

all: edge_detector Contour_drawer


Contour_drawer: Contour_drawer.cpp
	g++ Contour_drawer.cpp $(link) $(pkglib) -O3 -o Contour_drawer
edge_detector: edge.cpp
	g++ edge.cpp $(link) $(pkglib) -O3 -o edge_detector

clean:
	rm edge_detector Contour_drawer

