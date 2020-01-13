
#ifndef PROB_EVAL_H
#define PROB_EVAL_H

#include "HandEval.h"
#include "HandIter.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

class FlushSymmType
{
public:
	enum FlopFlushSymm :int { NoDraw = 0, BackDoor, FlushDraw, Flush, FlushSymmNo };

public: 
	int drawType[2];
	bool HigherCard[2];
	bool SameSuit;
	FlushSymmType(void) {
		drawType[0] = NoDraw;	drawType[1] = NoDraw;
		HigherCard[0] = false;	HigherCard[1] = false;
		SameSuit = false;
	};
	inline bool operator ==(const FlushSymmType &rhs) const{
		return ((this->drawType[0] == rhs.drawType[0]) && (this->drawType[1] == rhs.drawType[1]) && (this->HigherCard[0] == rhs.HigherCard[0]) && (this->HigherCard[1] == rhs.HigherCard[1]) && (this->SameSuit == rhs.SameSuit));
	};
	inline bool operator !=(const FlushSymmType &rhs) const { return !(*this == rhs); }

	inline bool operator< (const FlushSymmType& rhs) const;
	inline bool operator> (const FlushSymmType& rhs) const { return (rhs < (*this)); }
	inline bool operator<=(const FlushSymmType& rhs) const { return !((*this) > rhs); }
	inline bool operator>=(const FlushSymmType& rhs) const { return !((*this) < rhs); }
	FlushSymmType(const FlushSymmType & it){
		drawType[0] = it.drawType[0];
		drawType[1] = it.drawType[1];
		HigherCard[0] = it.HigherCard[0];
		HigherCard[1] = it.HigherCard[1];
		SameSuit = it.SameSuit;
	}

	inline FlushSymmType operator~ () const {
		FlushSymmType out;
		out.drawType[0] = this->drawType[1];
		out.drawType[1] = this->drawType[0];
		out.HigherCard[0] = this->HigherCard[1];
		out.HigherCard[1] = this->HigherCard[0];
		out.SameSuit = this->SameSuit;

		return out;
	}

	string strStat();


};


class CProbEval
{


public: 
	
	static const map<unsigned int, unsigned int> PocketMask2UnsuitRank_Dict;
	static const map<unsigned long long, unsigned int> FlopMask2UnsuitRank_Dict;
	static const map<FlushSymmType, unsigned int> SuitSymm_Dict;

public:

	enum GameState { Wins = 0, Ties, Loss, StateCnt };

	static vector<long>* Count_Win_Loss_Tie(unsigned long long FirstPocket, unsigned long long SecondPocket, unsigned long long BoardHand = 0ULL);
	static FlushSymmType ComputeFlopFlushSymm(unsigned long long FirstPocket, unsigned long long SecondPocket, unsigned long long BoardHand);


	static map<unsigned int, unsigned int> CreateMap_Pocket();
	static map<unsigned long long, unsigned int> CreateMap_Flop();
	static map<FlushSymmType, unsigned int> CreateMap_SuitSymm();

	static FlushSymmType FindSuitSymmType(unsigned int s);
	static unsigned int FindRank2PocketMask(unsigned int s);
	static unsigned long long FindRank2FlopMask(unsigned int s);
	static unsigned int HandUnsuitRank91_Index(unsigned long long Pocket);


	static unsigned long long  ComputeFlopMaskUnsuit(unsigned long long flop);
	static string  FlopMaskUnsuit2Str(unsigned long long FlopMaskUnsuit);
	static string  RankUnsuit2Str(unsigned long long FlopMaskUnsuit);
	
	static vector<vector<double> >* LoadFlopProb_fromFile(unsigned long long Flop);

private:

	static vector<long> Win_Tie_Loss_Cnt;
	static vector<vector<double> > vFlopProb;

	

};


#endif