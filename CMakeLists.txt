cmake_minimum_required(VERSION 3.15)

project(oop)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Treat warnings as errors by default
if(DEFINED WARNINGS_AS_ERRORS)
    # already defined
elseif(DEFINED ENV{WARNINGS_AS_ERRORS})
    set(WARNINGS_AS_ERRORS $ENV{WARNINGS_AS_ERRORS})
else()
    set(WARNINGS_AS_ERRORS ON)
endif()

# ── STRATEGIE UNIVERSALĂ DETECTARE SFML (v2 sau v3) ───────────────────────────
# Pasul 1: Încercăm mai întâi stilul SFML 2 (clasic, fără prefix)
find_package(SFML COMPONENTS graphics window system QUIET)

if(SFML_FOUND)
    message("S-a detectat SFML standard (Sintaxa clasica)")
    # Pe SFML 2, țintele se numesc direct sfml-graphics, sfml-window, sfml-system
    set(SFML_LIBRARIES sfml-graphics sfml-window sfml-system)
else()
    # Pasul 2: Dacă nu a găsit, înseamnă că MSYS2 a instalat SFML 3 (care cere prefix)
    message("SFML clasic nu a fost gasit. Se incearca sintaxa noua de SFML 3...")
    find_package(SFML COMPONENTS sfml-graphics sfml-window sfml-system REQUIRED)
    set(SFML_LIBRARIES sfml-graphics sfml-window sfml-system)
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

# ── LINKARE LIBRĂRII SFML ───────────────────────────────────────────────────────
# Folosește variabila setată automat mai sus, indiferent de ce versiune s-a găsit
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

# Regula de instalare
install(TARGETS ${PROJECT_NAME} DESTINATION .)
