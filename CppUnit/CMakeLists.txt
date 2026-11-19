set(LIBNAME "CppUnit")
set(POCO_LIBNAME "Poco${LIBNAME}")

# Sources
file(GLOB SRCS_G "src/*.cpp")
POCO_SOURCES_AUTO( SRCS ${SRCS_G})

# Headers
file(GLOB_RECURSE HDRS_G "include/*.h" )
POCO_HEADERS_AUTO( SRCS ${HDRS_G})

add_library( "${LIBNAME}" ${LIB_MODE} ${SRCS} )
add_library( "${POCO_LIBNAME}" ALIAS "${LIBNAME}")
set_target_properties( "${LIBNAME}"
    PROPERTIES
    VERSION "1" SOVERSION "1"
    OUTPUT_NAME ${POCO_LIBNAME}
    DEFINE_SYMBOL CppUnit_EXPORTS
    )

target_include_directories( "${LIBNAME}"
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        # TestRunner.h needs Poco/Exception.h visible (consumed by the
        # CppUnitMain / CppUnitPocoExceptionText macros at the consumer's
        # call site). Pull Foundation's include dirs in without linking it:
        # libPocoCppUnit itself never references Poco::Exception symbols.
        $<TARGET_PROPERTY:PocoFoundation,INTERFACE_INCLUDE_DIRECTORIES>
    PRIVATE
    	${CMAKE_CURRENT_SOURCE_DIR}/src
    )

# The upstream CppUnit uses C++17 ([[nodiscard]], std::is_arithmetic_v).
target_compile_features("${LIBNAME}" PUBLIC cxx_std_17)
target_compile_definitions("${LIBNAME}" PUBLIC ${LIB_MODE_DEFINITIONS})

POCO_INSTALL("${LIBNAME}")


