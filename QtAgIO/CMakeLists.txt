cmake_minimum_required(VERSION 3.22)

project(QtAgIO
        VERSION 0.0
        HOMEPAGE_URL https://github.com/torriem/QtAgOpenGPS
        LANGUAGES CXX
)


#Qt magic
find_package(Qt6 REQUIRED COMPONENTS Quick Bluetooth Widgets QuickWidgets)
qt_standard_project_setup()



# Create the AgIO executable
qt_add_executable(QtAgIO
    main.cpp
    #$<$<NOT:$<BOOL:${LOCAL_QML}>>:agio.qrc>
    agioproperties.cpp
    agioproperty.h agioproperty.cpp
    agiosettings.h agiosettings.cpp
    formloop.h formloop.cpp
    formloop_udpcomm.cpp
    formloop_nmea.cpp
    formloop_ntripcomm.cpp
    formloop_ui.cpp
    formloop_settings.cpp
    properties.h
    qmlsettings.h qmlsettings.cpp
    qmlsettings_addkeys.cpp
    qmlutil.h
    formloop_formudp.cpp
    inc/CTraffic.h src/CTraffic.cpp
    inc/CScanReply.h src/CScanReply.cpp
    inc/bluetoothdevicelist.h src/bluetoothdevicelist.cpp
    inc/bluetoothmanager.h src/bluetoothmanager.cpp
    interfaceproperty.h
)

# if(LOCAL_QML)
#     add_compile_definitions(LOCAL_QML)

#     # Display the qrc file in the project tree without compiling it
#     add_custom_target(AgIOQML SOURCES agio.qrc)

#     message("Local QML is ON!")
# elseif(NOT ANDROID)
#     message("Local QML is off. Turn on for faster QML development")
#     message("See https://github.com/torriem/QtAgOpenGPS/wiki/Building-QtAgOpenGPS-without-compiling-resources-with-QMake for more information")
# endif()

###### Handling Local QML Handling QML in module for QtAgIO######

# Function to process resource files in a given directory (recursive)
function(process_files DIR OUT_VAR)
    file(GLOB_RECURSE RESOURCE_FILES "${DIR}/*.*")
    set(REL_PATHS)
    foreach(RESOURCE_FILE ${RESOURCE_FILES})
        file(RELATIVE_PATH REL_PATH "${CMAKE_CURRENT_SOURCE_DIR}/" "${RESOURCE_FILE}")
        set_source_files_properties(${RESOURCE_FILE} PROPERTIES
            QT_RESOURCE_ALIAS "${REL_PATH}")
        #message(WARNING "QtAgIO image file: ${REL_PATH}")
        list(APPEND REL_PATHS "${REL_PATH}")
    endforeach()
    set(${OUT_VAR} ${REL_PATHS} PARENT_SCOPE)
endfunction()

# Process image files in images/ directory
process_files("${CMAKE_CURRENT_SOURCE_DIR}/images" QTAGIO_IMAGE_REL_PATHS)

#message(WARNING "All image relative paths: ${QTAGIO_IMAGE_REL_PATHS}")

# Function to process QML files in a given directory
function(process_qml_files DIR)
    file(GLOB QML_FILES "${DIR}/*.qml" "${DIR}/*.js")
    set(REL_PATHS)
    foreach(QML_FILE ${QML_FILES})
        
            file(RELATIVE_PATH REL_PATH "${CMAKE_CURRENT_SOURCE_DIR}/qml" "${QML_FILE}")
            set_source_files_properties(${QML_FILE} PROPERTIES
                QT_RESOURCE_ALIAS "${REL_PATH}")
            #message(WARNING "file QtAgIO QML_FILE: ${QML_FILE}")
            #message(WARNING "file QtAgIO REL_PATH: ${REL_PATH}")
            list(APPEND REL_PATHS "qml/${REL_PATH}")
       
    endforeach()
    set(${ARGV1} ${REL_PATHS} PARENT_SCOPE)
endfunction()

# Process QML files for each directory 
process_qml_files("${CMAKE_CURRENT_SOURCE_DIR}/qml" QTAGIO_QML_REL_PATHS)
process_qml_files("${CMAKE_CURRENT_SOURCE_DIR}/qml/interfaces" QTAGIO_INTERFACES_REL_PATHS)
process_qml_files("${CMAKE_CURRENT_SOURCE_DIR}/qml/components" QTAGIO_COMPONENTS_REL_PATHS)

# Combine all relative paths
set(QTAGIO_ALL_REL_PATHS
    ${QTAGIO_QML_REL_PATHS}
    ${QTAGIO_INTERFACES_REL_PATHS}
    ${QTAGIO_COMPONENTS_REL_PATHS}
)

# message(WARNING "All relative paths: ${ALL_REL_PATHS}")

# Configure QML module with relative paths
qt_add_qml_module(QtAgIO
    URI AGIO
    VERSION 1.0
    RESOURCE_PREFIX "/"
    QML_FILES
    ${QTAGIO_ALL_REL_PATHS}
    RESOURCES
    ${QTAGIO_IMAGE_REL_PATHS}
)  

###### End of Handling Local QML Handling QML in module for QtAgIO######


target_include_directories(QtAgIO
    PRIVATE src
    PUBLIC inc
    PRIVATE .
)

#this is needed by QtAgIO only

# Libraries to link
target_link_libraries(QtAgIO PRIVATE
    Qt6::Core
    Qt6::Network
    Qt6::Gui
    Qt6::Widgets
    Qt6::Quick
    Qt6::QuickWidgets
    Qt6::Bluetooth
)

target_compile_options(QtAgIO PRIVATE 
    -Wall 
    -Werror
)

