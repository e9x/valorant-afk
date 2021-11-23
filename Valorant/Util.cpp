#include "./Util.h"
#include <psapi.h>
#include <iostream>

ExKeyInfo::operator LPARAM() {
	return repeat | (scan << 16) | (extended << 24) | (previous_state << 30) | (transition_state << 31);
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
	std::cout << "ae " << module.size() << " " << GetLastError() << std::endl;

	CloseHandle(process);

	if (module.ends_with(data->match)) {
		std::cout << "no module match " << std::string(module.begin(), module.end()) << std::endl;
		return true;
	}

	std::wstring title;
	title.resize(GetWindowTextW(window, nullptr, 0));
	GetWindowTextW(window, &title[0], title.size());

	if (title == data->title) {
		data->result = window;
		return false;
	}
	else {
		// std::cout << "no title match" << std::endl;
		// std::cout << std::string(title.begin(), title.end()) << std::endl;
	}

	return true;
}

HWND find_process(EnumData& data) {
	EnumWindows(enum_cb, (LPARAM)&data);
	return data.result;
}