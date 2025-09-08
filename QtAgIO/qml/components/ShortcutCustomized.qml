import QtQuick
import AgIO

Shortcut{
	id: shortcut
	property int hotkeys: 0
    property var hotkeysVar: agiosettings.setKey_hotkeys
	sequences: [hotkeysVar[hotkeys], ("Shift+"+ hotkeysVar[hotkeys])]
}
