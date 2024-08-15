# Tên Dự Án

- Game Tetris trên nền tảng C++ sử dụng thư viện SDL2

## Cài đặt và Chạy Dự Án

### Yêu cầu hệ thống

- Hệ điều hành: Windows 10 trở lên
- Phần mềm cần cài đặt: Visual Studio 2022

### Hướng dẫn cài đặt

1. Clone repo về máy tính của bạn: git clone https://github.com/havan1279/GameProject_LTNC.git
2. Mở project bằng Visual Studio 2022
3. Mở file `main.cpp`
4. Thực hiện chạy project

## Cấu Trúc Thư Mục

- `Sounds/`: Chứa các tệp âm thanh dùng trong game, định dạng `.wav`.
- `Images/`: Chứa các tệp hình ảnh dùng trong game, định dạng `.png`, `.jpg`.
- `Resources/`: Chứa tệp lưu trữ điểm số của người chơi.
- `SettingProject.h`: Định nghĩa các cấu trúc và thiết lập game.
  - Vector2D: Định nghĩa tọa độ điểm ảnh: x, y
  - Transform: Định nghĩa Transform với thông tin về vị trí, góc xoay, kích thước, độ phóng đại.
  - Mathf: Định nghĩa các phép tính toán toán học.
  - Các enum: `TYPE_IMG`, `TYPE_ICON`, `ID_AUDIO`, `BLOCK_TYPE`.
  - Namespace `SettingProject`: Quản lý các đường dẫn, điểm, trạng thái game.
- `Textures2D.h`:
  - Thuộc tính:
    - màn hình hiển thị
    - thông tin đối tượng
    - biến lưu hình ảnh
    - biến lưu trạng thái
  - Phương thức:
    - Khởi tạo, hủy: constructer, destructer
    - Load hình ảnh
    - Thay đổi độ phóng đại
    - Cập nhật từng frame
    - Hiển thị
- `Mouse.h`: Kế thừa từ `Textures2D.h`
  - Phương thức update cho phép lấy vị trí hiện tại của chuột và show lên màn hình đối tượng chuột tương ứng
- `Button`: Kế thừa từ `Textures2D.h`
  - Thuộc tính bổ sung:
    - Loại btn
    - Trạng thái nổi bật
    - Trạng thái nhấp/nhả
    - Trạng thái chọn
  - Phương thức:
    - Khởi tạo
    - Cập nhật: lấy tọa độ chuột hiện tại
      - Nếu trong vùng btn => thực hiện Hiệu ứng phóng, ngược lại khi thoát khỏi thực hiện scale lại size ban đầu
      - Nếu click: đặt trạng thái choose = true
- `Score.h`:

  - Thuộc tính:
    - Màn hình hiển thị
    - Đối tượng text
    - Danh sách đối tượng hiển thị số
    - Tọa độ
    - Kích thước
  - Phương thức
    - Khởi tạo
    - Cập nhật: tùy vào trường hợp hiển thị đối tượng text đồng thời hiển thị danh sách các đối tượng hiển thị số
    - Thay đổi độ phóng đại: cập nhật toàn bộ độ phóng đại của các kí tự số
    - Cập nhật giá trị hiển thị:
      - Giới hạn giá trị
      - Hủy các đối tượng cũ
      - Chuyển đổi sang chuỗi và tạo các đối tượng mới tương ứng
    - Hủy đối tượng cũ

- `Block.h` kế thừa từ `Textures2D.h`
  - Thuộc tính thêm:
    - Loại màu
    - Level khối: phát triển thêm (khối bị đóng băng phải phá băng lần lượt, ...)
    - Vị trí khối trên ma trận
    - Kích thước khối
    - Vị trí bắt đầu tính
  - Phương thức:
    - Khởi tạo
    - Cập nhật:
      - Nếu kết thúc game thì hiển thị
      - Nếu không hoạt động => kết thúc
      - Nếu độ phóng đại >= 1 và tọa độ y (dòng) trên ma trận < 0 => kết thúc
      - Tính toán vị trí = tọa độ tính + tọa độ trên ma trận _ độ phóng đại _ kích thước thật khối
        Với kích thước thật = kích thước khối - kích thước viền thừa
      - Cập nhật đối tượng
- `Blocks`:
  - Thuộc tính
    - Ma trận tọa độ ứng với các khối
    - Loại khối
    - Danh sách các khối nhỏ
    - Thời gian rơi (thực tế, mặc định)
    - Kiểm tra sự kiện
    - Kiểm tra trạng thái
  - Phương thức
    - Khởi tạo
    - Hủy
    - Tạo khối
    - Kiểm tra va chạm
    - Xoay
    - Cập nhật
    - Di chuyển
    - Chuyển sang khối khác
- `BlockManager`:
  - Thuộc tính:
    - Đối tượng tên
    - Đối tượng khối hiển thị
    - Màn hình hiển thị
    - Vị trí
  - Phương thức:
    - Khởi tạo
    - Cập nhật
    - Tạo khối
- Các hàm khác:

  - bool InitSDL(): Khởi tạo SDL
  - void CloseSDL(): Đóng SDL
  - SDL_Texture\* LoadTextureFromFile(string path): Load đối tượng
  - void InitSoundEffect(): Khởi tạo âm thanh
  - void DisAudio(): Hủy âm thanh
  - void PlayAudio(ID_AUDIO type): Phát âm thanh

  - void DeleteRow(int index): Xóa hàng
  - void CheckMatrix(): Kiểm tra ma trận
  - bool CheckGameOver(): Kiểm tra trạng thái ma trận
  - void Menu(): màn hình menu
  - void SettingMenu(): màn hình setting
  - void PlayGame(): màn hình play
  - void ClearGame(): Làm mới dữ liệu màn chơi
  - vector<Texture2D\*> GetListType(int type, int n = 7): Trả về danh sách các ô màu
  - vector<Score\*> ReadScore(string filename, int score): Trả về danh sách các điểm

## Hướng Dẫn Sử Dụng

1. Người chơi có thể di chuyển nhân vật bằng các phím mũi tên hoặc WASD.
2. Nhấn phím Space để nhảy.
3. Ăn các viên kẹo để tích lũy điểm số.
4. Tránh các chướng ngại vật để không bị mất mạng.
5. Hoàn thành các cấp độ để đi đến cấp độ tiếp theo.

![Game Screenshot](Images/screenshot.png)

## Công Nghệ Sử Dụng

- Visual Studio 2022
- C++
- Git
