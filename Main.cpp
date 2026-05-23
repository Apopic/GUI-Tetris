#include <chrono>
#include <thread>

#include "Source/Include.h"
#include "winsparkle.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

	win_sparkle_set_appcast_url("https://apopic.github.io/GUI-Tetris/appcast.xml");
	win_sparkle_set_app_details(L"Apopic", L"GUI-Tetris", L"1.0.0");
	win_sparkle_set_eddsa_public_key("zt6allpereF1l/6MA/9ADS0s/U7NXaf3jVBpk06DWJI=");

	win_sparkle_init();

	ChangeWindowMode(TRUE);
	SetGraphMode(1280, 800, 32);
	SetWindowSize(1280, 800);
	SetMainWindowText("GUI-Tetris");

	SetAlwaysRunFlag(TRUE);
	SetWaitVSyncFlag(TRUE);
	SetDoubleStartValidFlag(TRUE);
	SetOutApplicationLogValidFlag(FALSE);

	if (FAILED(DxLib_Init())) {
		return -1;
	}

	SetDrawScreen(DX_SCREEN_BACK);

	auto tp = std::chrono::high_resolution_clock::now();
	std::chrono::nanoseconds frame = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::seconds(1)) / 60;

	bool key[256]{};

	Game game;

	while (!ProcessMessage()) {

		if (GetWindowActiveFlag()) {
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
		}

		game.IsScreenClick();

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