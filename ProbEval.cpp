
#include "ProbEval.h"

inline bool FlushSymmType::operator< (const FlushSymmType& rhs) const {
	if (drawType[0] < rhs.drawType[0])										return true;
	else if (drawType[0] == rhs.drawType[0]) {
		if (drawType[1] < rhs.drawType[1])									return true;
		else if (drawType[1] == rhs.drawType[1]){
			if ((SameSuit == false) && (rhs.SameSuit == true))				return true;
			else if (SameSuit == rhs.SameSuit){
				if (HigherCard[0] < rhs.HigherCard[0])						return true;
				else if (HigherCard[0] == rhs.HigherCard[0]) {
					if (HigherCard[1] < rhs.HigherCard[1])					return true;
				}
			}
		}

	}

	return false;
}


string FlushSymmType::strStat() {

	string out = "";

	for (size_t i = 0; i < 2; i++) {

		string drawStr;
		switch (drawType[i])
		{
		case NoDraw:	drawStr = "ND";			break;
		case BackDoor:	drawStr = "BD";		break;
		case FlushDraw:	drawStr = "FD";		break;
		case Flush:		drawStr = "FL";			break;
		}
		string HighCardStr;
		switch (HigherCard[i])
		{
		case true:		HighCardStr = "H";		break;
		case false:		HighCardStr = "L";		break;
		}
		out = out + drawStr + HighCardStr + "_";

	}

	switch (SameSuit)
	{
	case true:		out += "SS";		break;
	case false:		out += "OS";		break;
	}

	return out;
}



vector<long> CProbEval::Win_Tie_Loss_Cnt = vector<long>(CProbEval::StateCnt, 0);


vector<long>* CProbEval::Count_Win_Loss_Tie(unsigned long long FirstPocket, unsigned long long SecondPocket, unsigned long long BoardHand)
{
	Win_Tie_Loss_Cnt = vector<long>(StateCnt, 0);
	
	int NoBoardCard = 5;

	if (CHandEval::BitCount(FirstPocket | SecondPocket) == 4)
	{
		for (const unsigned long long& CompleteBoard : CHandIterator(NoBoardCard, BoardHand, FirstPocket | SecondPocket)) {


			// Evaluate all hands and determine the best hand
			unsigned int FirstHandValue = CHandEval::Evaluate(FirstPocket | CompleteBoard, 7);
			unsigned int SecondHandValue = CHandEval::Evaluate(SecondPocket | CompleteBoard, 7);

			if (FirstHandValue == SecondHandValue)
				Win_Tie_Loss_Cnt[Ties]++;
			else if (FirstHandValue > SecondHandValue)
				Win_Tie_Loss_Cnt[Wins]++;
			else
				Win_Tie_Loss_Cnt[Loss]++;

		}
	}
	
	return &Win_Tie_Loss_Cnt;
}

FlushSymmType CProbEval::ComputeFlopFlushSymm(unsigned long long FirstPocket, unsigned long long SecondPocket, unsigned long long BoardHand) {

	FlushSymmType flushSymType;

	vector<int> dominantSuit(2, -1);

	int BoardCard2Come = 5 - CHandEval::BitCount(BoardHand);

	for (size_t i = 0; i < 2; i++) { // Players
		unsigned int PocketRankUnsuit = 0;
		for (int j = 0; j < 4; j++)		
			PocketRankUnsuit |= CHandEval::CardMask((i == 0 ? FirstPocket : SecondPocket), j);

		for (int j = 0; j < 4; j++) { // Suits

			unsigned int HandsSuitRank = CHandEval::CardMask((i == 0 ? FirstPocket : SecondPocket) | BoardHand, j);
			unsigned int PocketSuitRank = CHandEval::CardMask((i == 0 ? FirstPocket : SecondPocket), j);
			if (CHandEval::nBitsTable[HandsSuitRank] == 5)
			{
				flushSymType.drawType[i] = FlushSymmType::Flush;
				flushSymType.HigherCard[i] = CHandEval::pocket2HighCardRankTable[PocketSuitRank] == CHandEval::pocket2HighCardRankTable[PocketRankUnsuit];
				dominantSuit[i] = j;
				break;
			}
			else if ((CHandEval::nBitsTable[HandsSuitRank] == 4) && (BoardCard2Come >= 1))
			{
				flushSymType.drawType[i] = FlushSymmType::FlushDraw;
				flushSymType.HigherCard[i] = CHandEval::pocket2HighCardRankTable[PocketSuitRank] == CHandEval::pocket2HighCardRankTable[PocketRankUnsuit];
				dominantSuit[i] = j;
				break;
			}
			else if ((CHandEval::nBitsTable[HandsSuitRank] == 3) && (BoardCard2Come >= 2)) {
				if (CHandEval::nBitsTable[CHandEval::CardMask(BoardHand, j)] < 3) {		
					flushSymType.drawType[i] = FlushSymmType::BackDoor;
					flushSymType.HigherCard[i] = CHandEval::pocket2HighCardRankTable[PocketSuitRank] == CHandEval::pocket2HighCardRankTable[PocketRankUnsuit];
					dominantSuit[i] = j;
				}
				else {		// All Board Backdoor
					flushSymType.drawType[i] = FlushSymmType::NoDraw;
				}
				break;
			}
			else{
				flushSymType.drawType[i] = FlushSymmType::NoDraw;
			}

		}

	
	}

	if ((dominantSuit[0] == dominantSuit[1]) && (dominantSuit[0] >= 0)) {
		flushSymType.SameSuit = true;
	}
	
	return flushSymType;
}

map<unsigned int, unsigned int> CProbEval::CreateMap_Pocket() {

	map<unsigned int, unsigned int> dict;

	unsigned int Index = 0;

	for (size_t i = 0; i < POCKET_HAND_COUNT; i++) {
		unsigned long long Pocket = CHandIterator::TwoCardTable[i];
		// Seperate out by suit
		unsigned int PocketRank = 0ULL;
		for (int j = 0; j < 4; j++) // Suits
			PocketRank |= CHandEval::CardMask(Pocket, j);

		map<unsigned int, unsigned int>::iterator it = dict.find(PocketRank);
		if (it == dict.end()) {
			dict.insert(pair<unsigned int, unsigned int>(PocketRank, Index));
			Index++;
		}
	}

	return dict;
}
const map<unsigned int, unsigned int> CProbEval::PocketMask2UnsuitRank_Dict = CProbEval::CreateMap_Pocket();



map<unsigned long long, unsigned int> CProbEval::CreateMap_Flop() {

	map<unsigned long long, unsigned int> dict;

	unsigned int Index = 0;

	for (const unsigned long long& flop : CHandIterator(3)) {
		// Seperate out by suit
		unsigned long long FlopMaskUnsuit = ComputeFlopMaskUnsuit(flop);

		map<unsigned long long, unsigned int>::iterator it = dict.find(FlopMaskUnsuit);
		if (it == dict.end()) {
			dict.insert(pair<unsigned long long, unsigned int>(FlopMaskUnsuit, Index));
			Index++;
		}
	}

	return dict;
}
const map<unsigned long long, unsigned int> CProbEval::FlopMask2UnsuitRank_Dict = CProbEval::CreateMap_Flop();


unsigned long long  CProbEval::ComputeFlopMaskUnsuit(unsigned long long flop) {

	unsigned long long FlopMaskUnsuit = 0ULL;
	for (int j = 0; j < 4; j++) {	// Suits
		unsigned int FlopRank = CHandEval::CardMask(flop, j);
		for (size_t m = 0; m < 13; m++) {
			FlopMaskUnsuit += ((FlopRank >> m) % 2) << (2 * m);
		}
	}

	return FlopMaskUnsuit;
}

string  CProbEval::FlopMaskUnsuit2Str(unsigned long long FlopMaskUnsuit) {
	
	string out = "";

	for (int m = 12; m >= 0; m--) {
		size_t CardCnt = (FlopMaskUnsuit >> (m*2)) % 4;
		for (size_t i = 0; i < CardCnt; i++)
			out += CHandEval::rankchar[m];
	}

	return out;
}

string  CProbEval::RankUnsuit2Str(unsigned long long MaskUnsuit) {

	string out = "";

	for (int m = 12; m >= 0; m--) {
		if ((MaskUnsuit >> m) & 1ULL)
			out += CHandEval::rankchar[m];
	}

	return out;
}



map<FlushSymmType, unsigned int> CProbEval::CreateMap_SuitSymm() {

	map<FlushSymmType, unsigned int> dict;
	unsigned int Index = 0;

	unsigned long long R1_1 = 0b1;
	unsigned long long R1_2 = 0b10;
	unsigned long long R2_1 = 0b100;
	unsigned long long R2_2 = 0b1000;
	unsigned long long RF_1 = 0b10000;
	unsigned long long RF_2 = 0b100000;
	unsigned long long RF_3 = 0b1000000;

	unsigned long long Hand1, Hand2, Flop;
	for (int H1_1 = 0; H1_1 < 4; H1_1++) {
		for (int H1_2 = 0; H1_2 < 4; H1_2++) {
			Hand1 = (R1_1 << (13 * H1_1)) + (R1_2 << (13 * H1_2));	// Hand1
			for (int H2_1 = 0; H2_1 < 4; H2_1++) {
				for (int H2_2 = 0; H2_2 < 4; H2_2++) {
					Hand2 = (R2_1 << (13 * H2_1)) + (R2_2 << (13 * H2_2));	// Hand2
					for (int F1 = 0; F1 < 4; F1++) {
						for (int F2 = 0; F2 < 4; F2++) {
							for (int F3 = 0; F3 < 4; F3++) {
								Flop = (RF_1 << (13 * F1)) + (RF_2 << (13 * F2)) + (RF_3 << (13 * F3));	// Flop

								FlushSymmType flushSymm = ComputeFlopFlushSymm(Hand1, Hand2, Flop);

								map<FlushSymmType, unsigned int>::iterator it = dict.find(flushSymm);
								if (it == dict.end()) {
									dict.insert(pair<FlushSymmType, unsigned int>(flushSymm, Index));
									Index++;
								}
							}
						}
					}
				}
			}
		}
	}

	return dict;
}
const map<FlushSymmType, unsigned int> CProbEval::SuitSymm_Dict = CProbEval::CreateMap_SuitSymm();


FlushSymmType CProbEval::FindSuitSymmType(unsigned int s) {

	for (map<FlushSymmType, unsigned int>::const_iterator it = CProbEval::SuitSymm_Dict.begin(); it != CProbEval::SuitSymm_Dict.end(); ++it)
		if (it->second == s)
			return it->first;

	cerr <<  "The index is not correct";
	return FlushSymmType();
}

unsigned int CProbEval::FindRank2PocketMask(unsigned int s) {

	for (map<unsigned int, unsigned int>::const_iterator it = CProbEval::PocketMask2UnsuitRank_Dict.begin(); it != CProbEval::PocketMask2UnsuitRank_Dict.end(); ++it)
		if (it->second == s)
			return it->first;

	cerr << "The index is not correct";
	return NULL;
}

unsigned long long CProbEval::FindRank2FlopMask(unsigned int s) {

	for (map<unsigned long long, unsigned int>::const_iterator it = CProbEval::FlopMask2UnsuitRank_Dict.begin(); it != CProbEval::FlopMask2UnsuitRank_Dict.end(); ++it)
		if (it->second == s)
			return it->first;

	cerr << "The index is not correct";
	return NULL;
}

unsigned int CProbEval::HandUnsuitRank91_Index(unsigned long long Pocket) {

	unsigned int PocketRank = 0ULL;
	for (int j = 0; j < 4; j++) // Suits
		PocketRank |= CHandEval::CardMask(Pocket, j);
	
	return CProbEval::PocketMask2UnsuitRank_Dict.find(PocketRank)->second;

}


vector<vector<double> > CProbEval::vFlopProb = vector<vector<double> >(POCKET_HAND_COUNT, vector<double>(POCKET_HAND_COUNT, -1));

vector<vector<double> >* CProbEval::LoadFlopProb_fromFile(unsigned long long Flop) {

	size_t suitSymmSize = CProbEval::SuitSymm_Dict.size();
	size_t pocketSize = CProbEval::PocketMask2UnsuitRank_Dict.size();

	vector<vector<vector<double> > > prob_Loaded(suitSymmSize, vector<vector<double> >(pocketSize, vector<double>(pocketSize, -1)));

	unsigned long long FlopMaskUnsuit = CProbEval::ComputeFlopMaskUnsuit(Flop);
	unsigned int flop_index = CProbEval::FlopMask2UnsuitRank_Dict.find(FlopMaskUnsuit)->second;

	
	///////////////////// Loading Files to Vectors
	ifstream inFile;
	string filePath;
	string basePath = "C:/Users/khatami/Google Drive/Mehrdad/Poker/HandEvalCpp/HandEvalCpp/PokerProj/FlopFilesFinal/";
	string line;
	char dummy[20];
	for (size_t s = 0; s < suitSymmSize; s++)
	{
		filePath = basePath + "Flop_" + CProbEval::FlopMaskUnsuit2Str(CProbEval::FindRank2FlopMask(flop_index)) + "_" + CProbEval::FindSuitSymmType(s).strStat();

		// Write2File
		inFile.open(filePath.c_str());
		if (inFile.is_open()) {
			getline(inFile, line);
			for (size_t h1 = 0; h1 < pocketSize; h1++) {
				inFile >> dummy;
				for (size_t h2 = 0; h2 < pocketSize; h2++) {
					inFile >> prob_Loaded[s][h1][h2];
				}
			}
			inFile.close();
		}
	}

	/////////////////////////////////////////////// Maping the loaded vectors to 1326x1326 Vector

	vFlopProb = vector<vector<double> >(POCKET_HAND_COUNT, vector<double>(POCKET_HAND_COUNT, -1));

	for (size_t h1 = 0; h1 < POCKET_HAND_COUNT; h1++) {
		unsigned long long FirstPocket = CHandIterator::TwoCardTable[h1];
		if ((Flop & FirstPocket) == 0ULL)
		{
			unsigned int H1_Index = CProbEval::HandUnsuitRank91_Index(FirstPocket);
			for (size_t h2 = h1 + 1; h2 < POCKET_HAND_COUNT; h2++)
			{
				unsigned long long SecondPocket = CHandIterator::TwoCardTable[h2];
				if (((Flop & SecondPocket) == 0ULL) && ((FirstPocket & SecondPocket) == 0))
				{
					unsigned int H2_Index = CProbEval::HandUnsuitRank91_Index(SecondPocket);
					FlushSymmType flushSymm = CProbEval::ComputeFlopFlushSymm(FirstPocket, SecondPocket, Flop);
					unsigned int SuitDraw_Index = CProbEval::SuitSymm_Dict.find(flushSymm)->second;
					vFlopProb[h1][h2] = prob_Loaded[SuitDraw_Index][H1_Index][H2_Index];
					vFlopProb[h2][h1] = 1 - vFlopProb[h1][h2];
				}
			}
		}
	}


	return &vFlopProb;
}