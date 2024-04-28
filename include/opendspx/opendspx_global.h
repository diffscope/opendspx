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

#ifndef QAS_QASC_RUN
#  ifdef QAS_JSON_NS
#    undef QAS_JSON_NS
#  endif
#  define QAS_JSON_NS(TYPE)                                                                        \
      OPENDSPX_EXPORT QAS::JsonStream &operator>>(QAS::JsonStream &stream, TYPE &var);             \
      OPENDSPX_EXPORT QAS::JsonStream &operator<<(QAS::JsonStream &stream, const TYPE &var);
#endif

#endif // OPENDSPX_GLOBAL_H
