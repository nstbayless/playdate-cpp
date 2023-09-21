function(add_playdate_application PLAYDATE_GAME_NAME)
    message(STATUS "Adding playdate application ${PLAYDATE_GAME_NAME}")

    if (TOOLCHAIN STREQUAL "armgcc")
        add_executable(${PLAYDATE_GAME_NAME})

        set_property(TARGET ${PLAYDATE_GAME_NAME} PROPERTY OUTPUT_NAME "${PLAYDATE_GAME_NAME}.elf")

        add_custom_command(
            TARGET ${PLAYDATE_GAME_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_CURRENT_BINARY_DIR}/${BUILD_SUB_DIR}${PLAYDATE_GAME_NAME}.elf
            ${CMAKE_CURRENT_SOURCE_DIR}/Source/pdex.elf
        )

        add_custom_command(
            TARGET ${PLAYDATE_GAME_NAME} POST_BUILD
            COMMAND ${CMAKE_STRIP} --strip-unneeded -R .comment -g
            ${PLAYDATE_GAME_NAME}.elf
            -o ${CMAKE_CURRENT_SOURCE_DIR}/Source/pdex.elf
        )

        add_custom_command(
            TARGET ${PLAYDATE_GAME_NAME} POST_BUILD
            COMMAND ${PDC} Source ${PLAYDATE_GAME_NAME}.pdx
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )

        set_property(
            TARGET ${PLAYDATE_GAME_NAME} PROPERTY ADDITIONAL_CLEAN_FILES
            ${CMAKE_CURRENT_SOURCE_DIR}/${PLAYDATE_GAME_NAME}.pdx
        )

    else ()
        add_library(${PLAYDATE_GAME_NAME} SHARED)

        if (MSVC)
            if(${CMAKE_GENERATOR} MATCHES "Visual Studio*" )
                set(BUILD_SUB_DIR $<CONFIG>/)
                file(TO_NATIVE_PATH ${SDK}/bin/PlaydateSimulator.exe SIMPATH)
                file(TO_NATIVE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/${PLAYDATE_GAME_NAME}.pdx SIMGAMEPATH)
                set_property(TARGET ${PLAYDATE_GAME_NAME} PROPERTY VS_DEBUGGER_COMMAND ${SIMPATH})
                set_property(TARGET ${PLAYDATE_GAME_NAME} PROPERTY VS_DEBUGGER_COMMAND_ARGUMENTS "\"${SIMGAMEPATH}\"")
            endif()

            add_custom_command(
                    TARGET ${PLAYDATE_GAME_NAME} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy
                    ${CMAKE_CURRENT_BINARY_DIR}/${BUILD_SUB_DIR}${PLAYDATE_GAME_NAME}.dll
                    ${CMAKE_CURRENT_SOURCE_DIR}/Source/pdex.dll)
        elseif (MINGW)
            add_custom_command(
                    TARGET ${PLAYDATE_GAME_NAME} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy
                    ${CMAKE_CURRENT_BINARY_DIR}/lib${PLAYDATE_GAME_NAME}.dll
                    ${CMAKE_CURRENT_SOURCE_DIR}/Source/pdex.dll)
        elseif(APPLE)
            if(${CMAKE_GENERATOR} MATCHES "Xcode" )
                set(BUILD_SUB_DIR $<CONFIG>/)
                set_property(TARGET ${PLAYDATE_GAME_NAME} PROPERTY XCODE_SCHEME_ARGUMENTS \"${CMAKE_CURRENT_SOURCE_DIR}/${PLAYDATE_GAME_NAME}.pdx\")
                set_property(TARGET ${PLAYDATE_GAME_NAME} PROPERTY XCODE_SCHEME_EXECUTABLE ${SDK}/bin/Playdate\ Simulator.app)
            endif()

            add_custom_command(
                TARGET ${PLAYDATE_GAME_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_BINARY_DIR}/${BUILD_SUB_DIR}lib${PLAYDATE_GAME_NAME}.dylib
                ${CMAKE_CURRENT_SOURCE_DIR}/Source/pdex.dylib)

        elseif(UNIX)
            add_custom_command(
                TARGET ${PLAYDATE_GAME_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_BINARY_DIR}/lib${PLAYDATE_GAME_NAME}.so
                ${CMAKE_CURRENT_SOURCE_DIR}/Source/pdex.so)
        else()
            message(FATAL_ERROR "Platform not supported!")
        endif()

        set_property(
            TARGET ${PLAYDATE_GAME_NAME} PROPERTY ADDITIONAL_CLEAN_FILES
            ${CMAKE_CURRENT_SOURCE_DIR}/${PLAYDATE_GAME_NAME}.pdx
        )

        add_custom_command(
            TARGET ${PLAYDATE_GAME_NAME} POST_BUILD
            COMMAND ${PDC} ${CMAKE_CURRENT_SOURCE_DIR}/Source
            ${CMAKE_CURRENT_SOURCE_DIR}/${PLAYDATE_GAME_NAME}.pdx)
    endif()


    target_compile_options(${PLAYDATE_GAME_NAME} PUBLIC $<$<CONFIG:DEBUG>:-O0>)
    target_compile_options(${PLAYDATE_GAME_NAME} PUBLIC $<$<CONFIG:RELEASE>:-O3>)
endfunction()