#ifndef RENDERING_H
#define RENDERING_H

// For Windows and Android, use direct openGL rendering using
// the QML engine's GL context
#if !defined(Q_OS_WINDOWS) && !defined(Q_OS_ANDROID)
#define USE_INDIRECT_RENDERING
#endif

// If you want to use the QSGRenderNode-based AOGRendererItem,
// uncomment this #define and then change MainWindow.qml to use
// AOGRendererItem instead of AOGRenderer
//#define USE_GSGRENDERNODE 1

#endif // RENDERING_H
