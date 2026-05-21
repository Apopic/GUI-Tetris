#pragma once
#include "System.h"

inline void Game::DrawMenu() {

	Skin.Menu.Image.BackGround.Draw();

	std::vector<bool> is_target = std::vector<bool>(3, false);
	
	for (int i = 0; i < 3; i++) {
		is_target[i] = Skin.Menu.Image.MenuBox.IsTarget({ 0, Skin.Menu.Config.BoxInterval.Y * (float)i });
		if (Targeted) { continue; }
		if (is_target[i]) {
			Skin.Menu.SE.Target.Play();
			Targeted = true;
		}
	}
	if (std::all_of(is_target.begin(), is_target.end(), [](const bool data) { return data == false; })) {
		Targeted = false;
	}

	SetDrawBright(128 * is_target[0], 128 * is_target[0], 255);
	Skin.Menu.Image.MenuBox.Draw({ 0, (float)(Skin.Menu.Config.BoxInterval.Y * 0 )});
	SetDrawBright(255, 128 * is_target[1], 128 * is_target[1]);
	Skin.Menu.Image.MenuBox.Draw({ 0, (float)(Skin.Menu.Config.BoxInterval.Y * 1 )});
	SetDrawBright(128 * is_target[2], 255, 128 * is_target[2]);
	Skin.Menu.Image.MenuBox.Draw({ 0, (float)(Skin.Menu.Config.BoxInterval.Y * 2 )});
	SetDrawBright(255, 255, 255);
	
	Skin.Menu.Font.Menu.Draw({ 
		Skin.Menu.Config.MenuPos.X,
		Skin.Menu.Config.MenuPos.Y + (int)(Skin.Menu.Config.BoxInterval.Y * 0)},
		GetColor(255, 255, 255),
		GetColor(0, 0, 0),
		"SinglePlay"
	);
	Skin.Menu.Font.Menu.Draw({
		Skin.Menu.Config.MenuPos.X,
		Skin.Menu.Config.MenuPos.Y + (int)(Skin.Menu.Config.BoxInterval.Y * 1) },
		GetColor(255, 255, 255),
		GetColor(0, 0, 0),
		"MultiPlay"
	);
	Skin.Menu.Font.Menu.Draw({
		Skin.Menu.Config.MenuPos.X,
		Skin.Menu.Config.MenuPos.Y + (int)(Skin.Menu.Config.BoxInterval.Y * 2) },
		GetColor(255, 255, 255),
		GetColor(0, 0, 0),
		"Config"
	);
}

inline void Game::ProcMenu() {

	for (int i = 0; i < 3; i++) {
		if (Skin.Menu.Image.MenuBox.IsClick(MOUSE_INPUT_LEFT, { 0, Skin.Menu.Config.BoxInterval.Y * (float)i })) {
			if (Clicked) { break; }
			Clicked = true;
			Targeted = false;
			Skin.Menu.SE.Select.Play();
			switch (i) {
			case 0:
				NowScene = Scene::Playing;
				Init();
				break;
			case 1:
				NowScene = Scene::Lobby;
				break;
			case 2:
				NowScene = Scene::Config;
				break;
			}

			break;
		}
		else if (i == 2) {
			Clicked = false;
		}
	}
}