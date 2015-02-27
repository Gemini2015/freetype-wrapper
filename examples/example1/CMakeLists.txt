project(example1)

set(SRC_LIB "main.cpp")

add_executable(${PROJECT_NAME} ${SRC_LIB})
target_link_libraries(${PROJECT_NAME} freetype-wrapper glew32 glu32 opengl32)