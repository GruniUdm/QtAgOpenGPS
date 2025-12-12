#ifndef INDIRECT_H
#define INDIRECT_H

// For Windows and Android, use direct openGL rendering using
// the QML engine's GL context
#if !defined(Q_OS_WINDOWS) && !defined(Q_OS_ANDROID)

#define USE_INDIRECT_RENDERING

#endif

#endif // INDIRECT_H
