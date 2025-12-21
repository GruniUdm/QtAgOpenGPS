# Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
# SPDX-License-Identifier: GNU General Public License v3.0 or later
# CMake 3.22 ships with Ubuntu 22.04
# CMake 3.27 and QtCreator 12 recommended
cmake_minimum_required(VERSION 3.22)

# Remove _cmake suffix when the .pro is removed.  Added here for clarity
project(QtAgOpenGPS
    VERSION 0.0
    HOMEPAGE_URL https://github.com/torriem/QtAgOpenGPS
    LANGUAGES CXX
)

# Configure project options
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)  # Required for automatic QML_ELEMENT/QML_SINGLETON registration

# MSVC-specific compiler flags
# /bigobj: Required for SettingsManager with 389 Q_OBJECT_BINDABLE_PROPERTY
# Phase 6.0: Qt6 property system generates >65,536 sections (MSVC default limit)
# /bigobj extends limit to 4,294,967,296 sections (COFF extended format)
if(MSVC)
    add_compile_options(/bigobj)
    message("MSVC: /bigobj flag enabled for large object files (SettingsManager)")

    # Optional: Enable debug symbols for profiling even in Release builds
    # Uncomment these lines when creating profiler builds:
    # set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi")
    # set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF")
    # message("MSVC: Debug symbols enabled in Release for profiling")
endif()

# Find required libraries
#find_package(Qt6 REQUIRED COMPONENTS Core)
 find_package(Qt6 REQUIRED COMPONENTS Widgets) #used only for opengl ?
 find_package(Qt6 REQUIRED COMPONENTS Gui)
 find_package(Qt6 REQUIRED COMPONENTS SerialPort)
# find_package(Qt6 REQUIRED COMPONENTS OpenGL)
find_package(Qt6 REQUIRED COMPONENTS Quick)
find_package(Qt6 REQUIRED COMPONENTS QuickWidgets) #used only for opengl ?
find_package(Qt6 REQUIRED COMPONENTS Network)
find_package(Qt6 REQUIRED COMPONENTS Multimedia)
find_package(Qt6 REQUIRED COMPONENTS LinguistTools) #for i18n internationalization support
find_package(Qt6 REQUIRED COMPONENTS Bluetooth)
find_package(Qt6 REQUIRED COMPONENTS Network)
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Core)



# Add Qt policies
qt_policy(SET QTP0001 NEW)
qt_policy(SET QTP0004 NEW)

# Qt standard project setup with internationalization support
qt_standard_project_setup(REQUIRES 6.8
    I18N_SOURCE_LANGUAGE en
    I18N_TRANSLATED_LANGUAGES en fr ru sr de hi kz
)

# Because modules are not in top folder//
    set(QT_QML_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/)
    
    # Configure QML import paths for Qt Creator and Qt Design Studio
    set(QML_IMPORT_PATH 
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_BINARY_DIR}
        ${QT_QML_OUTPUT_DIRECTORY}
        CACHE STRING "Import paths for Qt Creator's code model"
        FORCE
    )
    
    # Configure Designer-specific import paths
    set(QML_DESIGNER_IMPORT_PATH 
        ${CMAKE_BINARY_DIR}
        CACHE STRING "Import paths for Qt Design Studio designer mode"
        FORCE
    )
# //



# Create the executable
qt_add_executable(QtAgOpenGPS
    main.cpp
    # agopengps.qrc removed - resources now handled by qt_add_qml_module
    # $<$<NOT:$<BOOL:${LOCAL_QML}>>:agopengps.qrc>
    aogrenderer.h aogrenderer.cpp
    # Phase 6.0.19: SettingsManager Qt6 Pure Architecture with generated files
    classes/settingsmanager.h classes/settingsmanager.cpp
    classes/settingsmanager_macros.h
    classes/settingsmanager_properties.h
    classes/settingsmanager_members.h
    # NOTE: settingsmanager_implementations.cpp is INCLUDED in settingsmanager.cpp, not compiled separately
    btnenum.h
    common.h
    formgps.h formgps.cpp
    formgps_classcallbacks.cpp
    formgps_opengl.cpp
    formgps_position.cpp
    formgps_saveopen.cpp
    formgps_sections.cpp
    formgps_settings.cpp
    formgps_sim.cpp
    formgps_udpcomm.cpp
    formgps_ui.cpp
    formgps_ui_boundary.cpp
    formgps_ui_field.cpp
    formgps_ui_vehicle.cpp
    formheadache.h formheadache.cpp
    formheadland.h formheadland.cpp
    glutils.h glutils.cpp
    #qmlsectionbuttons.h qmlsectionbuttons.cpp
    qmlutil.h
    setter.h
    classes/cabcurve.h classes/cabcurve.cpp
    classes/cabline.h classes/cabline.cpp
    classes/cahrs.h classes/cahrs.cpp
    classes/cboundary.h classes/cboundary.cpp
    classes/cboundarylist.h classes/cboundarylist.cpp
    classes/ccamera.h classes/ccamera.cpp
    classes/ccontour.h classes/ccontour.cpp
    classes/cdubins.h classes/cdubins.cpp
    classes/cfence.cpp
    classes/cfielddata.h classes/cfielddata.cpp
    classes/cflag.h classes/cflag.cpp
    classes/cguidance.h classes/cguidance.cpp
    classes/chead.cpp
    classes/cheadline.h classes/cheadline.cpp
    classes/cmodulecomm.h classes/cmodulecomm.cpp
    classes/cnmea.h classes/cnmea.cpp
    classes/cpatches.h classes/cpatches.cpp
    classes/cpgn.h classes/cpgn.cpp
    classes/headachedesigner.h classes/headachedesigner.cpp
    classes/crecordedpath.h classes/crecordedpath.cpp
    classes/csection.h classes/csection.cpp
    classes/csim.h classes/csim.cpp
    classes/ctool.h classes/ctool.cpp
    # classes/ctrack.h classes/ctrack.cpp      # MOVED to qt_add_qml_module (QML_ELEMENT)
    classes/ctram.h classes/ctram.cpp
    classes/cturn.cpp
    classes/cturnlines.cpp
    # classes/cvehicle.h classes/cvehicle.cpp  # MOVED to qt_add_qml_module (QML_ELEMENT)
    classes/cworldgrid.h classes/cworldgrid.cpp
    classes/cyouturn.h classes/cyouturn.cpp
    classes/glm.h
    classes/vec2.h classes/vec2.cpp
    classes/vec3.h classes/vec3.cpp
    classes/vec4.h classes/vec4.cpp
    classes/vecfix2fix.h
    classes/qmlblockage.h classes/qmlblockage.cpp
    # LICENSE
    # TODO.md
    # README.md
     android/AndroidManifest.xml
    # android/build.gradle
    # android/gradle.properties
    # android/gradle/wrapper/gradle-wrapper.jar
    # android/gradle/wrapper/gradle-wrapper.properties
    # android/gradlew
    # android/gradlew.bat
    # android/res/values/libs.xml
    # android/res/xml/qtprovider_paths.xml
)

target_compile_definitions(QtAgOpenGPS PRIVATE QT_QML_DEBUG)



if(ANDROID)
    set(LOCAL_QML OFF)
    message("Local QML is off on Android")
else()
    option(LOCAL_QML "Load QML from disc instead of compiling them in.")
endif()

if(NOT LOCAL_QML AND NOT ANDROID)
    message("Local QML is off. Turn on for faster QML development")
    message("See https://github.com/torriem/QtAgOpenGPS/wiki/Building-QtAgOpenGPS-without-compiling-resources-with-QMake for more information")
endif()

###### Handling Local QML Handling QML in module in LOCAL_QML OFF######
if(NOT LOCAL_QML) #LOCAL_QML OFF or ANDROID
    
    # Qt Design Studio configuration (from official examples)
    set(QT_QML_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/)
    set(QML_IMPORT_PATH ${QT_QML_OUTPUT_DIRECTORY}
        CACHE STRING "Import paths for Qt Creator's code model"
        FORCE
    )
    # Function to process resource files in a given directory (recursive)
    function(process_files DIR OUT_VAR)
        file(GLOB_RECURSE RESOURCE_FILES "${DIR}/*.*")
        set(REL_PATHS)
        foreach(RESOURCE_FILE ${RESOURCE_FILES})
            file(RELATIVE_PATH REL_PATH "${CMAKE_CURRENT_SOURCE_DIR}" "${RESOURCE_FILE}")
            set_source_files_properties(${RESOURCE_FILE} PROPERTIES
                QT_RESOURCE_ALIAS "${REL_PATH}")
            # message(WARNING "file: ${REL_PATH}")
            list(APPEND REL_PATHS "${REL_PATH}")
        endforeach()
        set(${OUT_VAR} ${REL_PATHS} PARENT_SCOPE)
    endfunction()

    # Process image files in images/ directory
    process_files("${CMAKE_CURRENT_SOURCE_DIR}/images" IMAGE_REL_PATHS)

    # Process shader files in shaders/ directory
    process_files("${CMAKE_CURRENT_SOURCE_DIR}/shaders" SHADER_REL_PATHS)

    # Process sound files in sound/ directory
    process_files("${CMAKE_CURRENT_SOURCE_DIR}/sounds" SOUND_REL_PATHS)

    # message(WARNING "All image relative paths: ${IMAGE_REL_PATHS}")
    
    # Function to process QML files in a given directory
    function(process_qml_files DIR)
        file(GLOB QML_FILES "${DIR}/*.qml" "${DIR}/*.js")
        set(REL_PATHS)
        foreach(QML_FILE ${QML_FILES})
            if(NOT QML_FILE MATCHES ".*\\.test\\..*\\.qml$")  #Remove *Test*.qml files from the list
                file(RELATIVE_PATH REL_PATH "${CMAKE_CURRENT_SOURCE_DIR}" "${QML_FILE}")
                # Create alias relative to qml/ directory for module structure
                file(RELATIVE_PATH MODULE_REL_PATH "${CMAKE_CURRENT_SOURCE_DIR}/qml" "${QML_FILE}")
                set_source_files_properties(${QML_FILE} PROPERTIES
                    QT_RESOURCE_ALIAS "${MODULE_REL_PATH}")
                #message(WARNING "file QML_FILE: ${QML_FILE}")
                #message(WARNING "file REL_PATH: ${REL_PATH}")
                # Use relative path from project root for qt_add_qml_module
                list(APPEND REL_PATHS "${REL_PATH}")
            endif()
        endforeach()
        set(${ARGV1} ${REL_PATHS} PARENT_SCOPE)
    endfunction()

    # Process QML files for each directory
    process_qml_files("${CMAKE_CURRENT_SOURCE_DIR}/qml" QML_REL_PATHS)
    process_qml_files("${CMAKE_CURRENT_SOURCE_DIR}/qml/AOG" AOG_REL_PATHS)
    process_qml_files("${CMAKE_CURRENT_SOURCE_DIR}/qml/agio" AGIO_REL_PATHS)
    # process_qml_files("${CMAKE_CURRENT_SOURCE_DIR}/qml/agio/components" AGIO_COMPONENTS_REL_PATHS) # DISABLED: Use unified /qml/components instead
    process_qml_files("${CMAKE_CURRENT_SOURCE_DIR}/qml/agio/interfaces" AGIO_INTERFACES_REL_PATHS)
    process_qml_files("${CMAKE_CURRENT_SOURCE_DIR}/qml/boundary" BOUNDARY_REL_PATHS)
    process_qml_files("${CMAKE_CURRENT_SOURCE_DIR}/qml/components" COMPONENTS_REL_PATHS)
    process_qml_files("${CMAKE_CURRENT_SOURCE_DIR}/qml/config" CONFIG_REL_PATHS)
    process_qml_files("${CMAKE_CURRENT_SOURCE_DIR}/qml/config/steercomponents" STEERCOMPONENTS_REL_PATHS)
    process_qml_files("${CMAKE_CURRENT_SOURCE_DIR}/qml/field" FIELD_REL_PATHS)
    process_qml_files("${CMAKE_CURRENT_SOURCE_DIR}/qml/interfaces" INTERFACES_REL_PATHS)
    process_qml_files("${CMAKE_CURRENT_SOURCE_DIR}/qml/steerconfig" STEERCONFIG_REL_PATHS)
    process_qml_files("${CMAKE_CURRENT_SOURCE_DIR}/qml/tracks" TRACKS_REL_PATHS)
    process_qml_files("${CMAKE_CURRENT_SOURCE_DIR}/qml/wizards" WIZARDS_REL_PATHS)
    process_qml_files("${CMAKE_CURRENT_SOURCE_DIR}/qml/singletons" SINGLETONS_REL_PATHS) #adding singletons
 
    # Combine all relative paths
    set(ALL_REL_PATHS
        ${QML_REL_PATHS}
        ${AOG_REL_PATHS}
        ${AGIO_REL_PATHS}
        # ${AGIO_COMPONENTS_REL_PATHS} # DISABLED: Use unified /qml/components instead
        ${AGIO_INTERFACES_REL_PATHS}
        ${BOUNDARY_REL_PATHS}
        ${COMPONENTS_REL_PATHS}
        ${CONFIG_REL_PATHS}
        ${STEERCOMPONENTS_REL_PATHS}
        ${FIELD_REL_PATHS}
        ${INTERFACES_REL_PATHS}
        ${STEERCONFIG_REL_PATHS}
        ${TRACKS_REL_PATHS}
        ${WIZARDS_REL_PATHS}
        ${SINGLETONS_REL_PATHS} # adding singletons relative paths
    )


    # message(WARNING "All relative paths: ${ALL_REL_PATHS}")

    #add singletons (singletons/ path)
    set_source_files_properties(${SINGLETONS_REL_PATHS}
        PROPERTIES QT_QML_SINGLETON_TYPE TRUE
    )
else() #LOCAL_QML ON
    file(GLOB_RECURSE qml_files "qml/*")
    add_custom_target(AOGQML SOURCES ${qml_files})
    message("Local QML is ON!")
    add_compile_definitions(LOCAL_QML)

endif()

# Qt 6.8 PURE OFFICIAL convention
qt_add_qml_module(QtAgOpenGPS
    URI AOG
    VERSION 1.0
    RESOURCE_PREFIX "/"
    QML_FILES
    ${ALL_REL_PATHS} #all qml files
    SOURCES
    aogrenderer.cpp
    aogrenderer.h
    classes/settingsmanager.cpp
    classes/settingsmanager.h
    classes/settingsmanager_macros.h
    classes/settingsmanager_properties.h
    classes/settingsmanager_members.h
    # NOTE: settingsmanager_implementations.cpp is INCLUDED, not compiled separately
    classes/ctrack.cpp
    classes/ctrack.h
    classes/cvehicle.cpp
    classes/cvehicle.h
    classes/agioservice.cpp
    classes/agioservice.h
    # Phase 6.0.21: GPSWorker removed (useless thread - no I/O)
    classes/pgnparser.cpp
    classes/pgnparser.h
    classes/ctraffic.cpp
    classes/ctraffic.h
    classes/ntripworker.cpp
    classes/ntripworker.h
    # Phase 6.0.24: udpworker removed (event-driven QUdpSocket in main thread)
    # classes/udpworker.cpp
    # classes/udpworker.h
    classes/serialworker.cpp
    classes/serialworker.h

    RESOURCES
    ${IMAGE_REL_PATHS}
    ${SHADER_REL_PATHS}
    ${SOUND_REL_PATHS}
    RESOURCES android/AndroidManifest.xml android/build.gradle android/res/values/libs.xml android/res/xml/qtprovider_paths.xml android/gradle/wrapper/gradle-wrapper.jar android/gradle/wrapper/gradle-wrapper.properties android/gradle.properties android/gradlew android/gradlew.bat
    SOURCES rendering.h
    SOURCES backend/backend.h backend/backend.cpp
    SOURCES simpleproperty.h
    #SOURCES globals.h globals.cpp

    SOURCES backend/mainwindowstate.h backend/mainwindowstate.cpp
    SOURCES backend/boundaryinterface.h backend/boundaryinterface.cpp
    SOURCES backend/fieldinterface.h backend/fieldinterface.cpp
    SOURCES backend/fieldinfo.h backend/fieldinfo.cpp
    SOURCES backend/headlandinterface.h backend/headlandinterface.cpp
    SOURCES backend/headacheinterface.h backend/headacheinterface.cpp
    SOURCES backend/flagsinterface.h backend/flagsinterface.cpp
    SOURCES backend/fencelinemodel.h backend/fencelinemodel.cpp
    SOURCES backend/flagmodel.h backend/flagmodel.cpp
    SOURCES backend/fixframe.h
)

###### End Script of Handling QML in module ######

###### Script for Translation ######

# Set the output directory for the QM files
if(ANDROID)
    # Android requires QM files to be in the assets directory
    set(QM_OUTPUT_DIR "${CMAKE_BINARY_DIR}/android-build-QtAgOpenGPS/assets/i18n")
else()
    set(QM_OUTPUT_DIR "${CMAKE_BINARY_DIR}/i18n")
endif()

# Find all TS translation files
file(GLOB TS_FILES "${CMAKE_CURRENT_SOURCE_DIR}/i18n/*.ts")
set_source_files_properties(${TS_FILES} PROPERTIES OUTPUT_LOCATION "${QM_OUTPUT_DIR}")

# process all TS files (lupdate and lrelease)
qt_add_translations(QtAgOpenGPS
    RESOURCE_PREFIX /qt/qml/AOG/i18n
    TS_FILE_BASE qml
    TS_FILE_DIR i18n
)
message("Translation path for QM files: ${QM_OUTPUT_DIR}")

###### End script for Translation ######

# show some of our mockup files in the project tree without compiling it
set(qml_mockup_files
    qml/AOG/qmldir
    qml/AOG/MockAOGRenderer.qml
    # Interface module mocks removed - now using QML_SINGLETON + QML_ELEMENT
    qml/Settings/qmldir
    qml/Settings/MockSettings.qml
)

add_custom_target(MockQML SOURCES ${qml_mockup_files})

# Show documentation files in claudedocs/ in the project tree without compiling them
file(GLOB_RECURSE claudedocs_files "claudedocs/*.md" "claudedocs/*.txt")
file(GLOB_RECURSE docs_files "docs/*")
add_custom_target(Documentation SOURCES
    ${claudedocs_files}
    ${docs_files}
    TODO.md
    README.md
    README_TRANSLATIONS.md
)


# Where to look for header files
target_include_directories(QtAgOpenGPS PRIVATE
    .
    classes
    backend
)

# QtAgIO QML module disabled - migrated to qml/agio/ in Phase 4.2
# add_subdirectory(QtAgIO)
# Libraries to link
target_link_libraries(QtAgOpenGPS PRIVATE
    # QtAgIO  # QML module disabled - sources integrated in Phase 4.2
#   Qt6::Core
    Qt6::Widgets
    Qt6::Gui
#   Qt6::OpenGL
    Qt6::Quick
    Qt6::QuickWidgets
    Qt6::Network
    Qt6::Multimedia
    Qt6::SerialPort
)
target_link_libraries(QtAgOpenGPS PRIVATE Qt6::Core)
target_link_libraries(QtAgOpenGPS PRIVATE Qt6::Core)

# Compile defines
target_compile_definitions(QtAgOpenGPS PRIVATE
    QT_DEPRECATED_WARNINGS
)

set_target_properties(QtAgOpenGPS PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
    QT_ANDROID_PACKAGE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android
)

install(TARGETS QtAgOpenGPS
    #BUNDLE  DESTINATION .
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}QtAgOpenGPS"
    LIBRARY DESTINATION "${CMAKE_INSTALL_BINDIR}QtAgOpenGPS"
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/qmldir
    DESTINATION "${CMAKE_INSTALL_BINDIR}/QtAgOpenGPS"
)

# target_compile_options(QtAgOpenGPS PRIVATE
#     -Wall
#     -Werror
# )

# qt_generate_deploy_qml_app_script(
#     TARGET QtAgOpenGPS
#     OUTPUT_SCRIPT deploy_script
#     MACOS_BUNDLE_POST_BUILD
#     NO_UNSUPPORTED_PLATFORM_ERROR
#     DEPLOY_USER_QML_MODULES_ON_UNSUPPORTED_PLATFORM
# )
# install(SCRIPT ${deploy_script})

add_custom_command(TARGET QtAgOpenGPS POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/icons
        $<TARGET_FILE_DIR:QtAgOpenGPS>/icons
    COMMENT "Copying icons"
)




