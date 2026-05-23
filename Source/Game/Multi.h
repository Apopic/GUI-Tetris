#pragma once
#include "../Library/Socket/Socket.h"

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

enum struct Scene : int {
	Null = -1,
	Init,
	Menu,
	Lobby,
	Playing,
	Result,
	Config,
	Count
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
};

struct GameData {
	std::vector<std::vector<Myno>> Board;
	std::vector<PlayerData> PlayerDatas = std::vector<PlayerData>(1);
	Scene NowScene = Scene::Init;
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