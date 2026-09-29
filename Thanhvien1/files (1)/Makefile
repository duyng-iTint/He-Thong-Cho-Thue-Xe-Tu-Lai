CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

all: demo_mc1 benchmark_mc1 test_thanhvien1

demo_mc1: demo_mc1.cpp Persistence.cpp GenerateData.cpp Booking.h MyHashTable.h Persistence.h GenerateData.h
	$(CXX) $(CXXFLAGS) -o demo_mc1 demo_mc1.cpp Persistence.cpp GenerateData.cpp

benchmark_mc1: benchmark_mc1.cpp Persistence.cpp GenerateData.cpp Booking.h MyHashTable.h Persistence.h GenerateData.h
	$(CXX) $(CXXFLAGS) -o benchmark_mc1 benchmark_mc1.cpp Persistence.cpp GenerateData.cpp

test_thanhvien1: test_thanhvien1.cpp Persistence.cpp Booking.h MyHashTable.h Persistence.h
	$(CXX) $(CXXFLAGS) -o test_thanhvien1 test_thanhvien1.cpp Persistence.cpp

clean:
	rm -f demo_mc1 benchmark_mc1 test_thanhvien1 *.o donthue_xe.csv _bench_*.csv _test_round_trip.csv

.PHONY: all clean
