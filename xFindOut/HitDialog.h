#pragma once
#include <Windows.h>
#include <commctrl.h>
#include "pluginsdk/bridgemain.h"
#include "defs.h"

#define WM_UPDATE_HITS WM_USER+1
#define WM_UPDATE_INSERT_ROW WM_USER+20

class HitDialog
{
private:
	HWND hwnd;
	duint address;
	bool writeEntry;
public:
	HitDialog(duint address, bool isWriteEntry);
	void setHWND(HWND hwnd);
	HWND getHWND();
	duint getAddress();
	void updateHits(int index, int hits);
	void insertRow(char* instruction);
	bool isWriteEntryDlg();
};

