#include <chrono>
#include <thread>

#include "Source/Include.h"
#include "winsparkle.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

	win_sparkle_set_appcast_url("https://winsparkle.org/example/appcast.xml");
	win_sparkle_set_app_details(L"guncys.com", L"GUI-Tetris", L"v1.0");
	win_sparkle_init();
	win_sparkle_check_update_with_ui();

	ChangeWindowMode(TRUE);
	SetGraphMode(1280, 800, 32);
	SetWindowSize(1280, 800);
	SetMainWindowText("Tetris");

	SetAlwaysRunFlag(TRUE);
	SetWaitVSyncFlag(TRUE);
	SetDoubleStartValidFlag(TRUE);

	if (FAILED(DxLib_Init())) {
		return -1;
	}

	SetDrawScreen(DX_SCREEN_BACK);

	auto tp = std::chrono::high_resolution_clock::now();
	std::chrono::nanoseconds frame = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::seconds(1)) / 60;

	bool key[256]{};

	Game game;

	while (!ProcessMessage()) {

		if (game.IsUseGamePad()) {
			for (int i = 0; auto& p : key) {
				if (i >= 10) { break; }
				p = (bool)(GetJoypadInputState(DX_INPUT_PAD1) & game.GetPadKeyCode(i));
				++i;
			}
		}
		else {
			for (int i = 0; auto& k : key) {
				k = (bool)(GetAsyncKeyState(i) & 0x8000);
				++i;
			}
		}

		if (game.IsGameEnd()) {
			break;
		}

		game.Proc(key);
		game.Draw();

		std::this_thread::sleep_until(tp += frame);
	}

	DxLib_End();
	win_sparkle_cleanup();

	return 0;
}