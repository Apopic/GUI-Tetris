#pragma once
#include <fstream>
#include "../Library/ExLib.h"

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

		struct _BGM {
		} BGM;

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
			Pos2D<std::vector<float>> ScorePos = { {80, 80, 80, 80, 80, 80} ,{632, 660, 688, 716, 744, 722} };
			Pos2D<std::vector<float>> ResultPos = { {640, 640, 640, 640, 640, 640}, { 632, 660, 688, 716, 744, 722 } };
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
