#pragma once

#include <QtCore/qglobal.h>



#ifndef BUILD_STATIC
# if defined(GEOQTFRAMEWORK_LIB)
#  define GEOQTFRAMEWORK_EXPORT Q_DECL_EXPORT
# else
#  define GEOQTFRAMEWORK_EXPORT Q_DECL_IMPORT
# endif
#else
# define GEOQTFRAMEWORK_EXPORT
#endif
