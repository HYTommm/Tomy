cmake_minimum_required(VERSION 3.15)
project(Tomy VERSION 1.0.0 LANGUAGES C)

# ═══════════════════════════════════════════════════════════════
#  C23 标准检测
#  项目使用 C23 特性: #elifdef, __VA_OPT__, [[maybe_unused]], auto
# ═══════════════════════════════════════════════════════════════

if(MSVC)
  # MSVC 需要显式 /std:clatest 启用 C23 特性
  add_compile_options(/std:clatest)
else()
  include(CheckCSourceCompiles)

  set(C23_CANDIDATES c23 c2x gnu23 gnu2x)
  set(C23_FLAG "")
  foreach(candidate ${C23_CANDIDATES})
    set(CMAKE_REQUIRED_FLAGS "-std=${candidate}")
    check_c_source_compiles("
      #if __STDC_VERSION__ < 202000L
      #error pre-C2x
      #endif
      int main(void) { [[maybe_unused]] int x = 0; return x; }
    " C23_CAN_${candidate})
    if(C23_CAN_${candidate})
      set(C23_FLAG "-std=${candidate}")
      break()
    endif()
  endforeach()

  if(NOT C23_FLAG)
    message(FATAL_ERROR
      "C23/C2x not supported by ${CMAKE_C_COMPILER_ID} ${CMAKE_C_COMPILER_VERSION}.\n"
      "GCC >= 14 or Clang >= 16 required.")
  endif()

  message(STATUS "C standard: ${C23_FLAG}")
  add_compile_options(${C23_FLAG})
  set(CMAKE_C_STANDARD_REQUIRED ON)
endif()

# ═══════════════════════════════════════════════════════════════
#  编译器选项
# ═══════════════════════════════════════════════════════════════

if(MSVC)
  add_compile_options(/W3)
else()
  add_compile_options(-Wall)
  add_compile_options(-Wno-unused-parameter -Wno-unused-function -Wno-unused-variable)
  # 生效 typedef 匿名结构体嵌入（代码通过 VTABLE{ FROM(base) ... } 实现继承）
  add_compile_options(-fms-extensions)
  # Debug 模式用 -Og 代替默认 -O0，使 C23 inline 生成 COMDAT/weak symbol
  # 避免 inline 函数在多个 TU 中重复定义（对齐 C++ inline 语义）
  add_compile_options($<$<CONFIG:Debug>:-Og>)
endif()

# ═══════════════════════════════════════════════════════════════
#  源文件
# ═══════════════════════════════════════════════════════════════

set(TOMY_SOURCES
  Tomy/src/main.c
  Tomy/src/ustring.c
  Tomy/src/object.c
  Tomy/src/test_print.c
  Tomy/src/test_vector.c
  Tomy/src/test_hashmap.c
  Tomy/src/test_list.c
  Tomy/src/test_dlist.c
  Tomy/src/test_pool_list.c
  Tomy/src/test_pool_dlist.c
  Tomy/src/test_algorithm.c
  Tomy/src/benchmark.c
)

# ═══════════════════════════════════════════════════════════════
#  主目标
# ═══════════════════════════════════════════════════════════════

add_executable(Tomy ${TOMY_SOURCES})
target_include_directories(Tomy PRIVATE Tomy/include)

if(WIN32)
  target_compile_definitions(Tomy PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

# clock_gettime: 较老的 glibc (< 2.17) 需要 -lrt
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  include(CheckLibraryExists)
  check_library_exists(rt clock_gettime time.h TOMY_NEED_RT)
  if(TOMY_NEED_RT)
    target_link_libraries(Tomy PRIVATE rt)
  endif()
endif()

# ═══════════════════════════════════════════════════════════════
#  示例程序 (可选)
# ═══════════════════════════════════════════════════════════════

option(TOMY_BUILD_EXAMPLES "Build example programs" OFF)
if(TOMY_BUILD_EXAMPLES)
  set(EXAMPLE_CORE Tomy/src/ustring.c Tomy/src/object.c)

  set(EXAMPLE_LIST
    oop_example
    vector_example
    list_example
    doublylist_example
    pool_list_example
    hashmap_example
    algorithm_example
  )

  foreach(exe ${EXAMPLE_LIST})
    add_executable(${exe}
      Tomy/examples/${exe}.c
      ${EXAMPLE_CORE}
    )
    target_include_directories(${exe} PRIVATE Tomy/include)
    if(WIN32)
      target_compile_definitions(${exe} PRIVATE _CRT_SECURE_NO_WARNINGS)
    endif()
  endforeach()
endif()

# ═══════════════════════════════════════════════════════════════
#  安装
# ═══════════════════════════════════════════════════════════════

include(GNUInstallDirs)
install(TARGETS Tomy DESTINATION ${CMAKE_INSTALL_BINDIR})
