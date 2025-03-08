import QtQuick
import AgIO 1.0

Shortcut{
	id: shortcut
	property int hotkeys: 0
    property var hotkeysVar: agiosettings.setKey_hotkeys
	sequences: [hotkeysVar[hotkeys], ("Shift+"+ hotkeysVar[hotkeys])]
}
