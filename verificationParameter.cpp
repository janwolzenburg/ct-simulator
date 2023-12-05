#include "verificationParameter.h"


path GetPath( const string filename )
{
	
	#ifdef WIN32
		#define folder ".\\verification\\"
	#else
		#define folder  "./verification/"
	#endif // WIN32
	
	return path{ folder } / filename;

}