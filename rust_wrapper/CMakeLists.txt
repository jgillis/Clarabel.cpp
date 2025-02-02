# Build Clarabel.rs (Rust library)
project(clarabel_c VERSION ${CLARABEL_PROJECT_VERSION})

# Define interface targets for the imported Rust library
add_library(libclarabel_c INTERFACE)

# Set the build flags and output directory

# Debug/Release flags
if(CMAKE_BUILD_TYPE MATCHES Release)
   set(clarabel_c_build_flags "--release")
   set(clarabel_c_output_directory "${CMAKE_SOURCE_DIR}/rust_wrapper/target/release")
else()
    set(clarabel_c_build_flags "")
    set(clarabel_c_output_directory "${CMAKE_SOURCE_DIR}/rust_wrapper/target/debug")
endif()

set(CLARABEL_C_OUTPUT_DIR ${clarabel_c_output_directory} PARENT_SCOPE)


# -------------------------------------
# Cargo features configuration 
#--------------------------------------

# A list of features to pass to rustc 
set(CLARABEL_BUILD_FEATURES "")

# SDP feature flag
if(NOT CLARABEL_FEATURE_SDP STREQUAL "none")

    # Set the Rust feature flag
    set(CLARABEL_BUILD_FEATURES "${CLARABEL_BUILD_FEATURES},${CLARABEL_FEATURE_SDP}")
    
    # Define the FEATURE_SDP flag for all targets that link against clarabel_c
    target_compile_definitions(libclarabel_c INTERFACE FEATURE_SDP)
endif()

# FAER_SPARSE feature flag
if(CLARABEL_FEATURE_FAER_SPARSE)

    # Set the Rust feature flag
    set(CLARABEL_BUILD_FEATURES "${CLARABEL_BUILD_FEATURES},faer-sparse")
    
    # Define the FEATURE_FAER_SPARSE flag for all targets that link against clarabel_c
    target_compile_definitions(libclarabel_c INTERFACE FEATURE_FAER_SPARSE)
endif()

# SERDE feature flag
if(CLARABEL_FEATURE_SERDE)

    # Set the Rust feature flag
    set(CLARABEL_BUILD_FEATURES "${CLARABEL_BUILD_FEATURES},serde")
    
    # Define the FEATURE_SERDE flag for all targets that link against clarabel_c
    target_compile_definitions(libclarabel_c INTERFACE FEATURE_SERDE)
endif()



set(clarabel_c_build_flags "${clarabel_c_build_flags};--features=${CLARABEL_BUILD_FEATURES}")
message("-- Rust feature list: " ${CLARABEL_BUILD_FEATURES})
message("-- Cargo options: " "${clarabel_c_build_flags}")


# -------------------------------------
# -------------------------------------


# Add the cargo project as a custom target
add_custom_target(
    libclarabel_c_external
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/rust_wrapper
    # Commands for building the Rust library
    COMMAND cargo build ${clarabel_c_build_flags}
    COMMAND cargo install cbindgen --version 0.24.5
    # Generate the C header
    COMMAND cbindgen --config cbindgen.toml --crate clarabel_c --output ./headers/clarabel.h --lang c
    # Generate the C++ header
    COMMAND cbindgen --config cbindgen.toml --crate clarabel_c --output ./headers/clarabel.hpp
)

if(BUILD_SHARED_LIBS)
  # Get the path to the Rust library for linking
  if(APPLE)
      set(LIBCLARABEL_C_PATH "${clarabel_c_output_directory}/libclarabel_c.dylib")
  elseif(UNIX)
      set(LIBCLARABEL_C_PATH "${clarabel_c_output_directory}/libclarabel_c.so")
  elseif(WIN32)
      set(LIBCLARABEL_C_PATH "${clarabel_c_output_directory}/clarabel_c.dll.lib")
  endif()
else()
  # Get the path to the Rust library for linking
  if(APPLE)
      set(LIBCLARABEL_C_PATH "${clarabel_c_output_directory}/libclarabel_c.a")
  elseif(UNIX)
      set(LIBCLARABEL_C_PATH "${clarabel_c_output_directory}/libclarabel_c.a")
  elseif(WIN32)
      set(LIBCLARABEL_C_PATH "${clarabel_c_output_directory}/clarabel_c.lib")
  endif()
endif()

# Wrap the Rust library in a CMake library target

# Static library

target_link_libraries(libclarabel_c INTERFACE ${LIBCLARABEL_C_PATH})
target_include_directories(libclarabel_c INTERFACE ${CMAKE_SOURCE_DIR}/include)
add_dependencies(libclarabel_c libclarabel_c_external)

# Install manually, since CMake doesn't track custom targets
install(FILES ${LIBCLARABEL_C_PATH}
    DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
