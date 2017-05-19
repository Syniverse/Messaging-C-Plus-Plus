
cmake_minimum_required(VERSION 3.0)
project (scgapi)

if (NOT DEFINED SCGAPI_ROOT_DIR)
    set(SCGAPI_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
endif()

if (NOT DEFINED WITH_APIDOC)
    option(WITH_APIDOC "Generate Doxygen documentation" OFF)
endif()

if (NOT DEFINED SCGAPI_WITH_EXAMPLES)
    option(SCGAPI_WITH_EXAMPLES "Compile examples" ON)
endif()

if (NOT DEFINED RESTC_CPP_ROOT)
    set(RESTC_CPP_ROOT "${SCGAPI_ROOT_DIR}/externals/restc-cpp")
endif()

message(STATUS "Using ${CMAKE_CXX_COMPILER}")

if (NOT DEFINED RESTC_CPP_WITH_FUNCTIONALT_TESTS)
    option(RESTC_CPP_WITH_FUNCTIONALT_TESTS "Enable restc-cpp Functional Testing" OFF)
endif()

if (WIN32)
	add_definitions(-D_WIN32_WINNT=0x0600)
            set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO")
endif()

add_subdirectory(externals/restc-cpp)

include_directories(
    ${ZLIB_INCLUDE_DIR} ${ZLIB_INCLUDE_DIR}/build
    ${OPENSSL_INCLUDE_DIR}
    ${RESTC_CPP_ROOT}/include
    ${SCGAPI_ROOT_DIR}/include
    ${RESTC_CPP_ROOT}/externals/rapidjson/include/
)

# Asio trigger deprecation warnings
add_definitions(-DBOOST_COROUTINE_NO_DEPRECATION_WARNING=1)

if (WIN32)
	set(Boost_USE_STATIC_LIBS ON)
	find_package(Boost REQUIRED COMPONENTS program_options)
	include_directories(${Boost_INCLUDE_DIRS})
	if (WIN32)
		link_directories(${Boost_LIBRARY_DIRS})
	endif()
endif()

add_subdirectory(src)

if (SCGAPI_WITH_EXAMPLES)
    add_subdirectory(examples)
endif()
