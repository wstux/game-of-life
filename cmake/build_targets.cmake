################################################################################
# Keywords
################################################################################

set(_COMMON_TARGET_KW   HEADERS     # headers list
                        SOURCES     # sources list
                        COMMENT     # message before build target
                        LIBRARIES
)

set(_CUSTOM_TARGET_KW   COMMAND
                        DEPENDS
)

set(_EXE_TARGET_KW      ${_COMMON_TARGET_KW}
)

set(_LIB_TARGET_KW      ${_COMMON_TARGET_KW}
                        MODULE      # Dinamic module library type
                        SHARED      # Dinamic library type
                        STATIC      # Static library type
                        INTERFACE
                        INCLUDE_DIR
)

set(_LIST_VALUES_KW     HEADERS
                        SOURCES
                        LIBRARIES
                        COMMAND
                        DEPENDS
)

set(_FLAG_KW            MODULE
                        SHARED
                        STATIC
                        INTERFACE
)

function(_is_kw CHECK_STR KW_LIST RESULT)
    set(${RESULT} 0 PARENT_SCOPE)
    list(FIND ${KW_LIST} "${CHECK_STR}" IS_FIND)
    if(NOT IS_FIND EQUAL -1)
        set(${RESULT} 1 PARENT_SCOPE)
    endif()
endfunction()

function(_parse_target_args TARGET_NAME KW_LIST)
    set(key "")
    set(to_parent_scope FALSE)
    foreach(arg IN LISTS ARGN)
        # Check is 'arg' a keyword.
        _is_kw(${arg} "${KW_LIST}" is_keyword)
        # Check is 'arg' a keyword and applies to flags keyword.
        _is_kw(${arg} _FLAG_KW is_flag)

        # If 'arg' is keyword - save 'arg' to 'key' variable and save key-flag to parent scope.
        if(is_keyword)
            if (to_parent_scope)
                set(${TARGET_NAME}_${key} "${${TARGET_NAME}_${key}}" PARENT_SCOPE)
                set(to_parent_scope FALSE)
            endif()

            set(key "${arg}")

            if(is_flag)
                set(${TARGET_NAME}_${key} TRUE PARENT_SCOPE)
            endif()

            continue()
        endif()

        # If 'key' variable is defined - add data to key args and add to parent scope.
        if(key)
            if(NOT DEFINED ${TARGET_NAME}_${key})
                set(${TARGET_NAME}_${key} "${arg}")
                set(to_parent_scope TRUE)
            else()
                _is_kw(${key} _LIST_VALUES_KW is_valid)
                if(NOT is_valid)
                    message(ERROR " Invalid value for key '${key}'")
                endif()
                
                set(${TARGET_NAME}_${key} "${${TARGET_NAME}_${key}};${arg}")
                set(to_parent_scope TRUE)
            endif()
        endif()
    endforeach()

    if (to_parent_scope)
        set(${TARGET_NAME}_${key} "${${TARGET_NAME}_${key}}" PARENT_SCOPE)
        set(to_parent_scope FALSE)
    endif()
endfunction()

macro(CustomTarget TARGET_NAME)
    _parse_target_args(${TARGET_NAME} _CUSTOM_TARGET_KW ${ARGN})

    foreach(key IN LISTS _CUSTOM_TARGET_KW)
        foreach(dep IN LISTS ${TARGET_NAME}_${key})
            list(APPEND ${TARGET_NAME}_BUILD_ARGS ${key} ${dep})
        endforeach()
    endforeach()

    add_custom_target(${TARGET_NAME} ${${TARGET_NAME}_BUILD_ARGS})
endmacro()

macro(LibTarget TARGET_NAME)
    _parse_target_args(${TARGET_NAME} _LIB_TARGET_KW ${ARGN})

    if(${TARGET_NAME}_INTERFACE)
        add_library(${TARGET_NAME} INTERFACE)
        target_include_directories(
            ${TARGET_NAME} INTERFACE
                "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/${${TARGET_NAME}_INCLUDE_DIR}>"
                "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
        )
    elseif(${TARGET_NAME}_SHARED OR ${TARGET_NAME}_STATIC)
        set(LIB_TYPE SHARED)
        if (${TARGET_NAME}_STATIC)
            set(LIB_TYPE STATIC)
        endif()

        add_library(${TARGET_NAME} ${LIB_TYPE}
                                   ${${TARGET_NAME}_HEADERS}
                                   ${${TARGET_NAME}_SOURCES}
        )

        target_include_directories(${TARGET_NAME} PRIVATE ${${TARGET_NAME}_INCLUDE_DIR})

        set_target_properties(${TARGET_NAME}
                              PROPERTIES
                                  INCLUDE_DIRECTORIES ${PROJECT_SOURCE_DIR}/${${TARGET_NAME}_INCLUDE_DIR}
        )
    else()
        message(ERROR "[ERROR] Unsupported library type")
    endif()

    install(TARGETS ${TARGET_NAME} LIBRARY DESTINATION libs)
endmacro()

macro(ExeTarget TARGET_NAME)
    _parse_target_args(${TARGET_NAME} _EXE_TARGET_KW ${ARGN})

    add_executable(${TARGET_NAME} ${${TARGET_NAME}_HEADERS}
                                  ${${TARGET_NAME}_SOURCES}
    )
    foreach(lib IN LISTS ${TARGET_NAME}_LIBRARIES)
        target_link_libraries(${TARGET_NAME} ${lib})

        get_target_property(LIB_INCLUDE_DIR ${lib} INCLUDE_DIRECTORIES)
        target_include_directories(${TARGET_NAME} PRIVATE ${LIB_INCLUDE_DIR})
    endforeach()

    install(TARGETS ${TARGET_NAME} RUNTIME DESTINATION bin)
endmacro()

macro(TestTarget TARGET_NAME)
    _parse_target_args(${TARGET_NAME} _EXE_TARGET_KW ${ARGN})

    add_executable(${TARGET_NAME} ${${TARGET_NAME}_HEADERS}
                                  ${${TARGET_NAME}_SOURCES}
    )
    add_test(${TARGET_NAME} ${TARGET_NAME})
    foreach(lib IN LISTS ${TARGET_NAME}_LIBRARIES)
        target_link_libraries(${TARGET_NAME} ${lib})

        get_target_property(target_type ${lib} TYPE)
        if(target_type STREQUAL "INTERFACE_LIBRARY")
            continue()
        endif()

        get_target_property(LIB_INCLUDE_DIR ${lib} INCLUDE_DIRECTORIES)
        target_include_directories(${TARGET_NAME} PRIVATE ${LIB_INCLUDE_DIR})
    endforeach()
    enable_testing()

    install(TARGETS ${TARGET_NAME} RUNTIME DESTINATION tests)
endmacro()

