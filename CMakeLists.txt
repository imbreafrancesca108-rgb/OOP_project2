cmake_minimum_required(VERSION 3.15)

project(oop)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(DEFINED WARNINGS_AS_ERRORS)
    # already defined
elseif(DEFINED ENV{WARNINGS_AS_ERRORS})
    set(WARNINGS_AS_ERRORS $ENV{WARNINGS_AS_ERRORS})
else()
    set(WARNINGS_AS_ERRORS ON)
endif()

# ── DETECTARE MODERNA SFML (v2 sau v3) ───────────────────────────────────────
# Toate versiunile acceptă componentele scrise cu litere mici
find_package(SFML COMPONENTS graphics window system REQUIRED)

# Verificăm dacă s-au generat țintele moderne de SFML 3 (SFML::graphics)
if(TARGET SFML::graphics)
    message("S-a detectat SFML 3 (Sintaxa moderna cu namespace)")
    set(SFML_LIBRARIES SFML::graphics SFML::window SFML::system)
elseif(TARGET sfml-graphics)
    message("S-a detectat SFML 2 (Tinte clasice)")
    set(SFML_LIBRARIES sfml-graphics sfml-window sfml-system)
else()
    # Fallback pentru unele variante de instalare vechi
    set(SFML_LIBRARIES ${SFML_LIBRARIES}) 
endif()

# Main executable
add_executable(${PROJECT_NAME}
    main.cpp
    ColorFilters.cpp
    DetailFilters.cpp
    Exceptions.cpp
    Image.cpp
    ImageEditor.cpp
    Menu.cpp
    Presets.cpp
    SizeFilters.cpp
)

# Linkare biblioteci
target_link_libraries(${PROJECT_NAME} PRIVATE ${SFML_LIBRARIES})

if(GITHUB_ACTIONS)
    message("NOTE: GITHUB_ACTIONS defined")
    target_compile_definitions(${PROJECT_NAME} PRIVATE GITHUB_ACTIONS)
endif()

# Compiler flags
message("Compiler: ${CMAKE_CXX_COMPILER_ID} version ${CMAKE_CXX_COMPILER_VERSION}")
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /permissive- /wd4244 /wd4267 /wd4996 /external:anglebrackets /external:W0)
    if(WARNINGS_AS_ERRORS)
        target_compile_options(${PROJECT_NAME} PRIVATE /WX)
    endif()
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic)
    if(WARNINGS_AS_ERRORS)
        target_compile_options(${PROJECT_NAME} PRIVATE -Werror)
    endif()
endif()

# Sanitizers Configuration
set(is_debug "$<CONFIG:Debug>")
set(is_rel_with_deb "$<CONFIG:RelWithDebInfo>")
set(debug_mode "$<OR:${is_debug},${is_rel_with_deb}>")

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE "$<${debug_mode}:/fsanitize=address>")
elseif(APPLE)
    target_compile_options(${PROJECT_NAME} PRIVATE "$<${debug_mode}:-fsanitize=address,undefined>")
    target_link_options(${PROJECT_NAME} PRIVATE "$<${debug_mode}:-fsanitize=address,undefined>")
elseif(UNIX)
    if("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
        message("GCC detected: Sanitizers disabled to allow Valgrind usage.")
    elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
        if("${CMAKE_CXX_COMPILER_VERSION}" VERSION_GREATER_EQUAL "14.0")
            target_compile_options(${PROJECT_NAME} PRIVATE "$<${debug_mode}:-fsanitize=address,undefined>")
            target_link_options(${PROJECT_NAME} PRIVATE "$<${debug_mode}:-fsanitize=address,undefined>")
        else()
            message("Legacy Clang detected: Sanitizers skipped.")
        endif()
    endif()
endif()

install(TARGETS ${PROJECT_NAME} DESTINATION .)
