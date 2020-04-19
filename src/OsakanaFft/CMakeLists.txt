project(osakana_fft CXX)

if (GREETINGS_BUILD_SHARED_LIBS)
  add_library(osakanafft SHARED ${PROJECT_SOURCE_DIR}/src/OsakanaFft.cpp ${PROJECT_SOURCE_DIR}/src/OsakanaFpFft.cpp)
else()
  add_library(osakanafft STATIC ${PROJECT_SOURCE_DIR}/src/OsakanaFft.cpp ${PROJECT_SOURCE_DIR}/src/OsakanaFpFft.cpp)
endif()

target_include_directories(osakanafft
  PUBLIC ${PROJECT_SOURCE_DIR}/include
  )
