#pragma once
#include"Texture2D.h"
class Mouse :public Texture2D {
public:
	Mouse(SDL_Renderer* renderer, string path):Texture2D(renderer, path){}
	void Start() override {
		Texture2D::Start();
	}
	void Update(SDL_Event e, float deltaTime) override {
		Texture2D::Update(e, deltaTime);
		transform.position = Vector2D(Mathf::Clamp(e.button.x, 0, SCREEN_WIDTH), Mathf::Clamp(e.button.y, 0, SCREEN_HEIGHT)); 
		// cập nhật vị trí bằng cách lấy tọa độ chuột nhưng có giới hạn phạm vi trong màn hình hiển thị
	}
};

