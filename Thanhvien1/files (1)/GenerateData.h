// GenerateData.h
// --------------
// Sinh file CSV gia lap voi N ban ghi, dung cho:
// - Chay thu he thong (demo MC1).
// - Do bang chung hieu nang (Muc 5.3): toi thieu 10.000 ban ghi, do tai
//   >= 2 moc quy mo cach nhau >= 1 bac do lon (VD: 1.000 va 10.000 hoac
//   100.000).
//
// Du lieu la gia lap ngau nhien nhung co seed co dinh de tai lap duoc.

#pragma once

#include <string>

void generateCsv(const std::string& csvPath, int nRecords, unsigned int seed = 42);
