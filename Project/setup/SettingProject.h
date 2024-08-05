#pragma once
#include <iostream>
#include<vector>
#include<string>

using namespace std;

// class tọa độ 1 điểm
class Vector2D
{
public:
	float x;
	float y;

	Vector2D()
	{
		x = 0.0f;
		y = 0.0f;
	}

	Vector2D(float a, float b)
	{
		x = a;
		y = b;
	}
	Vector2D operator+(Vector2D other) {
		return Vector2D(x + other.x, y + other.y);
	}
	Vector2D operator-(Vector2D other) {
		return Vector2D(x - other.x, y - other.y);
	}
	Vector2D operator*(Vector2D other) {
		return Vector2D(x * other.x, y * other.y);
	}
	Vector2D operator*(float t) {
		return Vector2D(x * t, y * t);
	}
	Vector2D& operator+=(const Vector2D& other) {
		x += other.x;
		y += other.y;
		return *this;
	}
};
class Transform {
public:
	Vector2D rotation;
	Vector2D position;
	Vector2D size;
	Vector2D scale;
	Transform() {};
};
static class Mathf {
public:
	static float Clamp(float value, float min, float max) {
		if (value < min)
			return min;
		if (value > max)
			return max;
		return value;
	}
	static float Distacne(Vector2D x, Vector2D y) {
		Vector2D d = x - y;
		return sqrt(d.x * d.x + d.y * d.y);
	}
};
enum TYPE_IMG
{
	BG,
	BG2,
	BROAD,
	BORDER,
	MOUSE,
	ICON,
	BLOCK,
	NUMBER,
	SCORE,
	NEXT
};
enum TYPE_ICON {
	REPEAT,
	HOME,
	START,
	AUDIO_ON,
	AUDIO_OFF,
	SETTING,
	CHOOSE,
	MENU,
	CHOOSE_LEFT
};
enum ID_AUDIO {
	AUDIO_BG,
	AUDIO_MOUSE,
	AUDIO_GAME_OVER
};
enum BLOCK_TYPE {
	BLOCK_I,
	BLOCK_O,
	BLOCK_T,
	BLOCK_L,
	BLOCK_J,
	BLOCK_S,
	BLOCK_Z
};
namespace SettingProject {
	static std::string pathImg = "./Images\\";
	static std::string fileExtensions[2] = { ".png", ".jpg" };
	static std::string fileNames[10] = { "BG", "Tetris", "Broad", "Border", "iconMouse","", "", "", "Score", "Next"};
	static int isPlayAudio = 1;
	static int indexSkin = 0;
	static BLOCK_TYPE nextBlockType = BLOCK_I;
	static std::string getPath(TYPE_IMG type, int i = 0, int k = 0) {
		switch (type) {
		case BG: {
			return pathImg + fileNames[(int)type] + to_string(rand()%(4) + 1) + fileExtensions[1];
		}
		case SCORE:
		case NEXT:
		case BG2: 
		case BROAD:
		case BORDER:{
			return pathImg + fileNames[(int)type] + fileExtensions[0];
		}
		case MOUSE: {
			return pathImg + fileNames[(int)type] + fileExtensions[0];
		}
		case ICON: {
			return pathImg + to_string(i + 1) + fileExtensions[0];
		}
		case BLOCK: {
			return pathImg + to_string(i*10 +k) + fileExtensions[0];
		}
		case NUMBER: {
			return  pathImg + "n" + to_string(i) + fileExtensions[0];
		}
		}
	}
}