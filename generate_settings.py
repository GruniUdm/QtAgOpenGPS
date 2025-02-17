#!/usr/bin/python3
import sys

def parse_aogsettings_line(line):
    line = line[12:]
    parts = line.split('(')
    remainder = '('.join(parts[1:])
    key = remainder.split('"')[1].split('/')
    default = '"'.join(remainder.split('"')[2:])[1:-3]
    print ("'%s','%s','%s','%s'" % ( parts[0], key[0], key[1], default) )

for line in sys.stdin.readlines():
    parse_aogsettings_line(line)
