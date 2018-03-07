# TODO add those flags
#debug: CXXFLAGS += '-ggdb'
#opt:   CXXFLAGS += '-O2'

AM_CPPFLAGS = -DPKGDATA="\"$(pkgdatadir)\""

headers = '-I../include/'
flags = @AM_CXXFLAGS@ $(headers)

lib_LTLIBRARIES = libwtk-sdl2.la

libwtk_sdl2_la_SOURCES = \
	bin.cpp                \
	box.cpp                \
	button.cpp             \
	color_widget.cpp       \
	container.cpp          \
	context_info.cpp       \
	draw_context.cpp       \
	empty.cpp              \
	font_manager.cpp       \
	font_word_cache.cpp    \
	geometry.cpp           \
	grid.cpp               \
	label.cpp              \
	list_view.cpp          \
	mouse_event.cpp        \
	mouse_tracker.cpp      \
	notebook.cpp           \
	padding.cpp            \
	region.cpp             \
	sdl_util.cpp           \
	selectable.cpp         \
	selection_context.cpp  \
	slider.cpp             \
	swipe.cpp              \
	swipe_area.cpp         \
	texture_view.cpp       \
	utf8.cpp               \
	util.cpp               \
	widget.cpp             \
	widget_context.cpp

libwtk_sdl2_la_CXXFLAGS = $(flags) $(sdl2_CFLAGS) $(SDL2_ttf_CFLAGS) $(SDL2_image_CFLAGS)
libwtk_sdl2_la_LIBADD = $(sdl2_LIBS) $(SDL2_ttf_LIBS) $(SDL2_image_LIBS)


bin_PROGRAMS = libwtk-sdl2-test
libwtk_sdl2_test_SOURCES = gui.cpp
libwtk_sdl2_test_LDADD = libwtk-sdl2.la $(SDL2_image_LIBS)
libwtk_sdl2_test_CXXFLAGS = $(flags)

