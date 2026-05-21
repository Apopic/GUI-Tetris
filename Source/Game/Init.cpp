#pragma once
#include "System.h"

inline void Game::DrawInit() {

	Skin.Playing.Image.BackGround.Draw();

	if (MultiPlay) {
		Skin.Playing.Font.SystemStr.Draw(
			Skin.Playing.Config.SystemStrPos,
			GetColor(255, 255, 255),
			GetColor(0, 0, 0),
			"ServerConnecting.\nPlease Wait..."
		);
	}
}

inline void Game::ProcInit() {
	Init();
	NowScene = Scene::Menu;
}