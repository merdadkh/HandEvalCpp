
#ifndef Hand_EVAL_H
#define Hand_EVAL_H

using namespace std;

#include <string>
#include <assert.h>
#include <stdint.h>

#include "Utilities.h"


#define POCKET_HAND_COUNT 1326
#define POCKET_PREFLOP_COUNT 169

class CHandEval
{

public: 
	
	enum HandTypes
	{
		HighCard = 0,
		Pair = 1,
		TwoPair = 2,
		Trips = 3,
		Straight = 4,
		Flush = 5,
		FullHouse = 6,
		FourOfAKind = 7,
		StraightFlush = 8
	};
	
	static const int Hearts = 2;
	static const int Diamonds = 1;
	static const int Clubs = 0;
	static const int Spades = 3;
	static const int SPADE_OFFSET = 13 * Spades;
	static const int CLUB_OFFSET = 13 * Clubs;
	static const int DIAMOND_OFFSET = 13 * Diamonds;
	static const int HEART_OFFSET = 13 * Hearts;
	static const int Rank2 = 0;
	static const int Rank3 = 1;
	static const int Rank4 = 2;
	static const int Rank5 = 3;
	static const int Rank6 = 4;
	static const int Rank7 = 5;
	static const int Rank8 = 6;
	static const int Rank9 = 7;
	static const int RankTen = 8;
	static const int RankJack = 9;
	static const int RankQueen = 10;
	static const int RankKing = 11;
	static const int RankAce = 12;
	static const int CardJoker = 52;
	static const int NumberOfCards = 52;
	static const int NCardsWJoker = 53;

private:	// Consts
	static const int HANDTYPE_SHIFT = 24;
	static const int TOP_CARD_SHIFT = 16;
	static const unsigned int TOP_CARD_MASK = 0x000F0000;
	static const int SECOND_CARD_SHIFT = 12;
	static const unsigned int SECOND_CARD_MASK = 0x0000F000;
	static const int THIRD_CARD_SHIFT = 8;
	static const int FOURTH_CARD_SHIFT = 4;
	static const int FIFTH_CARD_SHIFT = 0;
	static const unsigned int FIFTH_CARD_MASK = 0x0000000F;
	static const int CARD_WIDTH = 4;
	static const unsigned int CARD_MASK = 0x0F;
	static const unsigned int HANDTYPE_VALUE_STRAIGHTFLUSH = (((unsigned int)HandTypes::StraightFlush) << HANDTYPE_SHIFT);
	static const unsigned int HANDTYPE_VALUE_STRAIGHT = (((unsigned int)HandTypes::Straight) << HANDTYPE_SHIFT);
	static const unsigned int HANDTYPE_VALUE_FLUSH = (((unsigned int)HandTypes::Flush) << HANDTYPE_SHIFT);
	static const unsigned int HANDTYPE_VALUE_FULLHOUSE = (((unsigned int)HandTypes::FullHouse) << HANDTYPE_SHIFT);
	static const unsigned int HANDTYPE_VALUE_FOUR_OF_A_KIND = (((unsigned int)HandTypes::FourOfAKind) << HANDTYPE_SHIFT);
	static const unsigned int HANDTYPE_VALUE_TRIPS = (((unsigned int)HandTypes::Trips) << HANDTYPE_SHIFT);
	static const unsigned int HANDTYPE_VALUE_TWOPAIR = (((unsigned int)HandTypes::TwoPair) << HANDTYPE_SHIFT);
	static const unsigned int HANDTYPE_VALUE_PAIR = (((unsigned int)HandTypes::Pair) << HANDTYPE_SHIFT);
	static const unsigned int HANDTYPE_VALUE_HIGHCARD = (((unsigned int)HandTypes::HighCard) << HANDTYPE_SHIFT);

private:
	unsigned long long handmask;	/// <summary> Hand mask for the current card set </summary>
	string pocket;					/// <summary> Contains string representing the pocket cards </summary>
	string board;					/// <summary> Contains a string representing the board (common cards). </summary>
	unsigned int handval;			/// <summary> The value of the current hand. This value allows hands to be compared using a normal arithmitic compare function.</summary>

public: 
	CHandEval()
	{
		pocket = "";
		board = "";
	}

	CHandEval(string _pocket, string _board)
	{
		pocket = trim(_pocket);
		board = trim(_board);
	}


	/// <summary> This function takes a string representing a full or partial holdem hand and validates that the text represents valid cards and that no card is duplicated. </summary>
	static bool ValidateHand(string hand);
	static bool ValidateHand(string pocket, string board) { return ValidateHand(pocket + " " + board); };
	
	/// <summary> Parses an string description of a hand and returns a hand mask.  </summary>
	static unsigned long long ParseHand(string hand, int& cards);
	static unsigned long long ParseHand(string pocket, string board, int& cards) { return ParseHand(pocket + " " + board, cards); };
	static unsigned long long ParseHand(string hand) { int cards = 0;	return ParseHand(hand, cards); };

	/// <summary> Reads an string definition of a card and returns the Card value.</summary>
	static int ParseCard(string card) { int index = 0;	return NextCard(card, index); };
	/// <summary> Parses Card strings (internal) </summary>
	static int NextCard(string cards, int& index);

	static int CardRank(int card) { assert(card >= 0 && card <= 52);	return card % 13; };
	static int CardSuit(int card) { assert(card >= 0 && card <= 52);	return card / 13; };

	/// <summary> Takes a hand value and returns a description string. This function is obsolute. </summary>
	static string DescriptionFromMask(unsigned long long cards);
	static string DescriptionFromHand(string hand) { int cards = 0;	return DescriptionFromMask(ParseHand(hand, cards)); };
	static string DescriptionFromHandValueInternal(unsigned int handValue);

	/// <summary> Returns the string representing the hand. </summary>
	string ToString() { return pocket + " " + board; }
	static unsigned long long Mask(int index) { return CardMasksTable[index]; };
	static unsigned int HandType(unsigned int handValue) { return (handValue >> HANDTYPE_SHIFT); };
	static unsigned int TopCard(unsigned int hv) { return ((hv >> TOP_CARD_SHIFT) & CARD_MASK); };


	/// <summary> Turns a card mask into the equivalent human readable string. </summary>
	static string MaskToString(unsigned long long mask);

	/// <summary>  This function is faster (but provides less information) than Evaluate or Evaluate. </summary>
	static HandTypes EvaluateType(unsigned long long mask, int cards);
	static HandTypes EvaluateType(unsigned long long mask) { return EvaluateType(mask, BitCount(mask)); };
	
	/// <summary> Evaluates a hand (passed as a hand mask) and returns a hand value. A hand value can be compared against another hand value to determine which has the higher value. </summary>
	static unsigned int Evaluate(unsigned long long cards) { return Evaluate(cards, BitCount(cards)); };
	static unsigned int Evaluate(string hand) { return Evaluate(ParseHand(hand)); };
	static unsigned int Evaluate(unsigned long long cards, int numberOfCards);

	static int BitCount(unsigned long long bitField);

public: 
	/// <summary> Updates handmask and handval, called when card strings change </summary>
	void UpdateHandMask();

	// HandValue Functions
	static unsigned int SECOND_CARD(unsigned int hv) { return (((hv) >> SECOND_CARD_SHIFT) & CARD_MASK); };
	static unsigned int THIRD_CARD(unsigned int hv) { return (((hv) >> THIRD_CARD_SHIFT) & CARD_MASK); };
	static unsigned int FOURTH_CARD(unsigned int hv) { return (((hv) >> FOURTH_CARD_SHIFT) & CARD_MASK); };
	static unsigned int FIFTH_CARD(unsigned int hv) { return (((hv) >> FIFTH_CARD_SHIFT) & CARD_MASK); };
	static unsigned int HANDTYPE_VALUE(HandTypes ht) { return (((unsigned int)ht) << HANDTYPE_SHIFT); };
	static unsigned int TOP_CARD_VALUE(unsigned int c) { return ((c) << TOP_CARD_SHIFT); };
	static unsigned int SECOND_CARD_VALUE(unsigned int c) { return ((c) << SECOND_CARD_SHIFT); };
	static unsigned int THIRD_CARD_VALUE(unsigned int c) { return ((c) << THIRD_CARD_SHIFT); };
	static unsigned int CardMask(unsigned long long cards, int suit) { return (unsigned int)((cards >> (13 * suit)) & 0x1fffULL); };
	


	// Lookup Tables Const --> Public
public:

	/// <summary> Bit count table from snippets.org </summary>
	static const uint8_t bits[];
	static const uint16_t nBitsAndStrTable[];
	static const uint16_t topCardTable[];
	static const uint32_t topFiveCardsTable[];
	static const int8_t pocket2HighCardRankTable[];

	/// <summary> This table returns a straights starting card (0 if not a straight) </summary>
	static const uint16_t straightTable[];

	/// <summary>  A table representing the bit count for a 13 bit integer. </summary>
	static const uint16_t nBitsTable[];
	
	/// <summary> Converts card number into the card rank text string. </summary>
	static const string ranktbl[];

	/// <summary> Converts card number into the card suit text string </summary>
	static const string suittbl[];

	/// <summary> Converts card number into the card rank char </summary>
	static const char rankchar[];

	/// <summary>  Converts card number into the card suit text string </summary>
	static const char suitchar[];

public:
	/// <summary>  converts card number into the equivalent text string.  </summary>
	static const string CardTable[];

	/// <summary> This table is equivalent to 1ULL left shifted by the index. The lookup is faster than the left shift operator. </summary>
	static const unsigned long long CardMasksTable[];
};



#endif