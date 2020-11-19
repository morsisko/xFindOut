#pragma once
#include <Windows.h>
#include "pluginsdk/bridgemain.h"

class HitDialog
{
private:
	HWND hwnd;
public:
	HitDialog(HINSTANCE instance, duint address);
};

