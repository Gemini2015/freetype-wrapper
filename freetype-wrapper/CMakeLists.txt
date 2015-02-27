project(freetype-wrapper)

set(Header "Font.h")
set(SRC "Font.cpp")

add_library(freetype-wrapper STATIC ${Header} ${SRC})
target_link_libraries(freetype-wrapper ${FREETYPE_LIB})
