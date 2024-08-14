#pragma once
#include"Texture2D.h"


class Block : public Texture2D {
	public:
		int _type;
		int _lvBlock;
		Vector2D _index;
		float sizeBlock = 45;
		Vector2D startIndex = Vector2D(38, 149.8);

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
			if (!isActive) return;
			if (transform.scale.x >= 1) {
				if( _index.y < 0) return;
			}
			transform.position = startIndex + _index * transform.scale.x * (sizeBlock - 8.7);
			Texture2D::Update(e, deltaTime);
		}
	};

