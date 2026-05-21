#pragma once
#include "Config.h"
#include "System.h"

inline void Game::DrawLobby() {

	Skin.Playing.Image.BackGround.Draw();
	StopBGM();

	std::string str;

	if (!MultiPlay) {
		str = "ServerConnecting...";
	}
	else {
		for (auto data : ShareData.PlayerDatas) {
			str += (data.State ? "[*]" : "[ ]") + data.PlayerName + '\n';
		}
	}

	Skin.Playing.Font.SystemStr.Draw(
		Skin.Playing.Config.SystemStrPos,
		GetColor(255, 255, 255),
		GetColor(0, 0, 0),
		str
	);
}

inline void Game::ProcLobby() {

	if (!MultiPlay) {

		if (CheckKey(InputType::GameBack)) {
			Socket.Close();
			MultiPlay = false;
			Socket = TCPSocket();
			ShareData = GameData();
			NowScene = Scene::Menu;
			return;
		}

		if (Socket.Connect(IPAddress::SolveHostName(Config.ServerAddress)->Port(Config.ServerPort))) {
			ShareData.PlayerDatas.front().PlayerName = Config.PlayerName;
			Send();
		}
		MultiPlay = Recv();
	}
	else {

		GameInitFlag = false;

		if (CheckKey(InputType::GameReturn) && !CheckState()) {
			SwitchState(true);
			Send();
		}
		if (CheckKey(InputType::GameBack) && !CheckState()) {
			Socket.Close();
			MultiPlay = false;
			Socket = TCPSocket();
			ShareData = GameData();
			NowScene = Scene::Menu;
			return;
		}
		if (CheckKey(InputType::GameBack) && CheckState()) {
			SwitchState(false);
			Send();
		}
		Recv();
	}
}