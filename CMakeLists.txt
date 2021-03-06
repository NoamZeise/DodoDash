cmake_minimum_required(VERSION 3.10)

#customize
set(ProjectName "The-Last-Dodo")

#windows only
set(Lib "D:/noam4/Libraries/VS/lib/x64/")
set(Include "D:/noam4/Libraries/VS/include")

project(${ProjectName} VERSION 1.0)

set(CMAKE_CXX_STANDARD 14)

#packages
find_package(Vulkan REQUIRED)

if (UNIX)
    find_package(X11 REQUIRED)
    link_libraries(${X11_LIBRARIES})
    add_definitions(-DVK_USE_PLATFORM_XLIB_KHR)
    find_package(glfw3 3.3 REQUIRED)
    find_package(Freetype REQUIRED)
    set(CMAKE_CXX_FLAGS "-O2 -std=c++14")
    set(CMAKE_EXE_LINKER_FLAGS "-v")

elseif (MSVC)
    # https://docs.microsoft.com/en-us/cpp/build/reference/compiler-options-listed-by-category?view=msvc-170
    set(CMAKE_CXX_FLAGS_DEBUG_INIT "/DDEBUG /MDd /Zi /Ob0 /Od /RTC1")
    set(CMAKE_CXX_FLAGS_RELEASE_INIT "/MD /Zi /O2 /Ob1 /DNDEBUG")
endif()

#add executables
file(GLOB SOURCES src/*)
file(GLOB VULKAN-RENDER src/vulkan-render/*)
add_executable(${ProjectName} ${SOURCES}
                              ${VULKAN-RENDER}
                              ${PROJ_INCLUDE}
                            "resources/resource/resource.o")

#link
target_include_directories(${ProjectName} PUBLIC include/)
if(UNIX)
    target_include_directories(${ProjectName} PUBLIC ${GLFW_INCLUDE_DIRS}
                                                 PUBLIC ${VULKAN_INCLUDE_DIRS}
                                                 PUBLIC ${FREETYPE_INCLUDE_DIRS}
                                                 PUBLIC ${PROJECT_BINARY_DIR})
    target_link_libraries(${ProjectName} glfw
                                         Vulkan::Vulkan
                                         ${FREETYPE_LIBRARIES}
                                         assimp
                                         sndfile
                                         portaudio)
    #add assimp
elseif(MSVC)
    target_include_directories(${ProjectName} PUBLIC ${Include})
    target_link_libraries(${ProjectName} ${Lib}glfw3.lib
                                         ${Lib}freetyped.lib
                                         ${Lib}vulkan-1.lib
                                         ${Lib}portaudio_x64.lib
                                         ${Lib}sndfile.lib)
endif()
