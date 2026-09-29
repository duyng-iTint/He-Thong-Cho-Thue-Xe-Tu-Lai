# Phần Thành viên 1 (C++) — MC1 + MyHashTable + Persistence

Ngôn ngữ: C++17 (chuẩn thư viện, không dùng thư viện ngoài).
Đã kiểm tra: biên dịch sạch với `-Wall -Wextra`, chạy qua
AddressSanitizer + LeakSanitizer không phát hiện leak/lỗi bộ nhớ.

## Các file

| File | Vai trò |
|---|---|
| `Booking.h` | struct đơn thuê xe (không tính là cấu trúc DSA tự cài) |
| `MyHashTable.h` | **Cấu trúc tự cài đặt #1**: template MyHashTable, Separate Chaining, tự viết hàm băm, tự resize |
| `Persistence.h/.cpp` | Tầng Persistence — chỉ load/save CSV, không chứa logic tra cứu |
| `GenerateData.h/.cpp` | Sinh dữ liệu giả lập (1.000 / 10.000 / 100.000 dòng) |
| `demo_mc1.cpp` | Tầng Presentation — CLI demo cho MC1 |
| `benchmark_mc1.cpp` | Bằng chứng hiệu năng Mục 5.3: HashTable vs Linear Scan |
| `test_thanhvien1.cpp` | Bộ test tự động (tự viết macro CHECK, không dùng framework ngoài) — 11 test case |
| `Makefile` | build tất cả |

## Cách build & chạy

```bash
# Build tất cả (demo_mc1, benchmark_mc1, test_thanhvien1)
make all

# Chạy test tự động
./test_thanhvien1

# Chạy demo tra cứu MC1 (tự sinh dữ liệu nếu chưa có)
./demo_mc1

# Chạy benchmark hiệu năng
./benchmark_mc1

# Dọn file build/dữ liệu tạm
make clean
```

## Kết quả benchmark mẫu (đã build và chạy thử với -O2)

| N bản ghi | HashTable (ms/lượt) | Linear Scan (ms/lượt) | Speedup |
|---|---|---|---|
| 1.000 | ~0.00008 | ~0.0017 | ~21x |
| 10.000 | ~0.00009 | ~0.019 | ~214x |
| 100.000 | ~0.00012 | ~0.33 | ~2635x |

→ HashTable gần như không đổi khi N tăng (O(1) trung bình), Linear Scan
tăng gần tuyến tính (O(N)) — đúng bằng chứng cần cho D5/D3.

*Lưu ý kỹ thuật:* benchmark dùng biến `volatile checksum` để ngăn trình
biên dịch (`-O2`) loại bỏ hoàn toàn vòng lặp tra cứu (dead code
elimination) khi kết quả không được dùng ở đâu khác — nếu không sẽ đo
ra 0.00000ms giả.

## Thiết kế bộ nhớ (quan trọng khi bảo vệ cá nhân)

- `PersistenceContext::storage` là nơi **sở hữu thật sự** các object
  `Booking` (bằng `std::vector<std::unique_ptr<Booking>>`).
- Hai bảng băm (`tableById`, `tableByPlate`) chỉ lưu `Booking*` (con trỏ
  thô, không sở hữu) trỏ vào `storage` — tránh double-free và tránh
  con trỏ treo khi `storage` không bao giờ bị di chuyển/resize.
- `MyHashTable` tự quản lý bộ nhớ Node của chính nó (new/delete thủ
  công trong insert/remove/resize/destructor), không dùng smart pointer
  bên trong để thấy rõ cơ chế cấp phát/giải phóng khi bảo vệ.

## Điểm cần nhớ khi bảo vệ cá nhân (Mục 14)

- Giải thích được hàm băm polynomial rolling hash tự viết trong `hashKey()`.
- Giải thích Separate Chaining xử lý va chạm ra sao (xem `test_collision_handling`).
- Giải thích vì sao cần `resize()` và khi nào nó được gọi (load factor > 0.75).
- Giải thích vì sao dùng con trỏ thô `Booking*` trong bảng băm thay vì
  copy toàn bộ `Booking` (tiết kiệm bộ nhớ, tránh 2 bảng lưu 2 bản sao
  khác nhau của cùng 1 đơn thuê).
- Bảo vệ đánh đổi: HashTable nhanh cho tra cứu theo khóa nhưng không hỗ trợ
  duyệt theo thứ tự/khoảng thời gian — đó là lý do MC2 (Thành viên 2) cần
  Sorted Array riêng.
