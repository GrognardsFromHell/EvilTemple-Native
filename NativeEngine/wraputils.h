#ifndef WRAPUTILS_H
#define WRAPUTILS_H

#include <QtCore/QtGlobal>

#if defined(Q_OS_WIN)
#define WRAPPER_STDCALL __stdcall
#else
#define WRAPPER_STDCALL
#endif

#define WRAPPER_EXPORT Q_DECL_EXPORT

#endif // WRAPUTILS_H
