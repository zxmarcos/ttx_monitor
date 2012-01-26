// ============================================================================
// ttx_monitor: Opensource TaitoTypeX loader
// by Romhack
// ============================================================================

#include <iostream>
#include <string>
#include "stdafx.h"
#include "../Shared/DInput.h"

using namespace std;

#define GetInputFor(name)	\
	printf("Defina a entrada para " #name ": ");	\
im.BindInput( ##name, im.GetInput() );	\
	Sleep(300)



int _tmain(int argc, _TCHAR* argv[])
{
	TTX_InputManager im;
	TTX_ConfigManager cm;
	im.Init();
	cm.Config();
	
	Sleep(300);

	GetInputFor(P1_START);
	GetInputFor(P1_COIN);
	GetInputFor(P1_SERVICE);
	GetInputFor(P1_UP);
	GetInputFor(P1_DOWN);
	GetInputFor(P1_LEFT);
	GetInputFor(P1_RIGHT);
	GetInputFor(P1_BUTTON_1);
	GetInputFor(P1_BUTTON_2);
	GetInputFor(P1_BUTTON_3);
	GetInputFor(P1_BUTTON_4);
	GetInputFor(P1_BUTTON_5);
	GetInputFor(P1_BUTTON_6);

	GetInputFor(P2_START);
	GetInputFor(P2_COIN);
	GetInputFor(P2_SERVICE);
	GetInputFor(P2_UP);
	GetInputFor(P2_DOWN);
	GetInputFor(P2_LEFT);
	GetInputFor(P2_RIGHT);
	GetInputFor(P2_BUTTON_1);
	GetInputFor(P2_BUTTON_2);
	GetInputFor(P2_BUTTON_3);
	GetInputFor(P2_BUTTON_4);
	GetInputFor(P2_BUTTON_5);
	GetInputFor(P2_BUTTON_6);

	GetInputFor(TEST_MODE);
	GetInputFor(TTX_EXIT_CODE);

	im.Save();
	cm.Save();
	return 0;
}

