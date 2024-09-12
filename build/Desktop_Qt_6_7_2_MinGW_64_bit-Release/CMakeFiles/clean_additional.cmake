# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Release")
  file(REMOVE_RECURSE
  "CMakeFiles\\killoverlap_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\killoverlap_autogen.dir\\ParseCache.txt"
  "killoverlap_autogen"
  )
endif()
