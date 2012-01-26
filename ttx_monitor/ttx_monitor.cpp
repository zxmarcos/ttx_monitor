// ttx_monitor.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"



// This is an example of an exported variable
TTX_MONITOR_API int nttx_monitor=0;

// This is an example of an exported function.
TTX_MONITOR_API int fnttx_monitor(void)
{
	return 42;
}

// This is the constructor of a class that has been exported.
// see ttx_monitor.h for the class definition
Cttx_monitor::Cttx_monitor()
{
	return;
}
