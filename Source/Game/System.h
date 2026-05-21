#pragma once
#include <array>
#include <deque>
#include <cstdint>
#include <algorithm>
#include <random>
#include <bitset>

#include "Multi.h"
#include "Skin.h"
#include "Config.h"
#include "../Library/Input.h"
#include "../Library/timer.hpp"

#define _def_logic_op(t)\
static inline Myno operator##t##(Myno lhs, Myno rhs) { return static_cast<Myno>((uint8_t)lhs t (uint8_t)rhs); }\
static inline Myno operator##t##(Myno lhs, uint8_t rhs) { return static_cast<Myno>((uint8_t)lhs t rhs); }\
static inline Myno operator##t##(uint8_t lhs, Myno rhs) { return static_cast<Myno>(lhs t (uint8_t)rhs); }

_def_logic_op(&);
_def_logic_op(| );
_def_logic_op(^);
#undef _def_logic_op

static const inline std::string None = "  ";

enum class Direction : uint8_t {
	Upper,
	Right,
	Lower,
	Left,
	Count
};

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

class Game {

	libarrier::Timer InGameTimer;
	InputFlag Keyboard[256]{};

	_Skin Skin;
	_Config Config;

	TCPSocket Socket;
	std::vector<std::vector<std::vector<Myno>>> Boards;
	GameData ShareData = GameData();

	Scene NowScene = Scene::Init;

	MouseWheel Wheel = MouseWheel();

	float Current_Rot = 0.0f;
	bool Targeted = false;
	bool Clicked = false;

	bool EndFlag = false;
	bool GameEnd = false;

	bool MultiPlay = false;
	bool GameInitFlag = false;

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
	double SoftDropRate = 0.03125;
	char MoveState = '\0';
	libarrier::Timer MoveTimer;
	double MoveTime = 0.15;

	MynoObject Hold;
	bool HoldOnce = false;

	std::deque<SoundData> BGMQueue;
	SoundData CurrentBGM;

	inline void DrawScore();
	inline void DrawResultData();

	inline void MakeMynoSets();
	inline Myno GetMynoQueue();

	inline bool CheckInBoard(const MynoObject& obj);
	inline int PlaceBoard(const MynoObject& obj, bool place, bool clear, bool preview);
	inline void ClearBoardPrev(const MynoObject& obj);
	inline void PlaceBoard(const MynoObject& obj);

	inline int CalculateLine(int clearcount, int nearcount, bool perfect, int ren, bool spin, int b2b, Myno t);

	inline void ApplyBoard(const MynoObject& obj);
	inline void PreviewBoard(const MynoObject& obj);

	inline int LineClear();
	inline void LineApply();

	inline bool IsPerfect();

	inline int TableIndexConverter(bool LR, Direction cur);

	inline void SetCurrent(Myno t);
	inline void CurrentMoveLeft();
	inline void CurrentMoveRight();
	inline void CurrentRotateLeft();
	inline void CurrentRotateRight();
	inline void CurrentSoftDrop();
	inline void CurrentHardDrop();
	inline void CurrentPreview(bool clear = true);
	inline void CurrentSpinFlag();
	inline void LockTimeInMove();
	inline void ResetLockTime();
	inline void Next();

	inline void SwapHold();

	void DrawBoard() {
		SetDrawBright(255, 255, 255);
		Skin.Playing.Image.MainFrame.Draw();
		Pos2D<float> offset = {
			std::round(Skin.Playing.Image.MainMyno.Size.Width * (10 - Config.Width) * 0.5),
			std::round(Skin.Playing.Image.MainMyno.Size.Height * (20 - (Config.Height - 4)) * 0.5)
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
				std::round(Skin.Playing.Image.EnemyMyno.Size.Width * (10 - Config.Width) * 0.5 + (Skin.Playing.Config.EnemyFrameInterval.X * l)),
				std::round(Skin.Playing.Image.EnemyMyno.Size.Height * (20 - (Config.Height - 4)) * 0.5 + (Skin.Playing.Config.EnemyFrameInterval.Y * (j % 4)))
			};
			Skin.Playing.Image.EnemyFrame.Draw({ Skin.Playing.Config.EnemyFrameInterval.X * l, Skin.Playing.Config.EnemyFrameInterval.Y * (j % 4) });

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
		for (int n = 0; n < Config.NextCount; ++n) {
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

	// StateType
	// // 0 = Down
	// // 1 = Press
	// // 2 = Up
	bool CheckKey(InputType type, int statetype = 0, bool statesave = false) {
		bool keyresult = false;
		int keytype = Config.UseGamePad ? (int)type : Config.Keys[(int)type];
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
		int keytype = Config.UseGamePad ? (int)type : Config.Keys[(int)type];
		return state == keytype;
	}

	inline void ProcInit();
	inline void ProcMenu();
	inline void ProcResult();
	inline void ProcPlaying();
	inline void ProcLobby();
	inline void ProcConfig();

	inline void DrawInit();
	inline void DrawMenu();
	inline void DrawLobby();
	inline void DrawPlaying();
	inline void DrawResult();
	inline void DrawConfig();

	void Init() {
		Config.Load();
		Skin.Load(Config.Width, Config.Height);
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
		Board.resize(Config.Height);
		bool xflag = false;
		bool yflag = false;
		for (auto& line : Board) {
			line.resize(Config.Width);
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

	inline void Send();
	inline bool Recv();
	inline bool CheckState();
	inline void SwitchState(bool val);

public:

	bool IsGameEnd() const {
		return GameEnd;
	}
	bool IsUseGamePad() const {
		return Config.UseGamePad;
	}
	int GetPadKeyCode(int i) const {
		return Config.Keys[i];
	}

	void Proc(bool* key) {

		for (size_t i = 0; auto& k : Keyboard) {
			k.Update(key[i]);
			++i;
		}

		if (MultiPlay) {
			NowScene = ShareData.NowScene;
		}

		switch (NowScene) {
		case Scene::Init:
			ProcInit();
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
	}

	void Draw() {

		switch (NowScene) {
		case Scene::Init:
			DrawInit();
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
		ClearDrawScreen();
	}
};