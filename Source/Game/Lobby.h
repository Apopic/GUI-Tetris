#pragma once
#include "System.h"

class _Lobby {
public:

	std::vector<std::string> Menu = { "Back", "GameStart", "ResetGameRule", "Width", "Height", "NextCount", "GravitySpeedRate"};
	std::vector<std::string> RuleName = { "Width", "Height", "NextCount", "GravitySpeedRate" };

	int Width = 10;
	int Height = 20;
	int NextCount = 5;
	double GravitySpeedRate = 1.0;
};