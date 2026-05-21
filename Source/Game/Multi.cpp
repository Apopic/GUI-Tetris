#pragma once
#include "System.h"

inline void Game::Send() {
	Socket.Send(Packet(ShareData));
}
inline bool Game::Recv() {
	if (Socket.Available() <= 0) { return false; }
	ShareData = *Socket.Recv()->Get<GameData>();
	return true;
}
inline bool Game::CheckState() {
	return ShareData.PlayerDatas[ShareData.MyIndex].State > 0;
}
inline void Game::SwitchState(bool val) {
	ShareData.PlayerDatas[ShareData.MyIndex].State = val;
}