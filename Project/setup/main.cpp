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

#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 900

using namespace std;

SDL_Window* gWindow = NULL; // cửa sổ game
SDL_Renderer* gRenderer = NULL; // màn hình xử lý
vector <Mix_Chunk*> sounds;
vector<int> soundIds;
Uint32 gOldTime; // tho
float deltaTime = 0; // chênh lệch 2 frame
float sizeBlock = 45;
// khởi tạo sdl
bool InitSDL();
// đóng sdl
void CloseSDL();
// load ảnh
SDL_Texture* LoadTextureFromFile(string path);
void InitSoundEffect();
void DisAudio();
void PlayAudio(ID_AUDIO type);

// class lưu trữ khi load ảnh lên
class Texture2D
{
public:
	SDL_Renderer* mRenderer; // màn hình hienr thị
	Transform transform;
	SDL_Texture* mTexture; // biến lưu thông tin
	bool isActive;
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
	bool LoadFromFile(string path, int type=0) { // load ảnh
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
		float sizeX = transform.size.x * abs(transform.scale.x);
		float sizeY = transform.size.y * abs(transform.scale.y);
		SDL_Rect r = {transform.position.x - sizeX/2, transform.position.y - sizeY/2, sizeX, sizeY};
		SDL_RendererFlip flip = transform.scale.x < 0 ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
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
		transform.position = Vector2D(Mathf::Clamp(e.button.x, 0, SCREEN_WIDTH), Mathf::Clamp(e.button.y, 0, SCREEN_HEIGHT));
	}
};
class Button :public Texture2D {
public:
	TYPE_ICON _type;
	bool isHigh;
	bool isClick;
	bool isChoose;
	Button(SDL_Renderer* renderer, TYPE_ICON type) :Texture2D(renderer, SettingProject::getPath(TYPE_IMG::ICON, (int)type)) {
		this->_type = type;
		isHigh = false;
		isClick = true;
		isChoose = false;
	}
	void Start() override {
		Texture2D::Start();
	}
	void Update(SDL_Event e, float deltaTime) override {
		Texture2D::Update(e, deltaTime);
		int x = e.button.x;
		int y = e.button.y;
		if (x >= transform.position.x - transform.size.x*abs(transform.scale.x) / 2 && x <= transform.position.x + transform.size.x * abs(transform.scale.x/2)
			&& y >= transform.position.y - transform.size.y * abs(transform.scale.y) / 2 && y <= transform.position.y + transform.size.y * abs(transform.scale.y/2)) { // chuột ngang qua
			OnHigh();  // hiệu ứng phóng to
			switch (e.type)
			{
				case SDL_MOUSEBUTTONDOWN:{
					if (e.button.button == SDL_BUTTON_LEFT && !isClick) {
						isClick = true;
						isChoose = true;
						PlayAudio(ID_AUDIO::AUDIO_MOUSE);
						SDL_Delay(200);
					}
					break;
				}
				case SDL_MOUSEBUTTONUP: {
					if (e.button.button == SDL_BUTTON_LEFT && isClick) {
						isClick = false;
					}
					break;
				}
			}
		}
		else {
			if (!isHigh) return;
			isHigh = false;
			SetScale(Vector2D(transform.scale.x - 0.1f*(transform.scale.x > 0 ? 1 : -1), transform.scale.y - 0.1f * (transform.scale.y > 0 ? 1 : -1)));
		}
	}
	void OnHigh() {
		if (isHigh) return;
		isHigh = true;
		SetScale(Vector2D(transform.scale.x + 0.1f * (transform.scale.x > 0 ? 1 : -1), transform.scale.y + 0.1f * (transform.scale.y > 0 ? 1 : -1)));
	}
};

class Block : public Texture2D {
public:
	int _type;
	int _lvBlock;

	Block(SDL_Renderer* renderer, int type, int lvBlock = 0) : Texture2D(renderer, SettingProject::getPath(BLOCK, SettingProject::indexSkin + 1, type + 1)) {
		_type = type;
		_lvBlock = lvBlock;
		SetScale(Vector2D(sizeBlock / transform.size.x, sizeBlock / transform.size.y));
	}
	void Start() override {

	}
	void Update(SDL_Event e, float deltaTime) override {
		Texture2D::Update(e, deltaTime);
	}
};
class Blocks {
public:
	BLOCK_TYPE _typeBlock;
	Vector2D _position;
	int _face;
	vector<Block*> _listBlock;

	Blocks(SDL_Renderer* renderer, BLOCK_TYPE _type, Vector2D _p, Vector2D _s) {
		_typeBlock = _type;
		_position = _p;
		_face = rand() % 4;

		Block* b1 = new Block(gRenderer, (int)BLOCK_I);
		Block* b2 = new Block(gRenderer, (int)BLOCK_I);
		Block* b3 = new Block(gRenderer, (int)BLOCK_I);
		Block* b4 = new Block(gRenderer, (int)BLOCK_I);
		_listBlock.push_back(b1);
		_listBlock.push_back(b2);
		_listBlock.push_back(b3);
		_listBlock.push_back(b4);
		UpdateIndex();
	}
	void Flip(int** matrix) {
		if (checkCanFlip(matrix)) {
			_face = (_face + 1) % 4;
			UpdateIndex();
		}
	}
	bool checkCanFlip(int** matrix) {
		return true;
	}
	void UpdateIndex() {
		switch (_typeBlock) {
			case BLOCK_I: {
				if (_face == 0) {
					_listBlock[0]->transform.position = _position + Vector2D(-1, 0) * (sizeBlock - 9);
					_listBlock[1]->transform.position = _position + Vector2D(0, 0) * (sizeBlock - 9);
					_listBlock[2]->transform.position = _position + Vector2D(1, 0) * (sizeBlock - 9);
					_listBlock[3]->transform.position = _position + Vector2D(2, 0) * (sizeBlock - 9);
				}
				else if (_face == 1) {
					_listBlock[0]->transform.position = _position + Vector2D(0, -1) * (sizeBlock - 9);
					_listBlock[1]->transform.position = _position + Vector2D(0, 0) * (sizeBlock - 9);
					_listBlock[2]->transform.position = _position + Vector2D(0, 1) * (sizeBlock - 9);
					_listBlock[3]->transform.position = _position + Vector2D(0, 2) * (sizeBlock - 9);
				}
				else if (_face == 2) {
					_listBlock[0]->transform.position = _position + Vector2D(1, 0) * (sizeBlock - 9);
					_listBlock[1]->transform.position = _position + Vector2D(0, 0) * (sizeBlock - 9);
					_listBlock[2]->transform.position = _position + Vector2D(-1, 0) * (sizeBlock - 9);
					_listBlock[3]->transform.position = _position + Vector2D(-2, 0) * (sizeBlock - 9);
				}
				else if (_face == 3) {
					_listBlock[0]->transform.position = _position + Vector2D(0, -2) * (sizeBlock - 9);
					_listBlock[1]->transform.position = _position + Vector2D(0, -1) * (sizeBlock - 9);
					_listBlock[2]->transform.position = _position + Vector2D(0, 0) * (sizeBlock - 9);
					_listBlock[3]->transform.position = _position + Vector2D(0, 1) * (sizeBlock - 9);
				}
				break;
			}
		}
	}
	void Update(SDL_Event e, float deltaTime) {
		for (int i = 0; i < _listBlock.size(); i++)
			_listBlock[i]->Update(e, deltaTime);
	}
};
void Menu();
void SettingMenu();
void PlayGame();
vector<Texture2D*> GetListType(int type, int n = 7);

int main(int argc, char* args[])
{
	if (InitSDL())
	{
		SDL_ShowCursor(SDL_DISABLE);
		srand(time(0)); // cập nhật thời gian hiện tại để làm mới random
		InitSoundEffect();
		soundIds.push_back(Mix_PlayChannel(-1, sounds[0], -1));
		//Menu();
		//SettingMenu();
		PlayGame();
	}
	CloseSDL();
	return 0;
}
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
void InitSoundEffect() {
	sounds.push_back(Mix_LoadWAV("./Sounds/BG.wav")); /// phát nhạc
	sounds.push_back(Mix_LoadWAV("./Sounds/Mouse.wav")); /// phát nhạc
}
void DisAudio() {
	Mix_Pause(soundIds[0]);
	for (int i = 1; i < soundIds.size(); i++)
		Mix_HaltChannel(soundIds[i]);
}
void PlayAudio(ID_AUDIO type) {
	if (SettingProject::isPlayAudio == -1) return;
	if (type == 0) {
		Mix_Resume(soundIds[0]);
	}
	else
		soundIds.push_back(Mix_PlayChannel(-1, sounds[(int)type], type == 0 ? -1 : 0));
}
void Menu() {
	Texture2D BG(gRenderer, SettingProject::getPath(TYPE_IMG::BG));
	BG.SetScale(Vector2D(SCREEN_WIDTH / BG.transform.size.x, SCREEN_HEIGHT / BG.transform.size.y));
	BG.transform.position = Vector2D(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);

	Texture2D BG2(gRenderer, SettingProject::getPath(TYPE_IMG::BG2));
	BG2.SetScale(Vector2D(SCREEN_WIDTH / BG.transform.size.x + 1, SCREEN_HEIGHT / BG.transform.size.y + 1));
	BG2.transform.position = Vector2D(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 150);

	Mouse mouse(gRenderer, SettingProject::getPath(TYPE_IMG::MOUSE));
	mouse.SetScale(Vector2D(0.7f, 0.7f));


	Button btnStart(gRenderer, TYPE_ICON::START);
	btnStart.SetScale(Vector2D(1, 1));
	btnStart.transform.position = Vector2D(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);

	Button btnAudio(gRenderer, TYPE_ICON::AUDIO_ON);
	btnAudio.SetScale(Vector2D(0.5f, 0.5f));
	btnAudio.transform.position = Vector2D(SCREEN_WIDTH / 2 - 140, SCREEN_HEIGHT / 2 + 170);

	Button btnSetting(gRenderer, TYPE_ICON::SETTING);
	btnSetting.SetScale(Vector2D(0.5f, 0.5f));
	btnSetting.transform.position = Vector2D(SCREEN_WIDTH / 2 + 140, SCREEN_HEIGHT / 2 + 170);

	SDL_Event e;
	gOldTime = SDL_GetTicks(); // lấy thời gian hiện tại
	while (true) {
		deltaTime = (SDL_GetTicks() - gOldTime) / 1000.0; // tính bằng giây
		//cout << deltaTime << endl;
		SDL_PollEvent(&e); // sự kiện 
		SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0x00);
		SDL_RenderClear(gRenderer); // xóa dữ liệu cũ
		BG.Update(e, deltaTime);
		BG2.Update(e, deltaTime);
		btnStart.Update(e, deltaTime);
		btnAudio.Update(e, deltaTime);
		btnSetting.Update(e, deltaTime);
		mouse.Update(e, deltaTime);

		if (btnStart.isChoose) {
			PlayGame();
			return;
		}
		if (btnAudio.isChoose) {
			btnAudio.isChoose = false;
			SettingProject::isPlayAudio *= -1;
			if (SettingProject::isPlayAudio == 1) {
				PlayAudio(ID_AUDIO::AUDIO_BG);
				btnAudio.LoadFromFile(SettingProject::getPath(TYPE_IMG::ICON, TYPE_ICON::AUDIO_ON), 1);
			}
			else {
				DisAudio();
				btnAudio.LoadFromFile(SettingProject::getPath(TYPE_IMG::ICON, TYPE_ICON::AUDIO_OFF), 1);
			}
		}
		if (btnSetting.isChoose) {
			SettingMenu();
			return;
		}

		SDL_RenderPresent(gRenderer); // hiển thị ra màn hình
		//cout << SDL_GetTicks() - gOldTime << endl;
		//SDL_zero(e);
		gOldTime = SDL_GetTicks(); // lấy thời gian hiện tại
		SDL_Delay(20);
	}
}

vector<Texture2D*> GetListType(int type, int n) {
	vector<Texture2D*> result;
	for (int i = 0; i < n; i++) {
		Texture2D* x = new Texture2D(gRenderer, SettingProject::getPath(BLOCK, type + 1, i + 1));
		x->SetScale(Vector2D(0.2, 0.2));
		result.push_back(x);
	}
	return result;
}
void SettingMenu() {
	Texture2D BG(gRenderer, SettingProject::getPath(TYPE_IMG::BG));
	BG.SetScale(Vector2D(SCREEN_WIDTH / BG.transform.size.x, SCREEN_HEIGHT / BG.transform.size.y));
	BG.transform.position = Vector2D(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
	//cout << SCREEN_WIDTH  << " " << BG->transform.size.x << " " << SCREEN_HEIGHT  << " " << BG->transform.size.y;

	Mouse mouse(gRenderer, SettingProject::getPath(TYPE_IMG::MOUSE));
	mouse.SetScale(Vector2D(0.7f, 0.7f));

	Button btnHome(gRenderer, TYPE_ICON::HOME);
	btnHome.SetScale(Vector2D(0.3f, 0.3f));
	btnHome.transform.position = Vector2D(SCREEN_WIDTH - 50, 50);

	Button btnChoose(gRenderer, TYPE_ICON::CHOOSE);
	btnChoose.SetScale(Vector2D(0.7f, 0.7f));
	btnChoose.transform.position = Vector2D(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 300);

	Button btnLeft(gRenderer, TYPE_ICON::CHOOSE_LEFT);
	btnLeft.SetScale(Vector2D(0.5f, 0.5f));
	btnLeft.transform.position = Vector2D(SCREEN_WIDTH / 2 - 200, SCREEN_HEIGHT / 2 + 300);

	Button btnRight(gRenderer, TYPE_ICON::CHOOSE_LEFT);
	btnRight.SetScale(Vector2D(-0.5f, 0.5f));
	btnRight.transform.position = Vector2D(SCREEN_WIDTH / 2 + 200, SCREEN_HEIGHT / 2 + 300);


	int index = SettingProject::indexSkin;

	float _x = 20;
	vector<Texture2D*> imgs = GetListType(index);
	imgs[0]->transform.position = Vector2D(150 - _x, 300);
	imgs[1]->transform.position = Vector2D(300 - _x, 300);
	imgs[2]->transform.position = Vector2D(450 - _x, 300);
	imgs[3]->transform.position = Vector2D(100 - _x, 500);
	imgs[4]->transform.position = Vector2D(250 - _x, 500);
	imgs[5]->transform.position = Vector2D(400 - _x, 500);
	imgs[6]->transform.position = Vector2D(550 - _x, 500);

	SDL_Event e;
	//int deltatime = 0;
	gOldTime = SDL_GetTicks(); // lấy thời gian hiện tại
	while (true) {
		deltaTime = (SDL_GetTicks() - gOldTime) / 1000.0; // tính bằng giây
		//cout << deltaTime << endl;
		SDL_PollEvent(&e); // sự kiện 
		SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0x00);
		SDL_RenderClear(gRenderer); // xóa dữ liệu cũ
		BG.Update(e, deltaTime);
		btnHome.Update(e, deltaTime);
		btnChoose.Update(e, deltaTime);
		btnLeft.Update(e, deltaTime);
		btnRight.Update(e, deltaTime);
		for (int i = 0; i < imgs.size(); i++)
			imgs[i]->Update(e, deltaTime);
		mouse.Update(e, deltaTime);

		if (btnHome.isChoose) {
			for (int i = 0; i < imgs.size(); i++)
				delete imgs[i];
			Menu();
			return;
		}
		if (btnChoose.isChoose) {
			for (int i = 0; i < imgs.size(); i++)
				delete imgs[i];
			SettingProject::indexSkin = index;
			Menu();
			return;
		}
		if (btnLeft.isChoose) {
			btnLeft.isChoose = false;
			if (index > 0) {
				index--;
				for (int i = 0; i < imgs.size(); i++)
					delete imgs[i];
				imgs = GetListType(index);
				imgs[0]->transform.position = Vector2D(150 - _x, 300);
				imgs[1]->transform.position = Vector2D(300 - _x, 300);
				imgs[2]->transform.position = Vector2D(450 - _x, 300);
				imgs[3]->transform.position = Vector2D(100 - _x, 500);
				imgs[4]->transform.position = Vector2D(250 - _x, 500);
				imgs[5]->transform.position = Vector2D(400 - _x, 500);
				imgs[6]->transform.position = Vector2D(550 - _x, 500);
			}
		}
		if (btnRight.isChoose) {
			btnRight.isChoose = false;
			if (index < 7) {
				index++;
				for (int i = 0; i < imgs.size(); i++)
					delete imgs[i];
				imgs = GetListType(index);
				imgs[0]->transform.position = Vector2D(150 - _x, 300);
				imgs[1]->transform.position = Vector2D(300 - _x, 300);
				imgs[2]->transform.position = Vector2D(450 - _x, 300);
				imgs[3]->transform.position = Vector2D(100 - _x, 500);
				imgs[4]->transform.position = Vector2D(250 - _x, 500);
				imgs[5]->transform.position = Vector2D(400 - _x, 500);
				imgs[6]->transform.position = Vector2D(550 - _x, 500);
			}
		}
		SDL_RenderPresent(gRenderer); // hiển thị ra màn hình
		gOldTime = SDL_GetTicks(); // lấy thời gian hiện tại
		SDL_Delay(20);
	}
}
void PlayGame() {
	Texture2D BG(gRenderer, SettingProject::getPath(TYPE_IMG::BG));
	BG.SetScale(Vector2D(SCREEN_WIDTH / BG.transform.size.x, SCREEN_HEIGHT / BG.transform.size.y));
	BG.transform.position = Vector2D(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
	
	Texture2D broad(gRenderer, SettingProject::getPath(TYPE_IMG::BROAD));
	broad.SetScale(Vector2D(1.22f, 1.22f));
	broad.transform.position = Vector2D(SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 50);

	Texture2D border(gRenderer, SettingProject::getPath(TYPE_IMG::BORDER));
	border.SetScale(Vector2D(1.2f, 1.2f));
	border.transform.position = Vector2D(SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 50);

	Mouse mouse(gRenderer, SettingProject::getPath(TYPE_IMG::MOUSE));
	mouse.SetScale(Vector2D(0.7f, 0.7f));

	Button btnHome(gRenderer, TYPE_ICON::HOME);
	btnHome.SetScale(Vector2D(0.3f, 0.3f));
	btnHome.transform.position = Vector2D(SCREEN_WIDTH - 50, 50);

	Block* block1 = new Block(gRenderer, 0);
	block1->SetScale(Vector2D(45/ block1->transform.size.x, 45/ block1->transform.size.y));
	block1->transform.position = Vector2D(110, 295);

	Blocks* t = new Blocks(gRenderer, BLOCK_I, Vector2D(110, 295), Vector2D(1, 1));
	//cout << block1->transform.scale.x * block1->transform.size.x;
	SDL_Event e;
	//int deltatime = 0;
	gOldTime = SDL_GetTicks(); // lấy thời gian hiện tại
	while (true) {
		deltaTime = (SDL_GetTicks() - gOldTime) / 1000.0; // tính bằng giây
		//cout << deltaTime << endl;
		SDL_PollEvent(&e); // sự kiện 
		SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0x00);
		SDL_RenderClear(gRenderer); // xóa dữ liệu cũ
		BG.Update(e, deltaTime);
		broad.Update(e, deltaTime);
		border.Update(e, deltaTime);
		btnHome.Update(e, deltaTime);
		t->Update(e, deltaTime);

		mouse.Update(e, deltaTime);

		if (btnHome.isChoose) {
			//Menu();
			//return;
			t->Flip(NULL);
			btnHome.isChoose = false;
		}
		
		SDL_RenderPresent(gRenderer); // hiển thị ra màn hình
		gOldTime = SDL_GetTicks(); // lấy thời gian hiện tại
		SDL_Delay(20);
	}
}