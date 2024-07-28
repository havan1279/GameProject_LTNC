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
	void SetPosition(Vector2D p) {
		position.x = p.x - size.x / 2;
		position.y = p.y - size.y / 2;
	}
	Vector2D GetPosition() {
		return Vector2D(position.x + size.x / 2, position.y + size.y / 2);
	}
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
	MOUSE
};
namespace SettingProject {
	static std::string pathImg = "./Images\\";
	static std::string fileExtension = ".png";
	static std::string fileNames[1] = { "iconMouse" };

	static std::string getPath(TYPE_IMG type) {
		return pathImg + fileNames[(int)type] + fileExtension;
	}
}