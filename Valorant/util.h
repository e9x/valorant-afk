#pragma once
#include <string>
#include <windows.h>

struct EnumData {
	std::wstring match;
	std::wstring title;
	HWND result = 0;
};

struct ExKeyInfo {
	unsigned short repeat = 0;
	unsigned char scan = 0;
	int extended = 0, previous_state = 0, transition_state = 0;
	operator LPARAM();
};

HWND GetConsole();

HWND find_process(EnumData& data);