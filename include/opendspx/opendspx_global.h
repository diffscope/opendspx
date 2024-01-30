#ifndef OPENDSPX_GLOBAL_H
#define OPENDSPX_GLOBAL_H

#include <QtCore/QtGlobal>

#ifndef OPENDSPX_EXPORT
#  ifdef OPENDSPX_STATIC
#    define OPENDSPX_EXPORT
#  else
#    ifdef OPENDSPX_LIBRARY
#      define OPENDSPX_EXPORT Q_DECL_EXPORT
#    else
#      define OPENDSPX_EXPORT Q_DECL_IMPORT
#    endif
#  endif
#endif

#endif // OPENDSPX_GLOBAL_H
