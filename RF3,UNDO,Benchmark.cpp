#include<iostream>
#include<string>
#include<chrono>
#include<vector>
#include<limits>
using namespace std;
using namespace chrono;

//thong tin don thue
struct renTal {
	string bookingID;
	string customerName;
	string carPlate;
	string starDate;
	string endDate;

	void print() const {
		cout << "Booking ID: " << bookingID << endl;
		cout << "Ten khach hang: " << customerName << endl;
		cout << "Bien so: " << carPlate << endl;
		cout << "Ngay bat dau: " << starDate << endl;
		cout << "Ngay ket thuc: " << endDate << endl;
	}
};


//loai thao tac
enum class ActionType {
	ADD,
	UPDATE,
	DELETE_ACTION
};

//thao tac co the UNDO
struct Action {
	ActionType type;

	//du lieu truoc thao tac
	renTal oldData;

	//du lieu sau thao tac
	renTal newData;

	//vi tri cu cua don thue
	int position = -1;
};

//node cho stack
struct StackNode {
	Action action;
	StackNode* next;

	StackNode(const Action& a) {
		action = a;
		next = nullptr;
	}
};

//stack tu cai dat
class Mystack {
private:
	StackNode* topNode;
	int size;

public:

	//constructor
	Mystack() {
		topNode = nullptr;
		size = 0;
	}

	//destructor
	~Mystack() {
		clear();
	}

	//them action vao dau stack
	void push(const Action& action) {
		StackNode* newNode = new StackNode(action);

		newNode->next = topNode;
		topNode = newNode;

		size++;
	}

	//lay action tren cung ra
	bool pop(Action& action) {

		if (topNode == nullptr) {
			return false;
		}

		StackNode* temp = topNode;

		action = temp->action;

		topNode = topNode->next;

		delete temp;

		size--;

		return true;
	}

	//kiem tra stack co rong khong
	bool empty() const {
		return topNode == nullptr;
	}

	//xem action tren cung
	bool top(Action& action) const {

		if (topNode == nullptr) {
			return false;
		}

		action = topNode->action;

		return true;
	}

	//size
	int getSize() const {
		return size;
	}

	//clear
	void clear() {

		while (topNode != nullptr) {

			StackNode* temp = topNode;

			topNode = topNode->next;

			delete temp;
		}

		size = 0;
	}
};

//quan ly lich su UNDO
class UnoManager {
private:
	Mystack undoStack;

public:

	//luu thao tac vao lich su
	void saveAction(const Action& action) {
		undoStack.push(action);
	}

	//lay thao tac gan nhat
	bool undo(Action& action) {
		return undoStack.pop(action);
	}

	//kiem tra co the UNDO khong
	bool canUndo() const {
		return !undoStack.empty();
	}

	//xem thao tac moi nhat
	bool top(Action& action) const {
		return undoStack.top(action);
	}

	//so luong thao tac dang luu
	int historySize() const {
		return undoStack.getSize();
	}

	//xoa lich su UNDO
	void clearHistory() {
		undoStack.clear();
	}
};


//tim don thue theo booking ID
int findRental(
	const vector<renTal>& rentals,
	const string& bookingID
) {

	for (
		int i = 0;
		i < static_cast<int>(rentals.size());
		i++
		) {

		if (rentals[i].bookingID == bookingID) {
			return i;
		}
	}

	return -1;
}

// cac ham kiem tra ngay
//kiem tra nam nhuan
bool isLeapYear(int year) {

	if (year % 400 == 0) {
		return true;
	}

	if (year % 100 == 0) {
		return false;
	}

	return year % 4 == 0;
}

//so ngay trong thang
int daysInMonth(
	int month,
	int year
) {

	int days[] = {
		31, 28, 31, 30, 31, 30,
		31, 31, 30, 31, 30, 31
	};

	//thang 2 nam nhuan co 29 ngay
	if (
		month == 2 &&
		isLeapYear(year)
		) {
		return 29;
	}

	return days[month - 1];
}


//chuyen ngay thanh so
//de so sanh 2 ngay
int dateToNumber(
	int day,
	int month,
	int year
) {

	return year * 10000
		+ month * 100
		+ day;
}


//kiem tra ngay co hop le khong
bool isValidDate(
	const string& date
) {

	//kiem tra do dai
	if (date.length() != 10) {
		return false;
	}


	//kiem tra dau /
	if (
		date[2] != '/' ||
		date[5] != '/'
		) {
		return false;
	}


	//kiem tra cac ky tu phai la so
	for (int i = 0; i < 10; i++) {

		if (i == 2 || i == 5) {
			continue;
		}

		if (
			date[i] < '0' ||
			date[i] > '9'
			) {
			return false;
		}
	}


	//lay ngay
	int day =
		stoi(date.substr(0, 2));


	//lay thang
	int month =
		stoi(date.substr(3, 2));


	//lay nam
	int year =
		stoi(date.substr(6, 4));


	//nam phai lon hon 0
	if (year <= 0) {
		return false;
	}


	//kiem tra thang
	if (
		month < 1 ||
		month > 12
		) {
		return false;
	}


	//lay so ngay toi da cua thang
	int maxDay =
		daysInMonth(
			month,
			year
		);


	//kiem tra ngay
	if (
		day < 1 ||
		day > maxDay
		) {
		return false;
	}


	return true;
}


//so sanh ngay bat dau va ngay ket thuc
bool isStartBeforeOrEqualEnd(
	const string& startDate,
	const string& endDate
) {

	int startDay =
		stoi(startDate.substr(0, 2));

	int startMonth =
		stoi(startDate.substr(3, 2));

	int startYear =
		stoi(startDate.substr(6, 4));


	int endDay =
		stoi(endDate.substr(0, 2));

	int endMonth =
		stoi(endDate.substr(3, 2));

	int endYear =
		stoi(endDate.substr(6, 4));


	int start =
		dateToNumber(
			startDay,
			startMonth,
			startYear
		);


	int end =
		dateToNumber(
			endDay,
			endMonth,
			endYear
		);


	return start <= end;
}

// quan ly toan bo don thue

class renTalSystem {
private:

	vector<renTal> rentals;

	UnoManager undoManager;

public:

	//them don thue
	bool addRental(
		const renTal& rental,
		bool saveHistory = true
	) {

		//kiem tra booking ID trung
		if (
			findRental(
				rentals,
				rental.bookingID
			) != -1
			) {
			return false;
		}


		//kiem tra ngay bat dau
		if (
			!isValidDate(
				rental.starDate
			)
			) {
			return false;
		}


		//kiem tra ngay ket thuc
		if (
			!isValidDate(
				rental.endDate
			)
			) {
			return false;
		}


		//kiem tra ngay ket thuc phai >= ngay bat dau
		if (
			!isStartBeforeOrEqualEnd(
				rental.starDate,
				rental.endDate
			)
			) {
			return false;
		}


		//them don thue
		rentals.push_back(rental);


		//luu vao lich su
		if (saveHistory) {

			Action action;

			action.type =
				ActionType::ADD;

			action.newData =
				rental;

			action.position =
				static_cast<int>(
					rentals.size()
					) - 1;


			undoManager.saveAction(
				action
			);
		}

		return true;
	}


	//cap nhat don thue
	bool updateRental(
		const string& bookingID,
		const renTal& newrenTal,
		bool saveHistory = true
	) {

		//tim don thue
		int index =
			findRental(
				rentals,
				bookingID
			);


		if (index == -1) {
			return false;
		}


		//kiem tra ngay bat dau
		if (
			!isValidDate(
				newrenTal.starDate
			)
			) {
			return false;
		}


		//kiem tra ngay ket thuc
		if (
			!isValidDate(
				newrenTal.endDate
			)
			) {
			return false;
		}


		//kiem tra ngay ket thuc >= ngay bat dau
		if (
			!isStartBeforeOrEqualEnd(
				newrenTal.starDate,
				newrenTal.endDate
			)
			) {
			return false;
		}


		//luu du lieu cu
		renTal oldRental =
			rentals[index];


		//tao du lieu moi
		renTal updatedRental =
			newrenTal;


		//khong cho doi booking ID
		updatedRental.bookingID =
			bookingID;


		//cap nhat du lieu
		rentals[index] =
			updatedRental;


		//luu lich su de UNDO
		if (saveHistory) {

			Action action;

			action.type =
				ActionType::UPDATE;

			action.oldData =
				oldRental;

			action.newData =
				updatedRental;

			action.position =
				index;


			undoManager.saveAction(
				action
			);
		}


		return true;
	}


	//xoa don thue
	bool deleteRental(
		const string& bookingID,
		bool saveHistory = true
	) {

		//tim don thue
		int index =
			findRental(
				rentals,
				bookingID
			);


		if (index == -1) {
			return false;
		}


		//luu du lieu cu
		renTal oldRental =
			rentals[index];


		//xoa don thue
		rentals.erase(
			rentals.begin() + index
		);


		//luu lich su
		if (saveHistory) {

			Action action;

			action.type =
				ActionType::DELETE_ACTION;

			action.oldData =
				oldRental;

			action.position =
				index;


			undoManager.saveAction(
				action
			);
		}


		return true;
	}


	//UNDO thao tac gan nhat
	bool undoRental() {

		Action action;


		//lay thao tac gan nhat
		if (
			!undoManager.undo(action)
			) {

			return false;
		}


		//neu thao tac truoc do la ADD
		//UNDO ADD = XOA
		if (
			action.type ==
			ActionType::ADD
			) {

			int index =
				findRental(
					rentals,
					action.newData.bookingID
				);


			if (index != -1) {

				rentals.erase(
					rentals.begin() + index
				);
			}
		}


		//neu thao tac truoc do la UPDATE
		//UNDO UPDATE = KHOI PHUC DU LIEU CU
		else if (
			action.type ==
			ActionType::UPDATE
			) {

			int index =
				findRental(
					rentals,
					action.oldData.bookingID
				);


			if (index != -1) {

				rentals[index] =
					action.oldData;
			}
		}


		//neu thao tac truoc do la DELETE
		//UNDO DELETE = THEM LAI DON CU
		else if (
			action.type ==
			ActionType::DELETE_ACTION
			) {

			int position =
				action.position;


			//kiem tra vi tri
			if (
				position < 0 ||
				position >
				static_cast<int>(
					rentals.size()
					)
				) {

				position =
					static_cast<int>(
						rentals.size()
						);
			}


			//them lai don thue
			rentals.insert(
				rentals.begin() + position,
				action.oldData
			);
		}


		return true;
	}


	//tim don thue
	const renTal* getRental(
		const string& bookingID
	) const {

		int index =
			findRental(
				rentals,
				bookingID
			);


		if (index == -1) {
			return nullptr;
		}


		return &rentals[index];
	}


	//hien thi tat ca don thue
	void showAll() const {

		if (rentals.empty()) {

			cout
				<< "\nDanh sach don thue dang rong!\n";

			return;
		}


		cout
			<< "\n===== DANH SACH DON THUE =====\n";


		for (
			const renTal& rental :
			rentals
			) {

			rental.print();

			cout
				<< "-----------------------------\n";
		}
	}


	//tim va hien thi don thue
	void searchRental(
		const string& bookingID
	) const {

		const renTal* rental =
			getRental(
				bookingID
			);


		if (rental == nullptr) {

			cout
				<< "\nKhong tim thay don thue!\n";

			return;
		}


		cout
			<< "\n===== KET QUA TIM KIEM =====\n";


		rental->print();
	}


	//so luong don thue
	int rentalCount() const {

		return static_cast<int>(
			rentals.size()
			);
	}


	//so luong thao tac UNDO
	int historySize() const {

		return undoManager.historySize();
	}


	//xem thao tac UNDO gan nhat
	bool getTopUndo(
		Action& action
	) const {

		return undoManager.top(action);
	}


	//xoa lich su UNDO
	void clearHistory() {

		undoManager.clearHistory();
	}
};


// ============================================================
// NHAP DON THUE
// ============================================================

renTal inputRental() {

	renTal rental;


	cout
		<< "\n===== NHAP DON THUE =====\n";


	cout
		<< "Booking ID: ";

	cin
		>> rental.bookingID;


	cin.ignore(
		numeric_limits<streamsize>::max(),
		'\n'
	);


	cout
		<< "Ten khach hang: ";

	getline(
		cin,
		rental.customerName
	);


	cout
		<< "Bien so: ";

	getline(
		cin,
		rental.carPlate
	);


	//nhap ngay bat dau
	while (true) {

		cout
			<< "Ngay bat dau (DD/MM/YYYY): ";


		getline(
			cin,
			rental.starDate
		);


		//kiem tra ngay
		if (
			isValidDate(
				rental.starDate
			)
			) {

			break;
		}


		cout
			<< "Ngay bat dau khong hop le! "
			<< "Vui long nhap lai.\n";
	}


	//nhap ngay ket thuc
	while (true) {

		cout
			<< "Ngay ket thuc (DD/MM/YYYY): ";


		getline(
			cin,
			rental.endDate
		);


		//kiem tra ngay co ton tai khong
		if (
			!isValidDate(
				rental.endDate
			)
			) {

			cout
				<< "Ngay ket thuc khong hop le! "
				<< "Vui long nhap lai.\n";

			continue;
		}


		//kiem tra ngay ket thuc >= ngay bat dau
		if (
			!isStartBeforeOrEqualEnd(
				rental.starDate,
				rental.endDate
			)
			) {

			cout
				<< "Ngay ket thuc phai >= "
				<< "ngay bat dau!\n";

			continue;
		}


		break;
	}


	return rental;
}


// ============================================================
// NHAP DU LIEU MOI KHI UPDATE
// ============================================================

renTal inputUpdatedRental(
	const string& bookingID
) {

	renTal rental;


	rental.bookingID =
		bookingID;


	cin.ignore(
		numeric_limits<streamsize>::max(),
		'\n'
	);


	cout
		<< "Ten khach hang moi: ";

	getline(
		cin,
		rental.customerName
	);


	cout
		<< "Bien so moi: ";

	getline(
		cin,
		rental.carPlate
	);


	//nhap ngay bat dau moi
	while (true) {

		cout
			<< "Ngay bat dau moi (DD/MM/YYYY): ";


		getline(
			cin,
			rental.starDate
		);


		if (
			isValidDate(
				rental.starDate
			)
			) {

			break;
		}


		cout
			<< "Ngay bat dau khong hop le! "
			<< "Vui long nhap lai.\n";
	}


	//nhap ngay ket thuc moi
	while (true) {

		cout
			<< "Ngay ket thuc moi (DD/MM/YYYY): ";


		getline(
			cin,
			rental.endDate
		);


		if (
			!isValidDate(
				rental.endDate
			)
			) {

			cout
				<< "Ngay ket thuc khong hop le! "
				<< "Vui long nhap lai.\n";

			continue;
		}


		//kiem tra ngay ket thuc >= ngay bat dau
		if (
			!isStartBeforeOrEqualEnd(
				rental.starDate,
				rental.endDate
			)
			) {

			cout
				<< "Ngay ket thuc phai >= "
				<< "ngay bat dau!\n";

			continue;
		}


		break;
	}


	return rental;
}


// ============================================================
// XU LY THEM
// ============================================================

void handleAdd(
	renTalSystem& system
) {

	renTal rental =
		inputRental();


	if (
		system.addRental(
			rental
		)
		) {

		cout
			<< "\nThem don thue thanh cong!\n";
	}
	else {

		cout
			<< "\nThem that bai! "
			<< "Booking ID da ton tai "
			<< "hoac ngay khong hop le.\n";
	}
}

//xu ly cap nhat

void handleUpdate(
	renTalSystem& system
) {

	string bookingID;


	cout
		<< "\nNhap Booking ID can cap nhat: ";

	cin
		>> bookingID;


	//kiem tra don thue co ton tai khong
	if (
		system.getRental(
			bookingID
		) == nullptr
		) {

		cout
			<< "\nKhong tim thay Booking ID!\n";

		return;
	}


	//nhap du lieu moi
	renTal newRental =
		inputUpdatedRental(
			bookingID
		);


	//cap nhat
	if (
		system.updateRental(
			bookingID,
			newRental
		)
		) {

		cout
			<< "\nCap nhat thanh cong!\n";
	}
	else {

		cout
			<< "\nCap nhat that bai!\n";
	}
}


//xu ly xoa

void handleDelete(
	renTalSystem& system
) {

	string bookingID;


	cout
		<< "\nNhap Booking ID can xoa: ";

	cin
		>> bookingID;


	if (
		system.deleteRental(
			bookingID
		)
		) {

		cout
			<< "\nXoa don thue thanh cong!\n";
	}
	else {

		cout
			<< "\nKhong tim thay don thue!\n";
	}
}


// xu ly tim kiem

void handleSearch(
	const renTalSystem& system
) {

	string bookingID;


	cout
		<< "\nNhap Booking ID can tim: ";

	cin
		>> bookingID;


	system.searchRental(
		bookingID
	);
}


// xu ly UNDO

void handleUndo(
	renTalSystem& system
) {

	if (
		system.undoRental()
		) {

		cout
			<< "\nUndo thanh cong!\n";
	}
	else {

		cout
			<< "\nKhong co thao tac nao de Undo!\n";
	}
}


// hien thi menu

void showMenu() {

	cout << "\n";
	cout << "====================================\n";
	cout << "       HE THONG QUAN LY CHO THUE XE\n";
	cout << "====================================\n";

	cout << "1. Them don thue\n";
	cout << "2. Cap nhat don thue\n";
	cout << "3. Xoa don thue\n";
	cout << "4. Tim kiem don thue\n";
	cout << "5. Hien thi tat ca\n";
	cout << "6. Undo\n";
	cout << "7. Xem so luong thao tac Undo\n";
	cout << "8. Xem thao tac Undo gan nhat\n";
	cout << "9. Automated Test\n";
	cout << "10. Benchmark\n";
	cout << "0. Thoat\n";

	cout << "====================================\n";
}


// doi action Type thanh chuoi

string actionName(
	ActionType type
) {

	switch (type) {

	case ActionType::ADD:
		return "ADD";

	case ActionType::UPDATE:
		return "UPDATE";

	case ActionType::DELETE_ACTION:
		return "DELETE_ACTION";
	}

	return "UNKNOWN";
}


//xem thao tac UNDO gan nhat

void showTopUndo(
	const renTalSystem& system
) {

	Action action;


	if (
		!system.getTopUndo(
			action
		)
		) {

		cout
			<< "\nKhong co lich su Undo!\n";

		return;
	}


	cout
		<< "\nThao tac gan nhat: "
		<< actionName(action.type)
		<< endl;


	if (
		action.type ==
		ActionType::ADD
		) {

		cout
			<< "Booking ID: "
			<< action.newData.bookingID
			<< endl;
	}
	else {

		cout
			<< "Booking ID: "
			<< action.oldData.bookingID
			<< endl;
	}
}


// test stack  LIFO

bool testStackLIFO() {

	Mystack stack;


	Action action1;

	action1.type =
		ActionType::ADD;

	action1.newData.bookingID =
		"A001";


	Action action2;

	action2.type =
		ActionType::DELETE_ACTION;

	action2.oldData.bookingID =
		"A002";


	//push A001
	stack.push(action1);


	//push A002
	stack.push(action2);


	Action result;


	//pop lan 1
	bool pop1 =
		stack.pop(result);


	bool correct1 =
		pop1 &&
		result.type ==
		ActionType::DELETE_ACTION;


	//pop lan 2
	bool pop2 =
		stack.pop(result);


	bool correct2 =
		pop2 &&
		result.type ==
		ActionType::ADD;


	return
		correct1 &&
		correct2 &&
		stack.empty();
}


// test pop khi stack rong

bool testPopEmptyStack() {

	Mystack stack;

	Action action;


	return
		!stack.pop(action) &&
		stack.empty();
}


// test UNDO ADD

bool testUndoAdd() {

	renTalSystem system;


	renTal rental = {

		"T001",

		"Nguyen Van A",

		"59A11111",

		"20/09/2026",

		"22/09/2026"
	};


	//them don
	bool added =
		system.addRental(
			rental
		);


	//kiem tra ton tai
	bool existsBefore =
		system.getRental(
			"T001"
		) != nullptr;


	//UNDO
	bool undone =
		system.undoRental();


	//sau UNDO phai bien mat
	bool removedAfter =
		system.getRental(
			"T001"
		) == nullptr;


	return
		added &&
		existsBefore &&
		undone &&
		removedAfter;
}


// test UNDO Update

bool testUndoUpdate() {

	renTalSystem system;


	//du lieu cu
	renTal oldRental = {

		"T002",

		"Nguyen Van A",

		"59A22222",

		"20/09/2026",

		"22/09/2026"
	};


	//du lieu moi
	renTal newRental = {

		"T002",

		"Nguyen Van B",

		"59A99999",

		"25/09/2026",

		"28/09/2026"
	};


	//them du lieu ban dau
	bool added =
		system.addRental(
			oldRental
		);


	//xoa lich su ADD
	system.clearHistory();


	//UPDATE
	bool updated =
		system.updateRental(
			"T002",
			newRental
		);


	//UNDO
	bool undone =
		system.undoRental();


	//lay du lieu sau UNDO
	const renTal* result =
		system.getRental(
			"T002"
		);


	//kiem tra du lieu cu
	bool restored =
		result != nullptr &&

		result->customerName ==
		"Nguyen Van A" &&

		result->carPlate ==
		"59A22222" &&

		result->starDate ==
		"20/09/2026" &&

		result->endDate ==
		"22/09/2026";


	return
		added &&
		updated &&
		undone &&
		restored;
}


// test UNDO Delete

bool testUndoDelete() {

	renTalSystem system;


	renTal rental = {

		"T003",

		"Nguyen Van C",

		"59A33333",

		"23/09/2026",

		"24/09/2026"
	};


	//them
	bool added =
		system.addRental(
			rental
		);


	//xoa lich su ADD
	system.clearHistory();


	//DELETE
	bool deleted =
		system.deleteRental(
			"T003"
		);


	//kiem tra da xoa
	bool absent =
		system.getRental(
			"T003"
		) == nullptr;


	//UNDO
	bool undone =
		system.undoRental();


	//kiem tra da khoi phuc
	const renTal* result =
		system.getRental(
			"T003"
		);


	bool restored =
		result != nullptr &&

		result->bookingID ==
		"T003" &&

		result->customerName ==
		"Nguyen Van C";


	return
		added &&
		deleted &&
		absent &&
		undone &&
		restored;
}


// test UNDO theo LIFO

bool testUndoOrder() {

	renTalSystem system;


	renTal rental1 = {

		"T004",

		"Nguyen Van D",

		"59A44444",

		"01/09/2026",

		"02/09/2026"
	};


	renTal rental2 = {

		"T005",

		"Nguyen Van E",

		"59A55555",

		"03/09/2026",

		"04/09/2026"
	};


	//them T004
	system.addRental(
		rental1
	);


	//them T005
	system.addRental(
		rental2
	);


	//UNDO lan 1
	bool undo1 =
		system.undoRental();


	//T005 phai bi xoa
	bool t005Removed =
		system.getRental(
			"T005"
		) == nullptr;


	//T004 van con
	bool t004Exists =
		system.getRental(
			"T004"
		) != nullptr;


	//UNDO lan 2
	bool undo2 =
		system.undoRental();


	//T004 phai bi xoa
	bool t004Removed =
		system.getRental(
			"T004"
		) == nullptr;


	return
		undo1 &&
		t005Removed &&
		t004Exists &&
		undo2 &&
		t004Removed;
}


// chay Automated test

void runAutomatedTests() {

	cout << "\n";
	cout << "====================================\n";
	cout << "          AUTOMATED TEST\n";
	cout << "====================================\n";


	bool t1 =
		testStackLIFO();


	bool t2 =
		testPopEmptyStack();


	bool t3 =
		testUndoAdd();


	bool t4 =
		testUndoUpdate();


	bool t5 =
		testUndoDelete();


	bool t6 =
		testUndoOrder();


	cout
		<< "Test Stack LIFO     : "
		<< (t1 ? "PASS" : "FAIL")
		<< endl;


	cout
		<< "Test Pop Stack rong : "
		<< (t2 ? "PASS" : "FAIL")
		<< endl;


	cout
		<< "Test Undo ADD       : "
		<< (t3 ? "PASS" : "FAIL")
		<< endl;


	cout
		<< "Test Undo UPDATE    : "
		<< (t4 ? "PASS" : "FAIL")
		<< endl;


	cout
		<< "Test Undo DELETE    : "
		<< (t5 ? "PASS" : "FAIL")
		<< endl;


	cout
		<< "Test Undo LIFO      : "
		<< (t6 ? "PASS" : "FAIL")
		<< endl;


	int passed =

		static_cast<int>(t1) +

		static_cast<int>(t2) +

		static_cast<int>(t3) +

		static_cast<int>(t4) +

		static_cast<int>(t5) +

		static_cast<int>(t6);


	cout
		<< "------------------------------------\n";


	cout
		<< "Ket qua: "
		<< passed
		<< "/6 test PASS"
		<< endl;


	cout
		<< "====================================\n";
}


//Hash Table cho Benchmark

class MyHashTable {

private:

	vector<
		vector<string>
	> table;


	//ham hash
	size_t hashFunction(
		const string& key
	) const {

		size_t hash = 0;


		for (
			unsigned char c :
		key
			) {

			hash =
				hash * 131 + c;
		}


		return
			hash % table.size();
	}


public:

	//constructor
	MyHashTable(
		size_t capacity
	) {

		table.resize(
			capacity
		);
	}


	//them key
	void insert(
		const string& key
	) {

		int index =
			hashFunction(
				key
			);


		table[index].push_back(
			key
		);
	}


	//tim key
	bool search(
		const string& key
	) const {

		int index =
			hashFunction(
				key
			);


		for (
			const string& item :
			table[index]
			) {

			if (
				item == key
				) {

				return true;
			}
		}


		return false;
	}
};


// liner search

int linearSearch(
	const vector<string>& data,
	const string& target
) {

	for (
		int i = 0;
		i < static_cast<int>(data.size());
		i++
		) {

		if (
			data[i] == target
			) {

			return i;
		}
	}


	return -1;
}


// tao du lieu benchmark

vector<string> generateData(
	int N
) {

	vector<string> data;


	data.reserve(N);


	for (
		int i = 0;
		i < N;
		i++
		) {

		data.push_back(
			"RENT_" + to_string(100000 + i).substr(1)
		);
	}


	return data;
}


// do thoi gian liner search

double benchmarkLinear(
	const vector<string>& data,
	const string& target,
	int repeat
) {

	volatile int result = -1;


	auto start =
		high_resolution_clock::now();


	for (
		int i = 0;
		i < repeat;
		i++
		) {

		result =
			linearSearch(
				data,
				target
			);
	}


	auto finish =
		high_resolution_clock::now();


	(void)result;


	return duration<double, milli>(
		finish - start
	).count();
}


// do thoi gian Hash set

double benchmarkHash(
	const MyHashTable& hashTable,
	const string& target,
	int repeat
) {

	volatile bool result = false;


	auto start =
		high_resolution_clock::now();


	for (
		int i = 0;
		i < repeat;
		i++
		) {

		result =
			hashTable.search(
				target
			);
	}


	auto finish =
		high_resolution_clock::now();


	(void)result;


	return duration<double, milli>(
		finish - start
	).count();
}


// chay Benchmark

void runBenchmark() {

	cout << "\n";
	cout << "====================================\n";
	cout << "             BENCHMARK\n";
	cout << "====================================\n";


	int testSizes[] = {
		1000,
		10000,
		100000
	};


	for (
		int N :
	testSizes
		) {

		//tao du lieu
		vector<string> data =
			generateData(N);


		//chon phan tu cuoi
		string target =
			"B" + to_string(N - 1);


		//tao hash table
		MyHashTable hashTable(
			static_cast<size_t>(N) * 2 + 1
		);


		//them du lieu vao hash table
		for (
			const string& key :
			data
			) {

			hashTable.insert(
				key
			);
		}


		//so lan lap
		int repeat =
			(N <= 10000)
			? 1000
			: 200;


		//benchmark linear search
		double linearTime =
			benchmarkLinear(
				data,
				target,
				repeat
			);


		//benchmark hash table
		double hashTime =
			benchmarkHash(
				hashTable,
				target,
				repeat
			);


		cout
			<< "\nN = "
			<< N
			<< endl;


		cout
			<< "Linear Search: "
			<< linearTime
			<< " ms"
			<< endl;


		cout
			<< "Hash Table: "
			<< hashTime
			<< " ms"
			<< endl;
	}


	cout
		<< "\nLinear Search: O(N)\n";


	cout
		<< "Hash Table: trung binh O(1)\n";


	cout
		<< "====================================\n";
}


// chay chuong trinh

void runProgram() {

	renTalSystem system;


	int choice;


	do {

		showMenu();


		cout
			<< "Nhap lua chon: ";


		cin
			>> choice;


		switch (choice) {


			//them don thue
		case 1:

			handleAdd(
				system
			);

			break;


			//cap nhat
		case 2:

			handleUpdate(
				system
			);

			break;


			//xoa
		case 3:

			handleDelete(
				system
			);

			break;


			//tim kiem
		case 4:

			handleSearch(
				system
			);

			break;


			//hien thi tat ca
		case 5:

			system.showAll();

			break;


			//UNDO
		case 6:

			handleUndo(
				system
			);

			break;


			//xem so thao tac UNDO
		case 7:

			cout
				<< "\nSo thao tac co the Undo: "
				<< system.historySize()
				<< endl;

			break;


			//xem thao tac gan nhat
		case 8:

			showTopUndo(
				system
			);

			break;


			//Automated Test
		case 9:

			runAutomatedTests();

			break;


			//Benchmark
		case 10:

			runBenchmark();

			break;


			//thoat
		case 0:

			cout
				<< "\nKet thuc chuong trinh!\n";

			break;


		default:

			cout
				<< "\nLua chon khong hop le!\n";
		}


	} while (
		choice != 0
		);
}


int main() {

	runProgram();

	return 0;
}
