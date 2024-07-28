#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <iostream>
#include<vector>
#include<string>
#include<stdlib.h>
#include<fstream>
#include <random>
#include <ctime>
#include"SettingProject.h"

#define SCREEN_WIDTH 900
#define SCREEN_HEIGHT 600

using namespace std;

SDL_Window* gWindow = NULL; // cửa sổ game
SDL_Renderer* gRenderer = NULL; // màn hình xử lý
Uint32 gOldTime; // tho
float deltaTime = 0; // chênh lệch 2 frame

// khởi tạo sdl
bool InitSDL()
{
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	Mix_Init(MIX_INIT_MP3);
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		cout << "SDL did not initialise. Error: " << SDL_GetError();
		return false;
	}
	else
	{
		gWindow = SDL_CreateWindow("Games Engine Creation",
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			SCREEN_WIDTH,
			SCREEN_HEIGHT,
			SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
		{
			cout << "Window was not created. Error : " << SDL_GetError();
			return false;
		}
	}

	gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
	if (gRenderer != NULL)
	{
		int imageFlags = IMG_INIT_PNG;
		if (!(IMG_Init(imageFlags) & imageFlags))
		{
			cout << "SDL_Image could not initialise. Error: " << IMG_GetError();
			return false;
		}

	}

	else
	{
		cout << "Renderer could not initialise. Error: " << SDL_GetError();
		return false;
	}

	return true;


}
// đóng sdl
void CloseSDL()
{
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;

	IMG_Quit();
	SDL_Quit();

	SDL_DestroyRenderer(gRenderer);
	gRenderer = NULL;
	Mix_CloseAudio();
	Mix_Quit();
}
// load ảnh
SDL_Texture* LoadTextureFromFile(string path)
{
	SDL_Texture* pTexture = NULL;

	SDL_Surface* pSurface = IMG_Load(path.c_str());
	if (pSurface != NULL)
	{
		pTexture = SDL_CreateTextureFromSurface(gRenderer, pSurface);
		if (pTexture == NULL)
		{
			cout << "Unable to create texture from surface. Error: " << SDL_GetError() << endl;
		}
		SDL_FreeSurface(pSurface);
	}
	else
	{
		cout << "Unable to create texture from surface. Error: " << IMG_GetError() << endl;
	}
	return pTexture;
}

// class lưu trữ khi load ảnh lên
class Texture2D
{
public:
	SDL_Renderer* mRenderer; // màn hình hienr thị
	Transform transform;
	SDL_Texture* mTexture; // biến lưu thông tin
	bool isActive;
	Texture2D() {
		mRenderer = NULL;
		mTexture = NULL;
	}
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
	bool LoadFromFile(string path) { // load ảnh
		Free(); // giải phóng cũ
		SDL_Surface* pSurface = IMG_Load(path.c_str());

		if (pSurface != NULL)
		{
			SDL_SetColorKey(pSurface, SDL_TRUE, SDL_MapRGB(pSurface->format, 0, 0xFF, 0xFF)); // xóa nền

			transform.size.x = pSurface->w; 
			transform.size.y = pSurface->h;
			transform.scale = Vector2D(1, 1);

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
		transform.size.x *= s.x/transform.scale.x;
		transform.size.y *= s.y / transform.scale.y;
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
		SDL_Rect r = {transform.position.x, transform.position.y, transform.size.x, transform.size.y };
		SDL_RendererFlip flip = transform.scale.x == -1 ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
		float angle = transform.rotation.x;
		SDL_RenderCopyEx(mRenderer, mTexture, NULL, &r, angle, NULL, flip);
	}
};

class Mouse :public Texture2D {
public:
	Mouse(SDL_Renderer* renderer, string path):Texture2D(renderer, path){}
	void Start() override {
		Texture2D::Start();
	}
	void Update(SDL_Event e, float deltaTime) override {
		Texture2D::Update(e, deltaTime);
		transform.SetPosition(Vector2D(Mathf::Clamp(e.button.x, 0, SCREEN_WIDTH), Mathf::Clamp(e.button.y, 0, SCREEN_HEIGHT)));
	}
};


int main(int argc, char* args[])
{
	if (InitSDL())
	{
		SDL_ShowCursor(SDL_DISABLE);
		srand(time(0)); // cập nhật thời gian hiện tại để làm mới random
		/*Mix_Chunk* sound = Mix_LoadWAV("./Sounds/BackGround.wav"); /// phát nhạc
		Mix_PlayChannel(-1, sound, 0);*/
		Mouse mouse(gRenderer, SettingProject::getPath(TYPE_IMG::MOUSE));
		mouse.SetScale(Vector2D(0.7f, 0.7f));
		SDL_Event e;
		float deltatime = 0;
		while (true) {
			SDL_PollEvent(&e); // sự kiện 
			gOldTime = SDL_GetTicks(); // lấy thời gian hiện tại
			SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0x00);
			SDL_RenderClear(gRenderer); // xóa dữ liệu cũ
			
			mouse.Update(e, deltatime);


			SDL_RenderPresent(gRenderer); // hiển thị ra màn hình
			deltatime = (float)(SDL_GetTicks() - gOldTime) / 1000.0f; // tính độ chênh giữa 2
		}
	}
	CloseSDL();
	return 0;
}
