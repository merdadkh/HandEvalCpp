#ifndef TEST_CASES_H
#define TEST_CASES_H

#include <vector>

enum GameState { Wins = 0, Ties, Loss, NoState };

static const size_t twoCardCount = 1326;

void TestHandDescribtion();
void TestTimer(int rep = 10);

void ComputePreFlopPr();

void PrintOutput(size_t row, vector<vector<vector<long> > >* stat);

#endif