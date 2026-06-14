cmake_minimum_required(VERSION 3.12)

project(OpenScripter LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/project/*.cpp"
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

add_subdirectory(modules/OpenCore)

target_include_directories(${PROJECT_NAME}
PRIVATE
    "${CMAKE_SOURCE_DIR}/include"
)

target_link_libraries(${PROJECT_NAME}
PRIVATE
    OpenCore
)

if(WIN32)

    target_sources(${PROJECT_NAME}
    PRIVATE
        "${CMAKE_SOURCE_DIR}/resources/resource.rc"
    )

    add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD

        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/assets"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>"

        COMMENT "Copying assets to exe directory"
    )

    set_target_properties(${PROJECT_NAME}
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY
            "${CMAKE_SOURCE_DIR}/build"
    )

endif()

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION .)
install(DIRECTORY "${CMAKE_SOURCE_DIR}/assets/" DESTINATION .)

set(EXPORT_DIR "${CMAKE_SOURCE_DIR}/build/export")

add_custom_target(export
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "Exporting game to ${EXPORT_DIR}"
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} --install "${CMAKE_BINARY_DIR}"
        --prefix "${EXPORT_DIR}"
    COMMENT "Exporting clean game directory"
)

add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD

    COMMAND
        ${CMAKE_COMMAND} -E echo
        "Build completed: $<TARGET_FILE:${PROJECT_NAME}>"
)