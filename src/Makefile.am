lib_LTLIBRARIES = libwtk-sdl2.la

libwtk_sdl2_la_SOURCES = \
	bin.cpp                \
	bin.hpp                \
	box.cpp                \
	box.hpp                \
	button.cpp             \
	button.hpp             \
	color_widget.cpp       \
	color_widget.hpp       \
	container.cpp          \
	container.hpp          \
	draw_context.cpp       \
	draw_context.hpp       \
	empty.cpp              \
	empty.hpp              \
	font_word_cache.cpp    \
	font_word_cache.hpp    \
	geometry.cpp           \
	geometry.hpp           \
	key_event.hpp          \
	label.cpp              \
	label.hpp              \
	layout_info.cpp        \
	layout_info.hpp        \
	list_view.cpp          \
	list_view.hpp          \
	mouse_event.cpp        \
	mouse_event.hpp        \
	notebook.cpp           \
	notebook.hpp           \
	padding.cpp            \
	padding.hpp            \
	sdl_util.cpp           \
	sdl_util.hpp           \
	selectable.cpp         \
	selectable.hpp         \
	selection_context.cpp  \
	selection_context.hpp  \
	swipe_area.cpp         \
	swipe_area.hpp         \
	swipe_detector.cpp     \
	swipe_detector.hpp     \
	swipe_event.hpp        \
	table.cpp              \
	table.hpp              \
	util.cpp               \
	util.hpp               \
	widget.cpp             \
	widget.hpp             \
	widget_context.cpp     \
	widget_context.hpp

libwtk_sdl2_la_CXXFLAGS = '-std=c++17' '-std=gnu++17' '-Wall' $(sdl2_CFLAGS) $(SDL2_ttf_CFLAGS)
libwtk_sdl2_la_LIBADD = $(sdl2_LIBS) $(SDL2_ttf_LIBS)


bin_PROGRAMS = libwtk-sdl2-test
libwtk_sdl2_test_SOURCES = gui.cpp
libwtk_sdl2_test_LDADD = libwtk-sdl2.la
libwtk_sdl2_test_CXXFLAGS = '-std=c++17' '-std=gnu++17' '-Wall'

#debug: CXXFLAGS += '-ggdb'
#opt:   CXXFLAGS += '-O2'
#
#main: $(OBJ)
#	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(OBJ) -o main
#
#debug: main
#
#opt: main
#
#%.o: %.c
#	$(CXX) $(CXXFLAGS) -c $<
#
#clean:
#	rm $(OBJ) main
