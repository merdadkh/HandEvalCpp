
#include "HandEval.h"
#include "HandIter.h"
#include <iostream>
#include <ctime>
#include <fstream>
#include "TestCases.h"

void TestHandDescribtion() {

	string hand;
	cout << "Type a hand (no spaces):";
	cin >> hand;

	if (CHandEval::ValidateHand(hand)) {

		unsigned long long a = CHandEval::ParseHand(hand);
		cout << "Parsed Value: " << a << " is " << CHandEval::DescriptionFromMask(a) << endl;
		cout << "Test Mask to String: " << CHandEval::MaskToString(a) << endl;
	}
	else {
		cout << "Not a valid Hand" << endl;
	}


}


void TestTimer(int rep) {

	int j = 0;

	std::clock_t start;
	double duration;

	// 5 Card Iterator 
	start = std::clock();
	j = 0;
	for (int cnt = 0; cnt < rep; cnt++) {
		for (const unsigned long long& i : CHandIterator(5)) {
			j++;
		}
	}
	duration = (std::clock() - start) / (double)CLOCKS_PER_SEC;
	cout << "5 Card Hand Iter: " << (int)(j / duration) << " hand per second" << endl;

	//

	start = std::clock();
	j = 0;
	for (int cnt = 0; cnt < rep; cnt++) {
		for (const unsigned long long& i : CHandIterator(7)) {
			j++;
		}
	}
	duration = (std::clock() - start) / (double)CLOCKS_PER_SEC;
	cout << "7 Card Hand Iter: " << (int)(j / duration) << " hand per second" << endl;


	// 5 Card Iterator 
	start = std::clock();
	j = 0;
	for (int cnt = 0; cnt < rep; cnt++) {
		for (const unsigned long long& i : CHandIterator(5)) {
			CHandEval::EvaluateType(i);
			j++;
		}
	}
	duration = (std::clock() - start) / (double)CLOCKS_PER_SEC;
	cout << "5 Card Hand Iter: " << (int)(j / duration) << " hand per second" << endl;

	//

	start = std::clock();
	j = 0;
	for (int cnt = 0; cnt < rep; cnt++) {
		for (const unsigned long long& i : CHandIterator(7)) {
			CHandEval::EvaluateType(i);
			j++;
		}
	}
	duration = (std::clock() - start) / (double)CLOCKS_PER_SEC;
	cout << "7 Card Hand Iter: " << (int)(j / duration) << " hand per second" << endl;
}


void ComputePreFlopPr() {


	vector<vector<vector<long> > > Stat(twoCardCount, vector<vector<long> >(twoCardCount, vector<long>(NoState, 0)));

	ofstream ofs;
	ofs.open("ties.txt", std::ofstream::out);			ofs.close();
	ofs.open("wins.txt", std::ofstream::out);			ofs.close();
	ofs.open("loss.txt", std::ofstream::out);			ofs.close();


	long i = 0;
	for (const unsigned long long& FirstPocket : CHandIterator(2)) {
		long j = 0;
		for (const unsigned long long& SecondPocket : CHandIterator(2)) {
			
			Stat[i][j][Wins] = 0;
			Stat[i][j][Loss] = 0;
			Stat[i][j][Ties] = 0;
			
			if (CHandEval::BitCount(FirstPocket | SecondPocket) == 4)
			{
				for (const unsigned long long& BoardHand : CHandIterator(5, 0ULL, FirstPocket | SecondPocket)) {


					// Evaluate all hands and determine the best hand
					unsigned long long FirstHandValue = CHandEval::Evaluate(FirstPocket | BoardHand, 7);
					unsigned long long SecondHandValue = CHandEval::Evaluate(SecondPocket | BoardHand, 7);

					if (FirstHandValue == SecondHandValue)
						Stat[i][j][Ties]++;
					else if (FirstHandValue > SecondHandValue)
						Stat[i][j][Wins]++;
					else
						Stat[i][j][Loss]++;

				}
			}
			j++;
		}

		PrintOutput((size_t)i , &Stat);
		std::cout << "Row " << i << " Done!" << endl;
		i++;
	}

}


void PrintOutput(size_t row, vector<vector<vector<long> > >* stat)  {

	ofstream ofs;

	ofs.open("wins.txt", std::ofstream::app);
	for (size_t i = 0; i < twoCardCount; i++)	ofs << stat->at(row)[i][Wins] << "\t";
	ofs << "\n";
	ofs.close();

	ofs.open("ties.txt", std::ofstream::app);
	for (size_t i = 0; i < twoCardCount; i++)	ofs << stat->at(row)[i][Ties] << "\t";
	ofs << "\n";
	ofs.close();

	ofs.open("loss.txt", std::ofstream::app);
	for (size_t i = 0; i < twoCardCount; i++)	ofs << stat->at(row)[i][Loss] << "\t";
	ofs << "\n";
	ofs.close();


}


