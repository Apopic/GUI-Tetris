#pragma once
#include <nlohmann/json.hpp>
#include <string>
#include <DxLib.h>
#include "Easing.hpp"
#include "timer.hpp"

using json = nlohmann::json;
namespace fs = std::filesystem;

template<class T>
struct Pos2D {
	T X = { 0 };
	T Y = { 0 };

	template<class T>
	Pos2D<T> GetPos(size_t index) {
		return { X[index], Y[index] };
	}
};
template<class T>
void from_json(const json& data, Pos2D<T>& v);

template<class T>
struct Size2D {
	T Width = 0;
	T Height = 0;
};
template<class T>
void from_json(const json& data, Size2D<T>& v);

template<class T>
struct Color3 {
	T R = 0;
	T G = 0;
	T B = 0;
};
template<class T>
void from_json(const json& data, Color3<T>& v);

template<class T>
struct Color4 {
	T R = 0;
	T G = 0;
	T B = 0;
	T A = 0;
};
template<class T>
void from_json(const json& data, Color4<T>& v);

#define JSONPARSE(name) if(data.find(#name) != data.end()) name = data[#name]

class FadeData {

	libarrier::Timer FadeTimer;
	double FinishTime = 0;
	bool FadeType = false;

public:

	//FadeType
	//false = In
	//true = Out
	template<typename F, typename RF>
	bool Proc(F func, RF resetfunc) {
		double rate = ease::GetEasingRate(FadeTimer.GetElapsed().Second() / FinishTime, (ease::Base)FadeType, ease::Line::Sine);
		if (rate >= 1.0) {
			resetfunc();
			return true;
		}
		func(std::abs((double)FadeType - rate));
		return false;
	}
	void Start(bool fadetype, double finishtime) {
		FadeType = fadetype;
		FinishTime = finishtime;
		FadeTimer.Start();
	}
	void End() {
		FadeTimer.Stop();
		FadeTimer.Reset();
	}
	bool IsFading() const {
		return FadeTimer.IsRunning();
	}
	bool GetFadeType() const {
		return FadeType;
	}
};

struct GraphData {

	void Load(const std::string path, const Pos2D<float> pos, const Size2D<float> size = { 0,0 }, const double alpha = 255) {
		Handle = LoadGraph(path.c_str());
		Pos = pos;
		Size = size;
		Alpha = alpha;
		if (size.Width == 0 && size.Height == 0) {
			GetGraphSizeF(Handle, &Size.Width, &Size.Height);
		}
	}
	void Load(const std::string& dirpath, const json& data) {

		std::string FilePath = "";

		JSONPARSE(FilePath);

		FilePath = (dirpath + FilePath);

		Handle = LoadGraph(FilePath.c_str());

		JSONPARSE(Size);
		JSONPARSE(Pos);
		JSONPARSE(Alpha);

		if (Size.Width == 0 && Size.Height == 0) {
			GetGraphSizeF(Handle, &Size.Width, &Size.Height);
		}
	}
	void Delete() {
		DeleteGraph(Handle);
		Handle = 0;
	}
	void FadeStart(bool fadetype, double finishtime) {
		Fade.Start(fadetype, finishtime);
	}
	void FadeEnd() {
		Fade.End();
	}
	bool FadeProc() {
		return Fade.Proc(
			[&](const double rate) { Alpha = 255.0 * rate; },
			[&]() { Alpha = 255.0 * !Fade.GetFadeType(); }
		);
	}
	bool IsFading() const {
		return Fade.IsFading();
	}
	void Draw(Pos2D<float> pos = { 0,0 }) {
		if (Alpha != 256) { SetDrawBlendMode(DX_BLENDMODE_ALPHA, Alpha); }
		DrawExtendGraphF(
			Pos.X + pos.X - (Size.Width * 0.5f),
			Pos.Y + pos.Y - (Size.Height * 0.5f),
			Pos.X + pos.X + (Size.Width * 0.5f),
			Pos.Y + pos.Y + (Size.Height * 0.5f),
			Handle,
			TRUE
		);
		if (Alpha != 256) { SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255); }
	}
	void ResizeDraw(Pos2D<float> pos = { 0,0 }, Size2D<float> size = { 1.0f, 1.0f }) {
		if (Alpha != 256) { SetDrawBlendMode(DX_BLENDMODE_ALPHA, Alpha); }
		DrawExtendGraphF(
			Pos.X + pos.X - (Size.Width * 0.5f * size.Width),
			Pos.Y + pos.Y - (Size.Height * 0.5f * size.Height),
			Pos.X + pos.X + (Size.Width * 0.5f * size.Width),
			Pos.Y + pos.Y + (Size.Height * 0.5f * size.Height),
			Handle,
			TRUE
		);
		if (Alpha != 256) { SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255); }
	}
	void RectDraw(Pos2D<float> pos, Pos2D<float> orgpos, Size2D<float> size, Size2D<float> resize = { 0.0f, 0.0f }) {
		if (Alpha != 256) { SetDrawBlendMode(DX_BLENDMODE_ALPHA, Alpha); }
		DrawRectGraphF(
			Pos.X + pos.X - (Size.Width * 0.5f * resize.Width),
			Pos.Y + pos.Y - (Size.Height * 0.5f * resize.Height),
			orgpos.X * resize.Width,
			orgpos.Y * resize.Height,
			size.Width * resize.Width,
			size.Height * resize.Height,
			Handle,
			TRUE
		);
		if (Alpha != 256) { SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255); }
	}
	bool IsTarget(Pos2D<float> pos = { 0,0 }) {
		if (!GetWindowActiveFlag()) { return false; }
		Pos2D<int> mousepos;
		GetMousePoint(&mousepos.X, &mousepos.Y);
		return (Pos.X + pos.X - (Size.Width * 0.5f)) <= mousepos.X &&
			(Pos.Y + pos.Y - (Size.Height * 0.5f)) <= mousepos.Y &&
			(Pos.X + pos.X + (Size.Width * 0.5f)) >= mousepos.X &&
			(Pos.Y + pos.Y + (Size.Height * 0.5f)) >= mousepos.Y;
	}
	bool IsClick(int clicktype, Pos2D<float> pos = { 0,0 }) {
		if (!GetWindowActiveFlag()) { return false; }
		Pos2D<int> clickpos;
		GetMousePoint(&clickpos.X, &clickpos.Y);
		if ((GetMouseInput() & clicktype) != 0) {
			return (Pos.X + pos.X - (Size.Width * 0.5f)) <= clickpos.X &&
				(Pos.Y + pos.Y - (Size.Height * 0.5f)) <= clickpos.Y &&
				(Pos.X + pos.X + (Size.Width * 0.5f)) >= clickpos.X &&
				(Pos.Y + pos.Y + (Size.Height * 0.5f)) >= clickpos.Y;
		}
		return false;
	}

	Pos2D<float> Pos = { 0,0 };
	Size2D<float> Size = { 0,0 };
	int Handle = 0;
	int ScreenHandle = 0;
	double Alpha = 256;

	FadeData Fade;
};

struct FontData {

	float Size = 0;
	float Thick = 0;
	float Edge = 0;
	int Space = 0;

	enum Position : int {
		Left,
		Center,
		Right
	} Pos = Left;

	void Load(const std::string path, const std::string fontname, float size, float thick, float edge, int space, Position pos = Position::Left) {

		MemHandle = AddFontFile(path.c_str());

		Size = size;
		Thick = thick * 0.5f;
		Edge = edge;
		Space = space;
		Pos = pos;

		Handle = CreateFontToHandle(fontname.c_str(), Size, Thick, DX_FONTTYPE_ANTIALIASING_EDGE_16X16, DX_CHARSET_UTF8, Edge);
		SetFontSpaceToHandle(Space, Handle);
	}
	void Load(const std::string& dirpath, const json& data) {

		std::string FontName = "";
		std::string FilePath = "";

		JSONPARSE(FilePath);

		FilePath = (dirpath + FilePath);

		MemHandle = AddFontFile(FilePath.c_str());

		JSONPARSE(Size);
		JSONPARSE(Thick) * 0.5f;
		JSONPARSE(Edge);
		if (data.find("Position") != data.end()) ((int&)Pos) = data["Position"].get<int>();
		JSONPARSE(Space);
		JSONPARSE(Alpha);

		JSONPARSE(FontName);

		Handle = CreateFontToHandle(FontName.c_str(), Size, Thick, DX_FONTTYPE_ANTIALIASING_EDGE_16X16, DX_CHARSET_UTF8, Edge);
		SetFontSpaceToHandle(Space, Handle);
	}
	void Delete() {
		DeleteFontToHandle(Handle);
		RemoveFontFile(MemHandle);
		Handle = 0;
		MemHandle = nullptr;
	}
	void FadeStart(bool fadetype, double finishtime) {
		Fade.Start(fadetype, finishtime);
	}
	void FadeEnd() {
		Fade.End();
	}
	bool FadeProc() {
		return Fade.Proc(
			[&](const double rate) { Alpha = 255.0 * rate; },
			[&]() { Alpha = 255.0 * !Fade.GetFadeType(); }
		);
	}
	bool IsFading() const {
		return Fade.IsFading();
	}
	void Draw(Pos2D<float> pos, unsigned int color, unsigned int edgecolor, const std::string str) {
		if (Handle == -1) { return; }
		if (str.empty()) { return; }
		float offset = 0;
		switch (Pos)
		{
		case Left:
			break;
		case Center:
			offset = GetDrawStringWidthToHandle(str.c_str(), -1, Handle) * 0.5f;
			break;
		case Right:
			offset = GetDrawStringWidthToHandle(str.c_str(), -1, Handle);
			break;
		}
		if (Alpha != 256) { SetDrawBlendMode(DX_BLENDMODE_ALPHA, Alpha); }
		DrawStringToHandle(
			pos.X - offset,
			pos.Y - Size * 0.5f,
			str.c_str(),
			color,
			Handle,
			edgecolor
		);
		if (Alpha != 256) { SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255); }
	}

	int Handle = 0;
	HANDLE MemHandle = nullptr;
	double Alpha = 256;

	FadeData Fade;
};

struct SoundData {

	int BufferNum = 0;
	int Frequency = 0;
	int Volume = 0;

	void Load(const std::string path, int volume = -1, int buffernum = 1) {
		BufferNum = buffernum;
		Handle = LoadSoundMem(path.c_str(), BufferNum);

		Frequency = GetFrequencySoundMem(Handle);
		Volume = volume == -1 ? GetVolumeSoundMem(Handle) : volume;
	}
	void Load(const std::string& dirpath, const json& data) {

		std::string FilePath = "";

		JSONPARSE(FilePath);
		JSONPARSE(BufferNum);

		FilePath = (dirpath + FilePath);

		Handle = LoadSoundMem(FilePath.c_str(), BufferNum);

		Frequency = GetFrequencySoundMem(Handle);

		JSONPARSE(Volume);
	}
	void Delete() {
		DeleteSoundMem(Handle);
		Handle = 0;
	}
	void Play(bool toppositionflag = true) const {
		ChangeVolumeSoundMem(((Volume) / 100.0) * 255, Handle);
		PlaySoundMem(Handle, DX_PLAYTYPE_BACK, toppositionflag);
	}
	void Stop() const {
		StopSoundMem(Handle);
	}
	void FadeStart(bool fadetype, double finishtime) {
		Fade.Start(fadetype, finishtime);
	}
	void FadeEnd() {
		Fade.End();
	}
	bool FadeProc() {
		return Fade.Proc(
			[&](const double rate) { SetVolume(Volume * rate); },
			[&]() { SetVolume(100 * !Fade.GetFadeType()); Stop(); }
		);
	}
	bool IsFading() const {
		return Fade.IsFading();
	}
	bool IsPlay() const {
		return CheckSoundMem(Handle) == TRUE;
	}
	void SetFrequency(int frequency) const {
		SetFrequencySoundMem(frequency, Handle);
	}
	void SetVolume(float volume) const {
		ChangeVolumeSoundMem(((volume) / 100.0) * 255, Handle);
	}

	int Handle = 0;
	std::string FilePath = "";
	FadeData Fade;

};

#undef JSONPARSE

#define FROMJSON(name) data.at(#name).get_to(v.name)
template<class T>
inline void from_json(const json& data, Pos2D<T>& v) {
	FROMJSON(X);
	FROMJSON(Y);
}

template<class T>
inline void from_json(const json& data, Size2D<T>& v) {
	FROMJSON(Width);
	FROMJSON(Height);
}

inline void from_json(const json& data, SoundData& v) {
	FROMJSON(FilePath);
	FROMJSON(BufferNum);
	FROMJSON(Volume);
}

template<class T>
inline void from_json(const json& data, Color3<T>& v) {
	FROMJSON(R);
	FROMJSON(G);
	FROMJSON(B);
}

template<class T>
inline void from_json(const json& data, Color4<T>& v) {
	FROMJSON(R);
	FROMJSON(G);
	FROMJSON(B);
	FROMJSON(A);
}
#undef FROMJSON