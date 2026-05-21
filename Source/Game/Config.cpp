#pragma once
#include "System.h"

void Game::DrawConfig() {

	Skin.Menu.Image.BackGround.Draw();

	std::string str;
	std::vector<bool> is_target = std::vector<bool>((int)Config.Mode, false);

	for (int i = 0; i < is_target.size(); i++) {
		is_target[i] = Skin.Menu.Image.MenuBox.IsTarget({ 0, Skin.Menu.Config.BoxInterval.Y * (float)i + Current_Rot });
		if (Targeted) { continue; }
		if (is_target[i]) {
			Skin.Menu.SE.Target.Play();
			Targeted = true;
		}
	}
	if (std::all_of(is_target.begin(), is_target.end(), [](const bool data) { return data == false; })) {
		Targeted = false;
	}

	for (int i = 0; i < is_target.size(); i++) {

		SetDrawBright(200 + 55 * is_target[i], 200 + 55 * is_target[i], 200 + 55 * is_target[i]);
		Skin.Menu.Image.MenuBox.Draw({ 0, Skin.Menu.Config.BoxInterval.Y * (float)i + Current_Rot });
		SetDrawBright(255, 255, 255);

		switch (Config.Mode) {
		case ConfigMode::None:
			str = Config.ConfigRole[0][i];
			break;
		case ConfigMode::GameConfig:
			str = Config.ConfigRole[1][i];
			break;
		case ConfigMode::KeyConfig:
			str = Config.ConfigRole[2][i];
			break;
		}

		Skin.Menu.Font.Menu.Draw({
			Skin.Menu.Config.MenuPos.X,
			Skin.Menu.Config.MenuPos.Y + Skin.Menu.Config.BoxInterval.Y * (float)i + Current_Rot },
			GetColor(255, 255, 255),
			GetColor(0, 0, 0),
			str);

		if (Config.Mode == ConfigMode::None) { continue; }
		if (i == 0) { continue; }
		if (!is_target[i]) { continue; }
		if (Config.Mode == ConfigMode::GameConfig) {
			str = Config.myIni["General"][Config.ConfigRole[1][i]].as<std::string>();
		}
		if (Config.Mode == ConfigMode::KeyConfig) {
			if (!Config.UseGamePad) {
				char keyname[256];
				int keycode = Config.myIni["KeyConfig"][Config.ConfigRole[2][i]].as<int>();
				UINT scancode = MapVirtualKey(keycode, MAPVK_VK_TO_VSC);
				GetKeyNameText(scancode << 16, keyname, sizeof(keyname));
				str = keyname;
			}
			else {
				str = Config.PadInputName[Config.myIni["PadConfig"][Config.ConfigRole[2][i]].as<int>()];
			}
		}

		Skin.Menu.Font.Menu.Draw({
			Skin.Menu.Config.MenuPos.X,
			Skin.Menu.Config.MenuPos.Y + Skin.Menu.Config.BoxInterval.Y / 2.0f + Skin.Menu.Config.BoxInterval.Y * (float)i + Current_Rot },
			GetColor(255, 255, 255),
			GetColor(0, 0, 0),
			str);
	}
}

void Game::ProcConfig() {

	float min = Skin.Menu.Config.BoxInterval.Y * ((int)Config.Mode - (int)ConfigMode::None);
	Current_Rot = Wheel.GetWheelRot(100.0f, -min, 0.0f);

	for (int i = 0, j = (int)Config.Mode; i < j; i++) {
		if (Skin.Menu.Image.MenuBox.IsClick(MOUSE_INPUT_LEFT, { 0, Skin.Menu.Config.BoxInterval.Y * (float)i + Current_Rot})) {
			
			if (Clicked) { break; }
			Clicked = true;
			Targeted = false;
			Skin.Menu.SE.Select.Play();

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
				}
			}
			else if (Config.Mode == ConfigMode::GameConfig) {
				switch (i) {
				case 0:
					Config.Mode = ConfigMode::None;
					break;
				case 2:
					Config.InputBool(Config.myIni["General"][Config.ConfigRole[1][2]]);
					break;
				default:
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
					DrawConfig();
					ScreenFlip();
					SetDrawBlendMode(0, 0);
					SetKeyInputStringFont(Skin.Menu.Font.Menu.Handle);
					Config.InputString(Config.myIni["General"][Config.ConfigRole[1][i]]);		
					Skin.Menu.SE.Select.Play();
					break;
				}
			}
			else if (Config.Mode == ConfigMode::KeyConfig) {
				switch (i) {
				case 0:
					Config.Mode = ConfigMode::None;
					break;
				default:
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
					DrawConfig();
					ScreenFlip();
					SetDrawBlendMode(0, 0);
					if (!Config.UseGamePad) { Config.InputKey(Config.myIni["KeyConfig"][Config.ConfigRole[2][i]]); }
					else { Config.InputPad(Config.myIni["PadConfig"][Config.ConfigRole[2][i]]); }
					Skin.Menu.SE.Select.Play();
					break;
				}
			}

			if (Prev != Config.Mode) {
				Current_Rot = 0;
				Wheel.ResetWheelRot();
			}

			break;
		}
		else if (i == ((int)Config.Mode - 1)) {
			Clicked = false;
		}
	}
}