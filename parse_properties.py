#!/usr/bin/python3

import bs4 #beautifulSoup
import sys
from props import props

# any settings that are specific to QtAOG and not from parsed from
# AOG should be added to this list structure.  Settings for Dima's
# blockage monitor should go here, for example.  Any settings not
# in this list will get whenever the properties are re-synced with AOG

add_props = [
    { 'ini_path':      'display/showBack',
      'cpp_name':      'property_displayShowBack',
      'cpp_type':  'bool',
      'cpp_default': 'false',
    },

    { 'ini_path': 'display/antiAliasSamples',
      'cpp_name': 'property_displayAntiAliasSamples',
      'cpp_type': 'bool',
      'cpp_default': '0',
    },

    { 'ini_path': 'display/useTrackZero',               # this is the path inside the ini file
      'cpp_name': 'property_setDisplay_useTrackZero',   # cpp global symbol to access setting
      'cpp_default': 'false',                           # default cpp value of setting; should be valid C++ expression
      'cpp_type' : 'bool',                              # cpp value type
      'qml_name': 'setDisplay_useTrackZero',            # qml property name, accessed via the global settings object
      'qml_type': 'bool',                               # qml javascript type.  bool, color, point, rect, string, etc
      'qml_default' : 'false',                          # qml property default value; should be valid javascript expression
    },

    { 'ini_path': 'display/topTrackNum',
      'cpp_name': 'property_setDisplay_topTrackNum',
      'cpp_default' : 'false',
      'cpp_type' : 'bool',
      'qml_name': 'setDisplay_topTrackNum',
      'qml_type': 'bool',
      'qml_default': 'false',
    },
    
    { 'ini_path': 'display/colorDayBackground',
      'cpp_name': 'property_setDisplay_colorDayBackground',
      'cpp_default' : 'QColor::fromRgb(245, 245, 245)',
      'qml_name' : 'setDisplay_colorDayBackground',
      'qml_default' : "#f5f5f5"
    },
    
    { 'ini_path': 'display/colorNightBackground',
      'cpp_name': 'property_setDisplay_colorNightBackground',
      'cpp_default' : 'QColor::fromRgb(50, 50, 65)',
      'qml_name' : 'setDisplay_colorNightBackground',
      'qml_default' : "#323241"
    },
    
    { 'ini_path': 'display/colorDayBorder',
      'cpp_name': 'property_setDisplay_colorDayBorder',
      'cpp_default' : 'QColor::fromRgb(215, 228, 242)',
      'qml_name' : 'setDisplay_colorDayBorder',
      'qml_default' : "#d7e4f2"
    },
    
    { 'ini_path': 'display/colorNightBorder',
      'cpp_name': 'property_setDisplay_colorNightBorder',
      'cpp_default' : 'QColor::fromRgb(210, 210, 230)',
      'qml_name' : 'setDisplay_colorNightBorder',
      'qml_default' : "#d2d2e6"
    },

    { 'ini_path': 'seed/blockCountMin',
      'cpp_name': 'property_setSeed_blockCountMin',
      'cpp_default' : '100',
      'cpp_type' : 'double',
      'qml_name' : 'setSeed_blockCountMin',
      'qml_type': 'double',
      'qml_default': '100',
    },

    { 'ini_path': 'seed/blockCountMax',
      'cpp_name': 'property_setSeed_blockCountMax',
      'cpp_default' : '1000',
      'cpp_type' : 'double',
      'qml_name' : 'setSeed_blockCountMax',
      'qml_type': 'double',
      'qml_default': '1000',
    },

    { 'ini_path': 'seed/blockRow1',
      'cpp_name': 'property_setSeed_blockRow1',
      'cpp_default' : '16',
      'cpp_type' : 'double',
      'qml_name' : 'setSeed_blockRow1',
      'qml_type': 'double',
      'qml_default': '16',
    },

    { 'ini_path': 'seed/blockRow2',
      'cpp_name': 'property_setSeed_blockRow2',
      'cpp_default' : '16',
      'cpp_type' : 'double',
      'qml_name' : 'setSeed_blockRow2',
      'qml_type': 'double',
      'qml_default': '16',
    },

    { 'ini_path': 'seed/blockRow3',
      'cpp_name': 'property_setSeed_blockRow3',
      'cpp_default' : '16',
      'cpp_type' : 'double',
      'qml_name' : 'setSeed_blockRow3',
      'qml_type': 'double',
      'qml_default': '16',
    },

    { 'ini_path': 'seed/blockRow4',
      'cpp_name': 'property_setSeed_blockRow4',
      'cpp_default' : '0',
      'cpp_type' : 'double',
      'qml_name' : 'setSeed_blockRow4',
      'qml_type': 'double',
      'qml_default': '0',
    },

    { 'ini_path': 'seed/numRows',
      'cpp_name': 'property_setSeed_numRows',
      'cpp_default' : '32',
      'cpp_type' : 'double',
      'qml_name' : 'setSeed_numRows',
      'qml_type': 'double',
      'qml_default': '32',
    },

    { 'ini_path': 'seed/blockageIsOn',
      'cpp_name': 'property_setSeed_blockageIsOn',
      'cpp_default' : 'false',
      'cpp_type' : 'bool',
      'qml_name' : 'setSeed_blockageIsOn',
      'qml_type': 'bool',
      'qml_default': 'false',
     },

    { 'ini_path': 'vehicle/vehicleName',
      'cpp_name': 'property_setVehicle_vehicleName',
      'cpp_default' : 'QString("Default Vehicle")',
      'qml_name' : 'setVehicle_vehicleName',
      'qml_default' : "Default Vehicle"
    },

    { 'ini_path': 'cam/camLink',
      'cpp_name': 'property_setCam_CamLink',
      'cpp_default' : 'QString("rtsp://192.168.0.138:1945")',
      'qml_name' : 'setCam_CamLink',
      'qml_default' : "rtsp://192.168.0.138:1945"
    },
]

def parse_settings(file):
    cpp = []
    qml_cpp = []
    mock_qml = []
    h = []
    preamble = ['#include "aogproperty.h"','',
                '//Generated by parse_properties.py','']

    with file:
        parser = bs4.BeautifulSoup(file.read(),'lxml-xml')

        settings = parser.findAll('Setting')
        for s in settings:
            s['Name'] = s['Name'][0].lower() + s['Name'][1:]
            t = s['Type']
            n = s['Name']
            #print (t)
            if t == 'setFeatures':
                #we'll parse these separately
                pass 
            if t == 'System.Int32' or \
               t == 'System.Double' or \
               t == 'System.Decimal' or \
               t == 'System.Byte':

                default_value = s.Value.contents[0]

                #special case for a bad default value in Settings.settings
                if n == 'setVehicle_tankTrailingHitchLength' and default_value[0] != '-':
                    default_value = "-" + default_value

                if t == 'System.Int32' or t == 'system.Byte':
                    mock_qml.append("property int %s: %s" % (n, default_value))
                    qt = 'int'
                else:
                    mock_qml.append("property double %s: %s" % (n, default_value))
                    qt = 'double'

            elif t == 'System.Boolean':
                default_value = s.Value.contents[0].lower()
                mock_qml.append("property bool %s: %s" % (n, default_value))
                qt = 'bool'

            elif t == 'System.String' and n == 'setTool_zones':
                preamble.append('QVector<int> default_zones = { ' + s.Value.contents[0] + ' };')
                mock_qml.append("property var %s: [ %s ]" % (n, s.Value.contents[0]))
                default_value = 'default_zones'
                qt = 'leavealone'
           
            elif t == 'System.String' and n == 'setRelay_pinConfig':
                preamble.append('QVector<int> default_relay_pinConfig = { ' + s.Value.contents[0] + ' };')
                mock_qml.append("property var %s: [ %s ]" % (n, s.Value.contents[0]))
                default_value = 'default_relay_pinConfig'
                qt = 'leavealone'

            elif t == 'System.String' and n == 'setBlockageConfig':
                preamble.append('QVector<int> default_BlockageConfig = { ' + s.Value.contents[0] + ' };')
                mock_qml.append("property var %s: [ %s ]" % (n, s.Value.contents[0]))
                default_value = 'default_BlockageConfig'
                qt = 'leavealone'

            elif t == 'System.String' or \
                 t == 'AgOpenGPS.TBrand' or \
                 t == 'AgOpenGPS.HBrand' or \
                 t == 'AgOpenGPS.WDBrand' or \
                 t == 'AgOpenGPS.CFeatureSettings':
                c = s.Value.contents
                if c:
                    default_value = '"' + s.Value.contents[0] + '"'
                else:
                    default_value = '""'
                mock_qml.append("property string %s: %s" % (n, default_value))
                qt = 'QString'

            elif t == 'System.Drawing.Point':
                default_value = 'QPoint(%s)' % s.Value.contents[0]
                mock_qml.append("property point %s: \"%s\"" % (n, s.Value.contents[0]))
                qt = 'QPoint'

            elif t == 'System.Drawing.Color':
                fields = s.Value.contents[0].split(',')
                if len(fields) > 1:
                    default_value = 'QColor(%s)' % s.Value.contents[0]
                else:
                    default_value = 'QColor("%s")' % s.Value.contents[0]

                if "," in s.Value.contents[0]:
                    values = s.Value.contents[0].split(',')
                    colorstring = "#%02x%02x%02x" % (int(values[0]), int(values[1]), int(values[2]))
                else:
                    colorstring = s.Value.contents[0]
                mock_qml.append("property string %s: \"%s\"" % (n, colorstring))
                qt = 'QColor'

            else:
                if s.Value.contents:
                    default_value = '"' + s.Value.contents[0] + '"'
                else:
                    default_value = '""'
                mock_qml.append("property string %s: %s" % (n, default_value))
                qt = "QString"


            if s['Name'] in props:
                qs_name = props[s['Name']]
            else:
                props[s['Name']] = ''
                qs_name = ""

            if not qs_name:
                sys.stderr.write ("Warning! No ini path found for %s. Generate props.py and fix.\n" % s['Name'])

            cpp.append('AOGProperty property_%s("%s",%s);'% (s['Name'], qs_name, default_value))
            qml_cpp.append('    addKey(QString("%s"),QString("%s"),"%s");' % (s['Name'], qs_name, qt));
            h.append('extern AOGProperty property_%s;' % s['Name'])

            #preamble.extend(cpp)

    return (preamble, cpp, h, qml_cpp, mock_qml)

def parse_csettings(file):
    cpp = []
    qml_cpp = []
    h = []
    mock_qml = []

    with file:
        for line in file.readlines():
            if 'public bool' in line and 'is' in line:
                line = line.strip()[12:]
                line = line.split(';')[0]
                parts = [x.strip() for x in line.split('=')]
                name = 'setFeature_%s' % parts[0]

                if name in props:
                    qs_name = props[name]
                else:
                    props[name] = 'displayFeatures/%s' % parts[0]
                    qs_name = 'displayFeatures/%s' % parts[0]
                cpp.append('AOGProperty property_%s("%s",%s);'% (name, qs_name, parts[1]))
                qml_cpp.append('    addKey(QString("%s"),QString("%s"), "bool");' % (name, qs_name));
                h.append('extern AOGProperty property_%s;' % name)
                mock_qml.append('property bool %s: %s' % (name, parts[1]))

    return ([], cpp, h, qml_cpp, mock_qml)
                



if __name__ == '__main__':
    import argparse

    argparser = argparse.ArgumentParser(prog = sys.argv[0],
                                       description='Parse C# .settings file to create c++ declarations for AOGProperty')
    argparser.add_argument('-c','--cpp', action = "store_true", help = 'Output code for cpp file')
    argparser.add_argument('-q','--qmlcpp', action = "store_true", help = 'Output code for QMLSettings::setupKeys() cpp file')
    argparser.add_argument('-m','--mockqml', action = "store_true", help = 'Output code for MockSettings.qml')
    argparser.add_argument('-i','--header', action = "store_true", help = 'Output header file')
    argparser.add_argument('-d','--dict', action = "store_true", help = 'output python dict of names to help with this script.')

    argparser.add_argument('settings_file', help = 'path to AOG C# Settings.settings file')
    argparser.add_argument('csettings_file', help = 'path to the AOG C# Classes/CSettings.cs file')


    args = argparser.parse_args()
    
    cpp_pre,cpp,h,qml_cpp, mock_qml = parse_settings(open(args.settings_file,'r'))

    cpp_pre1,cpp1,h1,qml_cpp1, mock_qml1 = parse_csettings(open(args.csettings_file,'r'))



    if (args.header):
        print ('#ifndef PROPERTIES_H')
        print ('#define PROPERTIES_H')
        print ()
        print ('#include "aogsettings.h"')
        print ()
        for line in h:
            print (line)

        for line in h1:
            print (line)

        for i in add_props:
            print ('extern AOGProperty %s;' % i['cpp_name'])

        print ()

        print ('#endif // PROPERTIES_H')

    elif args.dict:
        import pprint
        print("props = ", end='')
        pprint.pprint (props, sort_dicts = False)
        
    elif args.mockqml:
        print ("// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team")
        print ("// SPDX-License-Identifier: GNU General Public License v3.0 or later")
        print ("//")
        print ("// For when we use qmlscene")

        print ("import QtQuick 2.15")
        print ()
        print ("//generated by parse_properties.py -m")
        print ()
        print ("Item {" )
        print ("    id: mockSettings")

        for line in mock_qml:
            print ("    %s"  % line)
        for line in mock_qml1:
            print ("    %s"  % line)
        for prop in add_props:
            if 'qml_name' in prop and 'qml_type' in prop:
                if 'qml_default' in prop:
                    print ('    property %s %s: %s' % (prop['qml_type'], prop['qml_name'], prop['qml_default']))
                else:
                    print ('    property %s %s' % (prop['qml_type'], prop['qml_name']))
        print ("}")

    elif args.qmlcpp:
        print ("// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team")
        print ("// SPDX-License-Identifier: GNU General Public License v3.0 or later")
        print ("//")
        print ("//")
        print ('#include "qmlsettings.h"')
        print ()
        print ('void QMLSettings::setupKeys() {')
        for line in qml_cpp:
            print (line)
        for line in qml_cpp1:
            print (line)
        for prop in add_props:
            if 'qml_name' in prop and 'cpp_type' in prop:
                print ('    addKey(QString("%s"),QString("%s"),"%s");' % (prop['qml_name'], prop['ini_path'], prop['cpp_type']))
        print ('}')



    else: 
        for line in cpp_pre:
            print (line)

        for line in cpp:
            print (line)

        for line in cpp1:
            print (line)

        for i in add_props:
            print ('AOGProperty %s("%s",%s);' % (i['cpp_name'], i['ini_path'], i['cpp_default']))

