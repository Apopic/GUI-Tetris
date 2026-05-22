#pragma once
#include <filesystem>
#include "../Library/inicpp.h"
#include "System.h"

namespace fs = std::filesystem;

const enum struct InputType : int {
	Null = -1,
	MoveLeft,
	MoveRight,
	RotateLeft,
	RotateRight,
	SoftDrop,
	HardDrop,
	SwapHold,
	GameReturn,
	GameBack,
	Count
};

enum struct ConfigMode : int{
	Null = -1,
	None = 4, 
	GameConfig = 5,
	KeyConfig = 10
};

class _Config {
public:

	int PadInputList[11] = {
		PAD_INPUT_DOWN,
		PAD_INPUT_LEFT,
		PAD_INPUT_RIGHT,
		PAD_INPUT_UP,
		PAD_INPUT_A,
		PAD_INPUT_B,
		PAD_INPUT_X,
		PAD_INPUT_Y,
		PAD_INPUT_L,
		PAD_INPUT_R,
		PAD_INPUT_START
	};

	std::string PadInputName[11] = {
		"Down",
		"Left",
		"Right",
		"Up",
		"A",
		"B",
		"X",
		"Y",
		"L",
		"R",
		"Start"
	};

	std::string KeyTypeName[(int)InputType::Count]{
		"MoveLeft",
		"MoveRight",
		"RotateLeft",
		"RotateRight",
		"SoftDrop",
		"HardDrop",
		"SwapHold",
		"GameReturn",
		"GameBack"
	};

	std::string Path = "config.ini";

	std::vector<std::vector<std::string>> ConfigRole = {
		{"Back", "GameConfig", "KeyConfig", "UpdateCheck"},
		{"Back", "PlayerName", "UseGamePad", "ServerAddress", "ServerPort"},
		{"Back", "MoveLeft", "MoveRight", "RotateLeft", "RotateRight", "SoftDrop", "HardDrop", "SwapHold", "GameReturn", "GameBack"}
	};
	std::vector<std::string> KeyRole = { "KeyConfig", "PadConfig" };

	ConfigMode Mode = ConfigMode::None;

	std::string PlayerName = "NoName";
	bool UseGamePad = false;

	int Width = 10;
	int Height = 20;
	int NextCount = 5;
	double GravitySpeedRate = 1.0;

	std::string ServerAddress = "localhost";
	uint16_t ServerPort = 8080;

	int Keys[(int)InputType::Count] = { 65, 68, 74, 76, 83, 32, 160, 13, 8 };

	ini::IniFile myIni;

	bool Load() {

		if (!fs::exists(Path)) {
			MessageBox(NULL, TEXT("\"config.ini\" could not be found."), TEXT("Error"), MB_ICONERROR);
			return false;
		}

		myIni.load(Path);

		PlayerName = myIni["General"]["PlayerName"].as<std::string>();
		UseGamePad = myIni["General"]["UseGamePad"].as<bool>();
		ServerAddress = myIni["General"]["ServerAddress"].as<std::string>();
		ServerPort = myIni["General"]["ServerPort"].as<uint16_t>();

		Width = myIni["GameRule"]["Width"].as<int>();
		Height = myIni["GameRule"]["Height"].as<int>() + 4;
		NextCount = myIni["GameRule"]["NextCount"].as<int>();
		GravitySpeedRate = myIni["GameRule"]["GravitySpeedRate"].as<double>() - 0.0000625;

		if (!UseGamePad) {
			for (size_t i = (size_t)InputType::MoveLeft; i < (size_t)InputType::Count; i++) {
				Keys[i] = myIni["KeyConfig"][KeyTypeName[i]].as<int>();
			}
		}
		else {
			for (size_t i = (size_t)InputType::MoveLeft; i < (size_t)InputType::Count; i++) {
				Keys[i] = PadInputList[myIni["PadConfig"][KeyTypeName[i]].as<int>()];
			}
		}

		return true;
	}

	void Save() {
		myIni.save(Path);
		Load();
	}

	template<class T>
	void InputString(T& data) {
		char inputstr[256];
		KeyInputString(64, 64, 256, inputstr, FALSE);
		std::string str(inputstr);
		data = str.empty() ? data : str;
		Save();
	}
	template<class T>
	void InputBool(T& data) {
		data = !data.as<bool>();
		Save();
	}
	template<class T>
	void InputKey(T& data) {
		WaitKey();
		for (int i = 0; i < 256; i++) {
			if (GetAsyncKeyState(i) & 0x8000) {
				data = i;
				break;
			}
		}
		Save();
	}
	template<class T>
	void InputPad(T& data) {
		WaitKey();
		for (int i = 0; i < 11; i++) {
			if (GetJoypadInputState(DX_INPUT_PAD1) & PadInputList[i]) {
				data = i;
				break;
			}
		}
		Save();
	}
};