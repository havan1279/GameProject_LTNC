#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <iostream>
#include"SettingProject.h"

class Texture2D
{
public:
	SDL_Renderer* mRenderer; // màn hình hienr thị
	Transform transform;
	SDL_Texture* mTexture; // biến lưu thông tin
	bool isActive; // trạng thái hiển thị
	Texture2D() { mRenderer = NULL; mTexture = NULL; }
	Texture2D(SDL_Renderer* renderer, string path) { // khởi tạo
		mRenderer = renderer;
		if (!LoadFromFile(path)) {
			cout << "Loi hinh anh " << path << endl;
		}
		isActive = true;
	}
	void Free() { // xóa bộ nhớ
		if (mTexture != NULL)
		{
			SDL_DestroyTexture(mTexture);
			mTexture = NULL;
		}
	}
	~Texture2D() { // giải phóng bộ nhớ
		Free();
		mRenderer = NULL;
	}
	bool LoadFromFile(string path, int type = 0) { // load ảnh
		Free(); // giải phóng cũ
		SDL_Surface* pSurface = IMG_Load(path.c_str());

		if (pSurface != NULL)
		{
			SDL_SetColorKey(pSurface, SDL_TRUE, SDL_MapRGB(pSurface->format, 0, 0xFF, 0xFF)); // xóa nền
			if (type == 0) {
				transform.size.x = pSurface->w;
				transform.size.y = pSurface->h;
				transform.scale = Vector2D(1, 1);
			}
			mTexture = SDL_CreateTextureFromSurface(mRenderer, pSurface);
			if (mTexture == NULL)
			{
				cout << "Unable to create texture from surface. Error: " << SDL_GetError() << endl;
			}
			SDL_FreeSurface(pSurface);
		}
		else
		{
			cout << "Unable to create texture from surface. Error: " << IMG_GetError() << endl;
		}
		return mTexture != NULL;
	}
	void SetScale(Vector2D s) {
		transform.scale = s;
	}
	// hiển thị đối tượng: vị trí, cách lấy đối xứng, góc xoay, phạm vi lấy, khung hiển thị
	virtual void Start() {

	}
	virtual void Update(SDL_Event e, float deltaTime) {
		if (!isActive) return;
		this->Render();
	}
	void Render() { // load ảnh lên màn hình xử lý
		SDL_SetRenderDrawColor(mRenderer, 0x00, 0x00, 0x00, 0x00);
		// vẽ đối tượng lên màn hình xử lý, với khung vừa có, góc xoay angle, và cách lấy đối xứng flip
		float sizeX = transform.size.x * abs(transform.scale.x); // = kích thước gốc* độ phóng đại (có thể âm nên phải lấy ||
		float sizeY = transform.size.y * abs(transform.scale.y);
		SDL_Rect r = { transform.position.x - sizeX / 2, transform.position.y - sizeY / 2, sizeX, sizeY };
		SDL_RendererFlip flip = transform.scale.x < 0 ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
		float angle = transform.rotation.x;
		SDL_RenderCopyEx(mRenderer, mTexture, NULL, &r, angle, NULL, flip);
	}
};

