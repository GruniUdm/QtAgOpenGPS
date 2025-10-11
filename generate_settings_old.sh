#!/bin/bash

echo 'Regenerating settings files.'
./generate_settings.py -a settings.csv > newsettings_addkeys.cpp
echo 'newsettings_addkeys.cpp'
./generate_settings.py -c settings.csv > settings_defines.h
echo 'settings_defines.h'
./generate_settings.py -m settings.csv > qml/Settings/MockSettings.qml
echo 'qml/Settings/MockSettings.qml'

