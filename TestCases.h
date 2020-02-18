
#ifndef TEST_CASES_H
#define TEST_CASES_H

#include <vector>

static const size_t pocketCount = 169;
static const size_t handCount = 1326;

void TestHandDescribtion();
void TestTimer(int rep = 10);
void TestPostFlopTime();

void ComputePreFlopPr();
void ComputePreFlopPrFull();

void PrintOutput(size_t row, std::vector<std::vector<std::vector<long> > >& stat );

void Create_TwoCard2HighCardRankTbl();
void TestFlushDraw();

void ComputeSaveFlopProb2File();

void TestFlopLoadFromFile();
void TestPreFlopLoadFromFile();
void TestAnyBoard();

#endif