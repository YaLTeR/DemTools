cmake_minimum_required (VERSION 3.1)
project (DemTools)

if (NOT MSVC)
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++14 -march=native -mtune=native -Ofast -Wall -Wextra")
endif ()

add_subdirectory ("HLDemo")
include_directories ("HLDemo/src")

set (Boost_USE_STATIC_LIBS ON)
if (MSVC)
	find_package (Boost 1.57.0 REQUIRED COMPONENTS nowide)
else ()
	find_package (Boost 1.57.0 REQUIRED)
endif ()
include_directories (${Boost_INCLUDE_DIR})

set (TOOLS
     DemoSanitizer
     )

foreach (TOOL ${TOOLS})
    add_executable (${TOOL} src/${TOOL}.cpp)
    target_link_libraries (${TOOL} HLDemo ${Boost_LIBRARIES})
endforeach ()
