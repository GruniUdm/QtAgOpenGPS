import QtQuick 2.9
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3
import Qt.labs.folderlistmodel 2.2
import QtQml.Models 2.3

Item {
    width: 600
    height: 400
    Rectangle{
        anchors.fill: parent
        border.width: 1
        border.color: "black"
        color: "lightgray"
        TopLine{
            id: topLine
            titleText: "AB Line"
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
                objectName: "btnLineCopy"
                icon.source: "/images/FileCopy.png"
                onClicked: copyLineName.visible = true
            }
            IconButtonTransparent{
                objectName: "btnLineEdit"
                icon.source: "/images/FileEditName.png"
                onClicked: editLineName.visible = true
            }
            IconButtonTransparent{
                objectName: "btnLineSwapPoints"
                icon.source: "/images/ABSwapPoints.png"
            }
            IconButtonTransparent{
                objectName: "btnLineExit"
                icon.source: "/images/OK64.png"
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
                    objectName: "btnLineDelete"
                    icon.source: "/images/ABLineDelete.png"
                }
                IconButtonTransparent{
                    objectName: "btnLineExit"
                    icon.source: "/images/SwitchOff.png"
                    onClicked: abLinePicker.visible = false
                }
                IconButtonTransparent{
                    objectName: "btnLineAdd"
                    icon.source: "/images/AddNew.png"
                    onClicked: abSetter.visible = true
                }
                IconButtonTransparent{
                    objectName: "btnLineLoadFromKML"
                    icon.source: "/images/BoundaryLoadFromGE.png"
                }
            }
        }
        Rectangle{ //don't ask any questions
            anchors.top: topLine.bottom
            height: 10
            color: parent.color
            width: picker.width
            anchors.left: picker.left
            z: 1
        }
        Rectangle{
            id: abSetter
            anchors.left: parent.left
            anchors.top: parent.top
            width: 300
            height: 400
            color: "lightgray"
            border.width: 1
            border.color: "black"
            z: 1
            visible: false
            TopLine{
                id: settertopLine
                titleText: "AB Line"
            }
            IconButtonTransparent{
                objectName: "a"
                id: a
                anchors.top: settertopLine.bottom
                anchors.left: parent.left
                anchors.margins: 5
                isChecked: false
                icon.source: "/images/LetterABlue.png"
            }
            IconButtonTransparent{
                objectName: "b"
                anchors.top: settertopLine.bottom
                anchors.right: parent.right
                anchors.margins: 5
                isChecked: true
                icon.source: "/images/LetterBBlue.png"
            }
//            Rectangle{
//                id: headingTextInput
//                anchors.topMargin: 20
//                anchors.top: a.bottom
//                height: 50
//                width: parent.width -50
//                anchors.horizontalCenter: parent.horizontalCenter
//                color: "white"
//                border.color: "gray"
//                border.width: 1
//                TextInput {
//                    objectName: "heading"
//                    anchors.fill: parent
//                    validator: RegExpValidator {
//                        regExp: /(\d{1,3})([.,]\d{1,5})?$/
//                    }
//                }
//            }
            SpinBox{
                id: headingTextInput
                objectName: "heading"
                from: 0
                to: 359999999
                stepSize: 1000000
                value: 0
                editable: true
                property real realValue: value/ 1000000
                property int decimals: 6

                anchors.topMargin: 20
                anchors.top: a.bottom
                width: parent.width -50
                anchors.horizontalCenter: parent.horizontalCenter
                onValueChanged: {
                    if (value == from) {
                        spin_message.visible = true
                        spin_message.text = "Must be "+from/1000000+" or greater"
                    } else if(value == to){
                        spin_message.visible = true
                        spin_message.text = "Can't be larger than " + to/ 1000000
                    }else {
                        spin_message.visible = false
                    }

                    //some validation here
                    //emit signal.  We know our section number because it's in the model
                }
                textFromValue: function(value, locale) {
                    return Number(value / 1000000).toLocaleString(locale, 'f', decimals)
                }

                valueFromText: function(text, locale) {
                    return Number.fromLocaleString(locale, text) * 1000000
                }
                Text {
                    id: spin_message
                    visible: false
                    text: "message"
                    color: "red"
                    anchors.top: parent.bottom
                    anchors.left: parent.left
                }
            }

            IconButtonTransparent{
               id: fancyEditor
               anchors.top: headingTextInput.bottom
               anchors.topMargin: 20
               anchors.horizontalCenter: parent.horizontalCenter
               icon.source: "/images/FileEditName.png"
           }

           IconButtonTransparent{
               objectName: "btnCancel"
               anchors.bottom: parent.bottom
               anchors.left: parent.left
               anchors.margins: 20
               icon.source: "/images/Cancel64.png"
               onClicked:{
                   parent.visible = false
               }
           }
           IconButtonTransparent{
               objectName: "btnOk"
               anchors.bottom: parent.bottom
               anchors.right: parent.right
               anchors.margins: 20
               icon.source: "/images/OK64.png"
               onClicked: newLineName.visible = true
           }
           LineName{
               id: newLineName
               objectName: "newLineName"
               anchors.top: parent.top
               anchors.left: parent.left
               title: "AB Line"
               visible: false
           }
        }

        LineName{
            id: copyLineName
            objectName: "copyLineName"
            anchors.top:parent.top
            anchors.left: parent.left
            title: "AB Line"
            visible: false
            z: 2
        }
        LineName{
            id: editLineName
            objectName: "editLineName"
            anchors.top:parent.top
            anchors.left: parent.left
            title: "AB Line"
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
            color: "white"
            ListView {
                anchors.fill: parent
                property Component mycomponent: fileName
                model: FolderListModel{
                    id: fieldList
                    folder: "file:/home/davidwedel/Documents/QtAgOpenGPS/Fields/ex"

                }

                delegate: RadioButton{
                    id: control
                    indicator: Rectangle{
                        anchors.fill: parent
                        anchors.margins: 2
                        color: control.down ? "white" : "blue"
                        visible: control.checked
                    }

                    width:parent.width
                    height:50
                    //anchors.fill: parent
                    //color: "light gray"
                    Text{
                        anchors.left: parent.left
                        anchors.leftMargin: 5
                        anchors.verticalCenter: parent.verticalCenter
                        text: fileName
                        font.pixelSize: 25
                        font.bold: true
                        color: control.checked ? "white" : "black"
                        z: 2
                    }
                }
            }
        }
    }
}
