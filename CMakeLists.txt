cmake_minimum_required(VERSION 2.6.2)
project(axisviewer)

INCLUDE(FindPkgConfig)

#hack - add -std=c++0x to CXXFLAGS
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

set(UI_SOURCES ui/image_transport.cpp ui/main_window.cpp ui/url_chooser.cpp ui/no_comm.xpm)
set(MAIN_SOURCES camera_viewer.cpp image_processing.cpp stream_handler.cpp main.cpp)

set(SOURCES ${UI_SOURCES} ${MAIN_SOURCES})

#dependencies
if(${PKG_CONFIG_FOUND})
	#use pkg-config if available
	pkg_check_modules(CURL REQUIRED libcurl)
    #search for gtkmm 3
    pkg_check_modules(GTKMM gtkmm-3.0)
    if(${GTKMM_FOUND})
        set(GTKMM_API_VERSION 3)
    else(${GTKMM_FOUND})
        #look for gtkmm 2
        pkg_check_modules(GTKMM gtkmm-2.4)
        if (${GTKMM_FOUND})
            set(GTKMM_API_VERSION 2)
        else(${GTKMM_FOUND})
            message(SEND_ERROR "No compatible version of GTKMM found")
        endif(${GTKMM_FOUND})
    endif(${GTKMM_FOUND})
	#provide backwards compatibility?
else(${PKG_CONFIG_FOUND})
    #search for gtk2 first as gtk3 will probably fail
    find_package(GTK2 COMPONENTS gtkmm)
    if(${GTK2_FOUND})
        set(GTKMM_INCLUDE_DIRS ${GTK2_INCLUDE_DIRS})
        set(GTKMM_LIBRARIES ${GTK2_LIBRARIES})
        set(GTKMM_API_VERSION 2)
    else(${GTK2_FOUND})
        find_package(GTK3 COMPONENTS gtkmm) #this'll probably fail TODO: Fix
        if(${GTK3_FOUND})
            set(GTKMM_INCLUDE_DIRS ${GTK3_INCLUDE_DIRS})
            set(GTKMM_LIBRARIES ${GTK3_LIBRARIES})
            set(GTKMM_API_VERSION 3)
        else(${GTK3_FOUND})
            message(SEND_ERROR "No compatible version of GTKMM found")
        endif(${GTK3_FOUND})
    endif(${GTK2_FOUND})
	#again, needs fixes to work with GTKMM 2.4
	find_package(CURL REQUIRED)
endif(${PKG_CONFIG_FOUND})

if(${GTKMM_API_VERSION} EQUAL 2)
    set(USE_DEPRECATED_GTKMM_API 1)
endif(${GTKMM_API_VERSION} EQUAL 2)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR}) #for #include <config.h>

add_executable(axisviewer ${SOURCES})

include_directories(${GTKMM_INCLUDE_DIRS} ${CURL_INCLUDE_DIRS})
target_link_libraries(axisviewer ${GTKMM_LIBRARIES} ${CURL_LIBRARIES})

