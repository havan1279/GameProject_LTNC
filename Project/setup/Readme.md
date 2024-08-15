# Tên Dự Án

Mô tả ngắn gọn về dự án game này, bao gồm mục đích, tính năng chính, nền tảng phát triển, v.v.

## Cài đặt và Chạy Dự Án

### Yêu cầu hệ thống

- Hệ điều hành: Windows 10 trở lên
- Phần mềm cần cài đặt: Unity 2021.3.x, Visual Studio Code
- Các thư viện/SDK cần cài đặt: ...

### Hướng dẫn cài đặt

1. Clone repo về máy tính của bạn: git clone https://github.com/username/ten-du-an.git
2. Mở project bằng Unity và chờ đến khi các package được tải về.
3. Mở file `main.scene` trong thư mục `Scenes`.
4. Ấn nút `Play` trong Unity Editor để chạy game.

## Cấu Trúc Thư Mục

- `Sounds/`: Chứa các tệp âm thanh dùng trong game, định dạng `.wav`.
- `Images/`: Chứa các tệp hình ảnh dùng trong game, định dạng `.png`, `.jpg`.
- `Resources/`: Chứa tệp lưu trữ điểm số của người chơi.
- `Scripts/`:
- `SettingProject.cs`: Định nghĩa các cấu trúc và thiết lập game.
- `Vector2D.cs`: Định nghĩa class Vector2D với các phép toán.
- `Transform.cs`: Định nghĩa class Transform với thông tin về vị trí, góc xoay, kích thước.
- `Mathf.cs`: Định nghĩa các phép tính toán toán học.
- Các enum: `TYPE_IMG`, `TYPE_ICON`, `ID_AUDIO`, `BLOCK_TYPE`.
- Namespace `SettingProject`: Quản lý các đường dẫn, điểm, trạng thái game.

## Hướng Dẫn Sử Dụng

1. Người chơi có thể di chuyển nhân vật bằng các phím mũi tên hoặc WASD.
2. Nhấn phím Space để nhảy.
3. Ăn các viên kẹo để tích lũy điểm số.
4. Tránh các chướng ngại vật để không bị mất mạng.
5. Hoàn thành các cấp độ để đi đến cấp độ tiếp theo.

![Game Screenshot](Images/screenshot.png)

## Công Nghệ Sử Dụng

- Unity 2021.3.x
- C#
- Git

## Thông Tin Bản Quyền

Dự án này được phát hành theo giấy phép MIT. Xem thêm tại [LICENSE](LICENSE).
