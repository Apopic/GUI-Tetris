#include <chrono>
#include <thread>

#include "Source/System.h"
#include "winsparkle.h"

_Game game;

int UpdateStart() {
	game.StartUpdate();
	return TRUE;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

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

	win_sparkle_set_appcast_url("https://apopic.github.io/GUI-Tetris/appcast.xml");
	win_sparkle_set_app_details(L"Apopic", L"GUI-Tetris", L"1.0.3");
	win_sparkle_set_eddsa_public_key("zt6allpereF1l/6MA/9ADS0s/U7NXaf3jVBpk06DWJI=");
	win_sparkle_set_can_shutdown_callback(UpdateStart);

	win_sparkle_init();
	win_sparkle_check_update_without_ui();

	while (!ProcessMessage()) {

		if (GetWindowActiveFlag()) {
			if (game.IsUseGamePad()) {
				for (size_t i = 0; auto& p : key) {
					if (i > (int)InputType::Count) { break; }
					p = (bool)(GetJoypadInputState(DX_INPUT_PAD1) & game.GetPadKeyCode(i));
					++i;
				}
			}
			else {
				for (size_t i = 0; auto& k : key) {
					k = (bool)(GetAsyncKeyState(i) & 0x8000);
					++i;
				}
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