CXXFLAGS = -Wall -ggdb $(shell sdl-config --cflags)
LDFLAGS = $(shell sdl-config --libs) -lGL

OBJS = hyperspace.o

all: hyperspace

hyperspace: $(OBJS)
	$(CXX) -o hyperspace $(OBJS) $(LDFLAGS)

clean:
	rm -f hyperspace $(OBJS)
