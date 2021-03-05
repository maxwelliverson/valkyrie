cmake_minimum_required(VERSION 3.18)
project(Valkyrie)

find_package(Vulkan REQUIRED)
find_package(CUDAToolkit REQUIRED)
message(STATUS "Vulkan Include Directory: ${Vulkan_INCLUDE_DIRS}")
include_directories(include ${Vulkan_INCLUDE_DIRS})
if(MSVC)
    add_compile_options(/Zc:preprocessor)
endif()

if(VALKYRIE_ENABLE_ASAN)
    set($ENV{ASAN_OPTIONS} windows_hook_rtl_allocators=true)
    if (CMAKE_SYSTEM_NAME MATCHES Windows)
      set(ASAN_RELEASE_COMPILE_OPTIONS /Zi)
    endif()
    if(VALKYRIE_DYNAMIC_CRT)
        set(ASAN_DEBUG_LIBRARY clang_rt.asan_cxx_dbg-x86_64 $<$<PLATFORM_ID:Windows>:clang_rt.asan_dynamic_dbg-x86_64>)
        set(ASAN_RELEASE_LIBRARY clang_rt.asan_cxx-x86_64 $<$<PLATFORM_ID:Windows>:clang_rt.asan_dynamic-x86_64>)
        set(ASAN_DEBUG_COMPILE_OPTIONS $<$<PLATFORM_ID:Windows>:/MDd>)
        list(APPEND ASAN_RELEASE_COMPILE_OPTIONS $<$<PLATFORM_ID:Windows>:/MD>)
    else()
        set(ASAN_DEBUG_LIBRARY clang_rt.asan_cxx_dbg-x86_64 $<$<PLATFORM_ID:Windows>:clang_rt.asan_dbg-x86_64>)
        set(ASAN_RELEASE_LIBRARY clang_rt.asan_cxx-x86_64 $<$<PLATFORM_ID:Windows>:clang_rt.asan-x86_64>)
        set(ASAN_DEBUG_COMPILE_OPTIONS $<$<PLATFORM_ID:Windows>:/MTd>)
        list(APPEND ASAN_RELEASE_COMPILE_OPTIONS $<$<PLATFORM_ID:Windows>:/MT>)
    endif()
    if (CMAKE_BUILD_TYPE MATCHES Debug)
        set(ASAN_LIBRARY ${ASAN_DEBUG_LIBRARY})
        set(ASAN_COMPILE_OPTIONS ${ASAN_DEBUG_COMPILE_OPTIONS})
        if (NOT VALKYRIE_DYNAMIC_CRT)
            add_link_options($<$<PLATFORM_ID:Windows>:/force:multiple>)
        endif()
    else()
        set(ASAN_LIBRARY ${ASAN_RELEASE_LIBRARY})
        set(ASAN_COMPILE_OPTIONS ${ASAN_RELEASE_COMPILE_OPTIONS})
    endif()
    link_libraries(${ASAN_LIBRARY})
    add_compile_options(${ASAN_COMPILE_OPTIONS})
    set($ENV{ASAN_OPTIONS} atexit=true print_stats=true)
endif()



set(CMAKE_CXX_STANDARD 20)

set(OptiX_INCLUDE_PATH "C:/ProgramData/NVIDIA Corporation/OptiX SDK 7.2.0/include")
include_directories(${OptiX_INCLUDE_PATH})

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
add_object(spirv.Shader)

add_object(valkyrie.agent.mailbox)
add_object(valkyrie.agent.backend)
add_object(valkyrie.agent.registry)
add_object(valkyrie.agent.private_agent)
add_object(valkyrie.agent.local_agent)
add_object(valkyrie.agent.shared_agent)
add_object(valkyrie.agent.sender)
add_object(valkyrie.agent.receiver)
add_object(valkyrie.agent.agent)
add_object(valkyrie.agent.message)

add_object(valkyrie.async.semaphore)
add_object(valkyrie.async.agent)

add_object(valkyrie.memory.manager)
add_object(valkyrie.memory)
add_object(valkyrie.utility)
add_object(valkyrie.error)
add_object(valkyrie.system_error)

add_object(valkyrie.win32_functions)

add_object(valkyrie.utility.json)
add_object(valkyrie.utility.random)

add_object(valkyrie.graphics.api.instance)
add_object(valkyrie.graphics.api.physical_device)
add_object(valkyrie.graphics.api.device)
add_object(valkyrie.graphics.api.queue)
add_object(valkyrie.graphics.api.command_buffer)
add_object(valkyrie.graphics.api.command_pool)
add_object(valkyrie.graphics.api.pipeline)
add_object(valkyrie.graphics.api.loader)
add_object(valkyrie.graphics.api.surface)
add_object(valkyrie.graphics.api.swapchain)
add_object(valkyrie.graphics.api.display)

add_object(valkyrie.gui.window)
add_object(valkyrie.gui.monitor)
add_object(valkyrie.gui.dxgi)

add_object(valkyrie.graphics.engine.engine)
add_object(valkyrie.graphics.engine.option)


add_object_library(spirv OBJECTS SpirV Shader)

add_object_library(valkyrie.agent           OBJECTS agent backend local_agent mailbox message private_agent receiver registry sender shared_agent)
add_object_library(valkyrie.async           OBJECTS semaphore agent)
add_object_library(valkyrie.graphics.api    OBJECTS instance physical_device device queue command_buffer loader surface display command_pool)
add_object_library(valkyrie.graphics.engine OBJECTS engine option)
add_object_library(valkyrie.gui             OBJECTS window monitor)

target_link_libraries(valkyrie.gui PUBLIC $<$<CXX_COMPILER_ID:MSVC>:Dxva2>)


add_library(valkyrie src/valkyrie/valkyrie.cpp)
#target_link_libraries(valkyrie PUBLIC valkyrie.Core valkyrie.graphics.api)


add_executable(valk_main main.cpp vk_object.h vk_util.h vk_object.cpp vk_pp.h vk_features.h include/spirv/enums.hpp)

add_dummy_target(include/valkyrie/graphics/image.hpp include/valkyrie/graphics/pixel.hpp)

#[[valkyrie_test(valkyrie.utility.Arrays)
valkyrie_test(valkyrie.graphics.api.Device)
valkyrie_test(valkyrie.utility.Enums)
valkyrie_test(valkyrie.GUI.Monitor)
valkyrie_test(valkyrie.GUI.DXGI)]]
valkyrie_test(valkyrie.memory)
valkyrie_test(valkyrie.graphics.api.swapchain)

add_executable(test.Mailbox test/Mailbox.cpp)
add_executable(test.VirtualFuncPtr test/VirtualFuncPtr.cpp)
target_compile_options(test.Mailbox PUBLIC -EHsc)


add_executable(catch
        test/valkyrie.cpp
        test/valkyrie/utility/Arrays.cpp
        test/valkyrie/utility/Enums.cpp
        test/valkyrie/Async/Agent.cpp
        test/valkyrie/graphics/api/Device.cpp
        test/valkyrie/GUI/DXGI.cpp
        test/valkyrie/GUI/Monitor.cpp)

add_executable(CudaGraph test/CudaGraph.cpp)
target_link_libraries(CudaGraph PUBLIC CUDA::cuda_driver)

add_executable(DirectedGraph test/directed_graph.cpp)
#target_link_libraries(directed_graph PUBLIC valkyrie.Core)

add_executable(RBTreeTest src/json/json_interval_tree.cpp)
add_executable(vulkan_example src/example.cpp)
add_executable(clangd_debug debug_clangd.cpp)
add_executable(tictactoe test/tictactoe.c)
add_executable(better_ttt test/better_tictactoe.c)


add_library(JSON src/json/json.c src/json/jsonmm.c src/json/jsonmm.h src/json/jsonctx.h src/json/jsonstring.h src/json/json_interval_tree.cpp src/json/jsonmm_types.h)




#target_link_libraries(test.valkyrie.GUI.Monitor PUBLIC valkyrie.Core $<$<CXX_COMPILER_ID:MSVC>:Dxva2> #[[CUDA::cuda_driver Vulkan::Vulkan]])
#target_link_libraries(test.valkyrie.GUI.DXGI PUBLIC valkyrie.Core $<$<CXX_COMPILER_ID:MSVC>:DXGI>)
#target_link_libraries(test.valkyrie.graphics.api.Swapchain PUBLIC valkyrie.Core valkyrie.graphics.api.Loader $<$<CXX_COMPILER_ID:MSVC>:DXGI>)


function(preprocess file path)
    message(STATUS "Preprocessing: ${path}/${file}")
    add_custom_target(${file}.preprocess COMMAND "cl.exe /std:c++latest /E ${path}/${file}")
endfunction()

preprocess(traits.hpp include/valkyrie/utility)