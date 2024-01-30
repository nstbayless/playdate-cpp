function(add_playdate_application PLAYDATE_GAME_NAME)
    message(STATUS "Adding playdate application ${PLAYDATE_GAME_NAME}")

    if (PDCPP_STAGE_IN_BINARY_DIR)
      set(PDCPP_STAGING_DIR ${CMAKE_CURRENT_BINARY_DIR})
    else ()
      set(PDCPP_STAGING_DIR ${CMAKE_CURRENT_SOURCE_DIR})
    endif ()

    if (TOOLCHAIN STREQUAL "armgcc")
        add_executable(${PLAYDATE_GAME_NAME})

        set_property(TARGET ${PLAYDATE_GAME_NAME} PROPERTY OUTPUT_NAME "${PLAYDATE_GAME_NAME}.elf")

        add_custom_command(
            TARGET ${PLAYDATE_GAME_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_CURRENT_BINARY_DIR}/${BUILD_SUB_DIR}${PLAYDATE_GAME_NAME}.elf
            ${PDCPP_STAGING_DIR}/Source/pdex.elf
        )

        add_custom_command(
            TARGET ${PLAYDATE_GAME_NAME} POST_BUILD
            COMMAND ${CMAKE_STRIP} --strip-unneeded -R .comment -g
            ${PLAYDATE_GAME_NAME}.elf
            -o ${PDCPP_STAGING_DIR}/Source/pdex.elf
        )

        add_custom_command(
            TARGET ${PLAYDATE_GAME_NAME} POST_BUILD
            COMMAND ${PDC} Source ${PLAYDATE_GAME_NAME}.pdx
            WORKING_DIRECTORY ${PDCPP_STAGING_DIR}
        )

        set_property(
            TARGET ${PLAYDATE_GAME_NAME} PROPERTY ADDITIONAL_CLEAN_FILES
            ${PDCPP_STAGING_DIR}/${PLAYDATE_GAME_NAME}.pdx
        )

    else ()
        add_library(${PLAYDATE_GAME_NAME} SHARED)

        if (MSVC)
            if(${CMAKE_GENERATOR} MATCHES "Visual Studio*" )
                set(BUILD_SUB_DIR $<CONFIG>/)
                file(TO_NATIVE_PATH ${SDK}/bin/PlaydateSimulator.exe SIMPATH)
                file(TO_NATIVE_PATH ${PDCPP_STAGING_DIR}/${PLAYDATE_GAME_NAME}.pdx SIMGAMEPATH)
                set_property(TARGET ${PLAYDATE_GAME_NAME} PROPERTY VS_DEBUGGER_COMMAND ${SIMPATH})
                set_property(TARGET ${PLAYDATE_GAME_NAME} PROPERTY VS_DEBUGGER_COMMAND_ARGUMENTS "\"${SIMGAMEPATH}\"")
            endif()

            add_custom_command(
                    TARGET ${PLAYDATE_GAME_NAME} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy
                    ${CMAKE_CURRENT_BINARY_DIR}/${BUILD_SUB_DIR}${PLAYDATE_GAME_NAME}.dll
                    ${PDCPP_STAGING_DIR}/Source/pdex.dll)
        elseif (MINGW)
            add_custom_command(
                    TARGET ${PLAYDATE_GAME_NAME} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy
                    ${CMAKE_CURRENT_BINARY_DIR}/lib${PLAYDATE_GAME_NAME}.dll
                    ${PDCPP_STAGING_DIR}/Source/pdex.dll)
        elseif(APPLE)
            if(${CMAKE_GENERATOR} MATCHES "Xcode" )
                set(BUILD_SUB_DIR $<CONFIG>/)
                set_property(TARGET ${PLAYDATE_GAME_NAME} PROPERTY XCODE_SCHEME_ARGUMENTS \"${PDCPP_STAGING_DIR}/${PLAYDATE_GAME_NAME}.pdx\")
                set_property(TARGET ${PLAYDATE_GAME_NAME} PROPERTY XCODE_SCHEME_EXECUTABLE ${SDK}/bin/Playdate\ Simulator.app)
            endif()

            add_custom_command(
                TARGET ${PLAYDATE_GAME_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_BINARY_DIR}/${BUILD_SUB_DIR}lib${PLAYDATE_GAME_NAME}.dylib
                ${PDCPP_STAGING_DIR}/Source/pdex.dylib)

        elseif(UNIX)
            add_custom_command(
                TARGET ${PLAYDATE_GAME_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_BINARY_DIR}/lib${PLAYDATE_GAME_NAME}.so
                ${PDCPP_STAGING_DIR}/Source/pdex.so)
        else()
            message(FATAL_ERROR "Platform not supported!")
        endif()

        set_property(
            TARGET ${PLAYDATE_GAME_NAME} PROPERTY ADDITIONAL_CLEAN_FILES
            ${PDCPP_STAGING_DIR}/${PLAYDATE_GAME_NAME}.pdx
        )

        add_custom_command(
            TARGET ${PLAYDATE_GAME_NAME} POST_BUILD
            COMMAND ${PDC} ${PDCPP_STAGING_DIR}/Source
            ${PDCPP_STAGING_DIR}/${PLAYDATE_GAME_NAME}.pdx)
    endif()

    target_link_libraries(${PLAYDATE_GAME_NAME} PUBLIC pdcpp_core)
endfunction()
