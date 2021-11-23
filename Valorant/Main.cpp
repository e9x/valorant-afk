#include "./Util.h"
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <iostream>

HWND game;

int quit = 0;

DWORD WINAPI repeat_key(LPVOID param) {
	short vk_1 = LOBYTE(VkKeyScanW('a'));
	short vk_2 = LOBYTE(VkKeyScanW('d'));
	short vk_current = vk_1;
	
	srand((unsigned int)time(0));

	while (!quit) {
		Sleep(1);

		CURSORINFO info{};
		info.cbSize = sizeof(info);

		if (!GetCursorInfo(&info)) {
			std::cerr << "GetCursorInfo() bad " << GetLastError() << std::endl;
			continue;
		}

		if (info.flags & CURSOR_SHOWING) continue;
		
		ExKeyInfo lparam{};
		lparam.scan = MapVirtualKeyW(vk_current, MAPVK_VK_TO_VSC);
		PostMessage(game, WM_KEYDOWN, vk_current, lparam);
		Sleep(100 + (rand() % 10));
		lparam.repeat = 1;
		lparam.previous_state = 1;
		lparam.transition_state = 1;
		PostMessage(game, WM_KEYUP, vk_current, lparam);
		Sleep(100 + (rand() % 10));

		vk_current = vk_current == vk_1 ? vk_2 : vk_1;
		std::cout << "sent" << std::endl;
	}

	return EXIT_SUCCESS;
}

EnumData search{L"VALORANT-Win64-Shipping.exe", L"VALORANT"};

void wait_game() {
	std::cout << "Waiting for HWND" << std::endl;;
	while (!(game = find_process(search))) Sleep(1);
	std::cout << "Found HWND: 0x" << std::hex << game << std::endl;
}

int main() {
	wait_game();

	HANDLE thread = CreateThread(0, 0, repeat_key, 0, 0, 0);
	
	std::cout << "Press [ESC] in this terminal to quit" << std::endl;;

	while (!(quit = _getch() == VK_ESCAPE)) if (!IsWindow(game)) {
		game = NULL;
		wait_game();
	}

	if (thread) WaitForSingleObject(thread, 100);
	
	return EXIT_SUCCESS;
}