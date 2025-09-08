// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later

// Mock AOGRenderer component for Qt Design Studio
// The real AOGRenderer is implemented in C++ and registered at runtime

import QtQuick

Rectangle {
    id: root
    
    // Properties that match the real AOGRenderer
    property double shiftX: 0
    property double shiftY: 0
    
    // Signals that match the real AOGRenderer
    signal clicked(var mouse)
    signal dragged(int fromX, int fromY, int toX, int toY)
    signal zoomOut()
    signal zoomIn()
    
    // Visual mock for design preview
    color: "#2a2a2a"
    border.color: "#555"
    border.width: 1
    
    Text {
        anchors.centerIn: parent
        text: "AOG Renderer\n(Design Preview)\n\nReal OpenGL renderer\nwill appear here\nwhen running the app"
        color: "#888"
        font.pixelSize: 14
        horizontalAlignment: Text.AlignHCenter
        lineHeight: 1.2
    }
    
    // Grid pattern to simulate field view
    Canvas {
        anchors.fill: parent
        opacity: 0.3
        
        onPaint: {
            var ctx = getContext("2d")
            ctx.strokeStyle = "#444"
            ctx.lineWidth = 1
            
            // Draw grid
            for (var x = 0; x < width; x += 50) {
                ctx.beginPath()
                ctx.moveTo(x, 0)
                ctx.lineTo(x, height)
                ctx.stroke()
            }
            
            for (var y = 0; y < height; y += 50) {
                ctx.beginPath()
                ctx.moveTo(0, y)
                ctx.lineTo(width, y)
                ctx.stroke()
            }
        }
    }
}