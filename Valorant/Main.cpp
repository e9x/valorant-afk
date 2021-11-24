#include "./Util.h"
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <iostream>

HWND game = 0;
bool afk = false;
EnumData search{ L"VALORANT-Win64-Shipping.exe", L"VALORANT" };

void wait_game() {
	while (!(game = find_process(search))) Sleep(1);
	std::cout << "Found HWND: 0x" << std::hex << game << std::endl;
}

DWORD WINAPI repeat_key(LPVOID param) {
	short
		vk_1 = LOBYTE(VkKeyScanW('a')),
		vk_2 = LOBYTE(VkKeyScanW('d')),
		vk_current = vk_1;
	
	srand((unsigned int)time(0));

	while (true) {
		Sleep(1);
		
		if (!IsWindow(game)) {
			game = NULL;
			wait_game();
		}

		if (afk) {
			CURSORINFO info{};
			info.cbSize = sizeof(info);
			GetCursorInfo(&info);

			if (!(info.flags & CURSOR_SHOWING)) {
				ExKeyInfo lparam{};
				lparam.scan = MapVirtualKeyW(vk_current, MAPVK_VK_TO_VSC);
				PostMessage(game, WM_KEYDOWN, vk_current, lparam);
				Sleep(100 + (rand() % 10));
				lparam.repeat = 1;
				lparam.previous_state = 1;
				lparam.transition_state = 1;
				PostMessage(game, WM_KEYUP, vk_current, lparam);
				Sleep(100 + (rand() % 10));
			}
		}

		vk_current = vk_current == vk_1 ? vk_2 : vk_1;
	}

	return EXIT_SUCCESS;
}

int main() {
	wait_game();

	HANDLE thread = CreateThread(0, 0, repeat_key, 0, 0, 0);
	
	std::cout << "Press [F6] to toggle AFK" << std::endl;;

	bool held = true;

	while (true) {
		if (GetAsyncKeyState(VK_F6)) {
			if (!held) {
				afk ^= 1;
				std::cout << "\r" << (afk ? "AFK          " : "No longer AFK");
				held = true;
			}
		}
		else held = false;
	}

	std::cout << std::endl;

	if (thread) WaitForSingleObject(thread, 100);
	
	return EXIT_SUCCESS;
}