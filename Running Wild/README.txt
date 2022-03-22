*Mở file project "Running Wild.cbp" chọn Build Target thành Release và Compile lại theo từng máy
(Chú ý phải có đủ các thư viện như ở trong thư mục SDLib)
Sau đó chạy file exe trong mục bin\Release\Running Man.exe là đc.

Các chức năng chính:

Tạo class GameObject để quản lý các đối tượng trong game:
  + biến totalFrame để biết số lượng chia tách spritesheet ra thành từng frame 
  + biến diffBox để tạo vùng collide để xác định giới hạn va chạm giữa người chơi và vật cản đường
  + biến scale để nhân vào kích thước đối tượng trên màn hình 
  + biến landing và landed dành riêng cho hoạt ảnh nhảy lên của người chơi 
  + biến check để xác định xem có kiểm tra va đập giữa người chơi và vật cản hay không
      -> nếu tắt nghĩa là người chơi đang sử dụng năng lực đi xuyên vật cản
  + Các methods:
     - checkCollision : để kiểm tra va chạm với đối tượng khác
     - UpdatePosition : cập nhật vị trí của đối tượng trên màn hình và cập nhật frame mới trong spritesheet
     - Render : render đối tượng
     - SetTexture : thay đổi hình ảnh của đối tượng, thuận tiện cho animation
     - ManagePower : quản lý việc bật tắt sức mạnh đặc biệt của người chơi

Hướng dẫn chơi: (Thể loại game Endless Runner) -> gần tương tự Dinosaur trên chrome
 Phím Space, W, Up Arrow và chuột trái : Nhảy
 Phím K hoặc chuột phải : Tiến hóa nhân vật và nhận năng lực xuyên vật cản trong 8 giây (8 giây hồi chiêu)

