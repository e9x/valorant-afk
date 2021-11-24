#include "./Util.h"
#include <psapi.h>
#include <iostream>

ExKeyInfo::operator LPARAM() {
	return repeat | (scan << 16) | (extended << 24) | (previous_state << 30) | (transition_state << 31);
}

HWND GetConsole() {
	std::wstring old_title;
	std::wstring new_title = std::to_wstring(GetTickCount64()) + L"/" + std::to_wstring(GetCurrentProcessId());

	old_title.resize(MAX_PATH);
	GetConsoleTitleW(&old_title[0], old_title.size());
	SetConsoleTitleW(new_title.c_str());

	Sleep(50);

	HWND window = FindWindowW(NULL, new_title.c_str());

	SetConsoleTitleW(old_title.c_str());

	return window;
}

BOOL CALLBACK WINAPI enum_cb(HWND window, LPARAM param) {
	EnumData* data = (EnumData*)param;

	DWORD pid = 0;

	GetWindowThreadProcessId(window, &pid);

	HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);

	if (!process) return true;
	
	std::wstring module;
	module.resize(MAX_PATH);
	module.resize(K32GetProcessImageFileNameW(process, &module[0], module.size()));
	
	CloseHandle(process);

	if (!module.ends_with(data->match)) return true;

	std::wstring title;
	title.resize(GetWindowTextLengthW(window));
	GetWindowTextW(window, &title[0], title.size());

	if (title.starts_with(data->title)) {
		data->result = window;
		return false;
	}

	return true;
}

HWND find_process(EnumData& data) {
	EnumWindows(enum_cb, (LPARAM)&data);
	return data.result;
}