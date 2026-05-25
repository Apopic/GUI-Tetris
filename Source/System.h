#pragma once
#include <array>
#include <deque>
#include <cstdint>
#include <algorithm>
#include <random>
#include <bitset>
#include <fstream>

#include "Library/Socket/Socket.h"
#include "Library/ExLib.h"
#include "Library/inicpp.h"
#include "Library/Input.h"
#include "Library/Escape.h"
#include "Library/timer.hpp"
#include "winsparkle.h"

enum class Myno : uint8_t {
	Null,
	I,
	O,
	S,
	Z,
	J,
	L,
	T,
	Gabage,
	PlaceBit = 0x40,
	pI,
	pO,
	pS,
	pZ,
	pJ,
	pL,
	pT,
	pGabage,
	DummyBit = 0x80,
	dI,
	dO,
	dS,
	dZ,
	dJ,
	dL,
	dT,
	dGabage,
};

enum class Direction : uint8_t {
	Upper,
	Right,
	Lower,
	Left,
	Count
};

enum struct Scene : uint8_t {
	Title,
	Menu,
	Lobby,
	Playing,
	Result,
	Config,
	Count
};

enum class InputType : uint8_t {
	Null = -1,
	MoveLeft,
	MoveRight,
	RotateLeft,
	RotateRight,
	SoftDrop,
	HardDrop,
	SwapHold,
	GameUpKey,
	GameDownKey,
	GameReturn,
	GameBack,
	Count
};

enum class MenuType : uint8_t {
	None,
	GameConfig,
	KeyConfig
};

#define _def_logic_op(t)\
static inline Myno operator##t##(Myno lhs, Myno rhs) { return static_cast<Myno>((uint8_t)lhs t (uint8_t)rhs); }\
static inline Myno operator##t##(Myno lhs, uint8_t rhs) { return static_cast<Myno>((uint8_t)lhs t rhs); }\
static inline Myno operator##t##(uint8_t lhs, Myno rhs) { return static_cast<Myno>(lhs t (uint8_t)rhs); }

_def_logic_op(&);
_def_logic_op(| );
_def_logic_op(^);
#undef _def_logic_op
#undef min
#undef max

namespace fs = std::filesystem;
static const inline std::string None = "  ";

class MynoObject {
	using inner_field = std::array<std::bitset<4>, 4>;

	Direction Dir = Direction::Upper;
	Myno Type = Myno::Null;
	inner_field Collision{};
	int x = 0;
	int y = 0;

	inner_field LRotateImpl() {
		auto [w, h] = GetFieldSize();
		inner_field ret;
		for (int j = 0; j < h; ++j) {
			for (int i = 0; i < w; ++i) {
				ret[i][j] = Collision[j][w - 1 - i];
			}
		}
		Dir = RotateDirectionImpl(false);
		return ret;
	}
	inner_field RRotateImpl() {
		auto [w, h] = GetFieldSize();
		inner_field ret;
		for (int j = 0; j < h; ++j) {
			for (int i = 0; i < w; ++i) {
				ret[i][j] = Collision[h - 1 - j][i];
			}
		}
		Dir = RotateDirectionImpl(true);
		return ret;
	}

	Direction RotateDirectionImpl(bool LR) {
		constexpr int8_t P = static_cast<int8_t>(Direction::Count);
		int8_t ret = static_cast<int8_t>(Dir);
		ret = ((ret + (int8_t)std::copysign(1, (int)(!LR) * -1)) % P + P) % P;
		return static_cast<Direction>(ret);
	}

	MynoObject(Myno t) : Type(t) {}

public:

	MynoObject() = default;
	MynoObject(const MynoObject&) = default;
	MynoObject(MynoObject&&) = default;
	MynoObject& operator=(const MynoObject&) = default;
	MynoObject& operator=(MynoObject&&) = default;

	static MynoObject I() {
		MynoObject ret(Myno::I);
		ret.Collision = {
			0b0000,
			0b1111,
			0b0000,
			0b0000
		};
		return ret;
	}
	static MynoObject O() {
		MynoObject ret(Myno::O);
		ret.Collision = {
			0b0000,
			0b0110,
			0b0110,
			0b0000
		};
		return ret;
	}
	static MynoObject S() {
		MynoObject ret(Myno::S);
		ret.Collision = {
			0b0110,
			0b0011,
			0b0000,
			0b0000
		};
		return ret;
	}
	static MynoObject Z() {
		MynoObject ret(Myno::Z);
		ret.Collision = {
			0b0011,
			0b0110,
			0b0000,
			0b0000
		};
		return ret;
	}
	static MynoObject J() {
		MynoObject ret(Myno::J);
		ret.Collision = {
			0b0001,
			0b0111,
			0b0000,
			0b0000
		};
		return ret;
	}
	static MynoObject L() {
		MynoObject ret(Myno::L);
		ret.Collision = {
			0b0100,
			0b0111,
			0b0000,
			0b0000
		};
		return ret;
	}
	static MynoObject T() {
		MynoObject ret(Myno::T);
		ret.Collision = {
			0b0010,
			0b0111,
			0b0000,
			0b0000
		};
		return ret;
	}
	static MynoObject Make(Myno t) {
		switch (t) {
		case Myno::I: return I();
		case Myno::O: return O();
		case Myno::S: return S();
		case Myno::Z: return Z();
		case Myno::J: return J();
		case Myno::L: return L();
		case Myno::T: return T();
		}
		return MynoObject(t);
	}

	void MoveLeft() {
		x -= 1;
	}
	void MoveRight() {
		x += 1;
	}
	void MoveDown() {
		y += 1;
	}
	void MoveUp() {
		y -= 1;
	}
	void Rotate(bool LR) {
		Collision = (!LR) ? (LRotateImpl()) : (RRotateImpl());
	}
	void RotateLeft() {
		Rotate(false);
	}
	void RotateRight() {
		Rotate(true);
	}
	void SetPosition(int _x, int _y) {
		x = _x; y = _y;
	}
	void AddPosition(int _x, int _y) {
		x += _x; y += _y;
	}
	std::pair<int, int> GetPosition() const {
		return { x, y };
	}
	std::pair<int, int> GetFieldSize() const {
		return (Type == Myno::I || Type == Myno::O) ? std::pair{ 4, 4 } : std::pair{ 3, 3 };
	}
	const inner_field& GetCollision() const {
		return Collision;
	}
	Myno GetType() const {
		return Type;
	}
	template<class T = Direction>
	T GetDirection() const {
		return static_cast<T>(Dir);
	}
	template<class T = Direction>
	T GetDirectionPrevRight() const {
		constexpr int8_t P = static_cast<int8_t>(Direction::Count);
		return static_cast<T>(((GetDirection<int8_t>() - 1) % P + P) % P);
	}
	template<class T = Direction>
	T GetDirectionPrevLeft() const {
		constexpr int8_t P = static_cast<int8_t>(Direction::Count);
		return static_cast<T>(((GetDirection<int8_t>() + 1) % P + P) % P);
	}
	static void GetColor(Myno t) {
		auto m = t & ~(uint8_t)(Myno::DummyBit | Myno::PlaceBit);
		switch (m) {
		case Myno::Null:
			break;
		case Myno::I:
			SetDrawBright(0, 255, 255);
			break;
		case Myno::O:
			SetDrawBright(255, 255, 0);
			break;
		case Myno::S:
			SetDrawBright(0, 255, 0);
			break;
		case Myno::Z:
			SetDrawBright(255, 0, 0);
			break;
		case Myno::J:
			SetDrawBright(0, 0, 255);
			break;
		case Myno::L:
			SetDrawBright(255, 165, 0);
			break;
		case Myno::T:
			SetDrawBright(255, 0, 255);
			break;
		case Myno::Gabage:
			SetDrawBright(128, 128, 128);
			break;
		}
	}
};

class _Skin {

public:

	struct _Menu {

		struct _Config {
			Pos2D<float> MenuPos = { 640,160 };
			Pos2D<float> BoxInterval = { 0,240 };
		} Config;

		struct _Image {
			GraphData BackGround;
			GraphData MenuBox;
		} Image;

		struct _Font {
			FontData Menu;
		} Font;

		struct _SE {
			SoundData Target;
			SoundData Select;
		} SE;

		struct _BGM {} BGM;

	} Menu;

	struct _Lobby {

		struct _Config {
			Pos2D<float> MenuPos = { 640,160 };
			Pos2D<float> BoxInterval = { 0,240 };
		} Config;

		struct _Image {
			GraphData BackGround;
			GraphData MenuBox;
		} Image;

		struct _Font {
			FontData Menu;
		} Font;

		struct _SE {
			SoundData Target;
			SoundData Select;
		} SE;

		struct _BGM {} BGM;

	} Lobby;

	struct _Playing {

		struct _Config {
			Pos2D<float> SystemStrPos = { 0,20 };
			Pos2D<float> EnemyNamePos = { 995,200 };
			Pos2D<std::vector<float>> ScorePos = { {80, 80, 80, 80, 80} ,{632, 660, 688, 716, 744} };
			Pos2D<std::vector<float>> ResultPos = { {640, 640, 640, 640, 640, 640, 640}, { 100, 200, 300, 400, 500, 600, 700 } };
			Pos2D<float> EnemyFrameInterval = { 110, 200 };
		} Config;

		struct _Image {
			GraphData BackGround;
			GraphData FadeBG;
			GraphData MainMyno;
			GraphData EnemyMyno;
			GraphData HoldMyno;
			GraphData QueueMyno;
			GraphData MainFrame;
			GraphData EnemyFrame;
			GraphData HoldFrame;
			GraphData QueueFrame;
			GraphData DamageGauge;
		} Image;

		struct _Font {
			FontData PlayData;
			FontData ResultData;
			FontData SystemStr;
			FontData EnemyName;
		} Font;

		struct _SE {
			SoundData Rotate;
			SoundData HardDrop;
			SoundData ClearLine;
			SoundData Tetris;
			SoundData Damage;
		} SE;

		struct _BGM {
			std::vector<SoundData> BGMList;
		} BGM;

	} Playing;

	struct _Config {

		struct __Config {
			Pos2D<float> MenuPos = { 640,120 };
			Pos2D<float> BoxInterval = { 0,200 };
		} Config;

		struct _Image {
			GraphData BackGround;
			GraphData MenuBox;
		} Image;

		struct _Font {
			FontData Menu;
		} Font;

		struct _SE {
			SoundData Target;
			SoundData Select;
		} SE;

	} Config;

	void Load() {

		std::string SkinFilePath = "Skin";

		std::ifstream ifs("Skin/SkinConfig.json");
		json data = json::parse(ifs);
		ifs.close();

		std::string SkinDir = SkinFilePath + "/";

		const json& Menudata = data["Menu"];
		const json& Lobbydata = data["Lobby"];
		const json& Playingdata = data["Playing"];
		const json& Configdata = data["Config"];

#define ValLoad(base, type, keyname) base.type.keyname = base##data[#type].value(#keyname, base.type.keyname)
#define DataLoad(base, type, keyname) base.type.keyname.Load(SkinDir, base##data[#type][#keyname])

#pragma region Menu

		ValLoad(Menu, Config, MenuPos);
		ValLoad(Menu, Config, BoxInterval);

		DataLoad(Menu, Image, BackGround);
		DataLoad(Menu, Image, MenuBox);

		DataLoad(Menu, SE, Target);
		DataLoad(Menu, SE, Select);

		DataLoad(Menu, Font, Menu);

#pragma endregion

#pragma region Lobby

		ValLoad(Lobby, Config, MenuPos);
		ValLoad(Lobby, Config, BoxInterval);

		DataLoad(Lobby, Image, BackGround);
		DataLoad(Lobby, Image, MenuBox);

		DataLoad(Lobby, SE, Target);
		DataLoad(Lobby, SE, Select);

		DataLoad(Lobby, Font, Menu);

#pragma endregion

#pragma region Playing

		ValLoad(Playing, Config, ScorePos);
		ValLoad(Playing, Config, ResultPos);
		ValLoad(Playing, Config, SystemStrPos);
		ValLoad(Playing, Config, EnemyNamePos);
		ValLoad(Playing, Config, EnemyFrameInterval);

		DataLoad(Playing, Image, BackGround);
		DataLoad(Playing, Image, FadeBG);
		DataLoad(Playing, Image, MainMyno);
		DataLoad(Playing, Image, EnemyMyno);
		DataLoad(Playing, Image, HoldMyno);
		DataLoad(Playing, Image, QueueMyno);
		DataLoad(Playing, Image, MainFrame);
		DataLoad(Playing, Image, EnemyFrame);
		DataLoad(Playing, Image, HoldFrame);
		DataLoad(Playing, Image, QueueFrame);
		DataLoad(Playing, Image, DamageGauge);

		DataLoad(Playing, SE, Rotate);
		DataLoad(Playing, SE, HardDrop);
		DataLoad(Playing, SE, ClearLine);
		DataLoad(Playing, SE, Tetris);
		DataLoad(Playing, SE, Damage);

		SetCreateSoundDataType(DX_SOUNDDATATYPE_FILE);
		for (auto&& bgm : Playingdata["BGM"]["BGMList"].get<std::vector<SoundData>>()) {
			SoundData BGM;
			BGM.Load(SkinDir + bgm.FilePath, bgm.Volume);
			Playing.BGM.BGMList.push_back(BGM);
		}
		SetCreateSoundDataType(DX_SOUNDDATATYPE_MEMNOPRESS);

		DataLoad(Playing, Font, PlayData);
		DataLoad(Playing, Font, ResultData);
		DataLoad(Playing, Font, SystemStr);
		DataLoad(Playing, Font, EnemyName);

#pragma endregion

#pragma region Config

		ValLoad(Config, Config, MenuPos);
		ValLoad(Config, Config, BoxInterval);

		DataLoad(Config, Image, BackGround);
		DataLoad(Config, Image, MenuBox);

		DataLoad(Config, SE, Target);
		DataLoad(Config, SE, Select);

		DataLoad(Config, Font, Menu);

#pragma endregion

#undef ValLoad
#undef DataLoad
#undef BGMLoad

	}
};

struct PlayerData {
	std::string PlayerName = "NoName";
	short State = 0;
	bool IsHost = false;

	Packet::bytearray ToBytes() const {
		Packet::bytearray ret;
		Packet::StoreBytes(ret, PlayerName);
		Packet::StoreBytes(ret, State);
		Packet::StoreBytes(ret, IsHost);
		return ret;
	}

	Packet::byte_view FromBytes(Packet::byte_view view) {
		Packet::LoadBytes(view, PlayerName);
		Packet::LoadBytes(view, State);
		Packet::LoadBytes(view, IsHost);
		return view;
	}
};

struct GameRule {
	int Width = 10;
	int Height = 20;
	int NextCount = 5;
	double GravitySpeedRate = 1.0;
	double DamageRate = 1.0;
};

struct GameData {
	std::vector<std::vector<Myno>> Board;
	std::vector<PlayerData> PlayerDatas = std::vector<PlayerData>(1);
	Scene NowScene = Scene::Menu;
	GameRule Rule = GameRule();
	int Attack = 0;
	int Damage = 0;
	int MyIndex = 0;
	int RecvIndex = -1;

	Packet::bytearray ToBytes() const {
		Packet::bytearray ret;
		uint32_t size = Board.size();
		Packet::StoreBytes(ret, size);
		for (auto& elem : Board) {
			Packet::StoreBytes(ret, elem);
		}
		Packet::StoreBytes(ret, PlayerDatas);
		Packet::StoreBytes(ret, NowScene);
		Packet::StoreBytes(ret, Rule);
		Packet::StoreBytes(ret, Attack);
		Packet::StoreBytes(ret, Damage);
		Packet::StoreBytes(ret, MyIndex);
		Packet::StoreBytes(ret, RecvIndex);
		return ret;
	}

	Packet::byte_view FromBytes(Packet::byte_view view) {
		uint32_t size = 0;
		Packet::LoadBytes(view, size);
		Board.reserve(size);
		for (size_t i = 0; i < size; ++i) {
			std::vector<Myno> elem;
			Packet::LoadBytes(view, elem);
			Board.push_back(std::move(elem));
		}
		Packet::LoadBytes(view, PlayerDatas);
		Packet::LoadBytes(view, NowScene);
		Packet::LoadBytes(view, Rule);
		Packet::LoadBytes(view, Attack);
		Packet::LoadBytes(view, Damage);
		Packet::LoadBytes(view, MyIndex);
		Packet::LoadBytes(view, RecvIndex);
		return view;
	}
};

class _Game {

	libarrier::Timer InGameTimer;
	InputFlag Keyboard[256]{};

	_Skin Skin;

	ScreenInput Screen;

	MenuType MenuType = MenuType::None;
	Scene NowScene = Scene::Title;

	bool Targeted = false;
	bool Selected = false;
	int SelectIndex = -1;
	float Current_Scroll = 0.0f;

	bool EndFlag = false;
	bool GameEnd = false;

	// StateType
	// 0 = Down
	// 1 = Press
	// 2 = Up
	bool CheckKey(InputType type, int statetype = 0, bool statesave = false) {
		bool keyresult = false;
		int keytype = UseGamePad ? (int)type : Keys[(int)type];
		switch (statetype) {
		case 0:
			keyresult = Keyboard[keytype].Down();
			break;
		case 1:
			keyresult = Keyboard[keytype].Press();
			break;
		case 2:
			keyresult = Keyboard[keytype].Up();
			break;
		}
		if (statesave) {
			if (keyresult) {
				MoveState = keytype;
			}
		}
		return keyresult;
	}
	bool CheckKeyState(InputType type, char state) const {
		int keytype = UseGamePad ? (int)type : Keys[(int)type];
		return state == keytype;
	}

	TCPSocket Socket;
	std::vector<std::vector<std::vector<Myno>>> Boards;
	GameData ShareData = GameData();

	bool MultiPlay = false;
	bool ServerConnected = false;

	void Send() {
		Socket.Send(Packet(ShareData));
	}
	bool Recv() {
		if (Socket.Available() <= 0) { return false; }
		ShareData = *Socket.Recv()->Get<GameData>();
		return true;
	}
	bool CheckState() {
		return ShareData.PlayerDatas[ShareData.MyIndex].State > 0;
	}
	void SwitchState(short val) {
		ShareData.PlayerDatas[ShareData.MyIndex].State = val;
	}

	std::vector<std::vector<std::string>> ConfigMenu = {
		{"Back", "GameConfig", "KeyConfig", "UpdateCheck"},
		{"Back", "PlayerName", "UseGamePad", "ServerAddress", "ServerPort", "SoftDropRate"},
		{"Back", "MoveLeft", "MoveRight", "RotateLeft", "RotateRight", "SoftDrop", "HardDrop", "SwapHold", "GameUpKey", "GameDownKey", "GameReturn", "GameBack"}
	};
	std::vector<std::string> KeyMenu = { "KeyConfig", "PadConfig" };

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
		"GameUpKey",
		"GameDownKey",
		"GameReturn",
		"GameBack"
	};

	std::string Path = "config.ini";

	std::string PlayerName = "NoName";
	bool UseGamePad = false;
	double SoftDropRate = 0.03125;

	int Width = 10;
	int Height = 20;
	int NextCount = 5;
	double GravitySpeedRate = 1.0;
	double DamageRate = 1.0;

	std::string ServerAddress = "localhost";
	uint16_t ServerPort = 8080;

	int Keys[(int)InputType::Count] = { 65, 68, 74, 76, 83, 32, 160, 38, 40, 13, 8 };

	ini::IniFile myIni;
	ini::IniFile PrevIni;

	bool ConfigLoad() {

		if (!fs::exists(Path)) {
			MessageBox(NULL, TEXT("\"config.ini\" could not be found."), TEXT("Error"), MB_ICONERROR);
			return false;
		}

		myIni.load(Path);

		PlayerName = myIni["General"]["PlayerName"].as<std::string>();
		UseGamePad = myIni["General"]["UseGamePad"].as<bool>();
		ServerAddress = myIni["General"]["ServerAddress"].as<std::string>();
		ServerPort = myIni["General"]["ServerPort"].as<uint16_t>();
		SoftDropRate = myIni["General"]["SoftDropRate"].as<double>();

		Width = myIni["GameRule"]["Width"].as<int>();
		Height = myIni["GameRule"]["Height"].as<int>() + 4;
		NextCount = myIni["GameRule"]["NextCount"].as<int>();
		GravitySpeedRate = myIni["GameRule"]["GravitySpeedRate"].as<double>() - 0.0000625;
		DamageRate = myIni["GameRule"]["DamageRate"].as<double>();

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

	void InputInit() {
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
		Draw(FALSE);
		SetDrawBlendMode(0, 0);
	}

	void InputEnd() {
		myIni.save(Path);
		ConfigLoad();
	}

	template<class T>
	void InputString(T& data, size_t length = 256) {
		InputInit();
		SetKeyInputStringFont(Skin.Menu.Font.Menu.Handle);
		char inputstr[256];
		KeyInputString(64, 64, length, inputstr, FALSE);
		std::string str(inputstr);
		data = str.empty() ? data : str;
		InputEnd();
	}
	template<class T>
	void InputInt(T& data, size_t length = 256, int min = std::numeric_limits<int>::min(), int max = std::numeric_limits<int>::min()) {
		InputInit();
		SetKeyInputStringFont(Skin.Menu.Font.Menu.Handle);
		char inputstr[256];
		KeyInputString(64, 64, length, inputstr, FALSE);
		std::string str(inputstr);
		bool is_digit = !str.empty() && std::all_of(str.begin(), str.end(), [](unsigned char c) { return std::isdigit(c); });
		data = !is_digit ? data : std::to_string(std::clamp(std::stoi(str), min, max));
		InputEnd();
	}
	template<class T>
	void InputDouble(T& data, size_t length = 256, double min = std::numeric_limits<double>::min(), double max = std::numeric_limits<double>::max()) {
		InputInit();
		SetKeyInputStringFont(Skin.Menu.Font.Menu.Handle);
		char inputstr[256];
		KeyInputString(64, 64, length, inputstr, FALSE);
		std::string str(inputstr);
		bool has_dot = false;
		bool is_digit = !str.empty() && std::all_of(str.begin(), str.end(), [&](unsigned char c) {
			if (c == '.') {
				if (has_dot) return false;
				has_dot = true;
				return true;
			}
			return (bool)std::isdigit(c);
			});
		data = !is_digit ? data : std::to_string(std::clamp(std::stod(str), min, max));
		InputEnd();
	}
	template<class T>
	void InputBool(T& data) {
		InputInit();
		data = !data.as<bool>();
		InputEnd();
	}
	template<class T>
	void InputKey(T& data) {
		InputInit();
		WaitKey();
		for (int i = 0; i < 256; i++) {
			if (GetAsyncKeyState(i) & 0x8000) {
				data = i;
				break;
			}
		}
		InputEnd();
	}
	template<class T>
	void InputPad(T& data) {
		InputInit();
		WaitKey();
		for (int i = 0; i < 11; i++) {
			if (GetJoypadInputState(DX_INPUT_PAD1) & PadInputList[i]) {
				data = i;
				break;
			}
		}
		InputEnd();
	}
	void SendGameRule() {
		ShareData.Rule.Width = myIni["GameRule"]["Width"].as<int>();
		ShareData.Rule.Height = myIni["GameRule"]["Height"].as<int>();
		ShareData.Rule.NextCount = myIni["GameRule"]["NextCount"].as<int>();
		ShareData.Rule.GravitySpeedRate = myIni["GameRule"]["GravitySpeedRate"].as<double>();
		ShareData.Rule.DamageRate = myIni["GameRule"]["DamageRate"].as<double>();
		Send();
	}
	void RecvGameRule() {
		myIni["GameRule"]["Width"] = ShareData.Rule.Width;
		myIni["GameRule"]["Height"] = ShareData.Rule.Height;
		myIni["GameRule"]["NextCount"] = ShareData.Rule.NextCount;
		myIni["GameRule"]["GravitySpeedRate"] = ShareData.Rule.GravitySpeedRate;
		myIni["GameRule"]["DamageRate"] = ShareData.Rule.DamageRate;
		myIni.save(Path);
		ConfigLoad();
	}
	void ResetGameRule() {
		myIni["GameRule"]["Width"] = 10;
		myIni["GameRule"]["Height"] = 20;
		myIni["GameRule"]["NextCount"] = 5;
		myIni["GameRule"]["GravitySpeedRate"] = 1.0;
		myIni["GameRule"]["DamageRate"] = 1.0;
		myIni.save(Path);
		ConfigLoad();
	}

	void DrawConfig() {

		Skin.Config.Image.BackGround.Draw();

		std::string str;
		std::vector<bool> is_target = std::vector<bool>(ConfigMenu[(int)MenuType].size(), false);

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

			Skin.Config.Font.Menu.Draw({
				Skin.Config.Config.MenuPos.X,
				Skin.Config.Config.MenuPos.Y + Skin.Config.Config.BoxInterval.Y * (float)i + Current_Scroll },
				GetColor(255, 255, 255),
				GetColor(0, 0, 0),
				ConfigMenu[(int)MenuType][i]);

			if (MenuType == MenuType::None) { continue; }
			if (i == 0) { continue; }
			if (!is_target[i]) { continue; }
			if (MenuType == MenuType::GameConfig) {
				str = myIni["General"][ConfigMenu[(int)MenuType][i]].as<std::string>();
			}
			if (MenuType == MenuType::KeyConfig) {
				if (!UseGamePad) {
					char keyname[256];
					int keycode = myIni["KeyConfig"][ConfigMenu[(int)MenuType][i]].as<int>();
					UINT scancode = MapVirtualKey(keycode, MAPVK_VK_TO_VSC);
					GetKeyNameText(scancode << 16, keyname, sizeof(keyname));
					str = keyname;
				}
				else {
					str = PadInputName[myIni["PadConfig"][ConfigMenu[(int)MenuType][i]].as<int>()];
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

	void ProcConfig() {

		float minpos = Skin.Config.Config.BoxInterval.Y * (ConfigMenu[(int)MenuType].size() - ConfigMenu[(int)MenuType::None].size());
		float addscr = GetMouseWheelRotVolF() * Skin.Config.Config.BoxInterval.Y;

		bool is_up = CheckKey(InputType::GameUpKey);
		bool is_down = CheckKey(InputType::GameDownKey);
		int dir = is_down - is_up;

		if (is_up || is_down) {
			addscr = Skin.Config.Config.BoxInterval.Y * (float)-dir;
			addscr *= SelectIndex < 1 || SelectIndex >(ConfigMenu[(int)MenuType].size() - 2) ? 0 : 1;
			int prev = SelectIndex;
			SelectIndex = std::clamp(SelectIndex + dir, 0, (int)ConfigMenu[(int)MenuType].size() - 1);
			if (prev != SelectIndex) {
				Targeted = false;
			}
		}
		Current_Scroll = Screen.GetScrollPos(addscr, -minpos, 0.0f);

		for (size_t i = 0; i < ConfigMenu[(int)MenuType].size(); i++) {

			bool click = Skin.Config.Image.MenuBox.IsClick(MOUSE_INPUT_LEFT, { 0, Skin.Config.Config.BoxInterval.Y * (float)i + Current_Scroll });
			bool is_return = CheckKey(InputType::GameReturn);

			if (click || is_return) {
				if (Selected) { break; }
				if (!click && is_return) { i = SelectIndex; }
				Selected = true;
				Targeted = false;
				Skin.Config.SE.Select.Play();

				auto Prev = MenuType;

				if (MenuType == MenuType::None) {
					switch (i) {
					case 0:
						NowScene = Scene::Menu;
						break;
					case 1:
						MenuType = MenuType::GameConfig;
						break;
					case 2:
						MenuType = MenuType::KeyConfig;
						break;
					case 3:
						win_sparkle_check_update_with_ui();
						break;
					}
				}
				else if (MenuType == MenuType::GameConfig) {
					switch (i) {
					case 0:
						MenuType = MenuType::None;
						break;
					case 2:
						InputBool(myIni["General"][ConfigMenu[1][2]]);
						break;
					case 4:
						InputInt(myIni["General"][ConfigMenu[1][i]], 5, 0, 65535);
						break;
					case 5:
						InputDouble(myIni["General"][ConfigMenu[1][i]], 9, 0.0, 1.0);
						break;
					default:
						InputString(myIni["General"][ConfigMenu[1][i]]);
						break;
					}
				}
				else if (MenuType == MenuType::KeyConfig) {
					switch (i) {
					case 0:
						MenuType = MenuType::None;
						break;
					default:
						if (!UseGamePad) {
							InputKey(myIni["KeyConfig"][ConfigMenu[2][i]]);
						}
						else {
							InputPad(myIni["PadConfig"][ConfigMenu[2][i]]);
						}
						break;
					}
				}

				if (Prev != MenuType) {
					Screen.ResetScrollPos();
					Current_Scroll = 0.0f;
					SelectIndex = -1;
				}

				Skin.Config.SE.Select.Play();

				break;
			}
			else if (i == (ConfigMenu[(int)MenuType].size() - 1)) {
				Selected = false;
			}
		}
	}

	void DrawTitle() {}
	void ProcTitle() {
		ConfigLoad();
		Skin.Load();
		NowScene = Scene::Menu;
	}

	std::vector<std::string> Menu = { "SinglePlay", "MultiPlay", "Config" };

	void DrawMenu() {

		Skin.Menu.Image.BackGround.Draw();

		std::vector<bool> is_target = std::vector<bool>(3, false);

		for (size_t i = 0; i < Menu.size(); i++) {
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

		for (size_t i = 0; i < Menu.size(); i++) {

			SetDrawBright(
				128 * is_target[i] + (i == 1 ? 255 : 0),
				128 * is_target[i] + (i == 2 ? 255 : 0),
				128 * is_target[i] + (i == 0 ? 255 : 0)
			);
			Skin.Menu.Image.MenuBox.Draw({ 0, (float)(Skin.Menu.Config.BoxInterval.Y * i) });
			SetDrawBright(255, 255, 255);

			Skin.Menu.Font.Menu.Draw({
			Skin.Menu.Config.MenuPos.X,
			Skin.Menu.Config.MenuPos.Y + (int)(Skin.Menu.Config.BoxInterval.Y * i) },
			GetColor(255, 255, 255),
			GetColor(0, 0, 0),
			Menu[i]);
		}
	}

	void ProcMenu() {

		bool is_up = CheckKey(InputType::GameUpKey);
		bool is_down = CheckKey(InputType::GameDownKey);
		int dir = is_down - is_up;

		if (is_up || is_down) {
			int prev = SelectIndex;
			SelectIndex = std::clamp(SelectIndex + dir, 0, (int)Menu.size() - 1);
			if (prev != SelectIndex) {
				Targeted = false;
			}
		}

		for (size_t i = 0; i < Menu.size(); i++) {

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

	std::vector<std::string> LobbyMenu = { "Back", "GameStart", "ResetGameRule", "Width", "Height", "NextCount", "GravitySpeedRate", "DamageRate" };
	std::vector<std::string> Rule = { "Width", "Height", "NextCount", "GravitySpeedRate" };

	void DrawLobby() {

		Skin.Lobby.Image.BackGround.Draw();
		StopBGM();

		std::vector<bool> is_target = std::vector<bool>(LobbyMenu.size(), false);

		for (int i = 0; i < LobbyMenu.size(); i++) {
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

		for (int i = 0; i < LobbyMenu.size(); i++) {

			int base = 200;
			if (MultiPlay) {
				if (ServerConnected) {
					if (i >= 1 && !ShareData.PlayerDatas[ShareData.MyIndex].IsHost) {
						base = 100;
					}
					if (i == 1 && ShareData.PlayerDatas.size() < 2) {
						base = 100;
					}
				}
			}
			else {
				if (i == 7) {
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
				LobbyMenu[i]);

			if (i <= 2) { continue; }
			if (!is_target[i]) { continue; }

			Skin.Lobby.Font.Menu.Draw({
				Skin.Lobby.Config.MenuPos.X,
				Skin.Lobby.Config.MenuPos.Y + Skin.Lobby.Config.BoxInterval.Y / 2.0f + Skin.Lobby.Config.BoxInterval.Y * (float)i + Current_Scroll },
				GetColor(255, 255, 255),
				GetColor(0, 0, 0),
				myIni["GameRule"][LobbyMenu[i]].as<std::string>());
		}

		if (MultiPlay) {

			std::string str;

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

	void ProcLobby() {

		float minpos = Skin.Lobby.Config.BoxInterval.Y * (LobbyMenu.size() - 4);
		float addscr = GetMouseWheelRotVolF() * Skin.Lobby.Config.BoxInterval.Y;

		bool is_up = CheckKey(InputType::GameUpKey);
		bool is_down = CheckKey(InputType::GameDownKey);
		int dir = is_down - is_up;

		if (is_up || is_down) {
			addscr = Skin.Lobby.Config.BoxInterval.Y * (float)-dir;
			addscr *= SelectIndex < 1 || SelectIndex >(LobbyMenu.size() - 2) ? 0 : 1;
			int prev = SelectIndex;
			SelectIndex = std::clamp(SelectIndex + dir, 0, (int)LobbyMenu.size() - 1);
			if (prev != SelectIndex) {
				Targeted = false;
			}
		}
		Current_Scroll = Screen.GetScrollPos(addscr, -minpos, 0.0f);

		for (size_t i = 0; i < LobbyMenu.size(); i++) {

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
				else {
					if (i == 7) { continue; }
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
					InputInt(myIni["GameRule"][Rule[i - 3]], 2, 4, 10);
					break;
				case 4:
					InputInt(myIni["GameRule"][Rule[i - 3]], 2, 4, 20);
					break;
				case 5:
					InputInt(myIni["GameRule"][Rule[i - 3]], 1, 1, 5);
					break;
				case 6:
					InputDouble(myIni["GameRule"][Rule[i - 3]], 9, 0.0, 10.0);
					break;
				case 7:
					InputDouble(myIni["GameRule"][Rule[i - 3]], 9, 0.0, 2.0);
					break;
				}

				if (ServerConnected && i != 0) {
					SendGameRule();
				}

				Skin.Lobby.SE.Select.Play();

				break;
			}
			else if (i == (LobbyMenu[(int)MenuType].size() - 1)) {
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

				if (Socket.Connect(IPAddress::SolveHostName(ServerAddress)->Port(ServerPort))) {
					ShareData.PlayerDatas.front().PlayerName = PlayerName;
					Send();
				}
				ServerConnected = Recv();

				if (ServerConnected) {
					PrevIni = myIni;
				}
			}
			else {

				if (NowScene == Scene::Menu) {
					myIni = PrevIni;
					PrevIni = ini::IniFile();
					myIni.save(Path);
					ConfigLoad();
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

	uint64_t Score = 0;
	uint64_t Line = 0;
	uint32_t Combo = 0;
	uint32_t B2B = 0;
	uint32_t Damage = 0;
	bool Spin = false;
	int Attack = 0;
	int RENTable[10]{ 0,1,1,2,2,3,3,4,4,4 };

	std::deque<std::deque<Myno>> BagQueue;
	std::vector<std::vector<Myno>> Board;

	MynoObject Current;
	MynoObject Preview;
	libarrier::Timer LockTimer;
	double LockTime = 0.5;
	int LockCount = 0;
	libarrier::Timer GravityTimer;
	double GravityTime = 1;
	char MoveState = '\0';
	libarrier::Timer MoveTimer;
	double MoveTime = 0.15;

	MynoObject Hold;
	bool HoldOnce = false;

	std::deque<SoundData> BGMQueue;
	SoundData CurrentBGM;

	void DrawScore() {
		SetDrawBright(255, 255, 255);
		Skin.Playing.Font.PlayData.Draw(Skin.Playing.Config.ScorePos.GetPos<float>(0), GetColor(255, 255, 255), GetColor(0, 0, 0), rformat("Time:  {:0.3f}s", InGameTimer.GetElapsed().Second()));
		Skin.Playing.Font.PlayData.Draw(Skin.Playing.Config.ScorePos.GetPos<float>(1), GetColor(255, 255, 255), GetColor(0, 0, 0), rformat("Score: {:0>12}", Score));
		Skin.Playing.Font.PlayData.Draw(Skin.Playing.Config.ScorePos.GetPos<float>(2), GetColor(255, 255, 255), GetColor(0, 0, 0), rformat("Line:  {}", Line));
		Skin.Playing.Font.PlayData.Draw(Skin.Playing.Config.ScorePos.GetPos<float>(3), GetColor(255, 255, 255), GetColor(0, 0, 0), rformat("Combo: {}", Combo));
		Skin.Playing.Font.PlayData.Draw(Skin.Playing.Config.ScorePos.GetPos<float>(4), GetColor(255, 255, 255), GetColor(0, 0, 0), rformat("B2B:   {}", B2B));
	}
	void MakeMynoSets() {
		static std::random_device device;
		std::mt19937 gen(device());

		std::deque<Myno> ret = { Myno::I, Myno::O, Myno::S, Myno::Z, Myno::J, Myno::L, Myno::T };
		std::shuffle(ret.begin(), ret.end(), gen);
		BagQueue.push_back(std::move(ret));
	}
	Myno GetMynoQueue() {
		auto& frontbag = BagQueue.front();
		auto ret = std::move(frontbag.front());
		frontbag.pop_front();
		if (frontbag.empty()) {
			BagQueue.pop_front();
			MakeMynoSets();
		}
		return ret;
	}

	bool CheckInBoard(const MynoObject& obj) {
		auto [w, h] = obj.GetFieldSize();
		auto [x, y] = obj.GetPosition();
		auto& c = obj.GetCollision();
		for (int j = 0; j < h; ++j) {
			for (int i = 0; i < w; ++i) {
				if (!c[j][i]) {
					continue;
				}
				if (!(0 <= x + i && x + i < Width)) {
					return false;
				}
				if (!(0 <= y + j && y + j < Height)) {
					return false;
				}
				auto& ref = Board[y + j][x + i];
				if (!(ref == Myno::Null || ref == obj.GetType() || (bool)(ref & Myno::DummyBit))) {
					return false;
				}
			}
		}
		return true;
	}
	int PlaceBoard(const MynoObject& obj, bool place, bool clear, bool preview) {
		auto [w, h] = obj.GetFieldSize();
		auto [x, y] = obj.GetPosition();
		auto& c = obj.GetCollision();
		int nearcount = 0;
		for (int j = 0; j < h; ++j) {
			for (int i = 0; i < w; ++i) {
				if (!(0 <= x + i && x + i < Width && 0 <= y + j && y + j < Height)) {
					nearcount += 1;
					continue;
				}
				auto& ref = Board[y + j][x + i];
				if ((bool)(ref & Myno::PlaceBit)) {
					nearcount += 1;
				}
				if (!c[j][i]) {
					continue;
				}
				if (clear) {
					ref = Myno::Null;
					continue;
				}
				if (place) {
					ref = obj.GetType() | Myno::PlaceBit;
					continue;
				}
				if (preview) {
					ref = obj.GetType() | Myno::DummyBit;
					continue;
				}
				ref = obj.GetType();
			}
		}
		return nearcount;
	}
	void ClearBoardPrev(const MynoObject& obj) {
		PlaceBoard(obj, false, true, false);
	}
	void PlaceBoard(const MynoObject& obj) {
		PlaceBoard(obj, false, false, false);
	}

	void SwapHold() {
		if (HoldOnce) {
			return;
		}
		HoldOnce = true;
		ClearBoardPrev(Current);
		Myno t = Current.GetType();
		SetCurrent(Hold.GetType());
		Hold = MynoObject::Make(t);
		if (Current.GetType() == Myno::Null) {
			ClearBoardPrev(Preview);
			Next();
			return;
		}
		CurrentPreview();
		PlaceBoard(Current);
		ResetLockTime();
	}

	int CalculateLine(int clearcount, int nearcount, bool perfect, int ren, bool spin, int b2b, Myno t) {
		if (perfect) { return 10; }
		int line = (t == Myno::T && spin) ? clearcount * 2 : std::max(0, clearcount - 1);
		line += b2b > 1 ? 1 : 0;
		line += (ren < 10 ? RENTable[ren] : 5);
		int damage = Damage;
		Damage = std::max(0, damage - line);
		return line;
	}
	void ApplyBoard(const MynoObject& obj) {
		HoldOnce = false;
		Myno t = Current.GetType();
		int nearcount = PlaceBoard(obj, true, false, false);
		int clearcount = LineClear();
		bool perfect = IsPerfect();
		if (clearcount == 0) {
			Score += 100;
			Combo = 0;
			LineApply();
			return;
		}
		clearcount == 4 ? Skin.Playing.SE.Tetris.Play() : Skin.Playing.SE.ClearLine.Play();
		Line += clearcount;
		Score += (1000 + ((int)perfect * 1000)) * clearcount;
		Score += 200 * Combo;
		Score += 200 * B2B;
		(clearcount == 4 || (t == Myno::T && nearcount >= 3 && Spin)) ? (B2B += 1) : (B2B = 0);
		Attack = CalculateLine(clearcount, nearcount, perfect, Combo, Spin, B2B, t);
		Spin = false;
		Combo += 1;
	}
	void PreviewBoard(const MynoObject& obj) {
		PlaceBoard(obj, false, false, true);
	}
	int LineClear() {
		std::deque<size_t> clearline;
		for (auto it = Board.rbegin(), end = Board.rend(); it != end; ++it) {
			auto& line = *it;
			bool clear = true;
			for (const auto& myno : line) {
				if ((bool)(myno & Myno::PlaceBit)) {
					continue;
				}
				if (myno == Myno::Null) {
					clear = false;
					break;
				}
				else if ((bool)(myno & Myno::DummyBit)) {
					clear = false;
					break;
				}
			}
			if (clear) {
				for (auto& myno : line) {
					myno = Myno::Null;
				}
				clearline.push_back(end - it - 1);
			}
		}
		int count = clearline.size();
		size_t offset = 0;
		while (!clearline.empty()) {
			size_t ibeg = clearline.front(); clearline.pop_front();
			size_t iend = (clearline.empty() ? 0 : clearline.front());
			for (size_t i = ibeg; i != iend; --i) {
				size_t idx = i + offset;
				if (idx >= Board.size()) {
					break;
				}
				std::swap(Board[idx], Board[idx - offset - 1]);
			}
			offset += 1;
		}
		return count;
	}
	void LineApply() {
		if (!MultiPlay) { return; }

		std::vector<Myno> GabageMyno = std::vector<Myno>(Width, Myno::Gabage);
		GabageMyno[GetRand(Width - 1)] = Myno::Null;

		if (Damage > 0) {
			Skin.Playing.SE.Damage.Play();
		}

		for (auto& d = Damage; d > 0; d--) {
			for (size_t i = 0; i < Board.size(); i++) {
				if (i >= Board.size() - 1) {
					for (size_t j = 0; j < GabageMyno.size(); j++) {
						Board[i][j] = GabageMyno[j];
					}
					break;
				}
				std::swap(Board[i], Board[i + 1]);
			}
		}
	}
	bool IsPerfect() {
		for (auto& line : Board) {
			for (auto& t : line) {
				if (t != Myno::Null) {
					return false;
				}
			}
		}
		return true;
	}

	using tableline = std::array<std::pair<int, int>, 4>;
	static constexpr auto SRSTableRight = std::array<tableline, 4>{
		tableline{std::pair{-1,0}, {-1,-1}, {0,+2}, {-1,+2}}, // upper -> right
		tableline{std::pair{+1,0}, {+1,+1}, {0,-2}, {+1,-2}}, // right -> lower
		tableline{std::pair{+1,0}, {+1,-1}, {0,+2}, {+1,+2}}, // lower -> left
		tableline{std::pair{-1,0}, {-1,+1}, {0,-2}, {-1,-2}}, // left  -> upper
		// left rotate is negative x
	};
	static constexpr auto SRSTableLeft = []() {
		std::decay_t<decltype(SRSTableRight)> ret;
		for (size_t j = 0; auto& tl : SRSTableRight) {
			for (size_t i = 0; auto& test : tl) {
				auto& [dx, dy] = ret[j][i];
				auto& [tx, ty] = test;
				std::tie(dx, dy) = std::pair{ -tx, ty };
				++i;
			}
			++j;
		}
		return ret;
		}();
	static constexpr auto SRSTableIMynoRight = std::array<tableline, 4>{
		tableline{std::pair{-2,0},{+1,0},{-2,+1},{+1,-2}},
		tableline{std::pair{-1,0},{+2,0},{-1,-2},{+2,+1}},
		tableline{std::pair{+2,0},{-1,0},{+2,+1},{-1,+2}},
		tableline{std::pair{+1,0},{-2,0},{+1,-2},{-2,-1}},
	};
	static constexpr auto SRSTableIMynoLeft = []() {
		std::decay_t<decltype(SRSTableIMynoRight)> ret;
		for (size_t j = 0; auto& tl : SRSTableIMynoRight) {
			for (size_t i = 0; auto& test : tl) {
				auto& [dx, dy] = ret[SRSTableIMynoRight.size() - 1 - j][i];
				auto& [tx, ty] = test;
				std::tie(dx, dy) = std::pair{ -tx, ty };
				++i;
			}
			++j;
		}
		return ret;
		}();
	int TableIndexConverter(bool LR, Direction cur) {
		int ret = [=]() {
			if (LR) {
				switch (cur) {
				case Direction::Upper: return 3;
				case Direction::Right: return 0;
				case Direction::Lower: return 1;
				case Direction::Left: return 2;
				}
			}
			else {
				switch (cur) {
				case Direction::Upper: return 3;
				case Direction::Right: return 2;
				case Direction::Lower: return 1;
				case Direction::Left: return 0;
				}
			}
			}();
		return ret;
	}
	void SetCurrent(Myno t) {
		Current = MynoObject::Make(t);
		auto [w, _] = Current.GetFieldSize();
		Current.SetPosition(Width / 2 - 2, 0);
		if (!CheckInBoard(Current)) {
			InGameTimer.Stop();
			EndFlag = true;
		}
	}
	void CurrentMoveLeft() {
		if (auto temp = Current; !CheckInBoard((temp.MoveLeft(), temp))) {
			return;
		}
		ClearBoardPrev(Current);
		Current.MoveLeft();
		CurrentPreview();
		PlaceBoard(Current);
		LockTimeInMove();
	}
	void CurrentMoveRight() {
		if (auto temp = Current; !CheckInBoard((temp.MoveRight(), temp))) {
			return;
		}
		ClearBoardPrev(Current);
		Current.MoveRight();
		CurrentPreview();
		PlaceBoard(Current);
		LockTimeInMove();
	}
	void CurrentRotateLeft() {
		Skin.Playing.SE.Rotate.Play();
		int x = 0;
		int y = 0;
		if (auto temp = Current; !CheckInBoard((temp.RotateLeft(), temp))) {
			int tableidx = TableIndexConverter(false, temp.GetDirection());
			auto& table = (temp.GetType() == Myno::I) ? SRSTableIMynoLeft : SRSTableLeft;
			bool testfound = false;
			for (auto [tx, ty] : table[tableidx]) {
				auto test = temp;
				test.AddPosition(tx, ty);
				if (CheckInBoard(test)) {
					x = tx;
					y = ty;
					testfound = true;
					break;
				}
			}
			if (!testfound) {
				return;
			}
		}
		ClearBoardPrev(Current);
		Current.AddPosition(x, y);
		Current.RotateLeft();
		CurrentPreview();
		PlaceBoard(Current);
		LockTimeInMove();
		CurrentSpinFlag();
	}
	void CurrentRotateRight() {
		Skin.Playing.SE.Rotate.Play();
		int x = 0;
		int y = 0;
		if (auto temp = Current; !CheckInBoard((temp.RotateRight(), temp))) {
			int tableidx = TableIndexConverter(true, temp.GetDirection());
			auto& table = (temp.GetType() == Myno::I) ? SRSTableIMynoRight : SRSTableRight;
			bool testfound = false;
			for (auto [tx, ty] : table[tableidx]) {
				auto test = temp;
				test.AddPosition(tx, ty);
				if (CheckInBoard(test)) {
					x = tx;
					y = ty;
					testfound = true;
					break;
				}
			}
			if (!testfound) {
				return;
			}
		}
		ClearBoardPrev(Current);
		Current.RotateRight();
		Current.AddPosition(x, y);
		CurrentPreview();
		PlaceBoard(Current);
		LockTimeInMove();
		CurrentSpinFlag();
	}
	void CurrentSoftDrop() {
		if (auto temp = Current; !CheckInBoard((temp.MoveDown(), temp))) {
			return;
		}
		ClearBoardPrev(Current);
		Current.MoveDown();
		PlaceBoard(Current);
		LockTimeInMove();
	}
	void CurrentHardDrop() {
		Skin.Playing.SE.HardDrop.Play();
		auto [x, y] = Current.GetPosition();
		while (true) {
			auto test = Current;
			test.SetPosition(x, y + 1);
			if (!CheckInBoard(test)) {
				break;
			}
			y += 1;
			if (y >= Height) {
				return;
			}
		}
		ClearBoardPrev(Current);
		Current.SetPosition(x, y);
		ApplyBoard(Current);
		Next();
	}
	void CurrentPreview(bool clear = true) {
		auto [x, y] = Current.GetPosition();
		while (true) {
			auto test = Current;
			test.SetPosition(x, y + 1);
			if (!CheckInBoard(test)) {
				break;
			}
			y += 1;
			if (y >= Height) {
				return;
			}
		}
		if (clear) { ClearBoardPrev(Preview); }
		Preview = Current;
		Preview.SetPosition(x, y);
		PreviewBoard(Preview);
	}
	void CurrentSpinFlag() {
		auto temp = Current;
		Spin = !CheckInBoard((temp.MoveDown(), temp));
	}
	void LockTimeInMove() {
		if (LockCount >= 15) {
			return;
		}
		else if (auto temp = Current; CheckInBoard((temp.MoveDown(), temp))) {
			LockTimer.Reset();
			return;
		}
		LockTimer.Start();
		LockCount += 1;
	}
	void ResetLockTime() {
		LockTimer.Reset();
		LockCount = 0;
		LockTimeInMove();
	}
	void Next() {
		SetCurrent(GetMynoQueue());
		CurrentPreview(false);
		PlaceBoard(Current);
		ResetLockTime();
		GravityTimer.Start();
	}

	void DrawBoard() {
		SetDrawBright(255, 255, 255);
		Size2D<float> ResizeRate{ Width / 10.0, (Height - 4) / 20.0 };
		Skin.Playing.Image.MainFrame.ResizeDraw({ 0,0 }, ResizeRate);
		Skin.Playing.Image.DamageGauge.RectDraw({ 0,0 }, { 0,0 },
			{ Skin.Playing.Image.DamageGauge.Size.Width, Skin.Playing.Image.MainMyno.Size.Height * -(float)(Damage) }, ResizeRate);
		Pos2D<float> offset = {
			std::round(Skin.Playing.Image.MainMyno.Size.Width * (10 - Width) * 0.5),
			std::round(Skin.Playing.Image.MainMyno.Size.Height * (20 - (Height - 4)) * 0.5)
		};
		for (int y = 0; y < Board.size(); y++) {
			for (int x = 0; x < Board[y].size(); x++) {
				if (Board[y][x] == Myno::Null) { continue; }
				if (Board[y][x] > Myno::DummyBit) { Skin.Playing.Image.MainMyno.Alpha = 100; }
				MynoObject::GetColor(Board[y][x]);
				Skin.Playing.Image.MainMyno.Draw({ x * Skin.Playing.Image.MainMyno.Size.Width + offset.X, y * Skin.Playing.Image.MainMyno.Size.Height + offset.Y });
				Skin.Playing.Image.MainMyno.Alpha = 255;
			}
		}
	}
	void DrawEnemy() {
		if (!MultiPlay) { return; }

		if (ShareData.RecvIndex != -1) {
			if (Boards.empty()) {
				Boards.resize(ShareData.PlayerDatas.size());
			}
			Boards[ShareData.RecvIndex] = ShareData.Board;
			ShareData.RecvIndex = -1;
		}

		auto boards = Boards;

		for (int i = 0, j = 0, l = 0; i < boards.size(); i++) {
			if (i == ShareData.MyIndex) { continue; }

			SetDrawBright(255, 255, 255);
			Pos2D<float> offset = {
				std::round(Skin.Playing.Image.EnemyMyno.Size.Width * (10 - ShareData.Rule.Width) * 0.5 + (Skin.Playing.Config.EnemyFrameInterval.X * l)),
				std::round(Skin.Playing.Image.EnemyMyno.Size.Height * (20 - (ShareData.Rule.Height - 4)) * 0.5 + (Skin.Playing.Config.EnemyFrameInterval.Y * (j % 4)))
			};

			Size2D<float> ResizeRate{ ShareData.Rule.Width / 10.0, (ShareData.Rule.Height - 4) / 20.0 };
			Skin.Playing.Image.EnemyFrame.ResizeDraw({ Skin.Playing.Config.EnemyFrameInterval.X * l, Skin.Playing.Config.EnemyFrameInterval.Y * (j % 4) }, ResizeRate);

			auto& board = boards[i];

			if (board.empty()) { continue; }
			for (int y = 0; y < board.size(); y++) {
				for (int x = 0; x < board[y].size(); x++) {
					if (board[y][x] == Myno::Null) { continue; }
					if (board[y][x] > Myno::DummyBit) { Skin.Playing.Image.EnemyMyno.Alpha = 100; }
					MynoObject::GetColor(board[y][x]);
					Skin.Playing.Image.EnemyMyno.Draw({
						x * Skin.Playing.Image.EnemyMyno.Size.Width + offset.X,
						y * Skin.Playing.Image.EnemyMyno.Size.Height + offset.Y
						});
					Skin.Playing.Image.EnemyMyno.Alpha = 255;
				}
			}

			SetDrawBright(255, 255, 255);
			Skin.Playing.Font.EnemyName.Draw({
				Skin.Playing.Image.EnemyMyno.Size.Width + Skin.Playing.Config.EnemyNamePos.X,
				Skin.Playing.Image.EnemyMyno.Size.Height + Skin.Playing.Config.EnemyNamePos.Y + offset.Y },
				GetColor(255, 255, 255),
				GetColor(0, 0, 0),
				ShareData.PlayerDatas[i].PlayerName);

			j++;
			if ((j % 4) == 0) {
				l++;
			}
		}
	}
	void DrawQueue() {
		SetDrawBright(255, 255, 255);
		for (int n = 0; n < NextCount; ++n) {
			Skin.Playing.Image.QueueFrame.Draw({ 0, n * Skin.Playing.Image.QueueFrame.Size.Height });
			Myno t = (n < BagQueue[0].size() ? BagQueue[0][n] : BagQueue[1][n - BagQueue[0].size()]);
			auto obj = MynoObject::Make(t);
			auto& c = obj.GetCollision();
			Pos2D<int> offset = {
				t != Myno::O && t != Myno::I ? 16 : 0,
				t != Myno::O && t != Myno::I ? 32 : t == Myno::I ? 16 : 0
			};
			for (int y = 0; y < c.size(); ++y) {
				for (int x = 0; x < c[y].size(); ++x) {
					if (!c[y][x]) { continue; }
					MynoObject::GetColor(t);
					Skin.Playing.Image.QueueMyno.Draw({
						x * Skin.Playing.Image.QueueMyno.Size.Width + offset.X,
						y * Skin.Playing.Image.QueueMyno.Size.Height + n * Skin.Playing.Image.QueueFrame.Size.Height + offset.Y
						});
				}
			}
			SetDrawBright(255, 255, 255);
		}
	}
	void DrawHold() {
		SetDrawBright(255, 255, 255);
		Skin.Playing.Image.HoldFrame.Draw();
		auto& c = Hold.GetCollision();
		Myno t = Hold.GetType();
		Pos2D<int> offset = {
				t != Myno::O && t != Myno::I ? 16 : 0,
				t != Myno::O && t != Myno::I ? 32 : t == Myno::I ? 16 : 0
		};
		for (int y = 0; y < c.size(); ++y) {
			for (int x = 0; x < c[y].size(); ++x) {
				if (!c[y][x]) {
					continue;
				}
				if (HoldOnce) {
					MynoObject::GetColor(Myno::Gabage);
				}
				else {
					MynoObject::GetColor(t);
				}
				Skin.Playing.Image.HoldMyno.Draw({
					x * Skin.Playing.Image.HoldMyno.Size.Width + offset.X,
					y * Skin.Playing.Image.HoldMyno.Size.Height + offset.Y });
			}
		}
	}
	void DrawPlaying() {
		Skin.Playing.Image.BackGround.Draw();
		PlayBGM();
		DrawBoard();
		DrawEnemy();
		DrawHold();
		DrawQueue();
		DrawScore();
	}
	void ProcPlaying() {

		bool IsBoardChange = false;

		if (!EndFlag) {
			if (LockTimer.GetElapsed().Second() > LockTime) {
				ApplyBoard(Current);
				Next();
				IsBoardChange = true;
			}
			if (GravityTimer.GetElapsed().Second() > GravityTime * (CheckKey(InputType::SoftDrop, 1) ? SoftDropRate : 1)) {
				GravityTimer.Start();
				CurrentSoftDrop();
				IsBoardChange = true;
			}
			if (InGameTimer.GetElapsed().Second() > 1) {
				GravityTime *= GravitySpeedRate;
			}

			if (CheckKey(InputType::MoveLeft, 0, true)) {
				MoveTimer.Start();
				CurrentMoveLeft();
				IsBoardChange = true;
			}
			if (CheckKey(InputType::MoveRight, 0, true)) {
				MoveTimer.Start();
				CurrentMoveRight();
				IsBoardChange = true;
			}
			if (CheckKeyState(InputType::MoveLeft, MoveState)) {
				if (MoveTimer.GetElapsed().Second() > MoveTime) {
					CurrentMoveLeft();
					IsBoardChange = true;
				}
				if (CheckKey(InputType::MoveLeft, 2)) {
					MoveState = '\0';
					MoveTimer.Reset();
				}
			}
			if (CheckKeyState(InputType::MoveRight, MoveState)) {
				if (MoveTimer.GetElapsed().Second() > MoveTime) {
					CurrentMoveRight();
					IsBoardChange = true;
				}
				if (CheckKey(InputType::MoveRight, 2)) {
					MoveState = '\0';
					MoveTimer.Reset();
				}
			}

			if (CheckKey(InputType::HardDrop)) {
				CurrentHardDrop();
				IsBoardChange = true;
			}
			if (CheckKey(InputType::SwapHold)) {
				SwapHold();
				IsBoardChange = true;
			}

			if (CheckKey(InputType::RotateLeft)) {
				CurrentRotateLeft();
				IsBoardChange = true;
			}
			if (CheckKey(InputType::RotateRight)) {
				CurrentRotateRight();
				IsBoardChange = true;
			}
		}

		if (MultiPlay) {
			if (!EndFlag && IsBoardChange) {
				ShareData.RecvIndex = ShareData.MyIndex;
				ShareData.Board = Board;
				ShareData.Attack = Attack;
				Send();
				Attack = 0;
			}
			if (EndFlag && !CheckState()) {
				SwitchState(true);
				Send();
			}
			Recv();
			Damage += !EndFlag ? ShareData.Damage * ShareData.Rule.DamageRate : 0;
			ShareData.Damage = 0;
		}
		else if (EndFlag) {
			NowScene = Scene::Result;
		}

		if (NowScene == Scene::Result || ShareData.NowScene == Scene::Result) {
			CurrentBGM.FadeStart(true, 1.0);
			Skin.Playing.Image.FadeBG.FadeStart(false, 0.5);
			Skin.Playing.Font.ResultData.FadeStart(false, 0.5);
		}
	}
	void DrawResultData() {
		if (!MultiPlay) {
			Skin.Playing.Font.ResultData.Draw(Skin.Playing.Config.ResultPos.GetPos<float>(0), GetColor(255, 255, 255), GetColor(0, 0, 0), "Result");
			Skin.Playing.Font.ResultData.Draw(Skin.Playing.Config.ResultPos.GetPos<float>(1), GetColor(255, 255, 255), GetColor(0, 0, 0), rformat("Time:  {:0.3f}s", InGameTimer.GetElapsed().Second()));
			Skin.Playing.Font.ResultData.Draw(Skin.Playing.Config.ResultPos.GetPos<float>(2), GetColor(255, 255, 255), GetColor(0, 0, 0), rformat("Score: {:0>12}", Score));
			Skin.Playing.Font.ResultData.Draw(Skin.Playing.Config.ResultPos.GetPos<float>(3), GetColor(255, 255, 255), GetColor(0, 0, 0), rformat("Line:  {}", Line));
			Skin.Playing.Font.ResultData.Draw(Skin.Playing.Config.ResultPos.GetPos<float>(4), GetColor(255, 255, 255), GetColor(0, 0, 0), rformat("Combo: {}", Combo));
			Skin.Playing.Font.ResultData.Draw(Skin.Playing.Config.ResultPos.GetPos<float>(5), GetColor(255, 255, 255), GetColor(0, 0, 0), rformat("B2B:   {}", B2B));
			Skin.Playing.Font.ResultData.Draw(Skin.Playing.Config.ResultPos.GetPos<float>(6), GetColor(255, 255, 255), GetColor(0, 0, 0), "Press \"GameReturn\" to Retry\n   \"GameBack\" to Lobby");
		}
		else {
			for (auto data : ShareData.PlayerDatas) {
				if (data.State < 2) { continue; }
				Skin.Playing.Font.ResultData.Draw(
					Skin.Playing.Config.ResultPos.GetPos<float>(3),
					GetColor(255, 255, 255),
					GetColor(0, 0, 0),
					"Winner:" + data.PlayerName
				);
			}
			Skin.Playing.Font.ResultData.Draw(
				Skin.Playing.Config.ResultPos.GetPos<float>(4),
				GetColor(255, 255, 255),
				GetColor(0, 0, 0),
				"Press \"GameBack\" to Lobby."
			);
		}
	}

	void DrawResult() {
		Skin.Playing.Image.BackGround.Draw();
		DrawBoard();
		DrawHold();
		DrawQueue();
		Skin.Playing.Image.FadeBG.Draw();
		SetDrawBright(255, 255, 255);
		DrawResultData();
	}

	void ProcResult() {

		if (CurrentBGM.IsFading() &&
			Skin.Playing.Image.FadeBG.IsFading() &&
			Skin.Playing.Font.ResultData.IsFading()) {
			if (IsFadeEnd()) {
				CurrentBGM.FadeEnd();
				Skin.Playing.Image.FadeBG.FadeEnd();
				Skin.Playing.Font.ResultData.FadeEnd();
			}
			return;
		}

		if (!MultiPlay) {

			if (CheckKey(InputType::GameReturn)) {
				Skin.Playing.Image.FadeBG.Alpha = 0;
				Init();
				NowScene = Scene::Playing;
			}
			if (CheckKey(InputType::GameBack)) {
				Skin.Playing.Image.FadeBG.Alpha = 0;
				NowScene = Scene::Lobby;
			}
		}
		else {

			if (CheckKey(InputType::GameBack)) {
				SwitchState(true);
				Send();
			}
			Recv();
		}
	}

	template<typename T>
	void QueueClear(T& queue) {
		if (!queue.empty()) { queue.clear(); }
	}
	void TimerReset(libarrier::Timer& timer) {
		if (timer.IsRunning()) {
			timer.Stop();
			timer.Reset();
		}
	}
	void BGMQueueSets() {
		static std::random_device device;
		std::mt19937 gen(device());
		std::deque<SoundData> ret(Skin.Playing.BGM.BGMList.begin(), Skin.Playing.BGM.BGMList.end());

		std::shuffle(ret.begin(), ret.end(), gen);
		BGMQueue = std::move(ret);
	}
	void PlayBGM() {
		if (CurrentBGM.IsPlay()) { return; }
		CurrentBGM = std::move(BGMQueue.front());
		BGMQueue.pop_front();
		if (BGMQueue.empty()) {
			BGMQueueSets();
		}
		CurrentBGM.Play();
	}
	void StopBGM() const {
		if (!CurrentBGM.IsPlay()) { return; }
		CurrentBGM.Stop();
	}

	void Init() {
		if (MultiPlay) {
			ShareData.Attack = 0;
			ShareData.Damage = 0;
		}
		Boards.clear();
		QueueClear(BGMQueue);
		QueueClear(BagQueue);
		Hold = MynoObject();
		EndFlag = false;
		HoldOnce = false;
		Score = 0;
		Line = 0;
		Combo = 0;
		B2B = 0;
		GravityTime = 1;
		Board.resize(Height);
		bool xflag = false;
		bool yflag = false;
		for (auto& line : Board) {
			line.resize(Width);
			for (auto& myno : line) {
				myno = Myno::Null;
				yflag = !yflag;
			}
			xflag = !xflag;
		}
		BGMQueueSets();
		for (int i = 0; i < 3; ++i) {
			MakeMynoSets();
		}
		Next();
		GravityTimer.Reset();
		InGameTimer.Reset();
		GravityTimer.Start();
		InGameTimer.Start();
	}
	bool IsFadeEnd() {
		bool bgm = CurrentBGM.FadeProc();
		bool graph = Skin.Playing.Image.FadeBG.FadeProc();
		bool font = Skin.Playing.Font.ResultData.FadeProc();
		return bgm && graph && font;
	}

public:

	void StartUpdate() {
		GameEnd = true;
	}
	bool IsGameEnd() const {
		return GameEnd;
	}
	bool IsUseGamePad() const {
		return UseGamePad;
	}
	int GetPadKeyCode(int i) const {
		return Keys[i];
	}
	void Proc(bool* key) {

		for (size_t i = 0; auto& k : Keyboard) {
			k.Update(key[i]);
			++i;
		}

		if (Screen.IsScreenClick(MOUSE_INPUT_LEFT)) {
			SelectIndex = -1;
		}

		if (MultiPlay && ServerConnected) {
			NowScene = ShareData.NowScene;
		}

		auto Prev = NowScene;

		switch (NowScene) {
		case Scene::Title:
			ProcTitle();
			break;
		case Scene::Menu:
			ProcMenu();
			break;
		case Scene::Lobby:
			ProcLobby();
			break;
		case Scene::Playing:
			ProcPlaying();
			break;
		case Scene::Result:
			ProcResult();
			break;
		case Scene::Config:
			ProcConfig();
			break;
		}

		if (Prev != NowScene) {
			Targeted = false;
			SelectIndex = -1;
			Current_Scroll = 0.0f;
		}
	}

	void Draw(bool clearscreen = true) {

		switch (NowScene) {
		case Scene::Title:
			DrawTitle();
			break;
		case Scene::Menu:
			DrawMenu();
			break;
		case Scene::Lobby:
			DrawLobby();
			break;
		case Scene::Playing:
			DrawPlaying();
			break;
		case Scene::Result:
			DrawResult();
			break;
		case Scene::Config:
			DrawConfig();
			break;
		}

		ScreenFlip();
		if (clearscreen) {
			ClearDrawScreen();
		}
	}
};