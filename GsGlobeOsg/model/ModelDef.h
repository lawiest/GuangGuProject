//#pragma  once
//
#ifndef _MODELDEF_H
#define _MODELDEF_H
//
//#ifdef /*DLL_EXPORT*/_EXPORTS
//#define /*DLL_EXPORT*/ __declspec(dllexport)
//#else
//#define /*DLL_EXPORT*/ __declspec(dllimport)
//#endif



typedef unsigned long long UID64;
typedef unsigned long long UINT64;

typedef signed long long int64;

typedef unsigned char byte; 
typedef unsigned int UINT;
#endif

#ifndef SafeDelete
#define SafeDelete(p)	\
	if ( (p) != NULL )	\
{						\
	delete (p);			\
	(p) = NULL;			\
}
#endif

#ifndef SafeDeleteArray
#define SafeDeleteArray(p)	\
	if ( (p) != NULL )	\
{						\
	delete[] (p);			\
	(p) = NULL;			\
}
#endif
