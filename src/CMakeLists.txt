cmake_minimum_required(VERSION 3.8)
project("shadowman" VERSION 0.5.0 LANGUAGES CXX)


include_directories(${CMAKE_CURRENT_SOURCE_DIR})


#list of library names (also the directory name)
set(library_names avatar state subsystem util)


#list of directories with executables
set(executable_directory_names shadowman)


#list of all sub-directories
set(sub_directory_names ${library_names} ${executable_directory_names})


#compiler specific tasks
set(custom_compile_flags "")
set(custom_link_flags "")

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")

    file (READ ../build-flags/msvc.txt custom_compile_flags)

elseif (CMAKE_CXX_COMPILER_ID STREQUAL GNU)

    set(custom_link_flags ${custom_link_flags} -pthread)
    file (READ ../build-flags/gnu.txt custom_compile_flags)

elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")

    file (READ ../build-flags/clang.txt custom_compile_flags)

    #Clang's Address Sanitizer stuff here
    option(ADDRESS_SANITIZER "Clang's Address Sanitizer" OFF)

    if(ADDRESS_SANITIZER)
	    message("Clang's Address Sanitizer ENABLED")
	    set(custom_compile_flags ${custom_compile_flags} -fsanitize=address -fno-omit-frame-pointer)
	    set(custom_link_flags ${custom_link_flags} -fsanitize=address -fno-omit-frame-pointer)
    endif()

else()

    message(FATAL_ERROR " Unknown Compiler: ${CMAKE_CXX_COMPILER_ID}")

endif()


#print/echo custom compile/link flags that were set above
message( ${CMAKE_CXX_COMPILER_ID} " Compile Flags: " ${custom_compile_flags})
message( ${CMAKE_CXX_COMPILER_ID} " Link Flags: " ${custom_link_flags})


#on windows and linux we build sfml from source, otherwise just use find_package()
if(WIN32)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
	option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

	include(FetchContent)
	FetchContent_Declare(SFML
		GIT_REPOSITORY https://github.com/SFML/SFML.git
		GIT_TAG 3.0.0
		GIT_SHALLOW ON)
	FetchContent_MakeAvailable(SFML)
	
elseif (CMAKE_CXX_COMPILER_ID STREQUAL GNU)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
	option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

	include(FetchContent)
	FetchContent_Declare(SFML
		GIT_REPOSITORY https://github.com/SFML/SFML.git
		GIT_TAG 3.0.0
		GIT_SHALLOW ON)
	FetchContent_MakeAvailable(SFML)
	
else()

find_package(SFML 3.0.0 COMPONENTS Window Graphics Audio REQUIRED)

endif()


#sets compiler flags, linker flags, and dependencies common to all targets
function(setup_target name)
    
    #why the fuck doesn't target_compile_options line work?! Using set_target_properties instead...
    #target_compile_options(${name} PUBLIC ${custom_compile_flags})
    set_target_properties(${name} PROPERTIES COMPILE_FLAGS ${custom_compile_flags})

    target_link_libraries(${name} PUBLIC ${custom_link_flags})
    
    target_link_libraries(${name} PUBLIC SFML::Window SFML::Graphics SFML::Audio)
    
    set(library_names_excluding_current ${library_names})
    list(REMOVE_ITEM library_names_excluding_current ${name})
    target_link_libraries(${name} PUBLIC ${library_names_excluding_current})
        
endfunction()


#add libraries
foreach(library_name ${library_names})
    add_subdirectory(${library_name})
endforeach()


#add executables
foreach(executable_directory_name ${executable_directory_names})
    add_subdirectory(${executable_directory_name})
endforeach()