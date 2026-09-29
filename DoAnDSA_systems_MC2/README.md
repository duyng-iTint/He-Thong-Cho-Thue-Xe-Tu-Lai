# MODULE MC2 — Tra cứu Xe theo Khoảng Thời gian Thuê & Top Xe Được Thuê Nhiều Nhất

**Đồ án:** Hệ thống Quản lý Cho thuê Xe Tự lái & Xử lý Tranh chấp Đặt xe
**Phần việc:** Thành viên 2 — *MC2 (Range Query + Extremes)*
**Ngôn ngữ:** C++17 · **Hệ điều hành chạy chương trình:** Windows

> Tài liệu này mô tả: (1) module MC2 do Thành viên 2 phụ trách, (2) cách nó **điều
> phối / kết nối** với 4 phần việc còn lại trong hệ thống chung, (3) cấu trúc
> source code, và (4) hướng dẫn cài đặt — biên dịch — chạy trên Windows.

---

## 1. Vai trò của module này trong toàn hệ thống

Theo bản phân công của nhóm, hệ thống được chia thành 5 phần việc độc lập nhưng
dùng chung một cấu trúc dữ liệu gốc `RentalRecord` (1 dòng = 1 đơn thuê xe) và
chung một file dữ liệu `donthue_xe.csv`:

| Thành viên | Yêu cầu phụ trách | Cấu trúc dữ liệu tự cài đặt |
|---|---|---|
| 1 | MC1 — Tra cứu đơn thuê / xe theo mã | `MyHashTable` (Separate Chaining) + module Persistence (đọc/ghi CSV) |
| **2 (module này)** | **MC2 — Lọc xe theo khoảng thời gian thuê / Top xe thuê nhiều nhất** | **Merge Sort + Binary Search trên Sorted Array** |
| 3 | RF2 — Ưu tiên xử lý tranh chấp xe cuối cùng | `MyMaxHeap` (Binary Max-Heap) |
| 4 | RF1 — Gợi ý nhanh tên hãng/dòng xe (Autocomplete) | `Trie` |
| 5 | RF3 — Hoàn tác thao tác tạo/hủy đơn thuê + Test & Benchmark tổng | `Stack` |

**Điểm kết nối (điều phối) với các phần khác:**

- **Nhận dữ liệu từ Thành viên 1:** module MC2 không tự ghi file CSV — nó **nhận
  lại** `vector<RentalRecord>` do tầng Persistence (Thành viên 1) nạp từ
  `donthue_xe.csv`, rồi mới sắp xếp/tra cứu. Trong repo demo này, để chạy **độc
  lập**, MC2 có kèm sẵn 1 bộ đọc CSV tối giản (`src/CSVUtils.cpp`) — khi ráp vào
  hệ thống chung, chỉ cần thay lời gọi `loadRentalCSV(...)` bằng hàm Persistence
  thật của Thành viên 1 (cùng chữ ký trả về `vector<RentalRecord>`).
- **Cung cấp dữ liệu đã sắp xếp cho Thành viên 3 (RF2):** khi có đơn thuê mới,
  hệ thống cần cập nhật **đồng thời** cấu trúc Sorted Array (MC2, module này)
  và Binary Max-Heap (RF2, Thành viên 3) — đây chính là hướng giải quyết
  "Composition" cho yêu cầu xung đột MC2 ↔ RF2 đã nêu trong tài liệu phân tích
  của nhóm (mục III).
- **Không phụ thuộc Thành viên 4 (Trie/Autocomplete)** và **Thành viên 5
  (Stack/Undo)** — hai phần này hoạt động độc lập trên tầng Presentation, chỉ
  cần cùng đọc/ghi trên `RentalRecord`.

---

## 2. Yêu cầu nghiệp vụ MC2 (trích tài liệu đồ án)

> **MC2 — Truy vấn xe theo Khoảng thời gian thuê / Top xe được thuê nhiều nhất
> (Range + Extremes):** Nhân viên cần lọc và xem toàn bộ lượt thuê xe trong một
> khoảng thời gian cụ thể (ví dụ: "từ ngày 1 đến ngày 15 tháng 8" để biết xe nào
> đang bận), hoặc lọc ra danh sách các xe được thuê nhiều nhất, sắp xếp giảm
> dần, để lên kế hoạch bảo dưỡng và bổ sung đội xe.

**Lựa chọn cấu trúc dữ liệu (theo quy trình Q1–Q4, chương 16):**

| Tiêu chí | Trả lời |
|---|---|
| Dominant Operation | Lọc theo khoảng thời gian thuê / Top xe được thuê nhiều nhất |
| Q1: Cần thứ tự? | **Có** |
| Q2: Loại Key / Workload | Bulk load dữ liệu lịch sử thuê xe, khóa theo ngày / số lượt thuê |
| Cấu trúc chọn | **Sorted Array + Binary Search** (xây dựng bằng **Merge Sort**) |
| Độ phức tạp | Tìm đầu khoảng: **O(log N)**; lấy k kết quả: **O(log N + k)** |
| Đánh đổi (trade-off) | Tối ưu khi nạp dữ liệu lớn ban đầu bằng Merge Sort; chèn/xóa giữa mảng tốn O(N) (chấp nhận được vì đơn thuê chủ yếu được thêm ở cuối và dữ liệu được nạp lại theo lô) |

**Ràng buộc bắt buộc của đồ án:** không dùng SQL, không dùng `std::sort`,
`std::lower_bound/upper_bound`, `std::map`… để lọc/sắp xếp dữ liệu — toàn bộ
Merge Sort và Binary Search phải **tự cài đặt bằng tay** (xem mục 4).

---

## 3. Cấu trúc thư mục source code

```
MC2_TopRented_RangeQuery/
├── README.md                  <- Tài liệu này
├── build.bat                  <- Script build 1-click trên Windows (MinGW g++)
│
├── src/                       <- TOÀN BỘ SOURCE CODE CHÍNH
│   ├── RentalRecord.h         <- struct dữ liệu 1 đơn thuê xe (dùng chung cả nhóm)
│   ├── MergeSort.h            <- Merge Sort tự cài đặt (template, generic comparator)
│   ├── BinarySearch.h         <- Binary Search tự cài đặt (lower/upper bound theo ngày)
│   ├── CSVUtils.h / .cpp      <- Đọc file donthue_xe.csv → vector<RentalRecord>
│   ├── RentalService.h / .cpp <- LÕI NGHIỆP VỤ MC2: Range Query + Top xe hot + Benchmark
│   └── main.cpp                <- Chương trình Console (CLI) — menu tương tác
│
├── data/                      <- DỮ LIỆU (đóng vai trò "hóa đơn" đơn thuê xe)
│   ├── donthue_xe.csv          <- Bộ dữ liệu demo nhanh (60 dòng)
│   ├── donthue_xe_10000.csv    <- Bộ dữ liệu lớn (10.000 dòng) để benchmark
│   └── generate_data.py        <- Script Python tự sinh dữ liệu giả lập (1k/10k/100k dòng)
│
└── demo/                      <- KỊCH BẢN DEMO & KIỂM THỬ
    ├── run_demo.bat            <- Chạy nhanh chương trình với bộ dữ liệu demo
    └── test_correctness.cpp    <- Bộ test tự kiểm chứng Merge Sort / Binary Search / Range Query
```

### Mô tả chi tiết từng file mã nguồn

| File | Vai trò | Ghi chú kỹ thuật |
|---|---|---|
| `RentalRecord.h` | Định nghĩa 1 "hóa đơn thuê xe": `bookingId, carPlate, carBrand, carModel, rentDate, returnDate, price, customerName, memberRank` | Ngày dùng định dạng `YYYY-MM-DD` để so sánh chuỗi = so sánh thời gian (không cần parse ngày tháng phức tạp) |
| `MergeSort.h` | `mergeSort(vector<T>&, Compare)` — chia để trị, đệ quy, ổn định (stable) | O(N log N) mọi trường hợp; dùng chung cho sắp theo ngày, theo biển số, theo số lượt thuê |
| `BinarySearch.h` | `lowerBoundByDate()`, `upperBoundByDate()`, `linearScanFirstDateGE()` | Tự cài đặt bằng tay, không gọi `<algorithm>`; hàm linear scan chỉ dùng để **benchmark đối chiếu** |
| `CSVUtils.h/.cpp` | Đọc `donthue_xe.csv` thành `vector<RentalRecord>` | Bỏ qua dòng tiêu đề, tự xử lý CRLF (`\r\n`) khi file sinh trên Windows |
| `RentalService.h/.cpp` | 4 chức năng chính (xem mục 5) | Đây là file **quan trọng nhất**, thể hiện toàn bộ thuật toán MC2 |
| `main.cpp` | Menu Console: nạp dữ liệu → tra cứu khoảng ngày → Top xe hot → benchmark | Có bật `SetConsoleOutputCP(CP_UTF8)` để hiển thị đúng tiếng Việt trên Windows Console |

---

## 4. Cách tra cứu / thống kê dữ liệu "hóa đơn" (donthue_xe.csv)

File `data/donthue_xe.csv` đóng vai trò tập hợp **hóa đơn/đơn thuê xe**. Mỗi
dòng có 9 trường, phân tách bằng dấu phẩy:

| Cột | Tên trường | Kiểu dữ liệu | Ví dụ | Ý nghĩa |
|---|---|---|---|---|
| 1 | `BookingID` | string | `RENT_HCM_00842` | Mã định danh đơn thuê (dùng cho MC1 — Thành viên 1) |
| 2 | `CarPlate` | string | `51F-123.45` | Biển số xe — **khóa gom nhóm** để tính Top xe hot |
| 3 | `CarBrand` | string | `Toyota` | Hãng xe |
| 4 | `CarModel` | string | `Vios` | Dòng xe |
| 5 | `RentDate` | date `YYYY-MM-DD` | `2024-06-01` | **Khóa sắp xếp chính của MC2** — ngày bắt đầu thuê |
| 6 | `ReturnDate` | date `YYYY-MM-DD` | `2024-06-05` | Ngày trả xe |
| 7 | `Price` | double | `3582369` | Giá trị đơn thuê (VNĐ) |
| 8 | `CustomerName` | string | `Nguyen Van An` | Tên khách hàng |
| 9 | `MemberRank` | int (0–3) | `1` | Hạng thành viên khách hàng (dùng cho RF2 — Thành viên 3) |

**Cách tra cứu nhanh (không mở file bằng mắt):**

1. **Tra cứu theo khoảng ngày thuê** → chọn menu `2`, nhập `RentDate` bắt đầu
   và kết thúc → chương trình dùng Binary Search tìm biên trong `O(log N)` rồi
   liệt kê toàn bộ đơn nằm trong khoảng đó.
2. **Xem xe nào được thuê nhiều nhất (để lên kế hoạch bảo dưỡng)** → chọn menu
   `3`, nhập số lượng Top-N muốn xem → chương trình gom nhóm theo `CarPlate`,
   đếm số lượt thuê, sắp xếp giảm dần bằng Merge Sort.
3. **Đo hiệu năng tra cứu** → chọn menu `4` để so sánh Binary Search với Linear
   Scan ngay trên bộ dữ liệu đang có.
4. Muốn có dữ liệu quy mô lớn hơn (giống thực tế "hàng chục nghìn giao dịch
   tích lũy") → chạy `generate_data.py` (xem mục 6) để sinh 10.000 hoặc 100.000
   dòng, rồi nạp lại bằng menu `1`.

---

## 5. Bốn chức năng chính (`RentalService`)

```cpp
class RentalService {
    static void sortByRentDate(vector<RentalRecord>&);
    static vector<RentalRecord> queryByDateRange(sortedByDate, fromDate, toDate);
    static vector<CarStat> buildCarStats(records);
    static vector<CarStat> topRentedCars(stats, topK);
    static void benchmarkRangeQuery(sortedByDate, fromDate, toDate);
};
```

1. **`sortByRentDate`** — Merge Sort toàn bộ đơn thuê theo `rentDate` tăng dần.
   Đây là bước "bulk load" bắt buộc trước khi Binary Search hoạt động đúng.
2. **`queryByDateRange`** — dùng `lowerBoundByDate` + `upperBoundByDate` để tìm
   vị trí đầu/cuối của khoảng `[from, to]` trong `O(log N)`, sau đó lấy `k` kết
   quả liên tiếp trong mảng → tổng **O(log N + k)**.
3. **`buildCarStats`** — Merge Sort bản sao dữ liệu theo `carPlate`
   (`O(N log N)`), rồi quét tuyến tính 1 lần (`O(N)`) để đếm số lượt thuê liên
   tiếp của từng xe. (Không dùng hash table vì đó là phạm vi MC1 của Thành
   viên 1 — MC2 chỉ dùng Sorted Array + Merge Sort theo đúng bảng lựa chọn
   DSA của nhóm.)
4. **`topRentedCars`** — Merge Sort giảm dần theo số lượt thuê, lấy Top-K đầu.
5. **`benchmarkRangeQuery`** — lặp lại 20.000 lần phép tìm kiếm để đo thời
   gian trung bình, so sánh Binary Search với Linear Scan trên cùng một tập dữ
   liệu (bằng chứng hiệu năng, mục 7).

---

## 6. Hướng dẫn cài đặt & chạy trên Windows

### 6.1. Cài đặt trình biên dịch C++ (một lần duy nhất)

Chọn **một trong hai** cách:

**Cách A — MinGW-w64 (khuyến nghị, nhẹ, dùng `build.bat` có sẵn):**
1. Tải MSYS2 tại https://www.msys2.org/ và cài đặt.
2. Mở "MSYS2 MinGW64" từ Start Menu, chạy: `pacman -S mingw-w64-x86_64-gcc`
3. Thêm `C:\msys64\mingw64\bin` vào biến môi trường `PATH` của Windows
   (Settings → System → About → Advanced system settings → Environment
   Variables → Path → New).
4. Mở Command Prompt mới, gõ `g++ --version` để kiểm tra đã nhận lệnh.

**Cách B — Visual Studio (nếu nhóm đã cài sẵn):**
1. Cài "Visual Studio Community" kèm gói **Desktop development with C++**.
2. Mở "Developer Command Prompt for VS", `cd` vào thư mục dự án, build bằng:
   `cl /std:c++17 /EHsc /Fe:mc2_app.exe src\main.cpp src\CSVUtils.cpp src\RentalService.cpp`
   — hoặc tạo Project mới trong Visual Studio và add toàn bộ file trong `src/`.

### 6.2. Biên dịch chương trình

Mở **Command Prompt**, `cd` vào thư mục `MC2_TopRented_RangeQuery`, chạy:

```bat
build.bat
```

Script sẽ biên dịch toàn bộ `src/*.cpp` bằng `g++ -std=c++17` và tạo ra file
`mc2_app.exe` ở thư mục gốc dự án. Nếu báo lỗi, kiểm tra lại bước 6.1.

### 6.3. Chạy chương trình

```bat
mc2_app.exe
```

hoặc chạy nhanh với dữ liệu demo có sẵn:

```bat
demo\run_demo.bat
```

Chương trình hiện menu số (`1`–`5`, `0` để thoát) — nhập số và Enter để chọn
chức năng, làm theo hướng dẫn nhập ngày/số lượng ngay trên màn hình.

### 6.4. Sinh dữ liệu lớn hơn để test/benchmark (tùy chọn)

Cần cài Python 3 (https://www.python.org/downloads/), sau đó:

```bat
cd data
python generate_data.py 100000 donthue_xe_100000.csv
cd ..
mc2_app.exe
```

Khi chương trình hỏi đường dẫn CSV ở menu `1`, nhập:
`data/donthue_xe_100000.csv`

### 6.5. Kiểm thử (unit test tự viết)

```bat
g++ -std=c++17 -O2 -o demo\test_mc2.exe demo\test_correctness.cpp src\RentalService.cpp src\CSVUtils.cpp
demo\test_mc2.exe
```

Kết quả mong đợi: `7/7 test PASS` (kiểm chứng Merge Sort, Binary Search, Range
Query đối chiếu với brute-force, và Top xe hot).

---

## 7. Bằng chứng hiệu năng (Benchmark thực đo)

Đo trên bộ dữ liệu giả lập sinh bởi `generate_data.py`, tìm vị trí bắt đầu
khoảng ngày `2024-06-01 → 2024-06-30`, lặp lại 20.000 lần để lấy trung bình:

| N (số đơn thuê) | Binary Search — TB/lần | Linear Scan — TB/lần | Binary Search nhanh hơn |
|---:|---:|---:|---:|
| 1.000 | 0,047 µs | 2,10 µs | ~45 lần |
| 10.000 | 0,070 µs | 20,82 µs | ~300 lần |
| 100.000 | 0,073 µs | 355,58 µs | ~4.871 lần |

**Nhận xét:** thời gian Binary Search gần như không đổi khi N tăng (đúng với
lý thuyết O(log N)), trong khi Linear Scan tăng tuyến tính theo N (O(N)) —
khớp với lập luận đã chọn "Sorted Array + Binary Search" cho MC2 trong tài
liệu phân tích DSA của nhóm (Chương 16, mục III).

> Số liệu trên có thể dao động nhẹ tùy cấu hình máy khi nhóm tự chạy lại; quan
> trọng là **xu hướng tăng trưởng** giữa hai thuật toán, không phải giá trị
> tuyệt đối.

---

## 8. Ghi chú khi tích hợp vào hệ thống chung của nhóm

- Thay `loadRentalCSV()` trong `main.cpp` bằng hàm Persistence thật của Thành
  viên 1 nếu đã có (miễn cùng trả về `vector<RentalRecord>`).
- Khi Thành viên 3 (RF2) thêm đơn thuê mới vào Binary Max-Heap, gọi thêm
  `RentalService::sortByRentDate()` (hoặc chèn đúng vị trí bằng Binary Search)
  để Sorted Array của MC2 luôn đồng bộ — đây là điểm "Composition" giải quyết
  xung đột MC2 ↔ RF2 đã nêu trong tài liệu phân tích của nhóm.
- Phần UI màu đỏ cho "xe đang tranh chấp" (yêu cầu RF2) và tính năng
  Autocomplete (RF1, Trie) không thuộc phạm vi module này — chỉ cần gọi các
  hàm `RentalService::*` ở trên khi cần dữ liệu đã sắp xếp/tra cứu.
