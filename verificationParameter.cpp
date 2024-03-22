#include "verificationParameter.h"


path GetPath( const string filename )
{
	
	#ifdef WIN32
		#define folder ".\\verification\\"
	#else
		#define folder  "./verification/"
	#endif // wIN32
	
	return path{ folder } / filename;

}