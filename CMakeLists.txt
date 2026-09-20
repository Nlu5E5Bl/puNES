cmake_minimum_required(VERSION 3.15)

project(puNES)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

string(TIMESTAMP COMPILED_TIMESTAMP "%m%d%Y%H%M%S")

# Options
option(ENABLE_GIT_INFO "Include Git info" OFF)
option(ENABLE_RELEASE "Build release version" ON)
option(DISABLE_PORTABLE_MODE "Disable compiling portable mode management" OFF)
option(ENABLE_TESTS "Build the unit test harness in tests/" OFF)

if (ENABLE_GIT_INFO)
	find_package(Git)

	if(Git_FOUND)
		execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
			WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
			OUTPUT_VARIABLE GIT_LAST_COMMIT
			ERROR_VARIABLE git_describe_error
			RESULT_VARIABLE git_describe_result
			OUTPUT_STRIP_TRAILING_WHITESPACE
			)

		execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse HEAD
			WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
			OUTPUT_VARIABLE GIT_LAST_COMMIT_HASH
			ERROR_VARIABLE git_describe_error
			RESULT_VARIABLE git_describe_result
			OUTPUT_STRIP_TRAILING_WHITESPACE
			)

		execute_process(COMMAND ${GIT_EXECUTABLE} rev-list --count HEAD
			WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
			OUTPUT_VARIABLE GIT_COUNT_COMMITS
			ERROR_VARIABLE git_describe_error
			RESULT_VARIABLE git_describe_result
			OUTPUT_STRIP_TRAILING_WHITESPACE
			)
	endif()
endif()

# For installation targets
include(GNUInstallDirs)

# sizeof checking for xdelta config.h generation
include(CheckTypeSize)
check_type_size("size_t" SIZEOF_SIZE_T)
check_type_size("unsigned int" SIZEOF_UNSIGNED_INT)
check_type_size("unsigned long" SIZEOF_UNSIGNED_LONG)
check_type_size("unsigned long long" SIZEOF_UNSIGNED_LONG_LONG)
configure_file(xdelta_config.h.in config.h)

configure_file(compiled.h.in compiled.h)

# handling external dependencies
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_AUTOUIC_SEARCH_PATHS gui/designer)

set(QT_LIBS_USED "")
set(WIN_LIBS_USED "")

if(NOT CMAKE_SYSTEM_NAME STREQUAL "Windows")
	message(FATAL_ERROR
		"This tree builds for Windows only: the ALSA, sndio and BSD/Linux "
		"joystick backends have been removed. CMAKE_SYSTEM_NAME is "
		"'${CMAKE_SYSTEM_NAME}'.")
endif()

# OpenGL is the only supported renderer.
find_package(OpenGL REQUIRED)

# Qt6 is the only supported Qt. The Qt5 and static Qt5 paths went away with the
# non-Windows backends; every Qt5 conditional in the sources is unreachable now.
find_package(Qt6 6.0.0 REQUIRED COMPONENTS Core LinguistTools Widgets Network Svg Gui OpenGLWidgets)
message(STATUS "Found Qt: ${Qt6_VERSION}")

set(QT_LIBS_USED
	Qt::Core
	Qt::Network
	Qt::Widgets
	Qt::Svg
	Qt::Gui
	Qt::OpenGLWidgets
	)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# FFmpeg is the recording backend and it is not optional.
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBAV REQUIRED IMPORTED_TARGET
	libavformat
	libavcodec
	libswresample
	libswscale
	libavutil
	)

# Report the target architecture explicitly. The same source tree builds for
# x86_64 and for ARM64 on the matching machine; every architecture dependent
# piece of the code is written as a pair of branches keyed off the CPU
# macros, so both are expected to compile. The compiler's own target triple
# is the authoritative answer and is also the only one that stays correct
# when the toolchain cross compiles, so try it first.
set(PUNES_TRIPLE "")
if(CMAKE_C_COMPILER_TARGET)
	set(PUNES_TRIPLE "${CMAKE_C_COMPILER_TARGET}")
elseif(CMAKE_C_COMPILER)
	execute_process(COMMAND "${CMAKE_C_COMPILER}" -dumpmachine
		OUTPUT_VARIABLE PUNES_TRIPLE
		OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)
endif()
if(NOT PUNES_TRIPLE)
	set(PUNES_TRIPLE "${CMAKE_SYSTEM_PROCESSOR}")
endif()
string(TOLOWER "${PUNES_TRIPLE}" PUNES_TRIPLE_LC)
string(REGEX MATCH "^(aarch64|arm64)" PUNES_ARM64 "${PUNES_TRIPLE_LC}")
string(REGEX MATCH "^(x86_64|amd64)" PUNES_X86_64 "${PUNES_TRIPLE_LC}")
if(PUNES_ARM64)
	set(PUNES_ARCH "arm64")
elseif(PUNES_X86_64)
	set(PUNES_ARCH "x86_64")
else()
	set(PUNES_ARCH "${PUNES_TRIPLE}")
endif()
message(STATUS "Target arch: ${PUNES_ARCH} (${PUNES_TRIPLE})")

set(WIN_LIBS_USED ${WIN_LIBS_USED} ws2_32)
set(WIN_LIBS_USED ${WIN_LIBS_USED} imm32)
# Modern XAudio2 (2.9) from the toolchain. XAudio2Create() is a plain
# exported function of XAudio2_9.dll, so it works on x86_64 and on ARM64
# alike. The old DirectX SDK header created the engine through a COM class
# registration instead, which is unusable on Windows on ARM.
set(WIN_LIBS_USED ${WIN_LIBS_USED} xaudio2_9)
# CoCreateInstance for the MMDevice enumerator used to list audio outputs.
set(WIN_LIBS_USED ${WIN_LIBS_USED} ole32)

add_subdirectory(src)

if(ENABLE_TESTS)
	enable_testing()
	add_subdirectory(tests)
endif()

# Install data files. The .desktop entry, the hicolor icon set and the AppStream
# metainfo were freedesktop packaging assets and went away with the other
# non-Windows backends; the Windows icon lives in gui/windows/punes.ico and is
# compiled into the executable through gui/windows/resources.rc.
install(FILES misc/nes20db.xml DESTINATION ${CMAKE_INSTALL_DATADIR}/puNES)
install(FILES misc/dip.cfg DESTINATION ${CMAKE_INSTALL_DATADIR}/puNES)

install(FILES README.md ChangeLog DESTINATION ${CMAKE_INSTALL_DOCDIR})
