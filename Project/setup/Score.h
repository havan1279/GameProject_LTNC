#pragma once
#include"Texture2D.h"
class Score {
public:
	SDL_Renderer* mRenderer;
	Texture2D* txtName; // đối tượng text hiển thị
	vector<Texture2D*> imgs; // danh sách các đối tượng số
	Vector2D position; // tọa độ
	Vector2D scale;

	Score(SDL_Renderer* renderer, Vector2D p, Vector2D s) {
		txtName = new Texture2D(renderer, SettingProject::getPath(TYPE_IMG::SCORE));
		txtName->transform.position = p - Vector2D(70, 0); // dịch đối tượng chữ sang trái
		mRenderer = renderer;
		position = p;
		scale = s;
	}
	void Update(SDL_Event e, float deltaTime, bool show = true) {
		if (show)
			txtName->Update(e, deltaTime);
		for (int i = 0; i < imgs.size(); i++)
			imgs[i]->Update(e, deltaTime);
	}
	void SetScale(Vector2D x) {
		scale = x;
		for (int i = 0; i < imgs.size(); i++) { // thay đổi kích thước của các đối tượng số
			imgs[i]->SetScale(x);
			imgs[i]->transform.position = position + Vector2D(25, 0) * i * scale.x; // tọa độ mới = tọa độ gốc + vector khoảng cách* thứ tự*size
		}
	}
	void SetValue(int score) { 
		if (score > 99999)// giới hạn điểm
			score = 99999;
		Destroy(); // hủy các đối tượng số trước đó
		string s_score = to_string(score); // chuyển sang chuỗi
		for (int i = 0; i < s_score.size(); i++) {
			Texture2D* t = new Texture2D(mRenderer, SettingProject::getPath(TYPE_IMG::NUMBER, s_score[i] - 48)); // tạo đối tượng với kí tự tương ứng
			t->transform.position = position + Vector2D(25, 0) * i * scale.x; // cập nhật tọa độ
			imgs.push_back(t);
		}
	}
	void Destroy() {
		for (int i = 0; i < imgs.size(); i++)
			delete imgs[i];
		imgs.clear();
	}
};

