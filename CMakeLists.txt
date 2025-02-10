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

# Find required libraries
# find_package(Qt6 REQUIRED COMPONENTS Core)
 find_package(Qt6 REQUIRED COMPONENTS Widgets) #used only for opengl ?
# find_package(Qt6 REQUIRED COMPONENTS Gui)
# find_package(Qt6 REQUIRED COMPONENTS OpenGL)
find_package(Qt6 REQUIRED COMPONENTS Quick)
 find_package(Qt6 REQUIRED COMPONENTS QuickWidgets) #used only for opengl ?
find_package(Qt6 REQUIRED COMPONENTS Network)
find_package(Qt6 REQUIRED COMPONENTS Multimedia)
find_package(Qt6 REQUIRED COMPONENTS LinguistTools) #for i18n internationalization support

# Add Qt policies
qt_policy(SET QTP0001 NEW)
qt_policy(SET QTP0004 NEW)

# Qt standard project setup with internationalization support
qt_standard_project_setup(REQUIRES 6.5
    I18N_SOURCE_LANGUAGE en
    I18N_TRANSLATED_LANGUAGES fr ru
)

# Create the executable
qt_add_executable(QtAgOpenGPS
    main.cpp
    $<$<NOT:$<BOOL:${LOCAL_QML}>>:agopengps.qrc>
    aogproperties.cpp
    aogproperty.h aogproperty.cpp
 #   aogrenderer.h aogrenderer.cpp
    aogsettings.h aogsettings.cpp
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
    formgps_ui_tracks.cpp
    formgps_ui_vehicle.cpp
    formheadache.h formheadache.cpp
    formheadland.h formheadland.cpp
    glutils.h glutils.cpp
    interfaceproperty.h
    properties.h
    qmlsectionbuttons.h qmlsectionbuttons.cpp
    qmlsettings.h qmlsettings.cpp
    qmlsettings_addkeys.cpp
    qmlutil.h
    setter.h
    testlists.cpp
    testnmea.cpp
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
    classes/crecordedpath.h classes/crecordedpath.cpp
    classes/csection.h classes/csection.cpp
    classes/csim.h classes/csim.cpp
    classes/ctool.h classes/ctool.cpp
    classes/ctrack.h classes/ctrack.cpp
    classes/ctram.h classes/ctram.cpp
    classes/cturn.cpp
    classes/cturnlines.cpp
    classes/cvehicle.h classes/cvehicle.cpp
    classes/cworldgrid.h classes/cworldgrid.cpp
    classes/cyouturn.h classes/cyouturn.cpp
    classes/glm.h
    classes/vec2.h classes/vec2.cpp
    classes/vec3.h classes/vec3.cpp
    classes/vec4.h classes/vec4.cpp
    classes/vecfix2fix.h
    classes/qmlblockage.h classes/qmlblockage.cpp
    LICENSE
    props.py
    TODO.md
    parse_properties.py
    README.md
    android/AndroidManifest.xml
    android/build.gradle
    android/gradle.properties
    android/gradle/wrapper/gradle-wrapper.jar
    android/gradle/wrapper/gradle-wrapper.properties
    android/gradlew
    android/gradlew.bat
    android/res/values/libs.xml
    android/res/xml/qtprovider_paths.xml
)

###### Handling Local QML Handling QML in module in LOCAL_QML OFF######

if(NOT LOCAL_QML)
    # Function to process image files in a given directory (non-recursive)
    function(process_files DIR OUT_VAR)
        file(GLOB_RECURSE IMAGE_FILES "${DIR}/*.*")
        set(REL_PATHS)
        foreach(IMAGE_FILE ${IMAGE_FILES})
            file(RELATIVE_PATH REL_PATH "${CMAKE_CURRENT_SOURCE_DIR}" "${IMAGE_FILE}")
            set_source_files_properties(${IMAGE_FILE} PROPERTIES
                QT_RESOURCE_ALIAS "${REL_PATH}")
            # message(WARNING "file: ${REL_PATH}")
            list(APPEND REL_PATHS "${REL_PATH}")
        endforeach()
        set(${OUT_VAR} ${REL_PATHS} PARENT_SCOPE)
    endfunction()

    # Process image files for each directory
    process_files("${CMAKE_CURRENT_SOURCE_DIR}/images" IMAGE_REL_PATHS)

    # Process shader files for each directory
    process_files("${CMAKE_CURRENT_SOURCE_DIR}/shaders" SHADER_REL_PATHS)


    # message(WARNING "All image relative paths: ${IMAGE_REL_PATHS}")
    
    # Function to process QML files in a given directory
    function(process_qml_files DIR)
        file(GLOB QML_FILES "${DIR}/*.qml" "${DIR}/*.js")
        set(REL_PATHS)
        foreach(QML_FILE ${QML_FILES})
            if(NOT QML_FILE MATCHES "Test.*\\.qml$")  #Remove Test*.qml files from the list
                file(RELATIVE_PATH REL_PATH "${CMAKE_CURRENT_SOURCE_DIR}/qml" "${QML_FILE}")
                set_source_files_properties(${QML_FILE} PROPERTIES
                    QT_RESOURCE_ALIAS "${REL_PATH}")
                # message(WARNING "file: ${REL_PATH}")
                list(APPEND REL_PATHS "qml/${REL_PATH}")
            endif()
        endforeach()
        set(${ARGV1} ${REL_PATHS} PARENT_SCOPE)
    endfunction()

    # Process QML files for each directory
    process_qml_files("${CMAKE_CURRENT_SOURCE_DIR}/qml" QML_REL_PATHS)
    #process_qml_files("${CMAKE_CURRENT_SOURCE_DIR}/qml/AOG" AOG_REL_PATHS)
    process_qml_files("${CMAKE_CURRENT_SOURCE_DIR}/qml/boundary" BOUNDARY_REL_PATHS)
    process_qml_files("${CMAKE_CURRENT_SOURCE_DIR}/qml/components" COMPONENTS_REL_PATHS)
    process_qml_files("${CMAKE_CURRENT_SOURCE_DIR}/qml/config" CONFIG_REL_PATHS)
    process_qml_files("${CMAKE_CURRENT_SOURCE_DIR}/qml/config/steercomponents" STEERCOMPONENTS_REL_PATHS)
    process_qml_files("${CMAKE_CURRENT_SOURCE_DIR}/qml/field" FIELD_REL_PATHS)
    process_qml_files("${CMAKE_CURRENT_SOURCE_DIR}/qml/interfaces" INTERFACES_REL_PATHS)
    process_qml_files("${CMAKE_CURRENT_SOURCE_DIR}/qml/steerconfig" STEERCONFIG_REL_PATHS)
    process_qml_files("${CMAKE_CURRENT_SOURCE_DIR}/qml/tracks" TRACKS_REL_PATHS)
    process_qml_files("${CMAKE_CURRENT_SOURCE_DIR}/qml/wizards" WIZARDS_REL_PATHS)

    # Combine all relative paths
    set(ALL_REL_PATHS
        ${QML_REL_PATHS}
        ${AOG_REL_PATHS}
        ${BOUNDARY_REL_PATHS}
        ${COMPONENTS_REL_PATHS}
        ${CONFIG_REL_PATHS}
        ${STEERCOMPONENTS_REL_PATHS}
        ${FIELD_REL_PATHS}
        ${INTERFACES_REL_PATHS}
        ${STEERCONFIG_REL_PATHS}
        ${TRACKS_REL_PATHS}
        ${WIZARDS_REL_PATHS}
    )

    # message(WARNING "All relative paths: ${ALL_REL_PATHS}")

    # Configure QML module with relative paths
    qt_add_qml_module(QtAgOpenGPS
        URI AOG
        VERSION 1.0
        RESOURCE_PREFIX "/"
        QML_FILES
        ${ALL_REL_PATHS} #all qml files
        SOURCES
        aogrenderer.cpp
        aogrenderer.h

        RESOURCES
        ${IMAGE_REL_PATHS}
        ${SHADER_REL_PATHS}
    )

    # Add translations
   qt_add_translations(QtAgOpenGPS
    RESOURCE_PREFIX /qt/qml/AOG/i18n
    TS_FILE_BASE qml 
    TS_FILE_DIR i18n
    )

 # Function to process translated QM files in a given directory
 function(process_qm_files DIR OUT_VAR)
 file(GLOB QM_FILES "${DIR}/*.qm")
 set(REL_PATHS)
 foreach(QM_FILE ${QM_FILES})
     file(RELATIVE_PATH REL_PATH "${CMAKE_CURRENT_SOURCE_DIR}" "${QM_FILE}")
     set_source_files_properties(${QM_FILE} PROPERTIES
         QT_RESOURCE_ALIAS "${REL_PATH}")
     # message(WARNING "file: ${REL_PATH}")
     list(APPEND REL_PATHS "${REL_PATH}")
 endforeach()
 set(${OUT_VAR} ${REL_PATHS} PARENT_SCOPE)
endfunction()

# Process QM files in the i18n directory
process_qm_files("${CMAKE_CURRENT_SOURCE_DIR}/i18n" QM_REL_PATHS)

# Add the translation file to the resources

# Add the translation files to the resources
qt_add_resources(QtAgOpenGPS "translations"
PREFIX "/"
FILES
${QM_REL_PATHS}
)

else()

    set(qml_interfaces_singletons
        qml/interfaces/TracksInterface.qml
    )

    set_source_files_properties(${qml_interfaces_singletons}
        PROPERTIES QT_QML_SINGLETON_TYPE TRUE
    )

    qt_add_qml_module(QtAgOpenGPS
        URI AOG
        VERSION 1.0
        QML_FILES ${qml_interfaces_singletons}
        SOURCES
        aogrenderer.cpp
        aogrenderer.h
        DEPENDENCIES Qt6::QtQuick
    )

endif()



###### End Script of Handling QML in module ######

# show some of our mockup files in the project tree without compiling it
set(qml_mockup_files
    qml/AOG/qmldir
    qml/MockSettings.qml
    qml/MockTracks.qml
    qml/MockVehicle.qml
)

add_custom_target(MockQML SOURCES ${qml_mockup_files})

if(ANDROID)
    set(LOCAL_QML OFF)
else()
        option(LOCAL_QML "Load QML from disc instead of compiling them in.")
endif()

if(LOCAL_QML)
    add_compile_definitions(LOCAL_QML)

    # Display the qrc file in the project tree without compiling it
    add_custom_target(AOGQML SOURCES agopengps.qrc)

    message("Local QML is ON!")
elseif(NOT ANDROID)
    message("Local QML is off. Turn on for faster QML development")
    message("See https://github.com/torriem/QtAgOpenGPS/wiki/Building-QtAgOpenGPS-without-compiling-resources-with-QMake for more information")
endif()

# Where to look for header files
target_include_directories(QtAgOpenGPS PRIVATE
    .
    classes
)

# Libraries to link
target_link_libraries(QtAgOpenGPS PRIVATE
#   Qt6::Core
    Qt6::Widgets
#   Qt6::Gui
#   Qt6::OpenGL
    Qt6::Quick
    Qt6::QuickWidgets
    Qt6::Network
    Qt6::Multimedia
)

# Compile defines
target_compile_definitions(QtAgOpenGPS PRIVATE
    QT_DEPRECATED_WARNINGS
)

set_target_properties(QtAgOpenGPS PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
    QT_ANDROID_PACKAGE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android
)

add_subdirectory(QtAgIO)
