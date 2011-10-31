CXXFLAGS = -Wall -ggdb $(shell sdl-config --cflags) $(shell pkg-config glew --cflags)
LDFLAGS = $(shell sdl-config --libs) $(shell pkg-config glew --libs) -lGL

OBJS = hyperspace.o

all: hyperspace

hyperspace: $(OBJS)
	$(CXX) -o hyperspace $(OBJS) $(LDFLAGS)

clean:
	rm -f hyperspace $(OBJS)
