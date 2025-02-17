add_executable(
  eeprom_unit_tests
  ${CMAKE_CURRENT_SOURCE_DIR}/EEPROM_journal_test.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/dummy_eeprom_chip.cpp
  ${CMAKE_SOURCE_DIR}/src/persistent_stores/journal/backend.cpp
  ${CMAKE_SOURCE_DIR}/src/persistent_stores/storage_drivers/eeprom_storage.cpp
  ../mock/bsod.cpp
  ../mock/timing_dummy.cpp
  ${CMAKE_SOURCE_DIR}/tests/stubs/fake_freertos_mutex.cpp
  ../mock/crc32_sw.cpp
  )

target_compile_definitions(eeprom_unit_tests PUBLIC EEPROM_UNITTEST)
target_include_directories(
  eeprom_unit_tests
  PUBLIC .
         ${CMAKE_SOURCE_DIR}/src/persistent_stores
         ${CMAKE_SOURCE_DIR}/src/persistent_stores/store_instances
         ${CMAKE_SOURCE_DIR}/src
         ${CMAKE_SOURCE_DIR}/src/lang
         ${CMAKE_SOURCE_DIR}/src/guiconfig
         ${CMAKE_SOURCE_DIR}/include
         ${CMAKE_BINARY_DIR}/include
         ${CMAKE_SOURCE_DIR}/tests/stubs
         ${CMAKE_SOURCE_DIR}/src/logging
         ${CMAKE_SOURCE_DIR}/src/common
         ${CMAKE_SOURCE_DIR}/src/common/utils
         ${CMAKE_SOURCE_DIR}/lib/cthash/include
         ${CMAKE_SOURCE_DIR}/lib/Marlin/Marlin/src/
  )

add_catch_test(eeprom_unit_tests)
