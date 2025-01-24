// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// Main Config window. Holds all the buttons.
import QtQuick
import QtQuick.Controls.Fusion
import QtQuick.Layouts
//import QtQuick.Extras 1.4
import QtQuick.Dialogs
import Qt.labs.folderlistmodel

import ".."
import "../components"
import "steercomponents"

Dialog {
	x: 0
	y: 0
	width: parent.width
	height: parent.height
    modal: true
    standardButtons: "NoButton"
    title: qsTr("General Settings")

    onVisibleChanged: {
        vehicleMenu.checked = false
        implementMenuBtn.checked = false
        sourcesMenubtn.checked = false
        uTurnMenu.checked = false
        modulesMenubtn.checked = false
        tramMenu.checked = false
        displayMenu.checked = false
        featureMenu.checked = false
    }

    Rectangle{
        id: configMain
        color: aog.borderColor
        border.color: aog.blackDayWhiteNight
        border.width: 8 * theme.scaleWidth
        visible: true
        anchors.fill: parent
        TopLine{
            id: topLine
        }

        ScrollView {
            id: leftColumnView
            anchors.top: topLine.bottom
            anchors.left: topLine.left
            width: 110 * theme.scaleWidth
            anchors.bottom: bottomLine.top

            //ScrollBar.vertical.policy: ScrollBar.AlwaysOn
            clip: true
            ScrollBar.vertical  : ScrollBar {
                policy: ScrollBar.AlwaysOn
                anchors.rightMargin: 10
                contentItem: Rectangle {
                    color: "red"
                }
            }

            Column{
                id:leftColumn
                anchors.fill: parent
                //anchors.top:topLine.bottom
                //anchors.left: topLine.left
                //width: childrenRect.width
                visible: true
                spacing: 15
                ButtonGroup {
                    buttons: [ vehicleMenu, implementMenuBtn, sourcesMenubtn, uTurnMenu,  modulesMenubtn, tramMenu, displayMenu, featureMenu ]
                }
                MainConfigMainBtns{
                    id:vehicleMenu
                    icon.source: prefix + "/images/Config/Con_VehicleMenu.png"
                }
                Column{
                    x: 25
                    id: tractorMenu
                    visible: vehicleMenu.checked
                    height: children.height

                    onVisibleChanged: {
                        if (visible)
                            configTrSettings.checked = true
                    }

                    ButtonGroup {
                        buttons: [configTrSettings, configTrDim, btnconfigTrAntDim, ]
                    }

                    MainConfigSecondaryBtns{
                        id: configTrSettings
                        icon.source: prefix + "/images/Config/ConS_VehicleConfig.png"
                    }
                    MainConfigSecondaryBtns{
                        id: configTrDim
                        icon.source: prefix + "/images/Config/ConS_ImplementHitch.png"
                    }
                    MainConfigSecondaryBtns{
                        id: btnconfigTrAntDim
                        icon.source: prefix + "/images/Config/ConS_ImplementAntenna.png"
                    }
                }
                MainConfigMainBtns{
                    id:implementMenuBtn
                    icon.source: prefix + "/images/Config/Con_ImplementMenu.png"
                }
                Column{
                    id: implementMenu
                    visible: implementMenuBtn.checked
                    height: children.height
                    x: 25

                    onVisibleChanged: {
                        if (visible)
                            configImpSettings.checked = true
                    }
                    ButtonGroup {
                        buttons: [ configImpSettings, configImpDim, configImpAxle, configImpOffset, configImpSection, configImpTiming, configImpSwitches ]
                    }
                    MainConfigSecondaryBtns{
                        id: configImpSettings
                        icon.source: prefix + "/images/Config/ConS_VehicleConfig.png"
                    }
                    MainConfigSecondaryBtns{
                        id: configImpDim
                        icon.source: prefix + "/images/Config/ConS_ImplementHitch.png"
                    }
                    MainConfigSecondaryBtns{
                        id: configImpAxle
                        icon.source: prefix + "/images/Config/ConS_ImplementPivot.png"
                    }
                    MainConfigSecondaryBtns{
                        id: configImpOffset
                        icon.source: prefix + "/images/Config/ConS_ImplementOffset.png"
                    }
                    MainConfigSecondaryBtns{
                        id: configImpSection
                        icon.source: prefix + "/images/Config/ConS_ImplementSection.png"
                    }
                    MainConfigSecondaryBtns{
                        id: configImpTiming
                        icon.source: prefix + "/images/Config/ConS_ImplementSettings.png"
                    }
                    MainConfigSecondaryBtns{
                        id: configImpSwitches
                        icon.source: prefix + "/images/Config/ConS_ImplementSwitch.png"
                    }
                }
                MainConfigMainBtns{
                    id:sourcesMenubtn
                    icon.source: prefix + "/images/Config/Con_SourcesMenu.png"
                }
                ButtonGroup {
                    buttons: [ configsrcHeading, configsrcRoll ]
                }
                Column{
                    id: sourcesMenu
                    visible: sourcesMenubtn.checked
                    height: children.height
                    x: 25
                    onVisibleChanged: {
                        if (visible)
                            configsrcHeading.checked = true
                    }

                    MainConfigSecondaryBtns{
                        id: configsrcHeading
                        icon.source: prefix + "/images/Config/ConS_VehicleConfig.png"
                    }
                    MainConfigSecondaryBtns{
                        id: configsrcRoll
                        icon.source: prefix + "/images/Config/ConS_ImplementHitch.png"
                    }
                }
                MainConfigMainBtns{
                    id:uTurnMenu
                    icon.source: prefix + "/images/Config/Con_UTurnMenu.png"
                }
                MainConfigMainBtns{
                    id:modulesMenubtn
                    icon.source: prefix + "/images/Config/Con_ModulesMenu.png"
                }
                ButtonGroup {
                    buttons: [ configModulesSettings, configModulesPinsbtn,  configBlockageMonbtn]
                }
                Column{
                    id: modulesMenu
                    visible: modulesMenubtn.checked
                    height: children.height
                    x: 25
                    onVisibleChanged: {
                        if (visible)
                            configModulesSettings.checked = true
                    }

                    MainConfigSecondaryBtns{
                        id: configModulesSettings
                        icon.source: prefix + "/images/Config/ConS_ModulesMachine.png"
                    }
                    MainConfigSecondaryBtns{
                        id: configModulesPinsbtn
                        icon.source: prefix + "/images/Config/ConS_ImplementSection.png"
                    }
                    MainConfigSecondaryBtns{
                        id: configBlockageMonbtn
                        icon.source: prefix + "/images/Config/ConS_Blockage.png"
                    }
                }
                MainConfigMainBtns{
                    id:tramMenu
                    icon.source: prefix + "/images/Config/Con_TramMenu.png"
                }
                MainConfigMainBtns{
                    id:displayMenu
                    icon.source: prefix + "/images/Config/Con_Display.png"
                }
                MainConfigMainBtns{
                    id:featureMenu
                    icon.source: prefix + "/images/Config/Con_FeatureMenu.png"
                }
            }
        }

        Rectangle{
            id:bottomLine
            color: aog.borderColor
            visible: true
            width: parent.width - 16
            height:80 * theme.scaleHeight
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 8
            anchors.horizontalCenter: parent.horizontalCenter
            z: 3
            TextLine{
                id: vehicle
                text: utils.cm_unit()
                anchors.verticalCenter: parent.verticalCenter
                color: "green"
                font.pixelSize: parent.height/2
            }
            TextLine {
                id: sections
                text: utils.m_to_ft_string(settings.setVehicle_toolWidth)
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: saveAndClose.left
                anchors.rightMargin: 80
                font.pixelSize: parent.height/2
                anchors.horizontalCenter: undefined
            }

            IconButtonTransparent{
                id: saveAndClose
                anchors.right: parent.right
                width:parent.height
                height: parent.height
                icon.source: prefix + "/images/OK64.png"
                onClicked: {
                    //closeAllConfig()
                    //fix up signs on implement dimensions

                    //TODO, have to do this if rejected() also.

                    if ((utils.isTrue(settings.setTool_isToolFront) && Number(settings.setVehicle_hitchLength < 0)) ||
                            (!utils.isTrue(settings.setTool_isToolFront) && Number(settings.setVehicle_hitchLength) > 0)) {
                        //if front-mounted tool, make sure the hitchLength is positive and if rear-mounted, make sure
                        //hitchLength is negative
                        settings.setVehicle_hitchLength = -Number(settings.setVehicle_hitchLength)
                        //console.debug("corrected sign on hitchLength")
                    }

                    accept()
                }
            }
        }
        Rectangle{
            id: mainConfig
            anchors.top: topLine.bottom
            anchors.right: parent.right
            anchors.left: leftColumnView.right
            anchors.bottom: bottomLine.top
            anchors.rightMargin: 8
            color: aog.backgroundColor
            border.color: "lime"
        }
        ConfigVehiclePicker{
            id: configWhichVehicle
            anchors.fill: mainConfig
            anchors.margins: 1
            visible: true
            /*
            visible: ! tractorMenu.visible &&
                     ! implementMenu.visible &&
                     ! sourcesMenu.visible &&
                     ! uTurnMenu.visible &&
                     ! modulesMenu.visible &&
                     ! tramMenu.visible &&
                     ! configFeatures.visible
                     */
        }

        ConfigVehicleType{
            id:configTractor
            anchors.fill: mainConfig
            anchors.margins:1
            visible: tractorMenu.visible && configTrSettings.checked
        }
        ConfigVehicleAntenna{
            id:configTractorAntenna
            anchors.fill: mainConfig
            anchors.margins:1
            visible: tractorMenu.visible && btnconfigTrAntDim.checked
        }
        ConfigVehicleDimensions{
            id:configTractorDimensions
            anchors.fill: mainConfig
            anchors.margins:1
            visible: tractorMenu.visible && configTrDim.checked
        }
        ConfigImplement{
            id:configImplement
            anchors.fill: mainConfig
            anchors.margins:1
            visible: implementMenu.visible && configImpSettings.checked
        }
        ConfigImplementFrontDimensions{
            id:configImplementFrontDimensions
            anchors.fill: mainConfig
            anchors.margins:1
            visible: implementMenu.visible && configImpDim.checked && settings.setTool_isToolFront
        }
        ConfigImplementRearDimensions{
            id:configImplementRearDimensions
            anchors.fill: mainConfig
            anchors.margins:1
            visible: implementMenu.visible && configImpDim.checked && settings.setTool_isToolRearFixed
        }
        ConfigImplementTBTDimensions{
            id:configImplementTBTDimensions
            anchors.fill: mainConfig
            anchors.margins:1
            visible: implementMenu.visible && configImpDim.checked && settings.setTool_isToolTBT
        }
        ConfigImplementTrailingDimensions{
            id:configImplementTrailingDimensions
            anchors.fill: mainConfig
            anchors.margins:1
            visible: implementMenu.visible && configImpDim.checked && settings.setTool_isToolTrailing && !settings.setTool_isToolTBT
        }
        ConfigImplementAxle{
            id:configImplementAxle
            anchors.fill: mainConfig
            anchors.margins:1
            visible: implementMenu.visible && configImpAxle.checked
        }
        ConfigImplementOffset{
            id:configImplementOffset
            anchors.fill: mainConfig
            anchors.margins:1
            visible: implementMenu.visible && configImpOffset.checked
        }
        ConfigImplementSections{
            id:configImplementSections
            anchors.fill: mainConfig
            anchors.margins:1
            visible: implementMenu.visible && configImpSection.checked
        }
        ConfigImplementTiming{
            id:configImplementTiming
            anchors.fill: mainConfig
            anchors.margins:1
            visible: implementMenu.visible && configImpTiming.checked
        }
        ConfigImplementSwitches{
            id:configImplementSwitches
            anchors.fill: mainConfig
            anchors.margins:1
            visible: implementMenu.visible && configImpSwitches.checked
        }
        ConfigSources{
            id:configSources
            anchors.fill: mainConfig
            anchors.margins:1
            visible: sourcesMenu.visible && configsrcHeading.checked
        }
        ConfigSourcesRoll{
            id:configSourcesRoll
            anchors.fill: mainConfig
            anchors.margins:1
            visible: sourcesMenu.visible && configsrcRoll.checked
        }
        ConfigYouTurn{
            id:configYouTurn
            anchors.fill: mainConfig
            anchors.margins:1
            visible: uTurnMenu.checked
        }
        ConfigModules{
            id:configModules
            anchors.fill: mainConfig
            anchors.margins:1
            visible: modulesMenu.visible && configModulesSettings.checked
        }
        ConfigModulesPins{
            id:configModulesPins
            anchors.fill: mainConfig
            anchors.margins:1
            visible: modulesMenu.visible && configModulesPinsbtn.checked
        }
        ConfigModulesBlockage{  //Dim
            id:configModulesBlockage
            anchors.fill: mainConfig
            anchors.margins:1
            visible: modulesMenu.visible && configBlockageMonbtn.checked
        }
        ConfigTrams{
            id:configTrams
            anchors.fill: mainConfig
            anchors.margins:1
            visible: tramMenu.checked
        }
        ConfigDisplay{
            id: configDisplay
            anchors.fill: mainConfig
            anchors.margins: 1
            visible: displayMenu.checked
        }

        ConfigFeatures{
            id:configFeatures
            anchors.fill: mainConfig
            anchors.margins:1
            visible: featureMenu.checked
        }
    }
}
