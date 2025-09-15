import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
//import Settings
//import AOG
// Interface import removed - TracksInterface now QML_SINGLETON
import "components" as Comp

RowLayout{
    property bool hydLiftIsOn: btnHydLift.isOn
    id:bottomButtons
    visible: aog.isJobStarted && leftColumn.visible

    onWidthChanged: {
        theme.btnSizes[1] = width / (children.length)
        theme.buttonSizesChanged()
    }
    onVisibleChanged: {
        if (visible === false)
            height = 0
        else
            height = children.height
        cbYouSkipNumber.currentIndex = Settings.youturn_skipWidth-1

    }
    ComboBox {
        id: cbYouSkipNumber
        editable: false
        Layout.alignment: Qt.AlignCenter
        implicitWidth: theme.buttonSize
        implicitHeight: theme.buttonSize
        model: ListModel {
            id: model
            ListElement {text: "0"}
            ListElement {text: "1"}
            ListElement {text: "2"}
            ListElement {text: "3"}
            ListElement {text: "4"}
            ListElement {text: "5"}
            ListElement {text: "6"}
            ListElement {text: "7"}
            ListElement {text: "8"}
            ListElement {text: "9"}
            ListElement {text: "10"}
        }
        onCurrentIndexChanged: {
            if (cbYouSkipNumber.find(currentText) === -1){
                model.append({text: editText})
                currentIndex = cbYouSkipNumber.find(editText)
            }
            Settings.youturn_skipWidth = cbYouSkipNumber.currentIndex+1
        }

    }
    Comp.IconButton3State {
        id: btnYouSkip // the "Fancy Skip" button
        //property bool isOn: false
        //isChecked: isOn
        //checkable: true
        icon1: prefix + "/images/YouSkipOff.png"
        icon2: prefix + "/images/YouSkipOn.png"
        icon3: prefix + "/images/YouSkipOn2.png"
        icon4: prefix + "/images/YouSkipOn2.png"
        //iconChecked: prefix + "/images/YouSkipOn.png"
        //buttonText: qsTr("YouSkips")
        onClicked:
         {
            //isOn = !isOn
            aog.isYouSkipOn()
        }
    }
    Comp.MainWindowBtns { //reset trailing tool to straight back
        id: btnResetTool
        icon.source: prefix + "/images/ResetTool.png"
        buttonText: qsTr("Reset Tool")
        onClicked: aog.btnResetTool()
        visible: Settings.tool_isToolTrailing === true //hide if front or rear 3 pt
    }
    Comp.MainWindowBtns {
        id: btnSectionMapping
        icon.source: prefix + "/images/SectionMapping.png"
        onClicked: cpSectionColor.open()
    }
    Comp.MainWindowBtns {
        id: btnTramLines
        icon.source: prefix + "/images/TramLines.png"
        buttonText: qsTr("Tram Lines")
        Layout.alignment: Qt.AlignCenter
        implicitWidth: theme.buttonSize
        implicitHeight: theme.buttonSize
        visible: Settings.feature_isTramOn
    }
    Comp.MainWindowBtns {
        property bool isOn: false
        id: btnHydLift
        isChecked: isOn
        checkable: true
        disabled: btnHeadland.checked
        visible: Utils.isTrue(Settings.ardMac_isHydEnabled) && btnHeadland.visible
        icon.source: prefix + "/images/HydraulicLiftOff.png"
        iconChecked: prefix + "/images/HydraulicLiftOn.png"
        buttonText: qsTr("HydLift")
        onClicked: {
            isOn = !isOn
            aog.isHydLiftOn(isOn)
        }
    }
    Comp.MainWindowBtns {
        id: btnHeadland
        isChecked: aog.isHeadlandOn
        checkable: true
        icon.source: prefix + "/images/HeadlandOff.png"
        iconChecked: prefix + "/images/HeadlandOn.png"
        buttonText: qsTr("Headland")
        onClicked: aog.btnHeadland()
    }
    Comp.MainWindowBtns {
        id: btnFlag
        objectName: "btnFlag"
        isChecked: false
        icon.source: prefix + contextFlag.icon
        onClicked: {
            flags.show();
            aog.btnFlag();
        }
        onPressAndHold: {
            if (contextFlag.visible) {
                contextFlag.visible = false;
            } else {
                contextFlag.visible = true;
            }
        }
        buttonText: qsTr("Flag")
    }

    Comp.MainWindowBtns{
        icon.source: prefix + "/images/SnapLeft.png"
        onClicked: TracksInterface.nudge(Settings.as_snapDistance/-100) //spinbox returns cm, convert to metres
        visible: Settings.feature_isNudgeOn && TracksInterface.idx > -1
    }
    Comp.MainWindowBtns{
        icon.source: prefix + "/images/SnapToPivot.png"
        onClicked: TracksInterface.nudge_center()
        visible: Settings.feature_isNudgeOn && TracksInterface.idx > -1
    }
    Comp.MainWindowBtns{
        icon.source: prefix + "/images/SnapRight.png"
        onClicked: TracksInterface.nudge(Settings.as_snapDistance/100) //spinbox returns cm, convert to metres
        visible: Settings.feature_isNudgeOn && TracksInterface.idx > -1
    }

    Comp.MainWindowBtns {
        property bool isOn: false
        id: btnTrack
        icon.source: prefix + "/images/TrackOn.png"
        iconChecked: prefix + "/images/TrackOn.png"
        buttonText: qsTr("Track")
        //onClicked: trackButtons.visible = !trackButtons.visible
        onClicked: {if (isOn == false && TracksInterface.count > 0) {
                        TracksInterface.select(0);
                        btnTrack.isChecked = false;
                        isOn = true;
                    }
                    else {
                        trackButtons.visible = !trackButtons.visible;
                        }
                    }
    }

}
