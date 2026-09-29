//  RF2_MaxHeap_giaithich.cpp
//  Ưu tiên xử lý khi tranh chấp xe cuối cùng (RF2)
//  Cài đặt Binary Max-Heap

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
using namespace std;

struct RentalRequest {
    string bookingId;            // mã đơn, VD "RENT_HCM_00904" -> dùng làm KHÓA cho Hash Table
    string customerId;
    string carId;
    int membershipTier;          // Gói khách hàng: 3 = gói VIP, 2 = gói Gold, 1 = gói Standard
    long long bookingTimestamp;  // thời điểm bấm đặt xe 
};

// BƯỚC 1: 2 vùng dữ liệu chính
// BookingId -> là vị trí hiện tại trong Heap
// Cần vì Heap CHỈ biết ai đang ở gốc (max), KHÔNG biết 1 bookingId
// cụ thể đang nằm ở dòng nào. Không có nó thì huỷ 1 đơn theo mã
vector<RentalRequest> heap;
unordered_map<string, int> indexMap;

//Function
//1. Hàm này sẽ xét xem là khách nào có gói cao hơn thì ưu tiên trước, còn nếu mà 2 khách có cùng gói với nhau thì sẽ xét qua thời gian mà khách book
bool HigherPriority(const RentalRequest& a, const RentalRequest& b);
//2.
void SwapAndSync(int i, int j);
void SwapAndSync(int i, int j);
void SiftUp(int i);
void SiftDown(int i);
void InsertRequest(const RentalRequest& req);
void RemoveAtIndex(int idx);
RentalRequest ExtractMax();
bool RemoveById(const string& bookingId);
void BuildHeap(const vector<RentalRequest>& initial);

int main() {
    vector<RentalRequest> batch = {
        {"RENT_HCM_00901", "CUS_010", "51A-12345", 1, 1690000005}, {"RENT_HCM_00902", "CUS_022", "51A-12345", 2, 1690000002}, {"RENT_HCM_00903", "CUS_031", "51A-12345", 3, 1690000004},{"RENT_HCM_00904", "CUS_047", "51A-12345", 3, 1690000001},{"RENT_HCM_00905", "CUS_058", "51A-12345", 2, 1690000003}};
    cout << "===== BƯỚC A: buildHeap() nạp hàng loạt =====\n";
    BuildHeap(batch);
    printHeap();

    cout << "===== BƯỚC B: removeById() - khách RENT_HCM_00902 rút yêu cầu =====" << endl;
    cout << "Kết quả: " << (RemoveById("RENT_HCM_00902") ? "THÀNH CÔNG" : "THẤT BẠI") << endl;
    printHeap();

    cout << "===== BƯỚC C: extractMax() liên tục để xử lý từng tranh chấp =====" << endl;
    int order = 1;
    while (!heap.empty()) {
        RentalRequest winner = ExtractMax();
        cout << "  Bước " << order++ << ": XỬ LÝ " << winner.bookingId << " (khách " << winner.customerId << ", tier=" << winner.membershipTier << ", ts=" << winner.bookingTimestamp << ")" << endl;
    }
    return 0;
}
// BƯỚC 2: Hàm so sánh "ai ưu tiên hơn ai" - nơi chứa quy luật của RF2.

bool HigherPriority(const RentalRequest& a, const RentalRequest& b) {
    if (a.membershipTier != b.membershipTier)
        return a.membershipTier > b.membershipTier;   // hạng cao hơn thắng
    return a.bookingTimestamp < b.bookingTimestamp;     // bằng hạng -> đặt sớm hơn thắng
}

// BƯỚC 3: Đổi chỗ 2 phần tử + đồng bộ lại Hash Table.
// Mỗi lần đổi chỗ trong heap PHẢI qua hàm này - quên đồng bộ sẽ khiến Hash Table gắn sai vị trí, gây lỗi ngầm rất khó debug.

void SwapAndSync(int i, int j) {
    RentalRequest tmp = heap[i];
    heap[i] = heap[j];
    heap[j] = tmp;
    indexMap[heap[i].bookingId] = i;
    indexMap[heap[j].bookingId] = j;
}

// BƯỚC 4: SiftUp - đẩy 1 phần tử LÊN TRÊN. Dùng sau khi vừa thêm
// 1 phần tử MỚI vào CUỐI mảng (insert). O(log M) vì chỉ đi theo
// 1 đường từ lá lên gốc.

void SiftUp(int i) {
    while (i > 0) {
        int parent = (i - 1) / 2;
        if (!HigherPriority(heap[i], heap[parent])) break;  // đã đúng chỗ
        SwapAndSync(i, parent);
        i = parent;
    }
}

// BƯỚC 5: siftDown - đẩy 1 phần tử XUỐNG DƯỚI. Dùng khi 1 phần tử
// vừa được đặt vào 1 vị trí có thể "yếu hơn" 1 trong 2 con.

void SiftDown(int i) {
    int n = (int)heap.size();
    while (true) {
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        int best = i;
        if (left < n && HigherPriority(heap[left], heap[best])) best = left;
        if (right < n && HigherPriority(heap[right], heap[best])) best = right;
        if (best == i) break;    // cả 2 con đều yếu hơn -> đúng chỗ, dừng lại
        SwapAndSync(i, best);
        i = best;
    }
}

// BƯỚC 6: insert - thêm 1 yêu cầu thuê MỚI

void InsertRequest(const RentalRequest& req) {
    heap.push_back(req);                       // đặt tạm vào CUỐI mảng
    int newIndex = (int)heap.size() - 1;
    indexMap[req.bookingId] = newIndex;
    SiftUp(newIndex);                           // cho nó "leo" lên đúng chỗ
}

// BƯỚC 7: removeAtIndex - XOÁ 1 phần tử tại vị trí idx bất kỳ.
// Hàm lõi dùng chung cho cả extractMax() (idx=0) và removeById()
// (idx bất kỳ) - viết 1 lần, dùng ở 2 nơi.

void RemoveAtIndex(int idx) {
    int lastIdx = (int)heap.size() - 1;
    indexMap.erase(heap[idx].bookingId);

    if (idx != lastIdx) {
        heap[idx] = heap.back();                 // đưa phần tử CUỐI lên lấp vào chỗ trống
        indexMap[heap[idx].bookingId] = idx;
    }
    heap.pop_back();

    // Phần tử vừa lên có thể mạnh hơn cha (cần leo lên) HOẶC yếu
    // hơn con (cần chìm xuống) - thử cả 2, chiều sai tự dừng ngay.
    if (idx < (int)heap.size()) {
        SiftUp(idx);
        SiftDown(idx);
    }
}

// BƯỚC 8: extractMax - lấy ra + xoá yêu cầu ƯU TIÊN CAO NHẤT.

RentalRequest ExtractMax() {
    RentalRequest top = heap[0];   // gốc luôn là max -> lưu lại TRƯỚC khi xoá
    RemoveAtIndex(0);
    return top;
}

// BƯỚC 9: removeById - HUỶ 1 đơn CỤ THỂ theo bookingId.

bool RemoveById(const string& bookingId) {
    auto it = indexMap.find(bookingId);
    if (it == indexMap.end()) return false;
    RemoveAtIndex(it->second);
    return true;
}

// BƯỚC 10: buildHeap - nạp hàng loạt nhiều yêu cầu cùng lúc, O(M)
// (heapify kiểu Floyd), thay vì insert từng cái O(M log M).
// Chỉ cần siftDown() cho các nút CHA (từ giữa mảng lùi về gốc) - nút LÁ không có con nên tự nó đã hợp lệ, không cần đụng tới.

void BuildHeap(const vector<RentalRequest>& initial) {
    heap = initial;
    indexMap.clear();
    for (int i = 0; i < (int)heap.size(); ++i)
        indexMap[heap[i].bookingId] = i;

    for (int i = (int)heap.size() / 2 - 1; i >= 0; --i)
        SiftDown(i);
}

void printHeap() {
    for (int i = 0; i < (int)heap.size(); ++i) {
        printf("  [%d] %s | tier=%d | ts=%lld | car=%s", i, heap[i].bookingId.c_str(), heap[i].membershipTier, heap[i].bookingTimestamp, heap[i].carId.c_str());
    }
}
