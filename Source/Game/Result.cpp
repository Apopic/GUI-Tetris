#pragma once
#include "System.h"
#include "Escape.h"

inline void Game::DrawResultData() {
	if (!MultiPlay) {
		Skin.Playing.Font.ResultData.Draw(Skin.Playing.Config.ResultPos.GetPos<float>(0), GetColor(255, 255, 255), GetColor(0, 0, 0), "Result");
		Skin.Playing.Font.ResultData.Draw(Skin.Playing.Config.ResultPos.GetPos<float>(1), GetColor(255, 255, 255), GetColor(0, 0, 0), rformat("Time:  {:0.3f}s", InGameTimer.GetElapsed().Second()));
		Skin.Playing.Font.ResultData.Draw(Skin.Playing.Config.ResultPos.GetPos<float>(2), GetColor(255, 255, 255), GetColor(0, 0, 0), rformat("Score: {:0>12}", Score));
		Skin.Playing.Font.ResultData.Draw(Skin.Playing.Config.ResultPos.GetPos<float>(3), GetColor(255, 255, 255), GetColor(0, 0, 0), rformat("Line:  {}", Line));
		Skin.Playing.Font.ResultData.Draw(Skin.Playing.Config.ResultPos.GetPos<float>(4), GetColor(255, 255, 255), GetColor(0, 0, 0), rformat("Combo: {}", Combo));
		Skin.Playing.Font.ResultData.Draw(Skin.Playing.Config.ResultPos.GetPos<float>(5), GetColor(255, 255, 255), GetColor(0, 0, 0), rformat("B2B:   {}", B2B));
		Skin.Playing.Font.ResultData.Draw(Skin.Playing.Config.ResultPos.GetPos<float>(6), GetColor(255, 255, 255), GetColor(0, 0, 0), "Press \"GameReturn\" Key to Retry\n   \"GameBack\" Key to GameEnd");
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
			"Press \"GameBack\" Key Return to Lobby."
		);
	}
}

inline void Game::DrawResult() {
	Skin.Playing.Image.BackGround.Draw();
	DrawBoard();
	DrawHold();
	DrawQueue();
	Skin.Playing.Image.FadeBG.Draw();
	SetDrawBright(255, 255, 255);
	DrawResultData();
}

inline void Game::ProcResult() {

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
			NowScene = Scene::Menu;
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