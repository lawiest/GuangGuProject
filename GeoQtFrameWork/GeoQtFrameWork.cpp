#include "GeoQtFrameWork.h"


#ifdef _WIN32
#ifdef _DEBUG
#pragma comment(lib,"gsgeodatabased.lib")
#pragma comment(lib,"gssymbold.lib")
#pragma comment(lib,"gsgeometryd.lib")
#pragma comment(lib,"gsmapd.lib") 
#pragma comment(lib,"gsutilityd.lib") 
#pragma comment(lib,"gsspatialreferenced.lib") 
#pragma comment(lib,"gsspatialanalysisd.lib")  
#pragma comment(lib,"gsqtport.lib") 
#pragma comment(lib,"gswin32portd.lib") 
#else
#pragma comment(lib,"gsutility.lib")
#pragma comment(lib,"gsgeodatabase.lib")
#pragma comment(lib,"gssymbol.lib")
#pragma comment(lib,"gsgeometry.lib")
#pragma comment(lib,"gsutility.lib") 
#pragma comment(lib,"gsspatialreference.lib") 
#pragma comment(lib,"gsmap.lib")  
#pragma comment(lib,"gsqtport.lib") 
#pragma comment(lib,"gswin32port.lib") 
#endif
#pragma comment(lib,"user32.lib") 
#endif


GeoQtFrameWork::GeoQtFrameWork()
{
}
