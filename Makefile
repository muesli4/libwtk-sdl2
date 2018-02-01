CXX      = g++

LIBS     = SDL2_ttf SDL2_image sdl2

CXXFLAGS = '-std=c++11' '-std=gnu++11' '-Wall' `pkg-config --cflags $(LIBS)`
LDFLAGS  = -lpthread `pkg-config --libs $(LIBS)`

OBJ      = draw_context.o selection_context.o widget.o geometry.o button.o color_widget.o padding.o mouse_event.o gui.o font_atlas.o util.o sdl_util.o

debug: CXXFLAGS += '-ggdb'
opt:   CXXFLAGS += '-O2'

main: $(OBJ)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(OBJ) -o main

debug: main

opt: main

%.o: %.c
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm $(OBJ) main
