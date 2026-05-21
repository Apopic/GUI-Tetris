#pragma once
#include <format>
#include <string>

enum class RGBColor : int {
	White = 0xffffff,
	Cyan = 0x348fca,
	Yellow = 0xe7bd22,
	Green = 0x2aa55d,
	Red = 0xda4b3c,
	Blue = 0x246eab,
	Orange = 0xdc7a23,
	Purple = 0x824597,
	Gray = 0x7f7f7f,
	Default = 0x80000000,
};

namespace escape {
#define SIFSS static inline std::string
	SIFSS MoveCursor(int l, int c) { return std::format("\033[{};{}H", l, c); }
	SIFSS LineMoveBegin() { return "\r"; }
	SIFSS LineMoveNext() { return "\n"; }
	SIFSS LineMoveUp(int n) { return std::format("\033[{}A", n); }
	SIFSS LineMoveDown(int n) { return std::format("\033[{}B", n); }
	SIFSS ColumunMoveLeft(int n) { return std::format("\033[{}C", n); }
	SIFSS ColumunMoveRight(int n) { return std::format("\033[{}D", n); }
	SIFSS ClearScreen() { return "\033[2J"; }
	SIFSS Color(int c) {
		bool defaultflag = (bool)(c & 0x80000000);
		if (defaultflag) {
			return "\033[0m";
		}
		uint8_t r = (c >> 16) & 0xff;
		uint8_t g = (c >> 8) & 0xff;
		uint8_t b = (c >> 0) & 0xff;
		std::string ret = std::format("\033[38;2;{};{};{}m", r, g, b);
		return ret;
	}
	SIFSS Color(RGBColor c) {
		return Color(static_cast<int>(c));
	}
#undef SCFSS
}

static inline void PutString(const std::string& str) {
	fwrite(str.c_str(), sizeof(char), str.size(), stdout);
}
template<class ...TArgs>
static inline std::string rformat(const std::string_view fmt, TArgs&&... args) {
	return std::vformat(fmt, std::make_format_args(args...));
}