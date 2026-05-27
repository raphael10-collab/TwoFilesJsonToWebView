#https://docs.wxwidgets.org/latest/overview_cmake.html
# https://github.com/wxWidgets/wxWidgets/blob/3.2/samples/minimal/CMakeLists.txt
# https://cmake.org/cmake/help/book/mastering-cmake/chapter/Finding%20Packages.html
# https://cmake.org/cmake/help/latest/manual/cmake-modules.7.html#manual:cmake-modules(7)
# https://askubuntu.com/questions/1211782/curl-openssl-4-not-found-required-by-curl

#cmake_minimum_required(VERSION 3.15...3.31)


cmake_minimum_required(VERSION 3.15...4.2)


#include("cmake/Hunter/init.cmake")


project(TwoFilesJsonToWebView)


include(CMakePrintHelpers)

include(FetchContent)


find_package(nlohmann_json REQUIRED)

set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_STANDARD 23)
#set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(WEBKIT_DISABLE_COMPOSITING_MODE=1)


#if defined(__linux__)
    set(sep "\\")
#else
    set(sep "\/")

#set(TESSDATA_PREFIX "/home/raphy/Grasp/src/tesseract/tesseract-ocr/tesseractdata/")

#TESSDATA_PREFIX="/home/raphy/Grasp+=;/+"src"+=;/+"tesseract"+=;/+"tesseract-ocr"+=;/+"5"+=;/+tesserdata"

#set(TESSDATA_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}+${sep}+"src"+${sep}+"tesseract"+${sep}+"tesseract-ocr"+${sep}+"5"+${sep}+tesserdata)

set(CD "${CMAKE_CURRENT_SOURCE_DIR}")
set(BUILDDIR "builddir")
set(SRC "src")

#find_package(PortAudio REQUIRED NO_MODULE)

find_package(nlohmann_json REQUIRED)


# https://github.com/PBfordev/wxecharts/blob/main/CMakeLists.txt
find_package(wxWidgets REQUIRED COMPONENTS base core webview aui stc net xml html richtext)

if(wxWidgets_USE_FILE)
  include(${wxWidgets_USE_FILE})
endif()

if(WIN32)
  # Don't know how to tell in CMake if the target CPU is x64 or Arm64 (when crosscompiling)
  if(NOT("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64" OR "${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "AMD64"))
    message(FATAL_ERROR "The only supported CPU architecture on Windows is x64.")
  endif()

  set(WEBVIEW2_FOLDER "" CACHE PATH "Path to webview2 package")
  if("${WEBVIEW2_FOLDER}" STREQUAL "")
    message(FATAL_ERROR "Variable \"WEBVIEW2_FOLDER\" must be set to the webview2 folder, e.g. \"c:\\libs\\wxwidgets\\3rdparty\\webiew2\".")
  endif()
  set(WEBVIEW2_LOADER_DLL "${WEBVIEW2_FOLDER}/build/native/x64/WebView2Loader.dll")
  if(NOT EXISTS ${WEBVIEW2_LOADER_DLL})
    message(FATAL_ERROR "Variable \"WEBVIEW2_FOLDER\" contains incorrect path (\"${WEBVIEW2_FOLDER}\").")
  endif()
elseif(LINUX)
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(GLIB REQUIRED gio-2.0)
  pkg_check_modules(WEBKIT2 webkit2gtk-4.0)
  if(NOT WEBKIT2_FOUND)
     pkg_check_modules(WEBKIT2 REQUIRED webkit2gtk-4.1)
  endif()
endif()

find_package(OpenSSL)

find_package(Threads)

find_package(PkgConfig REQUIRED)

# https://github.com/julp/FindICU.cmake
find_package(ICU 74.0 COMPONENTS uc i18n REQUIRED)
include_directories(${ICU_INCLUDE_DIRS})

# https://cmake.org/cmake/help/latest/variable/CMAKE_SYSTEM_NAME.html#system-names-known-to-cmake


add_executable(${PROJECT_NAME}
    src/main.cpp
)

# https://github.com/PBfordev/wxecharts/blob/main/CMakeLists.txt

if(MINGW) # work around the breaking change in wxWidgets 3.3
  target_link_libraries(${PROJECT_NAME} PRIVATE gdiplus msimg32)
endif()

if(WIN32)
  target_compile_definitions(${PROJECT_NAME} PRIVATE wxUSE_RC_MANIFEST wxUSE_DPI_AWARE_MANIFEST=2)
  target_include_directories(${PROJECT_NAME} PRIVATE "${WEBVIEW2_FOLDER}/build/native/include")
  set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE YES)

  if(MSVC)
    target_compile_definitions(${PROJECT_NAME} PRIVATE _CRT_SECURE_NO_DEPRECATE _CRT_NON_CONFORMING_SWPRINTFS _SCL_SECURE_NO_WARNINGS)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /MP)
    target_link_options(${PROJECT_NAME} PRIVATE /MANIFEST:NO)
  else() # GCC or clang
    target_compile_options(${PROJECT_NAME} PRIVATE -Wno-deprecated-declarations)
  endif()
elseif(LINUX)
  target_include_directories(${PROJECT_NAME} PRIVATE ${GIO_INCLUDE_DIRS})
  target_link_directories(${PROJECT_NAME} PRIVATE ${GIO_LIBRARY_DIRS})
  target_link_libraries(${PROJECT_NAME} PRIVATE ${GIO_LIBRARIES})

  target_include_directories(${PROJECT_NAME} PRIVATE ${WEBKIT2_INCLUDE_DIRS})
  target_link_directories(${PROJECT_NAME} PRIVATE ${WEBKIT2_LIBRARY_DIRS})
  target_link_libraries(${PROJECT_NAME} PRIVATE ${WEBKIT2_LIBRARIES})
endif()

# copy WebView2Loader.dll to the folder with the application executable
if(WIN32)
  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${WEBVIEW2_LOADER_DLL} $<TARGET_FILE_DIR:${PROJECT_NAME}>)
endif()

#add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/thirdparty/SQLiteCpp)

target_link_libraries (${PROJECT_NAME} PUBLIC
    wxWidgets::wxWidgets

    nlohmann_json PRIVATE nlohmann_json::nlohmann_json

    Threads::Threads
    OpenSSL::SSL

)

cmake_print_variables(CMAKE_BINARY_DIR)
cmake_print_variables(CMAKE_SOURCE_DIR)

add_custom_command(
  TARGET ${PROJECT_NAME} POST_BUILD

  COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/simul-assets"
  COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/src/simul-assets" "${CMAKE_BINARY_DIR}/simul-assets"

)
