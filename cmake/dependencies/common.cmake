# =================== SSE2NEON ===================
set(SSE2NEON_DIR ${CMAKE_BINARY_DIR}/_deps/sse2neon)
file(
  DOWNLOAD
  "https://raw.githubusercontent.com/DLTcollab/sse2neon/refs/heads/master/sse2neon.h"
  "${SSE2NEON_DIR}/sse2neon.h")

include_directories(${SSE2NEON_DIR})

# ================== SEMVER ===================
set(SEMVER_DIR ${CMAKE_BINARY_DIR}/_deps/semver)
file(
  DOWNLOAD
  "https://raw.githubusercontent.com/Neargye/semver/refs/tags/v1.0.0-rc/include/semver.hpp"
  "${SEMVER_DIR}/semver.hpp")

include_directories(${SEMVER_DIR})

# =================== DRLibs ===================
FetchContent_Declare(
  dr_libs
  GIT_REPOSITORY https://github.com/mackron/dr_libs.git
  GIT_TAG da35f9d6c7374a95353fd1df1d394d44ab66cf01)
FetchContent_MakeAvailable(dr_libs)

# =================== STB ===================
include_directories(${STB_DIR})

# =================== tomlplusplus ===================
include(FetchContent)
FetchContent_Declare(
  tomlplusplus
  GIT_REPOSITORY https://github.com/marzer/tomlplusplus.git
  GIT_TAG v3.4.0)
FetchContent_MakeAvailable(tomlplusplus)
target_link_libraries(${PROJECT_NAME} PRIVATE tomlplusplus::tomlplusplus)


