#pragma once
#include "System.h"
#include "winsparkle.h"

void Game::DrawConfig() {

	Skin.Config.Image.BackGround.Draw();

	std::string str;
	std::vector<bool> is_target = std::vector<bool>((int)Config.Mode, false);

	for (int i = 0; i < is_target.size(); i++) {
		bool target = Skin.Config.Image.MenuBox.IsTarget({ 0, Skin.Config.Config.BoxInterval.Y * (float)i + Current_Scroll });
		is_target[i] = target || i == SelectIndex;
		if (Targeted) { continue; }
		if (is_target[i]) {
			Skin.Config.SE.Target.Play();
			Targeted = true;
		}
	}
	if (std::all_of(is_target.begin(), is_target.end(), [](const bool data) { return data == false; })) {
		Targeted = false;
	}

	for (int i = 0; i < is_target.size(); i++) {

		SetDrawBright(200 + 55 * is_target[i], 200 + 55 * is_target[i], 200 + 55 * is_target[i]);
		Skin.Config.Image.MenuBox.Draw({ 0, Skin.Config.Config.BoxInterval.Y * (float)i + Current_Scroll });
		SetDrawBright(255, 255, 255);

		switch (Config.Mode) {
		case ConfigMode::None:
			str = Config.Menu[0][i];
			break;
		case ConfigMode::GameConfig:
			str = Config.Menu[1][i];
			break;
		case ConfigMode::KeyConfig:
			str = Config.Menu[2][i];
			break;
		}

		Skin.Config.Font.Menu.Draw({
			Skin.Config.Config.MenuPos.X,
			Skin.Config.Config.MenuPos.Y + Skin.Config.Config.BoxInterval.Y * (float)i + Current_Scroll },
			GetColor(255, 255, 255),
			GetColor(0, 0, 0),
			str);

		if (Config.Mode == ConfigMode::None) { continue; }
		if (i == 0) { continue; }
		if (!is_target[i]) { continue; }
		if (Config.Mode == ConfigMode::GameConfig) {
			str = Config.myIni["General"][Config.Menu[1][i]].as<std::string>();
		}
		if (Config.Mode == ConfigMode::KeyConfig) {
			if (!Config.UseGamePad) {
				char keyname[256];
				int keycode = Config.myIni["KeyConfig"][Config.Menu[2][i]].as<int>();
				UINT scancode = MapVirtualKey(keycode, MAPVK_VK_TO_VSC);
				GetKeyNameText(scancode << 16, keyname, sizeof(keyname));
				str = keyname;
			}
			else {
				str = Config.PadInputName[Config.myIni["PadConfig"][Config.Menu[2][i]].as<int>()];
			}
		}

		Skin.Config.Font.Menu.Draw({
			Skin.Config.Config.MenuPos.X,
			Skin.Config.Config.MenuPos.Y + Skin.Config.Config.BoxInterval.Y / 2.0f + Skin.Config.Config.BoxInterval.Y * (float)i + Current_Scroll },
			GetColor(255, 255, 255),
			GetColor(0, 0, 0),
			str);
	}
}

void Game::ProcConfig() {

	float min = Skin.Config.Config.BoxInterval.Y * ((int)Config.Mode - (int)ConfigMode::None);
	float addscr = GetMouseWheelRotVolF() * 100.0f;
	
	bool is_up = CheckKey(InputType::GameUpKey);
	bool is_down = CheckKey(InputType::GameDownKey);
	int dir = is_down - is_up;

	if (is_up || is_down) {
		addscr = 100.0f * (float)-dir;
		int prev = SelectIndex;
		SelectIndex = std::clamp(SelectIndex + dir, 0, (int)Config.Mode - 1);
		if (prev != SelectIndex) {
			Targeted = false;
		}
	}
	Current_Scroll = Screen.GetScrollPos(addscr, -min, 0.0f);

	for (int i = 0; i < (int)Config.Mode; i++) {
		
		bool click = Skin.Config.Image.MenuBox.IsClick(MOUSE_INPUT_LEFT, { 0, Skin.Config.Config.BoxInterval.Y * (float)i + Current_Scroll});
		bool is_return = CheckKey(InputType::GameReturn);

		if (click || is_return) {
			if (Selected) { break; }
			if (!click && is_return) { i = SelectIndex; }
			Selected = true;
			Targeted = false;
			Skin.Config.SE.Select.Play();

			ConfigMode Prev = Config.Mode;

			if (Config.Mode == ConfigMode::None) {
				switch (i) {
				case 0:
					NowScene = Scene::Menu;
					break;
				case 1:
					Config.Mode = ConfigMode::GameConfig;
					break;
				case 2:
					Config.Mode = ConfigMode::KeyConfig;
					break;
				case 3:
					win_sparkle_check_update_with_ui();
					break;
				}
			}
			else if (Config.Mode == ConfigMode::GameConfig) {
				switch (i) {
				case 0:
					Config.Mode = ConfigMode::None;
					break;
				case 2:
					InputBool(Config.myIni["General"][Config.Menu[1][2]]);
					break;
				case 4:
					InputInt(Config.myIni["General"][Config.Menu[1][i]], 5, 0, 65535);
					break;
				case 5:
					InputDouble(Config.myIni["General"][Config.Menu[1][i]], 9, 0.0, 1.0);
					break;
				default:
					InputString(Config.myIni["General"][Config.Menu[1][i]]);
					break;
				}
			}
			else if (Config.Mode == ConfigMode::KeyConfig) {
				switch (i) {
				case 0:
					Config.Mode = ConfigMode::None;
					break;
				default:
					if (!Config.UseGamePad) {
						InputKey(Config.myIni["KeyConfig"][Config.Menu[2][i]]);
					}
					else {
						InputPad(Config.myIni["PadConfig"][Config.Menu[2][i]]);				
					}
					break;
				}
			}

			if (Prev != Config.Mode) {
				Current_Scroll = 0.0f;
				Screen.ResetScrollPos();
				SelectIndex = -1;
			}

			Skin.Config.SE.Select.Play();

			break;
		}
		else if (i == ((int)Config.Mode - 1)) {
			Selected = false;
		}
	}
}