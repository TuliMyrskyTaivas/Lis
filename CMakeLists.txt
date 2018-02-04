cmake_minimum_required(VERSION 3.6)
project(Lis)

# C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Tune the warning level
if (CMAKE_COMPILER_IS_GNUCXX)
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall")
elseif (MSVC)
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX")
endif ()

# Qt5 library
find_package(Qt5Widgets)
find_package(Qt5OpenGL)
set(CMAKE_AUTOMOC ON) # enable MOC automatically for Qt targets
set(CMAKE_AUTORCC ON) # enable RCC automatically for Qt targets

# OpenGL and GLEW libraries
find_package(OpenGL)

set (SOURCES
	GlWindow.h
	GlWindow.cpp
	PlanetWindow.h
	PlanetWindow.cpp
	Lis.cpp
	Logger.h
	Logger.cpp	
)

include_directories(${CMAKE_SOURCE_DIR})

add_executable(Lis ${SOURCES} textures.qrc)
qt5_use_modules(Lis Widgets OpenGL)
target_link_libraries(Lis ${QT_LIBRARIES} ${OPENGL_LIBRARIES})

