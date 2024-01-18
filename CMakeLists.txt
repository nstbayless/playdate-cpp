project("PlaydateCPP")

option(PDCPP_STAGE_IN_BINARY_DIR "Use CMake binary dir (instead of source dir) to stage files" OFF)

set(ENVSDK $ENV{PLAYDATE_SDK_PATH})
file(TO_CMAKE_PATH ${ENVSDK} SDK)
set(SDK ${SDK} PARENT_SCOPE)
message(STATUS "Playdate SDK Path: " ${SDK})
set(PDC "${SDK}/bin/pdc" -sdkpath "${SDK}" CACHE FILEPATH "path to the Playdate Compiler")

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/CompileTargetForPlaydate.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/AddPlaydateApplication.cmake)

# Build the C API as a static library first
add_library(playdate_sdk STATIC ${CMAKE_CURRENT_SOURCE_DIR}/src/setup.c)

target_compile_definitions(playdate_sdk PUBLIC TARGET_EXTENSION=1)

if (TOOLCHAIN STREQUAL "armgcc")
    message(STATUS "Building for Playdate hardware")

    # Device-only
    set(HEAP_SIZE 8388208)
    set(STACK_SIZE 61800)
    set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -x assembler-with-cpp -D__HEAP_SIZE=${HEAP_SIZE} -D__STACK_SIZE=${STACK_SIZE}")
    set(MCFLAGS -mthumb -mcpu=cortex-m7 -mfloat-abi=hard -mfpu=fpv5-sp-d16 -D__FPU_USED=1)

    target_compile_definitions(playdate_sdk PUBLIC TARGET_PLAYDATE=1)
    target_compile_options(playdate_sdk PUBLIC -Wall -Wno-unknown-pragmas -Wdouble-promotion)
    target_compile_options(playdate_sdk PUBLIC $<$<CONFIG:DEBUG>:-O2>)
    target_compile_options(playdate_sdk PUBLIC $<$<CONFIG:RELEASE>:-O3>)

    target_compile_options(playdate_sdk PUBLIC ${MCFLAGS})
    target_compile_options(playdate_sdk PUBLIC -falign-functions=16 -fomit-frame-pointer)
    target_compile_options(playdate_sdk PUBLIC -gdwarf-2)
    target_compile_options(playdate_sdk PUBLIC -fverbose-asm)
    target_compile_options(playdate_sdk PUBLIC -ffunction-sections -fdata-sections)
    target_compile_options(playdate_sdk PUBLIC -mword-relocations -fno-common)
    target_compile_options(playdate_sdk PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>)

    target_link_options(playdate_sdk PUBLIC ${MCFLAGS})
    target_link_options(playdate_sdk PUBLIC -T${CMAKE_CURRENT_SOURCE_DIR}/buildsupport/link_map.ld)
    target_link_options(playdate_sdk PUBLIC "-Wl,-Map=game.map,--cref,--gc-sections,--no-warn-mismatch,--emit-relocs")
    target_link_options(playdate_sdk PUBLIC --entry eventHandlerShim)
else ()
    # Simulator build defs
    target_compile_definitions(playdate_sdk PUBLIC TARGET_SIMULATOR=1)
    if (MSVC)
        target_compile_definitions(playdate_sdk PUBLIC _WINDLL=1)
        target_compile_options(playdate_sdk PUBLIC /W3)
        target_compile_options(playdate_sdk PUBLIC $<$<CONFIG:DEBUG>:/Od>)
    else()
        target_compile_options(playdate_sdk PUBLIC -Wall -Wstrict-prototypes -Wno-unknown-pragmas -Wdouble-promotion)
        target_compile_options(playdate_sdk PUBLIC $<$<CONFIG:DEBUG>:-ggdb -O0>)
    endif()
endif()
target_include_directories(playdate_sdk PUBLIC ${SDK}/C_API)

# now we can build the core PDCPP Core library
add_library(pdcpp_core STATIC ${CMAKE_CURRENT_SOURCE_DIR}/src/pdnewlib.c ${CMAKE_CURRENT_SOURCE_DIR}/src/pdnewdelete.cpp)
target_include_directories(pdcpp_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)
target_link_libraries(pdcpp_core playdate_sdk)

if (PDCPP_BUILD_EXAMPLES)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/examples)
endif ()
