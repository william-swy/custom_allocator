# Adds warnings to the target based on ${warning_lang} and selected compiler Valid options for ${warning_lang}: - "CC"
# for C language warnings - "CXX" for C++ language warnings

function(set_project_warnings warning_lang project_name)
  option(WARNINGS_AS_ERRORS "Treat compiler warnings as errors" ON)

  set(CLANG_C_WARNINGS
      -Wall
      -Wextra # reasonable and standard
      -Wshadow # warn the user if a variable declaration shadows one from a parent context
      -Wcast-align # warn for potential performance problem casts
      -Wunused # warn on anything being unused
      -Wpedantic # warn if non-standard C++ is used
      -Wconversion # warn on type conversions that may lose data
      -Wsign-conversion # warn on sign conversions
      -Wnull-dereference # warn if a null dereference is detected
      -Wdouble-promotion # warn if float is implicit promoted to double
      -Wformat=2 # warn on security issues around functions that format output (ie printf)
      -Wimplicit-fallthrough # warn on statements that fallthrough without an explicit annotation
  )

  if(WARNINGS_AS_ERRORS)
    set(CLANG_C_WARNINGS ${CLANG_WARNINGS} -Werror)
  endif()

  set(CLANG_CXX_WARNINGS
      ${CLANG_C_WARNINGS}
      -Wnon-virtual-dtor # warn the user if a class with virtual functions has a non-virtual destructor. This helps
      # catch hard to track down memory errors
      -Wold-style-cast # warn for c-style casts
      -Woverloaded-virtual # warn if you overload (not override) a virtual function
  )

  set(GCC_C_WARNINGS
      ${CLANG_C_WARNINGS}
      -Wmisleading-indentation # warn if indentation implies blocks where blocks do not exist
      -Wduplicated-cond # warn if if / else chain has duplicated conditions
      -Wduplicated-branches # warn if if / else branches have duplicated code
      -Wlogical-op # warn about logical operations being used where bitwise were probably wanted
  )

  set(GCC_CXX_WARNINGS ${GCC_C_WARNINGS} -Wuseless-cast # warn if you perform a cast to the same type
  )

  if(${warning_lang} STREQUAL "CC")
    if(CMAKE_C_COMPILER_ID MATCHES ".*Clang")
      set(PROJECT_WARNINGS ${CLANG_C_WARNINGS})
    elseif(CMAKE_C_COMPILER_ID STREQUAL "GNU")
      set(PROJECT_WARNINGS ${GCC_C_WARNINGS})
    else()
      message(AUTHOR_WARNING "No compiler warnings set for '${CMAKE_C_COMPILER_ID}' compiler.")
    endif()
  elseif(${warning_lang} STREQUAL "CXX")
    if(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
      set(PROJECT_WARNINGS ${CLANG_CXX_WARNINGS})
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
      set(PROJECT_WARNINGS ${GCC_CXX_WARNINGS})
    else()
      message(AUTHOR_WARNING "No compiler warnings set for '${CMAKE_CXX_COMPILER_ID}' compiler.")
    endif()
  else()
    message(AUTHOR_WARNING "No compiler warnings set... Invalid lang: ${warning_lang}")
  endif()

  target_compile_options(${project_name} INTERFACE ${PROJECT_WARNINGS})

endfunction()
