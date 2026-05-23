#pragma once
#include "System.h"

inline void Game::DrawInit() {
	Skin.Playing.Image.BackGround.Draw();
}

inline void Game::ProcInit() {
	Config.Load();
	Skin.Load();
	NowScene = Scene::Menu;
}