cmake_minimum_required (VERSION 2.6)
project (Blatt01)

# list of source files to compile
set(sources main.cpp GLSLProgram.cpp)

# find/include libraries
find_package(OpenGL REQUIRED)
# automatically finding GLEW FREEGLUT GLM ..
#find_package(GLEW REQUIRED)
#find_package(GLUT REQUIRED)
#find_package(GLM  REQUIRED)

# use static lib for GLEW and FREEGLUT
set(GLEW_STATIC TRUE)
if(GLEW_STATIC)
   add_definitions(-DGLEW_STATIC)
endif(GLEW_STATIC)
set(GLUT_STATIC TRUE)
if(GLUT_STATIC)
   add_definitions(-DFREEGLUT_STATIC)
endif(GLUT_STATIC)

# include and link directories
include_directories(${PROJECT_SOURCE_DIR}/libs/glew/include)
include_directories(${PROJECT_SOURCE_DIR}/libs/freeglut/include)
include_directories(${PROJECT_SOURCE_DIR}/libs/glm)
# please change the 2 lines below for your platform!
# here: vs2015_x64/Release
link_directories (${PROJECT_SOURCE_DIR}/libs/glew/lib/vs2015_x64/Release)
link_directories (${PROJECT_SOURCE_DIR}/libs/freeglut/lib/vs2015_x64/Release)

# executable Blatt01
add_executable (Blatt01 ${sources})
target_link_libraries(Blatt01 ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} ${GLEW_LIBRARIES} ${GLM_LIBRARIES} libglew32.lib freeglut_static.lib)
# copy the shader directory relative to the executable
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/shader
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
