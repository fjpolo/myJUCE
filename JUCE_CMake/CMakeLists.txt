cmake_minimum_required(VERSION 3.16)
project(JUCECMakeRepo)

#including CPM.cmake, a package manager:
#https://github.com/TheLartians/CPM.cmake
include(CMake/CPM.cmake)

#Compile commands, useful for some IDEs like VS-Code
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

#Minimum MacOS target, set globally
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.9" CACHE STRING "Minimum OS X deployment version" FORCE)

option(UniversalBinary "Build universal binary for mac" OFF)

if (UniversalBinary)
    set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE INTERNAL "")
endif()

#static linking in Windows
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

#
# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall  -O3 -march=native -std=c++11")
# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -march=native -std=c++11")
ADD_DEFINITIONS (-Wall -std=c++11 -pthread -g -D_WIN32_WINNT=0x0501)

#Adds all the module sources so they appear correctly in the IDE
set_property(GLOBAL PROPERTY USE_FOLDERS YES)
option(JUCE_ENABLE_MODULE_SOURCE_GROUPS "Enable Module Source Groups" ON)

#set any of these to "ON" if you want to build one of the juce examples
#or extras (Projucer/AudioPluginHost, etc):
option(JUCE_BUILD_EXTRAS "Build JUCE Extras" OFF)
option(JUCE_BUILD_EXAMPLES "Build JUCE Examples" OFF)

#Fetching JUCE from git
#IF you want to instead point it to a local version, you can invoke CMake with
#-DCPM_JUCE_SOURCE="Path_To_JUCE"
CPMAddPackage(
        NAME JUCE
        GITHUB_REPOSITORY juce-framework/JUCE
        GIT_TAG origin/develop)

#By default, we are building
option(BUILD_JUCE_PROTOTYPE_EXAMPES "Build JUCE prototype examples" ON)

if (${BUILD_JUCE_PROTOTYPE_EXAMPES})
    #adding any custom modules you might have:
    add_subdirectory(Modules)

    #adding project folders:
    add_subdirectory(Apps)
    add_subdirectory(Plugins)
endif()
