#pragma once
#include "System.h"
#include "Escape.h"

inline void Game::DrawScore() {
	SetDrawBright(255, 255, 255);
	Skin.Playing.Font.PlayData.Draw(Skin.Playing.Config.ScorePos.GetPos<float>(0), GetColor(255, 255, 255), GetColor(0, 0, 0), rformat("Time:  {:0.3f}s", InGameTimer.GetElapsed().Second()));
	Skin.Playing.Font.PlayData.Draw(Skin.Playing.Config.ScorePos.GetPos<float>(1), GetColor(255, 255, 255), GetColor(0, 0, 0), rformat("Score: {:0>12}", Score));
	Skin.Playing.Font.PlayData.Draw(Skin.Playing.Config.ScorePos.GetPos<float>(2), GetColor(255, 255, 255), GetColor(0, 0, 0), rformat("Line:  {}", Line));
	Skin.Playing.Font.PlayData.Draw(Skin.Playing.Config.ScorePos.GetPos<float>(3), GetColor(255, 255, 255), GetColor(0, 0, 0), rformat("Combo: {}", Combo));
	Skin.Playing.Font.PlayData.Draw(Skin.Playing.Config.ScorePos.GetPos<float>(4), GetColor(255, 255, 255), GetColor(0, 0, 0), rformat("B2B:   {}", B2B));
	if (MultiPlay) { Skin.Playing.Font.PlayData.Draw(Skin.Playing.Config.ScorePos.GetPos<float>(5), GetColor(255, 255, 255), GetColor(0, 0, 0), rformat("Damage:{}", Damage)); }
}
inline void Game::MakeMynoSets() {
	static std::random_device device;
	std::mt19937 gen(device());

	std::deque<Myno> ret = { Myno::I, Myno::O, Myno::S, Myno::Z, Myno::J, Myno::L, Myno::T };
	std::shuffle(ret.begin(), ret.end(), gen);
	BagQueue.push_back(std::move(ret));
}
inline Myno Game::GetMynoQueue() {
	auto& frontbag = BagQueue.front();
	auto ret = std::move(frontbag.front());
	frontbag.pop_front();
	if (frontbag.empty()) {
		BagQueue.pop_front();
		MakeMynoSets();
	}
	return ret;
}

inline bool Game::CheckInBoard(const MynoObject& obj) {
	auto [w, h] = obj.GetFieldSize();
	auto [x, y] = obj.GetPosition();
	auto& c = obj.GetCollision();
	for (int j = 0; j < h; ++j) {
		for (int i = 0; i < w; ++i) {
			if (!c[j][i]) {
				continue;
			}
			if (!(0 <= x + i && x + i < Config.Width)) {
				return false;
			}
			if (!(0 <= y + j && y + j < Config.Height)) {
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
inline int Game::PlaceBoard(const MynoObject& obj, bool place, bool clear, bool preview) {
	auto [w, h] = obj.GetFieldSize();
	auto [x, y] = obj.GetPosition();
	auto& c = obj.GetCollision();
	int nearcount = 0;
	for (int j = 0; j < h; ++j) {
		for (int i = 0; i < w; ++i) {
			if (!(0 <= x + i && x + i < Config.Width && 0 <= y + j && y + j < Config.Height)) {
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
inline void Game::ClearBoardPrev(const MynoObject& obj) {
	PlaceBoard(obj, false, true, false);
}
inline void Game::PlaceBoard(const MynoObject& obj) {
	PlaceBoard(obj, false, false, false);
}

inline void Game::SwapHold() {
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

inline int Game::CalculateLine(int clearcount, int nearcount, bool perfect, int ren, bool spin, Myno t) {
	if (perfect) { return 10; }
	int line = (t == Myno::T && spin) ? clearcount * 2 : std::max(0, clearcount - 1);
	line += (clearcount == 4 || (t == Myno::T && nearcount >= 3 && spin));
	line += (ren < 10 ? RENTable[ren] : 5);
	int damage = Damage;
	Damage = std::max(0, damage - line);
	return line;
}
inline void Game::ApplyBoard(const MynoObject& obj) {
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
	Attack = CalculateLine(clearcount, nearcount, perfect, Combo, Spin, t);
	clearcount == 4 ? Skin.Playing.SE.Tetris.Play() : Skin.Playing.SE.ClearLine.Play();
	Line += clearcount;
	Score += (1000 + ((int)perfect * 1000)) * clearcount;
	Score += 200 * Combo;
	Score += 200 * B2B;
	(clearcount == 4 || (t == Myno::T && nearcount >= 3 && Spin)) ? (B2B += 1) : (B2B = 0);
	Spin = false;
	Combo += 1;
}
inline void Game::PreviewBoard(const MynoObject& obj) {
	PlaceBoard(obj, false, false, true);
}
inline int Game::LineClear() {
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
inline void Game::LineApply() {
	if (!MultiPlay) { return; }

	std::vector<Myno> GabageMyno = std::vector<Myno>(Config.Width, Myno::Gabage);
	GabageMyno[GetRand(Config.Width - 1)] = Myno::Null;

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
inline bool Game::IsPerfect() {
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
inline int Game::TableIndexConverter(bool LR, Direction cur) {
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
inline void Game::SetCurrent(Myno t) {
	Current = MynoObject::Make(t);
	auto [w, _] = Current.GetFieldSize();
	Current.SetPosition(Config.Width / 2 - 2, 0);
	if (!CheckInBoard(Current)) {
		InGameTimer.Stop();
		EndFlag = true;
	}
}
inline void Game::CurrentMoveLeft() {
	if (auto temp = Current; !CheckInBoard((temp.MoveLeft(), temp))) {
		return;
	}
	ClearBoardPrev(Current);
	Current.MoveLeft();
	CurrentPreview();
	PlaceBoard(Current);
	LockTimeInMove();
}
inline void Game::CurrentMoveRight() {
	if (auto temp = Current; !CheckInBoard((temp.MoveRight(), temp))) {
		return;
	}
	ClearBoardPrev(Current);
	Current.MoveRight();
	CurrentPreview();
	PlaceBoard(Current);
	LockTimeInMove();
}
inline void Game::CurrentRotateLeft() {
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
inline void Game::CurrentRotateRight() {
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
inline void Game::CurrentSoftDrop() {
	if (auto temp = Current; !CheckInBoard((temp.MoveDown(), temp))) {
		return;
	}
	ClearBoardPrev(Current);
	Current.MoveDown();
	PlaceBoard(Current);
	LockTimeInMove();
}
inline void Game::CurrentHardDrop() {
	Skin.Playing.SE.HardDrop.Play();
	auto [x, y] = Current.GetPosition();
	while (true) {
		auto test = Current;
		test.SetPosition(x, y + 1);
		if (!CheckInBoard(test)) {
			break;
		}
		y += 1;
		if (y >= Config.Height) {
			return;
		}
	}
	ClearBoardPrev(Current);
	Current.SetPosition(x, y);
	ApplyBoard(Current);
	Next();
}
inline void Game::CurrentPreview(bool clear) {
	auto [x, y] = Current.GetPosition();
	while (true) {
		auto test = Current;
		test.SetPosition(x, y + 1);
		if (!CheckInBoard(test)) {
			break;
		}
		y += 1;
		if (y >= Config.Height) {
			return;
		}
	}
	if (clear) { ClearBoardPrev(Preview); }
	Preview = Current;
	Preview.SetPosition(x, y);
	PreviewBoard(Preview);
}
inline void Game::CurrentSpinFlag() {
	auto temp = Current;
	Spin = !CheckInBoard((temp.MoveDown(), temp));
}
inline void Game::LockTimeInMove() {
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
inline void Game::ResetLockTime() {
	LockTimer.Reset();
	LockCount = 0;
	LockTimeInMove();
}
inline void Game::Next() {
	SetCurrent(GetMynoQueue());
	CurrentPreview(false);
	PlaceBoard(Current);
	ResetLockTime();
	GravityTimer.Start();
}

inline void Game::DrawPlaying() {
	Skin.Playing.Image.BackGround.Draw();
	PlayBGM();
	DrawBoard();
	DrawEnemy();
	DrawHold();
	DrawQueue();
	DrawScore();
}

inline void Game::ProcPlaying() {

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
			GravityTime *= Config.GravitySpeedRate;
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
		if (!GameInitFlag) {
			GameInitFlag = true;
			Init();
		}
		if (EndFlag && !CheckState()) {
			SwitchState(true);
			Send();
		}
		Recv();	
		Damage += !EndFlag ? ShareData.Damage : 0;
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
