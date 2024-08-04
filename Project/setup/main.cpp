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
bool isPlayGame = true;
float sizeBlock = 45;
Vector2D startIndex = Vector2D(38, 149.8);

int sizeM = 20, sizeN = 10;
int field[20][10] = { 0 };

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
class Score {
public:
	SDL_Renderer* mRenderer;
	Texture2D* txtName;
	vector<Texture2D*> imgs;
	Vector2D position;
	Vector2D scale;

	Score(SDL_Renderer* renderer, Vector2D p, Vector2D s) {
		txtName = new Texture2D(renderer, SettingProject::getPath(TYPE_IMG::SCORE));
		txtName->transform.position = p - Vector2D(70, 0);
		mRenderer = renderer;
		position = p;
		scale = s;
	}
	void Update(SDL_Event e, float deltaTime){
		txtName->Update(e, deltaTime);
		for (int i = 0; i < imgs.size(); i++)
			imgs[i]->Update(e, deltaTime);
	}
	void SetValue(int score) {
		if (score > 99999)
			score = 99999;
		Destroy();
		string s_score = to_string(score);
		for (int i = 0; i < s_score.size(); i++) {
			Texture2D* t = new Texture2D(mRenderer, SettingProject::getPath(TYPE_IMG::NUMBER, s_score[i] - 48));
			t->transform.position = position + Vector2D(25, 0) * i;
			imgs.push_back(t);
		}
	}
	void Destroy() {
		for (int i = 0; i < imgs.size(); i++)
			delete imgs[i];
		imgs.clear();
	}
};
class Block : public Texture2D {
public:
	int _type;
	int _lvBlock;
	Vector2D _index;

	Block(SDL_Renderer* renderer, Vector2D index, int type, int lvBlock = 0) : Texture2D(renderer, SettingProject::getPath(BLOCK, SettingProject::indexSkin + 1, type + 1)) {
		_type = type;
		_lvBlock = lvBlock;
		_index = index;
		transform.size = Vector2D(sizeBlock, sizeBlock);
	}
	void Start() override {

	}
	void Update(SDL_Event e, float deltaTime) override {
		if (!isActive) return;
		if (transform.scale.x >= 1) {
			if(_index.x < 0 || _index.x >= sizeN
			|| _index.y < 0 || _index.y >= sizeM) return;
		}
		transform.position = startIndex + _index * transform.scale.x * (sizeBlock - 8.7);
		Texture2D::Update(e, deltaTime);
	}
};
class Blocks {
	int figures[7][4] = {
		{1, 3, 5, 7}, // I
		{2, 3, 4, 5}, // O
		{3, 5, 4, 7}, // T
		{2, 3, 5, 7}, // L
		{3, 5, 7, 6}, // J
		{3, 5, 4, 6}, // S
		{2, 4, 5, 7} // Z
	};
public:
	BLOCK_TYPE _typeBlock;
	vector<Block*> _listBlock;
	float timeDelay;
	float _timeDelay;
	bool isClick = false;
	bool isActive = true;
	Blocks(SDL_Renderer* renderer, BLOCK_TYPE _type, Vector2D _i, Vector2D _s, float _time) {
		_typeBlock = _type;
		timeDelay = _time;
		_timeDelay = _time;
		Init(_i, _s);
	}
	~Blocks() {
		for (int i = 0; i < 4; i++)
			_listBlock[i]->Free();
	}
	void Init(Vector2D _i, Vector2D _s) {
		for (int i = 0; i < 4; i++) {
			Block* b1 = new Block(gRenderer, _i + Vector2D(figures[(int)_typeBlock][i] % 2, figures[(int)_typeBlock][i] / 2), (int)_typeBlock);
			b1->SetScale(_s);
			_listBlock.push_back(b1);
		}
	}
	bool Check(){
		for (int i = 0; i < 4; i++) {
			if (_listBlock[i]->_index.x < 0 || _listBlock[i]->_index.x >= sizeN
				|| _listBlock[i]->_index.y < 0 || _listBlock[i]->_index.y >= sizeM) return false;
			if (field[(int)_listBlock[i]->_index.y][(int)_listBlock[i]->_index.x] != 0) return false;
		}
		return true;
	}
	void Rotate() {
		Vector2D p = _listBlock[1]->_index;
		for (int i = 0; i < 4; i++) {
			Vector2D t = Vector2D(_listBlock[i]->_index.y - p.y, _listBlock[i]->_index.x - p.x);
			_listBlock[i]->_index = Vector2D(p.x - t.x, p.y + t.y);
		}
	}
	void Update(SDL_Event e, float deltaTime) {
		for (int i = 0; i < _listBlock.size(); i++) {
			_listBlock[i]->Update(e, deltaTime);
		}
			
		if (timeDelay < 0) return;
		int dx = 0, dy = 0;
		if (e.type == SDL_KEYDOWN && !isClick) {
			isClick = true;
			switch (e.key.keysym.sym) {
			case SDLK_LEFT: {
				dx = -1;
				break;
			}
			case SDLK_RIGHT: {
				dx = 1;
				break;
			}
			case SDLK_DOWN: {
				dy = 1;
				break;
			}
			case SDLK_UP: {
				Rotate();
				break;
			}
			}
		}
		else if (e.type == SDL_KEYUP) {
			isClick = false;
		}
		if (_timeDelay > 0) {
			_timeDelay -= deltaTime;
		}
		else {
			_timeDelay = timeDelay;
			dy += 1;
		}
		Move(Vector2D(dx, dy));
	}
	void Move(Vector2D velocity) {
		Vector2D b[4];
		for (int i = 0; i < 4; i++) {
			b[i] = _listBlock[i]->_index;
			_listBlock[i]->_index += velocity;
		}
		if (!Check()) {
			for (int i = 0; i < 4; i++) {
				_listBlock[i]->_index = b[i];
			}
			isActive = false;
		}
	}
};
/*
class BlockManager {
public:
	Texture2D* txtName;
	Blocks* show;
	SDL_Renderer* mRenderer;
	Vector2D position;

	int pre1, pre2;

	int m, n;
	int** matrix;

	BlockManager(SDL_Renderer* renderer, Vector2D p) {
		mRenderer = renderer;
		position = p;
		txtName = new Texture2D(renderer, SettingProject::getPath(TYPE_IMG::NEXT));
		txtName->transform.position = p - Vector2D(0, 70);

		m = 20;
		n = 10;
		int** matrix = new int* [m];
		for (int i = 0; i < m; i++)
			matrix[i] = new int[n];
		for (int i = 0; i < m; i++)
			for (int j = 0; j < n; j++)
				matrix[i][j] = 0;

		pre1 = -1;
		pre2 = -1;

		InitBlock();
	}
	void Update(SDL_Event e, float deltaTime) {
		txtName->Update(e, deltaTime);
		show->Update(e, deltaTime, NULL);
	}
	void InitBlock() {
		float tiLe[] = { 20, 10, 15, 15, 15, 12,5, 12.5 };
		int result = rand()%7;
		for (int i = 0; i < 10; i++) {
			float x = (rand() % 1000) / 10.0;
			vector<int> listPoint;
			for (int j = 0; j < 7; j++) {
				if (x < tiLe[j]) {
					listPoint.push_back(j);
				}
			}
			if (listPoint.size() > 0) {
				result = listPoint[rand() % listPoint.size()];
				if (result != pre1 && result != pre2) {
					break;
				}
			}
		}
		show = new Blocks(gRenderer, (BLOCK_TYPE)result, position, Vector2D(0.8, 0.8), -1);
		pre2 = pre1;
		pre1 = result;
	}
	int getValue(Vector2D p) {
		Vector2D x = ConvertPositionToIndex(p);
		return 0;
	}
};
*/
void Menu();
void SettingMenu();
void PlayGame();
vector<Texture2D*> GetListType(int type, int n = 7);
vector<Block*> GetMatrixShow();

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
vector<Block*> GetMatrixShow() {
	vector<Block*> result;
	for (int i = 0; i < sizeM; i++) 
		for(int j=0; j<sizeN; j++){ // Vector2D index, int type, int lvBlock = 0
			Block* x = new Block(gRenderer, Vector2D(j, i), 0);
			//x->isActive = false;
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

	int score = 0;
	Score showScore(gRenderer, Vector2D(170, 50), Vector2D(1, 1));
	showScore.SetValue(score);

	//BlockManager* t = new BlockManager(gRenderer, Vector2D(500, 295));
	Blocks* t2 = new Blocks(gRenderer, BLOCK_Z, Vector2D(0, 0), Vector2D(1, 1), 1);// 110, 295
	SDL_Event e;

	gOldTime = SDL_GetTicks(); // lấy thời gian hiện tại
	SDL_PollEvent(&e); // sự kiện 
	BG.Update(e, deltaTime);
	broad.Update(e, deltaTime);
	btnHome.Update(e, deltaTime);
	border.Update(e, deltaTime);
	SDL_RenderPresent(gRenderer); // hiển thị ra màn hình

	vector<Block*> matrixShow = GetMatrixShow();
	while (true) {
		deltaTime = (SDL_GetTicks() - gOldTime) / 1000.0; // tính bằng giây
		//cout << deltaTime << endl;
		SDL_PollEvent(&e); // sự kiện 
		SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0x00);
		SDL_RenderClear(gRenderer); // xóa dữ liệu cũ
		BG.Update(e, deltaTime);
		broad.Update(e, deltaTime);
		btnHome.Update(e, deltaTime);
		showScore.Update(e, deltaTime);

		//t->getValue(Vector2D(e.button.x, e.button.y));
		//cout << e.button.x << " " << e.button.y << endl;
		//t->Update(e, deltaTime);
		t2->Update(e, deltaTime);

		if (!t2->isActive) {
			for (int i = 0; i < 4; i++) {
				cout << (int)t2->_listBlock[i]->_index.x << " " << (int)t2->_listBlock[i]->_index.y << endl;
				field[(int)t2->_listBlock[i]->_index.x][(int)t2->_listBlock[i]->_index.y] = (int)t2->_typeBlock;
				matrixShow[int(t2->_listBlock[i]->_index.y * sizeN + t2->_listBlock[i]->_index.x)]->_type = t2->_typeBlock;
				matrixShow[int(t2->_listBlock[i]->_index.y * sizeN + t2->_listBlock[i]->_index.x)]->LoadFromFile(SettingProject::getPath(BLOCK, SettingProject::indexSkin + 1, t2->_typeBlock + 1));
			}
			delete t2;
			t2 = new Blocks(gRenderer, BLOCK_Z, Vector2D(0, 0), Vector2D(1, 1), 1);// 110, 295
		}
		for(int i=0; i<sizeM; i++)
			for (int j = 0; j < sizeN; j++) {
				if(field[i][j] != 0)
					matrixShow[i * sizeN + j]->Update(e, deltaTime);
			}

		border.Update(e, deltaTime);
		mouse.Update(e, deltaTime);

		score++;
		showScore.SetValue(score);

		if (btnHome.isChoose) {
			Menu();
			return;
		}
		
		SDL_RenderPresent(gRenderer); // hiển thị ra màn hình
		gOldTime = SDL_GetTicks(); // lấy thời gian hiện tại
		SDL_Delay(20);
	}
}