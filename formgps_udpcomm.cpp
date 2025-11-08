// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// ❌ UDP COMMUNICATION COMPLETELY REMOVED - Phase 4.6
// 
// ARCHITECTURAL CHANGE: Workers → AgIOService is the ONLY data source
// 
// Previously this file handled:
// - UDP communication on port 15550 (NOT the legacy 15555/17777 loopback)
// - GPS PGN 0xD6 processing from hardware modules
// - IMU data processing
// - Forward to AgIOService (causing double processing)
//
// NEW ARCHITECTURE (Phase 4.6):
// ✅ GPSWorker → AgIOService → FormGPS → pn/vehicle/ahrs → OpenGL
// ✅ UDPWorker → AgIOService → FormGPS (for PGN modules communication)
// ✅ SerialWorker → AgIOService → FormGPS (for IMU/GPS serial)
// ✅ NTRIPWorker → AgIOService → FormGPS (for RTK corrections)
//
// ❌ REMOVED: FormGPS UDP direct processing (eliminated double processing)
// ❌ REMOVED: udpSocket, ReceiveFromAgIO(), StartLoopbackServer()
// ❌ REMOVED: All UDP fallback mechanisms
//
// Justification: 
// - AgIOService Workers architecture is the canonical source of truth
// - No need for UDP fallback as Workers handle all hardware communication
// - Eliminates data flow conflicts and double processing
// - Conforms to Qt 6.8 modern threading patterns
//
// If UDP communication is needed, it should be handled through UDPWorker
// in the AgIOService architecture, not directly in FormGPS.

#include "formgps.h"

// All UDP communication functions removed - AgIOService Workers handle everything
// Functions that were removed:
// - void FormGPS::ReceiveFromAgIO()
// - void FormGPS::StartLoopbackServer() 
// - void FormGPS::stopUDPServer()
// - void FormGPS::SendPgnToLoop(QByteArray byteData)

// DisableSim() moved to appropriate file since it's not UDP-specific
void FormGPS::DisableSim()
{
    qDebug() << "DisableSim called - stopping simulator";
    timerSim.stop();
}