
#include <iostream>
#include <ctime>
#include <fstream>
#include "TestCases.h"
#include "ProbEval.h"

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

void TestPostFlopTime() {
	
	string board;
	int noBoardCards;
	cout << "Type a board (no spaces):";
	cin >> board;

	unsigned long long FirstPocket, SecondPocket;

	vector<vector<double> > WinProb(handCount, vector<double>(handCount, 0));
	vector<long>* WTL;

	std::clock_t start_all;

	// 5 Card Iterator 
	start_all = std::clock();

	unsigned long long boardMask = CHandEval::ParseHand(board, noBoardCards);
	cout << endl << "Number of Board Cards: " << noBoardCards << endl;

	for (size_t i = 0; i < handCount; i++) {
		FirstPocket = CHandIterator::TwoCardTable[i];
		if ((FirstPocket & boardMask) == 0) {
			for (size_t j = i + 1; j < handCount; j++) {
				SecondPocket = CHandIterator::TwoCardTable[j];
				if ((SecondPocket & boardMask) == 0) {
					if ((FirstPocket & SecondPocket) == 0) {
						WTL = CProbEval::Count_Win_Loss_Tie(FirstPocket, SecondPocket, boardMask);
						WinProb[i][j] = (double)(WTL->at(CProbEval::Wins) + (double)WTL->at(CProbEval::Ties) / 2) / (double)(WTL->at(CProbEval::Wins) + WTL->at(CProbEval::Ties) + WTL->at(CProbEval::Loss));
						WinProb[j][i] = 1 - WinProb[i][j];
					}
				}
			}
		}
	}

	cout << "Board: " << board << "\tTime All: " << (std::clock() - start_all) / (double)CLOCKS_PER_SEC << "\n\t";


	ofstream ofs("Win_Prob.txt");
	ofs.precision(3);

	ofs << "Board: " << board << "\tTime All: " << (std::clock() - start_all) / (double)CLOCKS_PER_SEC << "\n\t";
	for (size_t i = 0; i < POCKET_HAND_COUNT; i++)
		ofs << CHandEval::MaskToString(CHandIterator::TwoCardTable[i]) << "\t";
	ofs << "\n";

	for (size_t i = 0; i < WinProb.size(); i++) {
		ofs << CHandEval::MaskToString(CHandIterator::TwoCardTable[i]) << "\t";
		for (size_t j = 0; j < WinProb[i].size(); j++)
			ofs << WinProb[i][j] << "\t";
		ofs << "\n";
	}
	ofs.close();

}

void ComputePreFlopPr() {


	vector<vector<vector<long> > > Stat(pocketCount, vector<vector<long> >(pocketCount, vector<long>(CProbEval::StateCnt, 0)));

	ofstream ofs;
	ofs.open("ties.txt", std::ofstream::out);			ofs.close();
	ofs.open("wins.txt", std::ofstream::out);			ofs.close();
	ofs.open("loss.txt", std::ofstream::out);			ofs.close();


	for (size_t i = 0; i < CHandIterator::Pocket169Table.size(); i++)
	{
		for (size_t j = 0; j < CHandIterator::Pocket169Table.size(); j++)
		{

			unsigned long long FirstPocket = CHandIterator::Pocket169Table[i][0];
			unsigned long long SecondPocket = CHandIterator::Pocket169Table[j][0];
			size_t k = 0;
			while (CHandEval::BitCount(FirstPocket | SecondPocket) != 4)
			{
				k++;
				SecondPocket = CHandIterator::Pocket169Table[j][k];
			}
			
			Stat[i][j] = *CProbEval::Count_Win_Loss_Tie(FirstPocket, SecondPocket);

		}

		PrintOutput(i, Stat);
		std::cout << "Row " << i << " Done!" << endl;

	}

}


void PrintOutput(size_t row, vector<vector<vector<long> > >& stat)  {

	ofstream ofs;

	ofs.open("wins.txt", std::ofstream::app);
	for (size_t i = 0; i < stat.at(row).size(); i++)	ofs << stat.at(row)[i][CProbEval::Wins] << "\t";
	ofs << "\n";
	ofs.close();

	ofs.open("ties.txt", std::ofstream::app);
	for (size_t i = 0; i < stat.at(row).size(); i++)	ofs << stat.at(row)[i][CProbEval::Ties] << "\t";
	ofs << "\n";
	ofs.close();

	ofs.open("loss.txt", std::ofstream::app);
	for (size_t i = 0; i < stat.at(row).size(); i++)	ofs << stat.at(row)[i][CProbEval::Loss] << "\t";
	ofs << "\n";
	ofs.close();


}


void Create_TwoCard2HighCardRankTbl() {

	vector<int> Tbl(8192, -1);
	for (int Pocket = 0; Pocket < 8192; Pocket++) {

		for (int r = CHandEval::RankAce; r >= CHandEval::Rank2; r--) {
			if (((Pocket >> r) & 0x1)) {
				Tbl[Pocket] = r;
				break;
			}
		}

	}


	ofstream ofs("TwoCard2HighCardRankTbl.txt");
	for (size_t i = 0; i < Tbl.size(); i++)	ofs << "\t" << Tbl[i] << ",\n";
	ofs.close();


};

void TestFlushDraw() {

	string H1;
	cout << "Type a Hand 1 (no spaces):";
	cin >> H1;

	unsigned long long H1Mask = CHandEval::ParseHand(H1);


	string H2;
	cout << "\nType a Hand 2 (no spaces):";
	cin >> H2;

	unsigned long long H2Mask = CHandEval::ParseHand(H2);


	string flop;
	cout << "\nType a flop (no spaces):";
	cin >> flop;

	unsigned long long flopMask = CHandEval::ParseHand(flop);

	FlushSymmType type = CProbEval::ComputeFlopFlushSymm(H1Mask, H2Mask, flopMask);



	cout << "Hand1 Draw Type: " << type.drawType[0] << " with High Card " << type.HigherCard[0] << endl;
	cout << "Hand2 Draw Type: " << type.drawType[1] << " with High Card " << type.HigherCard[1] << endl;
	cout << "Same Suit Draw : " << type.SameSuit << endl;


}


void ComputeSaveFlopProb2File() {

	vector<long>* WTL;		// win tie lose

	size_t flopSize = CProbEval::FlopMask2UnsuitRank_Dict.size();
	size_t pocketSize = CProbEval::PocketMask2UnsuitRank_Dict.size();
	size_t suitSymmSize = CProbEval::SuitSymm_Dict.size();

	vector<vector<vector<vector< double > > > > Output;
	Output = vector<vector<vector<vector< double > > > >(flopSize, vector<vector<vector< double > > >(suitSymmSize, vector<vector< double > >(pocketSize, vector< double >(pocketSize, -1))));

	for (const unsigned long long& flop : CHandIterator(3)) {
		// Seperate out by suit
		unsigned long long FlopMaskUnsuit = CProbEval::ComputeFlopMaskUnsuit(flop);
		unsigned int flop_index = CProbEval::FlopMask2UnsuitRank_Dict.find(FlopMaskUnsuit)->second;

		for (size_t h1 = 0; h1 < POCKET_HAND_COUNT; h1++)
		{
			unsigned long long FirstPocket = CHandIterator::TwoCardTable[h1];	
			if ((flop & FirstPocket) == 0ULL) 
			{
				unsigned int FirstPocketRank = 0ULL;
				for (int j = 0; j < 4; j++) // Suits
					FirstPocketRank |= CHandEval::CardMask(FirstPocket, j);
				unsigned int H1_Index = CProbEval::PocketMask2UnsuitRank_Dict.find(FirstPocketRank)->second;

				for (size_t h2 = h1 + 1; h2 < POCKET_HAND_COUNT; h2++)
				{
					unsigned long long SecondPocket = CHandIterator::TwoCardTable[h2];
					if (((flop & SecondPocket) == 0ULL) && ((FirstPocket & SecondPocket) == 0))
					{
						unsigned int SecondPocketRank = 0ULL;
						for (int j = 0; j < 4; j++) // Suits
							SecondPocketRank |= CHandEval::CardMask(SecondPocket, j);
						unsigned int H2_Index = CProbEval::PocketMask2UnsuitRank_Dict.find(SecondPocketRank)->second;

						FlushSymmType flushSymm = CProbEval::ComputeFlopFlushSymm(FirstPocket, SecondPocket, flop);
						unsigned int SuitDraw_Index = CProbEval::SuitSymm_Dict.find(flushSymm)->second;

						if (Output[flop_index][SuitDraw_Index][H1_Index][H2_Index] < 0)
						{
							 
							WTL = CProbEval::Count_Win_Loss_Tie(FirstPocket, SecondPocket, flop);
							Output[flop_index][SuitDraw_Index][H1_Index][H2_Index] = (double)(WTL->at(CProbEval::Wins) + (double)WTL->at(CProbEval::Ties) / 2) / (double)(WTL->at(CProbEval::Wins) + WTL->at(CProbEval::Ties) + WTL->at(CProbEval::Loss));
							
							FlushSymmType InverseflushSymm = ~flushSymm;
							unsigned int InverseIndexSuitDraw_Index = CProbEval::SuitSymm_Dict.find(InverseflushSymm)->second;

							Output[flop_index][InverseIndexSuitDraw_Index][H2_Index][H1_Index] = 1 - Output[flop_index][SuitDraw_Index][H1_Index][H2_Index];
						}

					}
				}
			}
		}
	}


	ofstream ofs;

	string filePath;
	string basePath = "./FlopFiles/";

	for (size_t f = 0; f < flopSize; f++) 
	{
		for (size_t s = 0; s < suitSymmSize; s++) 
		{
			
			filePath = basePath + "Flop_" + CProbEval::FlopMaskUnsuit2Str(CProbEval::FindRank2FlopMask(f)) + "_" + CProbEval::FindSuitSymmType(s).strStat();

			// Write2File
			ofs.open(filePath.c_str());
			ofs.precision(3);

			ofs << "\t";
			for (size_t h2 = 0; h2 < pocketSize; h2++) {
				ofs << CProbEval::RankUnsuit2Str(CProbEval::FindRank2PocketMask(h2)) << "\t";
			}
			ofs << "\n";

			for (size_t h1 = 0; h1 < pocketSize; h1++) {
				ofs << CProbEval::RankUnsuit2Str(CProbEval::FindRank2PocketMask(h1)) << "\t";
				for (size_t h2 = 0; h2 < pocketSize; h2++)
					ofs << Output[f][s][h1][h2] << "\t";
				ofs << "\n";
			}
			ofs.close();
		}
	}


}


void ComputePreFlopPrFull() {


	vector<long>* WTL;		// win tie lose


	vector<vector<double> > Output(handCount, vector<double>(handCount, -1));
	
	string basePath = "./PreFlop/PreFlopProb";

	ofstream ofs(basePath.c_str());
	ofs.precision(3);

	ofs << "\t";
	for (size_t i = 0; i < handCount; i++) {
		ofs << CHandEval::MaskToString(CHandIterator::TwoCardTable[i]) << "\t";
	}
	ofs << "\n";


	for (size_t i = 0; i < handCount; i++)
	{
		unsigned long long FirstPocket = CHandIterator::TwoCardTable[i];
		ofs << CHandEval::MaskToString(FirstPocket) << "\t";
		for (size_t j = 0; j < handCount; j++)
		{
			if (Output[i][j] < 0) {
				unsigned long long SecondPocket = CHandIterator::TwoCardTable[j];

				if ((FirstPocket&SecondPocket) == 0) {
					WTL = CProbEval::Count_Win_Loss_Tie(FirstPocket, SecondPocket);
					Output[i][j] = (double)(WTL->at(CProbEval::Wins) + (double)WTL->at(CProbEval::Ties) / 2) / (double)(WTL->at(CProbEval::Wins) + WTL->at(CProbEval::Ties) + WTL->at(CProbEval::Loss));
					Output[j][i] = 1 - Output[i][j];
				}
			}
			ofs << Output[i][j] << "\t";
		}
		ofs << "\n";
	}

	ofs.close();
}




void TestFlopLoadFromFile() {

	//string H1;
	//cout << "Type a Hand 1 (no spaces):";
	//cin >> H1;

	//unsigned long long H1Mask = CHandEval::ParseHand(H1);


	//string H2;
	//cout << "\nType a Hand 2 (no spaces):";
	//cin >> H2;

	//unsigned long long H2Mask = CHandEval::ParseHand(H2);


	string flop;
	cout << "\nType a flop (no spaces):";
	cin >> flop;

	std::clock_t start;
	double duration;

	// 5 Card Iterator 
	start = std::clock();

	unsigned long long flopMask = CHandEval::ParseHand(flop);

	vector<vector<double> >* vector = CProbEval::LoadFlopProb_fromFile(flopMask);

	duration = (std::clock() - start) / (double)CLOCKS_PER_SEC;
	cout << "Flop Prob Load: " << duration << " second" << endl;



	string ans = "y";

	while (ans != "n")
	{

		string hand1, hand2;
		cout << "\nType hand 1 (no spaces):";
		cin >> hand1;
		cout << "\nType hand 2 (no spaces):";
		cin >> hand2;

		unsigned long long hand1Mask = CHandEval::ParseHand(hand1);
		size_t h1_index = CHandIterator::PocketMask2IndexMap.find(hand1Mask)->second;

		double out;
		if (hand2 == "n") {
			double sum = 0;
			int count = 0;
			for (size_t i = 0; i < POCKET_HAND_COUNT; i++) {

				if (vector->at(h1_index).at(i) >= 0)
				{
					sum += vector->at(h1_index).at(i);
					count++;
				}
			}
			out = sum / count;
		}
		else {
			unsigned long long hand2Mask = CHandEval::ParseHand(hand2);
			size_t h2_index = CHandIterator::PocketMask2IndexMap.find(hand2Mask)->second;
			out = vector->at(h1_index).at(h2_index);
		}

		cout << "\nProb: " << out;




		cout << "\nAnother Test(y/n) :";
		cin >> ans;
	}



	
}
