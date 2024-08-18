#pragma once
#include"Texture2D.h"


class Block : public Texture2D {
	public:
		int _type; // loại khối
		int _lvBlock; // dự án phát triển thêm: tạo khóa cho khối, khối đặc biệt, ...
		Vector2D _index; // tọa độ trong ma trận hiển thị
		float sizeBlock = 45; // kích thước khối
		Vector2D startIndex = Vector2D(38, 149.8); // tọa độ bắt đầu (điểm 0, 0 trong ma trận)	

		Block(SDL_Renderer* renderer, Vector2D index, int type, int lvBlock = 0) : Texture2D(renderer, SettingProject::getPath(BLOCK, SettingProject::indexSkin + 1, type + 1)) {
			_type = type;
			_lvBlock = lvBlock;
			_index = index;
			transform.size = Vector2D(sizeBlock, sizeBlock);
		}
		void Start() override {
	
		}
		void Update(SDL_Event e, float deltaTime) override {
			if (SettingProject::endGame == 1) {
				Texture2D::Update(e, deltaTime); 
				return;
			}
			if (!isActive) return; // nếu không hiển thị thì kết thúc
			if (transform.scale.x >= 1) { // nếu kích thước >= 1 và tạo độ trong ma trận trục y < 0 => không hiển thị tạo hiệu ứng không hiển thị khi khối không trong ma trận
				if( _index.y < 0) return;
			}
			transform.position = startIndex + _index * transform.scale.x * (sizeBlock - 8.7); 
			// tọa độ đối tượng = tọa độ bắt đầu + vị trí trong ma trận * độ phóng đại * kích thước thật
			// kích thước thật = kích thước ảnh - kích thước thừa (vùng thừa)
			Texture2D::Update(e, deltaTime);
		}
	};

