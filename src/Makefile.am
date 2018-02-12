# TODO add those flags
#debug: CXXFLAGS += '-ggdb'
#opt:   CXXFLAGS += '-O2'

headers = '-I../include/'
flags = @AM_CXXFLAGS@ $(headers)

lib_LTLIBRARIES = libwtk-sdl2.la

libwtk_sdl2_la_SOURCES = \
	bin.cpp                \
	box.cpp                \
	button.cpp             \
	color_widget.cpp       \
	container.cpp          \
	draw_context.cpp       \
	empty.cpp              \
	font_word_cache.cpp    \
	geometry.cpp           \
	label.cpp              \
	layout_info.cpp        \
	list_view.cpp          \
	mouse_event.cpp        \
	notebook.cpp           \
	padding.cpp            \
	sdl_util.cpp           \
	selectable.cpp         \
	selection_context.cpp  \
	swipe_area.cpp         \
	swipe_detector.cpp     \
	table.cpp              \
	util.cpp               \
	widget.cpp             \
	widget_context.cpp

libwtk_sdl2_la_CXXFLAGS = $(flags) $(sdl2_CFLAGS) $(SDL2_ttf_CFLAGS)
libwtk_sdl2_la_LIBADD = $(sdl2_LIBS) $(SDL2_ttf_LIBS)


bin_PROGRAMS = libwtk-sdl2-test
libwtk_sdl2_test_SOURCES = gui.cpp
libwtk_sdl2_test_LDADD = libwtk-sdl2.la
libwtk_sdl2_test_CXXFLAGS = $(flags)

