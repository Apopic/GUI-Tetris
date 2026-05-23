#pragma once
#include "System.h"

inline void Game::DrawMenu() {

	Skin.Menu.Image.BackGround.Draw();

	std::vector<bool> is_target = std::vector<bool>(3, false);
	
	for (int i = 0; i < 3; i++) {
		bool target = Skin.Menu.Image.MenuBox.IsTarget({ 0, Skin.Menu.Config.BoxInterval.Y * (float)i });
		is_target[i] = target || SelectIndex == i;
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

	bool is_up = CheckKey(InputType::GameUpKey);
	bool is_down = CheckKey(InputType::GameDownKey);
	int dir = is_down - is_up;

	if (is_up || is_down) {
		int prev = SelectIndex;
		SelectIndex = std::clamp(SelectIndex + dir, 0, 2);
		if (prev != SelectIndex) {
			Targeted = false;
		}
	}

	for (int i = 0; i < 3; i++) {
		
		bool click = Skin.Menu.Image.MenuBox.IsClick(MOUSE_INPUT_LEFT, { 0, Skin.Menu.Config.BoxInterval.Y * (float)i });
		bool is_return = CheckKey(InputType::GameReturn);

		if (click || is_return) {
			if (Selected) { break; }
			if (!click && is_return) { i = SelectIndex; }
			Selected = true;
			Targeted = false;
			Skin.Menu.SE.Select.Play();

			switch (i) {
			case 0:
				NowScene = Scene::Lobby;
				break;
			case 1:
				MultiPlay = true;
				NowScene = Scene::Lobby;
				break;
			case 2:
				NowScene = Scene::Config;
				break;
			}

			break;
		}
		else if (i == 2) {
			Selected = false;
		}
	}
}