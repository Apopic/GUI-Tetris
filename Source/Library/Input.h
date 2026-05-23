#pragma once

struct InputFlag {

	enum class State : uint8_t {
		Down = 0b0001,
		Press = 0b0010,
		Up = 0b0100,
		Release = 0b1000
	};

	bool Down() const {
		return state == State::Down;
	}

	bool Press() const {
		return state == State::Press || Down();
	}

	bool Up() const {
		return state == State::Up;
	}

	bool Release() const {
		return state == State::Release || Up();
	}

	void Update(bool flag) {
		//switch (state) {
		//case State::Down:
		//case State::Press:
		//	(byte&)state = (((int)flag - 1) ^ 0b0011) & 0b0110;
		//	break;
		//case State::Up:
		//case State::Release:
		//	(byte&)state = (((int)flag - 1) ^ 0b0011) & 0b1001;
		//	break;
		//}

		state = static_cast<State>(
			((((uint8_t)flag - 1) ^ 0b0011) & (((-((int8_t)state & 0b0011)) >> 7) ^ 0b1001)) & 0b1111
			);
	}

	State state = State::Release;
};

struct ScreenInput {

	float GetScrollPos(float addscr, float scr_min, float scr_max) {
		float scr = 0.0f;
		Get_Scroll += addscr;
		if (Get_Scroll != 0.0f && !IsScrolling()) {
			ScrollTimer.Start();
		}
		if (IsScrolling()) {
			double rate = ease::GetEasingRate(ScrollTimer.GetElapsed().Second() / ScrollTime, ease::Base::Out, ease::Line::Cubic);
			scr = std::clamp((float)(Current_Scroll + Get_Scroll * rate), scr_min, scr_max);
			if (rate >= 1.0) {
				Get_Scroll = 0.0f;
				Current_Scroll = scr;
				ScrollTimer.Stop();
				ScrollTimer.Reset();
			}
			return scr;
		}
		return Current_Scroll;
	}

	void SetScrollTimer(double time) {
		ScrollTime = time;
	}

	bool IsScrolling() const {
		return ScrollTimer.IsRunning();
	}

	void ResetScrollPos() {
		Get_Scroll = 0;
		Current_Scroll = 0;
	}

	bool IsScreenClick(int clicktype) const {
		return (GetMouseInput() & clicktype);
	}

private:

	libarrier::Timer ScrollTimer;
	double ScrollTime = 0.25;

	float Get_Scroll = 0.0f;
	float Current_Scroll = 0.0f;
};