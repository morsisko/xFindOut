#include "HitDialog.h"
#include "resource.h"

INT_PTR CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CLOSE:
	{
		DestroyWindow(hwndDlg);
		return true;
	}
	break;
	default:
		break;
	}

	return false;
}


HitDialog::HitDialog(HINSTANCE instance, duint address)
{
	this->hwnd = CreateDialog(instance, MAKEINTRESOURCE(IDD_DIALOG_HITS), GuiGetWindowHandle(), DialogProc);

	char windowName[64];
	snprintf(windowName, sizeof(windowName), "Find out what accesses %X address", address);
	SetWindowText(hwnd, windowName);
	ShowWindow(hwnd, SW_SHOW);
}
