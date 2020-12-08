cmake_minimum_required(VERSION 3.18)
project(Valkyrie)

find_package(Vulkan REQUIRED)
find_package(CUDAToolkit REQUIRED)
include_directories(include ${Vulkan_INCLUDE_DIRS})
if(MSVC)
    add_compile_options(/Zc:preprocessor)
endif()

set(CMAKE_CXX_STANDARD 20)

#set(OptiX_INCLUDE_PATH "C:/ProgramData/NVIDIA Corporation/OptiX SDK 7.2.0/include")
#include_directories(${OptiX_INCLUDE_PATH})

function(add_dummy_target file)
    string(REGEX REPLACE "\\..+" "" targetName ${file})
    string(REGEX REPLACE "/" "." targetName ${targetName})
    string(PREPEND targetName "DUMMY.")
    add_library(${targetName} src/dummy.cpp ${file} ${ARGN})
endfunction()

function(add_object name_)
    string(REGEX REPLACE "\\." "/" objectPath ${name_})
    string(PREPEND objectPath "src/")
    string(APPEND objectPath ".cpp")
    add_library(${name_} OBJECT ${objectPath})
endfunction()

function(valkyrie_test name_)
    string(REGEX REPLACE "\\." "/" testPath ${name_})
    string(REGEX REPLACE "\\." "/" sourcePath ${name_})
    string(PREPEND testPath "test/")
    string(PREPEND sourcePath "src/")
    string(PREPEND name_ "test.")
    string(APPEND testPath ".cpp")
    string(APPEND sourcePath ".cpp")
    add_executable(${name_} ${testPath} ${sourcePath})
endfunction()

function(add_object_library name_)
    set(singleValueArgs)
    set(multiValueArgs OBJECTS)
    set(optionalArgs)
    cmake_parse_arguments(OBJECT_LIB "${optionalArgs}" "${singleValueArgs}" "${multiValueArgs}" ${ARGN})
    list(TRANSFORM OBJECT_LIB_OBJECTS PREPEND ${name_}.)
    list(TRANSFORM OBJECT_LIB_OBJECTS REPLACE ".+" "$<TARGET_OBJECTS:\\0>")
    add_library(${name_} ${OBJECT_LIB_OBJECTS})
endfunction()

add_object(spirv.SpirV)

add_object(valkyrie.Core.Async.Semaphore)

add_object(valkyrie.Core.Log.Messages)
add_object(valkyrie.Core.Log.Serialize)
add_object(valkyrie.Core.Log.Logger)

add_object(valkyrie.Core.Memory)
add_object(valkyrie.Core.Utility)
add_object(valkyrie.Core.Error)
add_object(valkyrie.Core.SystemError)

add_object(valkyrie.Graphics.API.Instance)
add_object(valkyrie.Graphics.API.PhysicalDevice)
add_object(valkyrie.Graphics.API.Device)
add_object(valkyrie.Graphics.API.Queue)
add_object(valkyrie.Graphics.API.CommandBuffer)
add_object(valkyrie.Graphics.API.Pipeline)
add_object(valkyrie.Graphics.API.Loader)
add_object(valkyrie.Graphics.API.Surface)
add_object(valkyrie.Graphics.API.Swapchain)

add_object(valkyrie.GUI.Window)
add_object(valkyrie.GUI.Monitor)
add_object(valkyrie.GUI.DXGI)

add_object(valkyrie.Graphics.Engine.Engine)
add_object(valkyrie.Graphics.Engine.Option)

add_object_library(valkyrie.Core OBJECTS #[[Log.Messages]] Log.Serialize Log.Logger Memory Utility Error SystemError)
add_object_library(valkyrie.Graphics.API OBJECTS Instance PhysicalDevice Device Queue CommandBuffer Loader Surface)
add_object_library(valkyrie.Graphics.Engine OBJECTS Engine Option)
add_object_library(valkyrie.GUI OBJECTS Window Monitor)

target_link_libraries(valkyrie.GUI PUBLIC $<$<CXX_COMPILER_ID:MSVC>:Dxva2>)


add_library(valkyrie src/valkyrie/valkyrie.cpp)
target_link_libraries(valkyrie PUBLIC valkyrie.Core valkyrie.Graphics.API)


add_executable(valk_main main.cpp vk_object.h vk_util.h vk_object.cpp vk_pp.h vk_features.h include/spirv/Enums.hpp)

add_dummy_target(include/valkyrie.hpp include/valkyrie/Graphics.hpp include/valkyrie/Graphics/Image.hpp include/valkyrie/Graphics/Pixel.hpp)

valkyrie_test(valkyrie.Core.Utility.Arrays#[[ test/valkyrie/Core/Utility/Array.cpp]])
valkyrie_test(valkyrie.Graphics.API.Device)
valkyrie_test(valkyrie.Core.Utility.Enums)
valkyrie_test(valkyrie.GUI.Monitor)
valkyrie_test(valkyrie.GUI.DXGI)
valkyrie_test(valkyrie.Core.Log.Messages)

target_link_libraries(test.valkyrie.GUI.Monitor PUBLIC valkyrie.Core $<$<CXX_COMPILER_ID:MSVC>:Dxva2> #[[CUDA::cuda_driver Vulkan::Vulkan]])
target_link_libraries(test.valkyrie.GUI.DXGI PUBLIC valkyrie.Core $<$<CXX_COMPILER_ID:MSVC>:DXGI>)
target_link_libraries(test.valkyrie.Core.Log.Messages PUBLIC valkyrie.Core $<$<CXX_COMPILER_ID:MSVC>:DbgHelp;Kernel32>)



function(preprocess file path)
    message(STATUS "Preprocessing: ${path}/${file}")
    add_custom_target(${file}.preprocess COMMAND "cl.exe /std:c++latest /E ${path}/${file}")
endfunction()

preprocess(Traits.hpp include/valkyrie/Core/Utility)