#pragma once
#include <Windows.h>
#include <commctrl.h>
#include "pluginsdk/bridgemain.h"

#define WM_UPDATE_HITS WM_USER+1
#define WM_UPDATE_INSERT_ROW WM_USER+2

class HitDialog
{
private:
	HWND hwnd;
	duint address;
public:
	HitDialog(duint address);
	void setHWND(HWND hwnd);
	HWND getHWND();
	duint getAddress();
	void updateHits(int index, int hits);
	void insertRow(char* instruction);
};

