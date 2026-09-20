#
# Unit test harness. Off by default: the build script turns it on only for its
# optional test step, so a normal build never pays for it.
#
#   cmake -S . -B build -DENABLE_TESTS=ON
#   cmake --build build --target crc_test
#   ./build/tests/crc_test
#
add_executable(crc_test
	crc_test.cpp
	${CMAKE_SOURCE_DIR}/src/c++/crc/crc.cpp
	${CMAKE_SOURCE_DIR}/src/c++/crc/Crc32.cpp
	)

# crc.cpp includes "common.h", which lives in src/core/. The punes target gets
# there through a handful of include directories; mirror the ones the CRC
# sources actually need rather than pulling in the whole target.
target_include_directories(crc_test PRIVATE
	${CMAKE_SOURCE_DIR}/src
	${CMAKE_SOURCE_DIR}/src/core
	${CMAKE_SOURCE_DIR}/src/c++/crc
	)

# Crc32.cpp is third party and carries its own warnings.
set_source_files_properties(${CMAKE_SOURCE_DIR}/src/c++/crc/Crc32.cpp
	PROPERTIES COMPILE_OPTIONS "-w")

if(NOT MSVC)
	# Keep it portable: this is a test, not a release binary.
	target_compile_options(crc_test PRIVATE -O2)
endif()
