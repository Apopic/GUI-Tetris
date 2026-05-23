#pragma once
#include "Config.h"
#include "System.h"

inline void Game::DrawLobby() {

	Skin.Lobby.Image.BackGround.Draw();
	StopBGM();

	std::vector<bool> is_target = std::vector<bool>(Lobby.Menu.size(), false);
	std::string str;

	for (int i = 0; i < is_target.size(); i++) {
		bool target = Skin.Lobby.Image.MenuBox.IsTarget({ 0, Skin.Lobby.Config.BoxInterval.Y * (float)i + Current_Scroll });
		is_target[i] = target || i == SelectIndex;
		if (Targeted) { continue; }
		if (is_target[i]) {
			Skin.Lobby.SE.Target.Play();
			Targeted = true;
		}
	}
	if (std::all_of(is_target.begin(), is_target.end(), [](const bool data) { return data == false; })) {
		Targeted = false;
	}

	for (int i = 0; i < is_target.size(); i++) {

		int base = 200;
		if (ServerConnected) {
			if (i >= 1 && !ShareData.PlayerDatas[ShareData.MyIndex].IsHost) {
				base = 100;
			}
			if (i == 1 && ShareData.PlayerDatas.size() < 2) {
				base = 100;
			}
		}

		SetDrawBright(base + 55 * is_target[i], base + 55 * is_target[i], base + 55 * is_target[i]);
		Skin.Lobby.Image.MenuBox.Draw({ 0, Skin.Lobby.Config.BoxInterval.Y * (float)i + Current_Scroll });
		SetDrawBright(255, 255, 255);

		Skin.Lobby.Font.Menu.Draw({
			Skin.Lobby.Config.MenuPos.X,
			Skin.Lobby.Config.MenuPos.Y + Skin.Lobby.Config.BoxInterval.Y * (float)i + Current_Scroll },
			GetColor(255, 255, 255),
			GetColor(0, 0, 0),
			Lobby.Menu[i]);

		if (i <= 2) { continue; }
		if (!is_target[i]) { continue; }

		Skin.Lobby.Font.Menu.Draw({
			Skin.Lobby.Config.MenuPos.X,
			Skin.Lobby.Config.MenuPos.Y + Skin.Lobby.Config.BoxInterval.Y / 2.0f + Skin.Lobby.Config.BoxInterval.Y * (float)i + Current_Scroll },
			GetColor(255, 255, 255),
			GetColor(0, 0, 0),
			Config.myIni["GameRule"][Lobby.Menu[i]].as<std::string>());
	}

	if (MultiPlay) {

		if (!ServerConnected) {
			str = "ServerConnecting...";
		}
		else {
			for (auto data : ShareData.PlayerDatas) {
				str += data.PlayerName + '\n';
			}
		}

		Skin.Playing.Font.SystemStr.Draw(
			Skin.Playing.Config.SystemStrPos,
			GetColor(255, 255, 255),
			GetColor(0, 0, 0),
			str
		);
	}
}

inline void Game::ProcLobby() {

	float min = Skin.Lobby.Config.BoxInterval.Y * (Lobby.Menu.size() - 4);
	float addscr = GetMouseWheelRotVolF() * 100.0f;

	bool is_up = CheckKey(InputType::GameUpKey);
	bool is_down = CheckKey(InputType::GameDownKey);
	int dir = is_down - is_up;

	if (is_up || is_down) {
		addscr = 100.0f * (float)-dir;
		int prev = SelectIndex;
		SelectIndex = std::clamp(SelectIndex + dir, 0, (int)Lobby.Menu.size() - 1);
		if (prev != SelectIndex) {
			Targeted = false;
		}
	}
	Current_Scroll = Screen.GetScrollPos(addscr, -min, 0.0f);

	for (int i = 0; i < (int)Lobby.Menu.size(); i++) {

		bool click = Skin.Config.Image.MenuBox.IsClick(MOUSE_INPUT_LEFT, { 0, Skin.Config.Config.BoxInterval.Y * (float)i + Current_Scroll });
		bool is_return = CheckKey(InputType::GameReturn);

		if (click || is_return) {
			if (Selected) { break; }
			if (!click && is_return) { i = SelectIndex; }

			if (MultiPlay) {
				if (!ServerConnected) { continue; }
				if (i >= 1 && !ShareData.PlayerDatas[ShareData.MyIndex].IsHost) { continue; }
				if (i == 1 && ShareData.PlayerDatas.size() < 2) { continue; }
			}

			Selected = true;
			Targeted = false;
			Skin.Lobby.SE.Select.Play();

			switch (i) {
			case 0:
				NowScene = Scene::Menu;
				break;
			case 1:
				if (!MultiPlay) {
					Init();
					NowScene = Scene::Playing;
					return;
				}
				else {
					SwitchState(1);
					Send();
					return;
				}
				break;
			case 2:
				ResetGameRule();
				break;
			case 3:
				InputInt(Config.myIni["GameRule"][Lobby.RuleName[i - 3]], 2, 4, 10);
				break;
			case 4:
				InputInt(Config.myIni["GameRule"][Lobby.RuleName[i - 3]], 2, 4, 20);
				break;
			case 5:
				InputInt(Config.myIni["GameRule"][Lobby.RuleName[i - 3]], 1, 1, 5);
				break;
			case 6:
				InputDouble(Config.myIni["GameRule"][Lobby.RuleName[i - 3]], 9, 0.0, 10.0);
				break;
			}

			if (ServerConnected && i != 0) {
				SendGameRule();
			}

			Skin.Lobby.SE.Select.Play();

			break;
		}
		else if (i == ((int)Lobby.Menu.size() - 1)) {
			Selected = false;
		}
	}
	if (MultiPlay) {

		if (!ServerConnected) {

			if (NowScene == Scene::Menu) {
				Socket.Close();
				MultiPlay = false;
				ServerConnected = false;
				Socket = TCPSocket();
				ShareData = GameData();
				return;
			}

			if (Socket.Connect(IPAddress::SolveHostName(Config.ServerAddress)->Port(Config.ServerPort))) {
				ShareData.PlayerDatas.front().PlayerName = Config.PlayerName;
				Send();
			}
			ServerConnected = Recv();

			if (ServerConnected) {
				Config.PrevIni = Config.myIni;
			}
		}
		else {

			if (NowScene == Scene::Menu) {
				Config.myIni = Config.PrevIni;
				Config.PrevIni = ini::IniFile();
				Config.myIni.save(Config.Path);
				Config.Load();
				Socket.Close();
				MultiPlay = false;
				ServerConnected = false;
				Socket = TCPSocket();
				ShareData = GameData();
				return;
			}

			if (Recv()) {
				
				RecvGameRule();

				if (ShareData.NowScene == Scene::Playing) {
					Init();
				}
			}
		}
	}
}