include(FetchContent)

# --- SDL2 ---
message(STATUS "Fetching SDL2...")
FetchContent_Declare(
  SDL2
  URL "https://github.com/libsdl-org/SDL/archive/refs/tags/release-2.28.5.tar.gz"
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

# 设置 SDL2 的编译选项，禁用不需要的部分以加快速度
set(SDL2_TESTS OFF CACHE BOOL "" FORCE)
set(SDL2_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SDL_INSTALL OFF CACHE BOOL "" FORCE) # 避免安装到系统目录
set(SDL_STATIC ON CACHE BOOL "" FORCE)   # 强制生成静态库以便链接

# --- SDL2_ttf ---
message(STATUS "Fetching SDL2_ttf...")
FetchContent_Declare(
  SDL2_ttf
  URL "https://github.com/libsdl-org/SDL_ttf/archive/refs/tags/release-2.22.0.tar.gz"
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

# 设置 SDL2_ttf 选项
set(SDL2_TTF_VENDORED ON CACHE BOOL "" FORCE) # 让 SDL2_ttf 自动处理它依赖的 FreeType 库
set(SDL2_TTF_SAMPLES OFF CACHE BOOL "" FORCE)

# 一次性使所有内容可用
FetchContent_MakeAvailable(SDL2 SDL2_ttf)

# 如果 SDL2 没有定义别名，手动定义一个，方便后面 target_link_libraries 使用
if(NOT TARGET SDL2::SDL2)
    add_library(SDL2::SDL2 ALIAS SDL2-static)
endif()