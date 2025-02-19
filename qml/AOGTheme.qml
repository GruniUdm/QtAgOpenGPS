// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// UI  colors and sounds.
import QtQuick
import QtQuick.Controls.Fusion
import QtMultimedia
import Settings

/* This type contains the sounds, colors, and perhaps screen sizes,
  *Sounds will track AOGInterface.qml, and react when needed as set
  by the features page.
  *Colors will follow the settings files, and change based on day/night
  mode, so we don't need an if statement in every object.
  *Screen sizes-We'll see.
  */

Item {
    id: aogTheme

    property int defaultHeight: 768
    property int defaultWidth: 1024
    property double scaleHeight: mainWindow.height / defaultHeight
    property double scaleWidth: mainWindow.width / defaultWidth

    property color backgroundColor: "ghostWhite"
    property color textColor: "black"
    property color borderColor: "lightblue"
    property color blackDayWhiteNight: "black"
    property var btnSizes: [100, 100, 100]//clockwise, right bottom left
    property int buttonSize: 100
    function buttonSizesChanged() {
        buttonSize = Math.min(...btnSizes) - 2.5
        //console.log("Button size is now " + buttonSize)
    }
    property color whiteDayBlackNight: "white"


    //curr / default

    Connections{
        target: mainWindow
        function onHeightChanged(){
            scaleHeight = mainWindow.height / defaultHeight
        }
        function onWidthChanged(){
            scaleWidth = mainWindow.width / defaultWidth
        }
    }
    Connections{
        target: Settings
        function onDisplay_isDayModeChanged(){
            if (Settings.display_isDayMode){
                backgroundColor = "ghostWhite"
                textColor = "black"
                borderColor = "lightBlue"
                blackDayWhiteNight = "black"
                whiteDayBlackNight = "white"
            }
            else{
                backgroundColor = "darkgray"
                textColor = "white"
                borderColor= "lightGray"
                blackDayWhiteNight = "white"
                whiteDayBlackNight = "black"
            }
        }
    }
    Item {//button sizes
        width: 600
        enum ScreenSize {
            Phone, // 6" or less
            SmallTablet, //6-10"
            LargeTablet, //10" or larger
            Large //regular computer screen.
        }

        /*	property int screenDiag: Math.sqrt(Screen.width * Screen.width + Screen.height * Screen.height) / Screen.pixelDensity
    property int screenType: screenDiag < 165 ? Sizes.ScreenSize.Phone :
                             screenDiag < 230 ? Sizes.ScreenSize.SmallTablet :
                             screenDiag < 355 ? Sizes.ScreenSize.LargeTablet : Sizes.ScreenSize.Large

    property int buttonSquare: screenType == Sizes.ScreenSize.Phone ? 10 * Screen.pixelDensity :
                               screenType == Sizes.ScreenSize.SmallTablet ? 20 * Screen.pixelDensity :
                               screenType == Sizes.ScreenSize.LargeTablet ? 25 * Screen.pixelDensity : Screen.height / 12*/


    }
    Connections{//sounds functions go here.
        target: aog
        function onIsBtnAutoSteerOnChanged() {//will need another function for every sound option
            if(Settings.sound_isAutoSteerOn){//does the user want the sound on?
                if(aog.isBtnAutoSteerOn)
                    engage.play()
                else
                    disEngage.play()
            }
        }
        function onHydLiftDownChanged(){
            if(Settings.sound_isHydLiftOn){
                if(aog.HydLiftDown)
                    hydDown.play()
                else
                    hydUp.play()
            }
        }
        function onAutoBtnStateChanged(){
            if(Settings.sound_isSectionsOn)
                sectionOn.play()
        }
        function onManualBtnStateChanged(){
            if(Settings.sound_isSectionsOn)
                sectionOff.play()
        }
        function onAgeChanged(){
            if(aog.age > Settings.gps_ageAlarm)
                if(Settings.gps_isRTK)
                    rtkLost.play()
        }
        function onDistancePivotToTurnLineChanged(){
            if(aog.distancePivotToTurnLine == 20)
                if(Settings.sound_isUturnOn)
                    approachingYouTurn.play()
        }
    }
    //region sounds
    //as far as I can tell, these are all necessary
    SoundEffect{
        id: engage
        source: prefix + "/sounds/SteerOn.wav"
    }
    SoundEffect{
        id: disEngage
        source: prefix + "/sounds/SteerOff.wav"
    }
    SoundEffect{
        id: hydDown
        source: prefix + "/sounds/HydDown.wav"
    }
    SoundEffect{
        id: hydUp
        source: prefix + "/sounds/HydUp.wav"
    }
    SoundEffect{
        id: sectionOff
        source: prefix + "/sounds/SectionOff.wav"
    }
    SoundEffect{
        id: sectionOn
        source: prefix + "/sounds/SectionOn.wav"
    }
    SoundEffect{
        id: approachingYouTurn
        source: prefix + "/sounds/Alarm10.wav"
    }
    SoundEffect{
        id: rtkLost
        source: prefix + "/sounds/rtk_lost.wav"
    }
    SoundEffect{
        id: youturnFail
        source: prefix + "/sounds/TF012.wav"
    }//endregion sounds
}
