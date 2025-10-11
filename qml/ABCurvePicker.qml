import QtQuick
import QtQuick.Controls.Fusion
import QtQuick.Layouts
import QtQuick.Dialogs
import QtQml.Models
import "components"
import ".."

Dialog {
    id: abCurvePickerDialog
    width: 600
    height: 400

    modal: true
    standardButtons: "NoButton"
    title: qsTr("AB Curve")

    // ✅ PHASE 6.0.20: CURVE SIGNALS MODERNIZED TO mainForm DIRECT CALLS
    // REMOVED 3 LEGACY SIGNALS:
    // - updateABCurves() → aog.updateCurves() direct call
    // - switchToCurve(int) → aog.switchToCurve(int) direct call
    // - deleteCurve(int) → aog.deleteCurve(int) direct call

    Connections {
        target: linesInterface
        function onAbCurvesListChanged() {
            abCurvePickerDialog.reloadModel()
        }
    }

    function reloadModel() {
        abcurveModel.clear()
        for( var i = 0; i < aogInterface.abCurvesList.length ; i++ ) {
            abcurveModel.append(aogInterface.abCurvesList[i])
        }
        if (aogInterface.currentABCurve >-1){
            abcurveView.currentIndex = aogInterface.currentABCurve
        }

    }

    onVisibleChanged:  {
        //when we show or hide the dialog, ask the main
        //program to update our lines list directly
        aog.updateCurves()  // Qt 6.8 MODERN: Direct Q_INVOKABLE call
        abcurveView.currentIndex = aogInterface.currentABCurve
        //preselect first AB line if none was in use before
        //to make it faster for user
        if (abcurveView.currentIndex < 0)
            if (aogInterface.abCurvesList.length > 0)
                abcurveView.currentIndex = 0
    }

    Rectangle{
        anchors.fill: parent
        border.width: 1
        border.color: aogInterface.blackDayWhiteNight
        color: aogInterface.borderColor
        TopLine{
            id: topLine
            titleText: qsTr("AB Curve")
        }

        ColumnLayout{
            id: rightColumn
            anchors.top: topLine.bottom
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            anchors.rightMargin: 1
            anchors.bottomMargin: 1
            width: childrenRect.width
            IconButtonTransparent{
                icon.source: prefix + "/images/FileCopy.png"
                onClicked: copyCurveName.visible = true
            }
            IconButtonTransparent{
                 onClicked: editCurveName.visible = true
               icon.source: prefix + "/images/FileEditName.png"
            }
            IconButtonTransparent{
                icon.source: prefix + "/images/ABSwapPoints.png"
            }
            IconButtonTransparent{
                icon.source: prefix + "/images/OK64.png"
                onClicked: {
                    if (abcurveView.currentIndex > -1) {
                        TracksInterface.select(abcurveView.currentIndex)  // Qt 6.8: Use TracksInterface.select()
                        abCurvePickerDialog.accept()
                    } else
                        abCurvePickerDialog.reject()
                }
            }
        }

        Rectangle{
            id: bottomRow
            anchors.bottom: parent.bottom
            anchors.right: rightColumn.left
            anchors.left: parent.left
            anchors.bottomMargin: 1
            anchors.leftMargin: 1
            height: btnLineDelete.height + 10
            color: parent.color
            z: 1


            RowLayout{
                anchors.fill: parent

                IconButtonTransparent{
                    id: btnLineDelete
                    icon.source: prefix + "/images/ABLineDelete.png"
                }
                IconButtonTransparent{
                    icon.source: prefix + "/images/SwitchOff.png"
                    onClicked: {
                        TracksInterface.select(-1)  // Qt 6.8: Deselect track
                        abcurveView.currentIndex = -1
                        abCurvePickerDialog.reject()
                    }
                }
                IconButtonTransparent{
                    icon.source: prefix + "/images/AddNew.png"
                    onClicked: abSetter.visible = true
                }
                IconButtonTransparent{
                    icon.source: prefix + "/images/BoundaryLoadFromGE.png"
                }
            }
        }
        Rectangle{ //don't ask any questions
            anchors.top: topLine.bottom
            height: 10
            color: parent.color
            width: abCurvePickerDialog.width
            anchors.left: abCurvePickerDialog.left
            z: 1
        }
        Rectangle{
            id: abSetter
            anchors.left: parent.left
            anchors.top: parent.top
            width: 300
            height: 400
            color: aogInterface.backgroundColor
            border.width: 1
            border.color: aogInterface.blackDayWhiteNight
            z: 1
            visible: false
            TopLine{
                id: settertopLine
                titleText: "AB Curve"
            }
            IconButtonTransparent{
                id: a
                anchors.top: settertopLine.bottom
                anchors.left: parent.left
                anchors.margins: 5
                isChecked: false
                icon.source: prefix + "/images/LetterABlue.png"
            }
            IconButtonTransparent{
                anchors.top: settertopLine.bottom
                anchors.right: parent.right
                anchors.margins: 5
                isChecked: true
                onClicked: newLineName.visible = true
                icon.source: prefix + "/images/LetterBBlue.png"
            }
            IconButtonTransparent{
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                anchors.margins: 20
                icon.source: prefix + "/images/boundaryPause.png"
            }
            IconButtonTransparent{
                anchors.bottom: parent.bottom
                anchors.right: parent.right
                anchors.margins: 20
                icon.source: prefix + "/images/Cancel64.png"
                onClicked:{
                    parent.visible = false
                }
            }
            LineName{
                id: newLineName
                anchors.top: parent.top
                anchors.left: parent.left
                title: "AB Curve"
                visible: false
            }
        }

        LineName{
            id: copyCurveName
            anchors.top:parent.top
            anchors.left: parent.left
            title: "AB Curve"
            visible: false
            z: 2
        }
        LineName{
            id: editCurveName
            anchors.top:parent.top
            anchors.left: parent.left
            title: "AB Curve"
            visible: false
            z: 1
        }

        Rectangle{
            id: picker
            anchors.left: parent.left
            anchors.top:topLine.bottom
            anchors.right: rightColumn.left
            anchors.bottom: bottomRow.top
            anchors.bottomMargin: 0
            anchors.margins: 10
            color:aogInterface.borderColor

            ListModel {
                id: abcurveModel
            }

            Component.onCompleted: {
                reloadModel()
            }

            ListView {
                id: abcurveView
                anchors.fill: parent
                model: abcurveModel
                //property int currentIndex: -1

                delegate: RadioButton{
                    id: control
                    checked: abcurveView.currentIndex === index ? true : false
                    indicator: Rectangle{
                        anchors.fill: parent
                        anchors.margins: 2
                        color: control.down ? aogInterface.backgroundColor : "blue"
                        visible: control.checked
                    }
                    onDownChanged: {
                        abcurveView.currentIndex = index
                    }


                    width:parent.width
                    height:50
                    //anchors.fill: parent
                    //color: "light gray"
                    Text{
                        anchors.left: parent.left
                        anchors.leftMargin: 5
                        anchors.verticalCenter: parent.verticalCenter
                        text: model.name
                        font.pixelSize: 25
                        font.bold: true
                        color: control.checked ? aogInterface.backgroundColor : aogInterface.blackDayWhiteNight
                        z: 2
                    }
                }
            }
        }
    }
}
