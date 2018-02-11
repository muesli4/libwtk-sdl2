CXX      = g++

LIBS     = SDL2_ttf SDL2_image sdl2

CXXFLAGS = '-std=c++17' '-std=gnu++17' '-Wall' `pkg-config --cflags $(LIBS)`
LDFLAGS  = -lpthread `pkg-config --libs $(LIBS)`

OBJ      = draw_context.o selection_context.o widget.o geometry.o selectable.o container.o box.o notebook.o button.o color_widget.o padding.o mouse_event.o swipe_area.o list_view.o swipe_detector.o widget_context.o gui.o font_word_cache.o util.o sdl_util.o bin.o layout_info.o label.o empty.o table.o

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
