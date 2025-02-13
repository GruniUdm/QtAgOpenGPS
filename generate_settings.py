#!/usr/bin/python3

import sys
import csv

def generate_addkeys(reader):
    print ("""#include "newsettings.h"

void NewSettings::setupKeys() {
""")
    for row in reader:
        if row["special_case"]:
            print('    addKey("%s/%s", %s, NewSettings::%s);' % (row['section'],row['key'], row['default'], row['special_case']))
        else:
            print('    addKey("%s/%s", %s);' % (row['section'],row['key'], row['default']))


    print ("}")
    print ()

def generate_header(reader):
    print ("#ifndef __SETTINGS_KEYS_H__")
    print ("#define __SETTINGS_KEYS_H__")

    for row in reader:
        print('#define SETTINGS_%s_%s "%s/%s"' % (row['section'], row['key'], row['section'], row['key']))

    print ("#endif")

def color_to_hex(rgblist):
    color_str = '"#'
    for c in rgblist:
        color_str += "%02x" % int(c)

    return color_str + '"'

def generate_mockqml(reader):
    print ()
    print ("//This is a mock Settings singleton for use in qml viewer or QtDS")
    print ("//This mirrors the structure of the real Settings.Settings used")
    print ("//in the compiled binary.")
    print ()
    print ("pragma singleton")
    print ()
    print ("Item {")

    for row in reader:
        if row['special_case'] == 'VECTOR_OF_INTS':
            qml_type = 'var'
            if row['key'] == 'pinConfig':
                row['default'] = '[ 1,2,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 ]'
            else:
                row['default'] = '[ 2,10,20,0,0,0,0,0,0 ]'
        else:
            qml_type = row['cpp_type']
            if qml_type == 'QString': 
                qml_type = 'string'
                if row['default'][:7].lower() == 'qstring':
                    row['default'] =  row['default'][8:-1]
            elif qml_type == 'QPoint': 
                qml_type = 'point'
                if row['default'][:2].lower() == 'qp':
                    row['default'] = 'Qt.p' +  row['default'][2:]
            elif qml_type == 'QRect': 
                qml_type = 'rect'
                if row['default'][:2].lower() == 'qr':
                    row['default'] = 'Qt.r' +  row['default'][2:]
            elif qml_type == 'QSize':
                qml_type = 'size'
                if row['default'][:2].lower() == 'qs':
                    row['default'] = 'Qt.s' +  row['default'][2:]
            elif qml_type == 'QColor':
                qml_type = 'color'
                if row['default'][:16].lower() == 'qcolor::fromrgb(':
                    color = row['default'][16:-1]
                    if ',' in color:
                        color = [int(c.strip()) for c in color.split(',')]
                        color = color_to_hex(color)
                    row['default'] = color
                if row['default'][:7].lower() == 'qcolor(':
                    color = row['default'][7:-1]
                    if ',' in color:
                        color = [int(c.strip()) for c in color.split(',')]
                        color = color_to_hex(color)
                    row['default'] = color
                    
                    




        print("	property %s %s_%s: %s" % (qml_type, row['section'], row['key'], row['default']))


    print ("}")

if __name__ == "__main__":
    import argparse

    argparser = argparse.ArgumentParser(prog = sys.argv[0],
                                        description = 'Parse settings.csv and generate newsettings_addkeys.cpp and Settings.qml for the mock AOG module for qml viewer')
    argparser.add_argument('-a', '--addkeys', action = 'store_true', help = 'Generate the newsettings_addkeys.cpp file')
    argparser.add_argument('-m', '--mockqml', action = 'store_true', help = 'Generate the mock Settings.qml file')
    argparser.add_argument('-c', '--header', action = 'store_true', help = 'Generate convenience macro header file')
    argparser.add_argument('csv_file', help = 'path to settings.csv file.')

    args = argparser.parse_args()

    with open(args.csv_file,'r') as csv_file:
        reader = csv.DictReader(csv_file, quotechar="'")

        print ("// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team")
        print ("// SPDX-License-Identifier: GNU General Public License v3.0 or later")
        print ("//")

        if (args.addkeys):
            generate_addkeys(reader)
        elif (args.header):
            generate_header(reader)
        else:
            generate_mockqml(reader)

