
#include <iostream>
#include "HandEval.h"
#include "TestCases.h"

int main(int argc, char *argv[])
{

	//while (ans == "y")
	//{
	//	cout << "Testing Describtion" << endl;
	//	TestHandDescribtion();
	//	cout << "Another Test(y/n) :";
	//	cin >> ans;
	//} 
	TestAnyBoard();
	ComputePreFlopPrFull();
	// ComputeSaveFlopProb2File();

	TestFlopLoadFromFile();
	TestPreFlopLoadFromFile();
	// TestPostFlopTime();
	// Create_TwoCard2HighCardRankTbl();

	
	//string ans = "y";

	//while (ans == "y")
	//{
	//	cout << "Testing Flush Draw" << endl;
	//	TestFlushDraw();
	//	cout << "Another Test(y/n) :";
	//	cin >> ans;
	//}


	return 0;

}
