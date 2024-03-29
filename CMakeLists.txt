cmake_minimum_required(VERSION 3.18)
project(Valkyrie)


set(DEFAULT_ALLOCATOR_OPTIONS new static virtual malloc heap)

if(NOT DEFAULT_ALLOCATOR)
    set(DEFAULT_ALLOCATOR new)
endif()

list(FIND DEFAULT_ALLOCATOR_OPTIONS ${DEFAULT_ALLOCATOR} DEFAULT_ALLOC_IS_VALID)

if (DEFAULT_ALLOC_IS_VALID EQUAL -1)
    message(ERROR "DEFAULT_ALLOCATOR: ${DEFAULT_ALLOCATOR}\nDEFAULT_ALLOCATOR must match one of the following values: {${DEFAULT_ALLOCATOR_OPTIONS}}")
endif()

add_compile_definitions(VALKYRIE_DEFAULT_ALLOCATOR=${DEFAULT_ALLOCATOR})


find_package(Vulkan REQUIRED)
find_package(CUDAToolkit REQUIRED)
message(STATUS "Vulkan Include Directory: ${Vulkan_INCLUDE_DIRS}")
include_directories(include ${Vulkan_INCLUDE_DIRS})
if((MSVC) AND (NOT CMAKE_CXX_COMPILER_ID MATCHES Clang))
    add_compile_options(/Zc:preprocessor)
endif()

if(VALKYRIE_ENABLE_ASAN)
    set($ENV{ASAN_OPTIONS} windows_hook_rtl_allocators=true atexit=true print_stats=true)
    add_compile_options(/fsanitize=address)
endif()

#[[if(VALKYRIE_ENABLE_ASAN)
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
endif()]]



set(CMAKE_CXX_STANDARD 20)
#set(CMAKE_C_STANDARD   11)

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
    add_library(${name_} STATIC ${OBJECT_LIB_OBJECTS})
endfunction()






add_object(spirv.SpirV)
add_object(spirv.Shader)


add_object(valkyrie.adt.small_vector)
add_object(valkyrie.adt.dictionary)
add_object(valkyrie.adt.concurrent_list)
add_object(valkyrie.adt.hazard_ptr)
add_object(valkyrie.adt.rb_tree)
add_object(valkyrie.adt.twine)


add_object(valkyrie.memory.detail.debug_helpers)
add_object(valkyrie.memory.detail.free_list)
add_object(valkyrie.memory.detail.free_list_array)
add_object(valkyrie.memory.detail.small_free_list)

add_object(valkyrie.memory.debugging)
add_object(valkyrie.memory.error)
add_object(valkyrie.memory.heap_allocator)
add_object(valkyrie.memory.iteration_allocator)
add_object(valkyrie.memory.malloc_allocator)
add_object(valkyrie.memory.memory_arena)
add_object(valkyrie.memory.memory_pool)
add_object(valkyrie.memory.memory_pool_collection)
add_object(valkyrie.memory.memory_stack)
add_object(valkyrie.memory.new_allocator)
add_object(valkyrie.memory.static_allocator)
add_object(valkyrie.memory.temporary_allocator)
add_object(valkyrie.memory.virtual_memory)



add_object(valkyrie.agent.mailbox)
add_object(valkyrie.agent.backend)
add_object(valkyrie.agent.channel)
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
add_object(valkyrie.async.task)


add_object(valkyrie.status.domains.std)

add_object(valkyrie.status.error)
add_object(valkyrie.status.system)

add_object(valkyrie.win32_functions)

add_object(valkyrie.utility.simdjson)
add_object(valkyrie.utility.json)
add_object(valkyrie.utility.cpu)
add_object(valkyrie.utility.simd)
add_object(valkyrie.utility.random)
add_object(valkyrie.utility.library)
add_object(valkyrie.utility.demangle)
add_object(valkyrie.utility.components)
add_object(valkyrie.utility.reflect)
add_object(valkyrie.utility.uuid)

add_object(valkyrie.graphics.api.instance)
add_object(valkyrie.graphics.api.physical_device)
add_object(valkyrie.graphics.api.device)
add_object(valkyrie.graphics.api.queue)
add_object(valkyrie.graphics.api.structures)
add_object(valkyrie.graphics.api.command_buffer)
add_object(valkyrie.graphics.api.command_pool)
add_object(valkyrie.graphics.api.pipeline)
add_object(valkyrie.graphics.api.loader)
add_object(valkyrie.graphics.api.surface)
add_object(valkyrie.graphics.api.swapchain)
add_object(valkyrie.graphics.api.display)
add_object(valkyrie.graphics.api.api)
add_object(valkyrie.graphics.api.library)

add_object(valkyrie.gui.window)
add_object(valkyrie.gui.monitor)
add_object(valkyrie.gui.dxgi)

add_object(valkyrie.graphics.engine.engine)
add_object(valkyrie.graphics.engine.option)
add_object(valkyrie.graphics.engine.obj)
add_object(valkyrie.graphics.engine.resource)
add_object(valkyrie.graphics.engine.dispatch_table)



add_object_library(spirv OBJECTS SpirV Shader)

add_object_library(valkyrie.agent           OBJECTS agent backend channel local_agent mailbox message private_agent receiver registry sender shared_agent)
add_object_library(valkyrie.async           OBJECTS semaphore agent)
add_object_library(valkyrie.graphics.api    OBJECTS instance physical_device device queue command_buffer loader surface display command_pool)
add_object_library(valkyrie.graphics.engine OBJECTS engine option)
add_object_library(valkyrie.gui             OBJECTS window monitor)
add_object_library(valkyrie.status          OBJECTS error system)
add_object_library(valkyrie.memory.detail   OBJECTS debug_helpers free_list free_list_array small_free_list)
add_object_library(valkyrie.memory          OBJECTS debugging error heap_allocator iteration_allocator malloc_allocator memory_arena memory_pool memory_pool_collection memory_stack new_allocator static_allocator temporary_allocator virtual_memory)
add_object_library(valkyrie.utility         OBJECTS simd uuid reflect components library demangle random json cpu)


target_link_libraries(valkyrie.gui PUBLIC $<$<CXX_COMPILER_ID:MSVC>:Dxva2>)
target_link_libraries(valkyrie.memory PUBLIC valkyrie.memory.detail)

add_library(valkyrie.graphics STATIC src/valkyrie/graphics.cpp)
target_link_libraries(valkyrie.graphics PUBLIC
        valkyrie.graphics.api
        valkyrie.graphics.engine)


add_library(valkyrie STATIC src/valkyrie.cpp)
target_link_libraries(valkyrie PUBLIC
        valkyrie.adt
        valkyrie.agent
        valkyrie.async
        valkyrie.graphics
        valkyrie.gui
        valkyrie.memory
        valkyrie.status
        valkyrie.utility)




#target_link_libraries(valkyrie PUBLIC valkyrie.Core valkyrie.graphics.api)


add_executable(valk_main main.cpp vk_object.h vk_util.h vk_object.cpp vk_pp.h vk_features.h include/spirv/enums.hpp)

add_dummy_target(include/valkyrie/graphics/image.hpp include/valkyrie/graphics/pixel.hpp)

#[[valkyrie_test(valkyrie.utility.Arrays)
valkyrie_test(valkyrie.graphics.api.Device)
valkyrie_test(valkyrie.utility.Enums)
valkyrie_test(valkyrie.GUI.Monitor)
valkyrie_test(valkyrie.GUI.DXGI)]]
valkyrie_test(valkyrie.utility.cpu)
valkyrie_test(valkyrie.memory)
valkyrie_test(valkyrie.graphics.api.swapchain)
valkyrie_test(valkyrie.graphics.api.api)
target_link_libraries(test.valkyrie.graphics.api.swapchain PUBLIC valkyrie.status #[[valkyrie.graphics.api]])


add_executable(test.valkyrie.utility.waymarking test/valkyrie/utility/waymarking.cpp)

add_executable(test.Mailbox test/Mailbox.cpp)
add_executable(test.VirtualFuncPtr test/VirtualFuncPtr.cpp)
target_compile_options(test.Mailbox PUBLIC -EHsc)

add_executable(test.json.json test/json/json.c)



add_executable(catch
        test/valkyrie.cpp
        test/valkyrie/utility/Arrays.cpp
        test/valkyrie/utility/Enums.cpp
        test/valkyrie/Async/Agent.cpp
        test/valkyrie/Graphics/api/Device.cpp
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
add_executable(api_test test/api_test.cpp $<TARGET_OBJECTS:valkyrie.graphics.api.api>)
target_link_libraries(api_test PUBLIC valkyrie.status)


add_library(imgui STATIC IMPORTED)
add_library(imgui_vulkan STATIC IMPORTED)
add_library(imgui_glfw STATIC IMPORTED)
set(CUSTOM_LIBRARY_FOLDER ~/lib)
if(CMAKE_BUILD_TYPE MATCHES Debug)
    set(IMGUI_LIBRARY_NAME debug/dimgui)
    set(IMGUI_VULKAN_LIBRARY_NAME debug/dimgui_vulkan)
    set(IMGUI_GLFW_LIBRARY_NAME debug/dimgui_glfw)
else()
    set(IMGUI_LIBRARY_NAME imgui)
    set(IMGUI_VULKAN_LIBRARY_NAME imgui_vulkan)
    set(IMGUI_GLFW_LIBRARY_NAME imgui_glfw)
endif()

set_target_properties(imgui PROPERTIES IMPORTED_LOCATION "${CUSTOM_LIBRARY_FOLDER}/${IMGUI_LIBRARY_NAME}.lib")
set_target_properties(imgui_vulkan PROPERTIES IMPORTED_LOCATION "${CUSTOM_LIBRARY_FOLDER}/${IMGUI_VULKAN_LIBRARY_NAME}.lib")
set_target_properties(imgui_glfw PROPERTIES IMPORTED_LOCATION "${CUSTOM_LIBRARY_FOLDER}/${IMGUI_GLFW_LIBRARY_NAME}.lib")

add_executable(graph_gui test/graph_gui.cpp)
target_link_libraries(graph_gui PUBLIC valkyrie.status valkyrie.memory imgui imgui_vulkan imgui_glfw)
add_executable(windowsapi_test test/windowsapi.cpp)
target_link_libraries(windowsapi_test PUBLIC valkyrie.status)
add_executable(intrinsics_test test/intrin.cpp)




add_library(argent
        src/argent/agent.c
        src/argent/mailbox.c
        src/argent/message.c
        src/argent/sender.c
        src/argent/receiver.c
        src/argent/quartz/init.c
        src/argent/quartz/mailbox.c
        src/argent/quartz/shmem.c
        src/argent/mailbox/sssr.c
        src/argent/mailbox/ssmr.c
        src/argent/mailbox/mssr.c
        src/argent/mailbox/msmr.c
        src/argent/mailbox/cmssr.c
        src/argent/mailbox/cmsmr.c
        src/argent/mailbox/sscmr.c
        src/argent/mailbox/mscmr.c
        src/argent/mailbox/cmscmr.c)

set_target_properties(

        argent

        PROPERTIES
            C_STANDARD 11)



add_library(JSON src/json/json.c
                 src/json/jsonmm.c
                 src/json/jsonvmm.c
                 src/json/jsonvalue.c
                 src/json/jsonctx.c
                 src/json/jsonbitmap.c
                 src/json/jsonstring.c
                 src/json/jsonparser.c)
#set_target_properties(JSON PROPERTIES C_STANDARD 11)



#target_link_libraries(test.valkyrie.GUI.Monitor PUBLIC valkyrie.Core $<$<CXX_COMPILER_ID:MSVC>:Dxva2> #[[CUDA::cuda_driver Vulkan::Vulkan]])
#target_link_libraries(test.valkyrie.GUI.DXGI PUBLIC valkyrie.Core $<$<CXX_COMPILER_ID:MSVC>:DXGI>)
#target_link_libraries(test.valkyrie.graphics.api.Swapchain PUBLIC valkyrie.Core valkyrie.graphics.api.Loader $<$<CXX_COMPILER_ID:MSVC>:DXGI>)


function(preprocess file path)
    message(STATUS "Preprocessing: ${path}/${file}")
    add_custom_target(${file}.preprocess COMMAND "cl.exe /std:c++latest /E ${path}/${file}")
endfunction()

preprocess(traits.hpp include/valkyrie/utility)


add_executable(print_modules test/print_modules.cpp)

target_compile_definitions(print_modules PUBLIC PSAPI_VERSION=1)