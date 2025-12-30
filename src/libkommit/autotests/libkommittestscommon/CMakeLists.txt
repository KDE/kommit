add_library(libkommitTestsCommon SHARED
  libkommitTestsCommon_global.h
  testcommon.cpp
  testcommon.h
  repo.qrc
  kommit-git-sample-test.qrc
)

target_link_libraries(libkommitTestsCommon PRIVATE Qt${QT_VERSION_MAJOR}::Core libkommit)

target_compile_definitions(libkommitTestsCommon PRIVATE LIBKOMMITTESTSCOMMON_LIBRARY)

add_definitions(-DSAMPLES_PATH=\"${CMAKE_CURRENT_SOURCE_DIR}/kommit_sample_repo\")
