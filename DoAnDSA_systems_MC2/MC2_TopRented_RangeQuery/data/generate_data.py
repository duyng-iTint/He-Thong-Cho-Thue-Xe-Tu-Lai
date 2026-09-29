"""
generate_data.py
Sinh du lieu don thue xe gia lap (khong sap xep san) de test & benchmark module MC2.

Cach chay:
    python generate_data.py 10000 donthue_xe_10000.csv
    python generate_data.py 100000 donthue_xe_100000.csv

Neu khong truyen tham so, mac dinh sinh 10000 dong vao donthue_xe_large.csv
"""
import csv
import random
import sys
from datetime import date, timedelta

CARS = [
    ("Toyota", "Vios"), ("Toyota", "Innova"), ("Toyota", "Camry"),
    ("Hyundai", "Accent"), ("Hyundai", "i10"), ("Hyundai", "Grand i10"),
    ("Kia", "Morning"), ("Kia", "Seltos"), ("Kia", "Soluto"),
    ("Honda", "City"), ("Honda", "CR-V"),
    ("Mazda", "CX-5"), ("Mazda", "3"),
    ("Ford", "Ranger"), ("Ford", "EcoSport"),
    ("Vinfast", "Fadil"), ("Vinfast", "Lux A2.0"),
]

FIRST_NAMES = ["Nguyen Van", "Tran Thi", "Le Van", "Pham Thi", "Hoang Van",
               "Do Thi", "Vu Van", "Bui Thi", "Ngo Van", "Dang Thi"]
LAST_NAMES = ["An", "Binh", "Cuong", "Dung", "Em", "Phuc", "Giang", "Hoa",
              "Khoa", "Linh", "Minh", "Nam", "Oanh", "Phong", "Quyen"]

START_DATE = date(2021, 1, 1)
END_DATE = date(2026, 9, 28)
TOTAL_DAYS = (END_DATE - START_DATE).days


def random_date():
    offset = random.randint(0, TOTAL_DAYS)
    return START_DATE + timedelta(days=offset)


def gen_plate():
    region = random.randint(11, 99)
    letter = random.choice("ABCDEFGHK")
    number = random.randint(10000, 99999)
    return f"{region}{letter}-{number // 100}.{number % 100:02d}"


def main():
    n = int(sys.argv[1]) if len(sys.argv) > 1 else 10000
    out_path = sys.argv[2] if len(sys.argv) > 2 else "donthue_xe_large.csv"

    # Gioi han so bien so xe khac nhau de co du lieu "top xe hot" thuc te
    fleet_size = max(20, n // 40)
    fleet = [gen_plate() for _ in range(fleet_size)]
    fleet_cars = [random.choice(CARS) for _ in fleet]

    with open(out_path, "w", newline="", encoding="utf-8") as f:
        writer = csv.writer(f)
        writer.writerow([
            "BookingID", "CarPlate", "CarBrand", "CarModel",
            "RentDate", "ReturnDate", "Price", "CustomerName", "MemberRank"
        ])

        for i in range(1, n + 1):
            idx = random.randint(0, fleet_size - 1)
            plate = fleet[idx]
            brand, model = fleet_cars[idx]

            rent_date = random_date()
            duration = random.randint(1, 7)
            return_date = rent_date + timedelta(days=duration)

            price = duration * random.randint(400000, 1200000)
            customer = f"{random.choice(FIRST_NAMES)} {random.choice(LAST_NAMES)}"
            member_rank = random.choices([0, 1, 2, 3], weights=[50, 30, 15, 5])[0]

            writer.writerow([
                f"RENT_HCM_{i:05d}",
                plate,
                brand,
                model,
                rent_date.isoformat(),
                return_date.isoformat(),
                price,
                customer,
                member_rank,
            ])

    print(f"Da sinh {n} dong du lieu vao file: {out_path}")
    print(f"So xe khac nhau trong doi xe (fleet_size): {fleet_size}")


if __name__ == "__main__":
    main()
