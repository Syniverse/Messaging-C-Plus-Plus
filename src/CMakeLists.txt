set(ACTUAL_SOURCES
    ScgImpl.cpp
    SessionImpl.cpp
    AuthInfo.cpp
    )

set(HEADERS
    ${SCGAPI_ROOT_DIR}/include/scgapi/Scg.h
    )

if (WIN32)
    set(SOURCES ${ACTUAL_SOURCES} ${HEADERS} ${RESFILES})
else()
    set(SOURCES ${ACTUAL_SOURCES})
endif()

message(STATUS "DEFAULT_LIBRARIES: ${DEFAULT_LIBRARIES}")

add_library(scgapi ${SOURCES})
set_target_properties(scgapi PROPERTIES DEBUG_OUTPUT_NAME scgapiD)
target_link_libraries(scgapi restc-cpp)
