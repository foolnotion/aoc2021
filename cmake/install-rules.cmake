if(PROJECT_IS_TOP_LEVEL)
  set(CMAKE_INSTALL_INCLUDEDIR include/aoc2021 CACHE PATH "")
endif()

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

# find_package(<package>) call for consumers to find this project
set(package aoc2021)

install(
    TARGETS aoc2021_aoc2021
    EXPORT aoc2021Targets
    RUNTIME COMPONENT aoc2021_Runtime
)

write_basic_package_version_file(
    "${package}ConfigVersion.cmake"
    COMPATIBILITY SameMajorVersion
)

# Allow package maintainers to freely override the path for the configs
set(
    aoc2021_INSTALL_CMAKEDIR "${CMAKE_INSTALL_DATADIR}/${package}"
    CACHE PATH "CMake package config location relative to the install prefix"
)
mark_as_advanced(aoc2021_INSTALL_CMAKEDIR)

install(
    FILES cmake/install-config.cmake
    DESTINATION "${aoc2021_INSTALL_CMAKEDIR}"
    RENAME "${package}Config.cmake"
    COMPONENT aoc2021_Development
)

install(
    FILES "${PROJECT_BINARY_DIR}/${package}ConfigVersion.cmake"
    DESTINATION "${aoc2021_INSTALL_CMAKEDIR}"
    COMPONENT aoc2021_Development
)

install(
    EXPORT aoc2021Targets
    NAMESPACE aoc2021::
    DESTINATION "${aoc2021_INSTALL_CMAKEDIR}"
    COMPONENT aoc2021_Development
)

if(PROJECT_IS_TOP_LEVEL)
  include(CPack)
endif()
