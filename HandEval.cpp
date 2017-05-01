
#include "HandEval.h"

bool CHandEval::ValidateHand(string hand)
{
	int index = 0;
	unsigned long handmask = 0ULL;
	int cards = 0;
	int card = 0;


	for (card = NextCard(hand, index); card >= 0; card = NextCard(hand, index))
	{
		if ((handmask & (1ULL << card)) != 0)
			return false;
		handmask |= (1ULL << card);
		cards++;
	}

	return card == -1 && cards > 0 && (size_t)index >= hand.length();

}

unsigned long long CHandEval::ParseHand(string hand, int& cards)
{
	int index = 0;
	unsigned long long handmask = 0ULL;

	// A null hand is okay
	if (trim(hand).length() == 0)
	{
		cards = 0;
		return 0ULL;
	}

#if _DEBUG
	assert(ValidateHand(hand));
#endif

	// Parse the hand
	cards = 0;
	for (int card = NextCard(hand, index); card >= 0; card = NextCard(hand, index))
	{
		handmask |= (1ULL << card);
		cards++;
	}
	return handmask;
}


int CHandEval::NextCard(string cards, int& index)
{
	int rank = 0, suit = 0;

	// Remove whitespace
	while ((size_t)index < cards.length() && cards[index] == ' ')
		index++;

	if ((size_t)index >= cards.length())
		return -1;

	// Parse cards
	if ((size_t)index < cards.length())
	{
		switch (cards[index++])
		{
		case '1':
			if (cards[index] == '0')
			{
				index++;
				rank = RankTen;
			}
			else
			{
				return -1;
			}
			break;
		case '2':
			rank = Rank2;
			break;
		case '3':
			rank = Rank3;
			break;
		case '4':
			rank = Rank4;
			break;
		case '5':
			rank = Rank5;
			break;
		case '6':
			rank = Rank6;
			break;
		case '7':
			rank = Rank7;
			break;
		case '8':
			rank = Rank8;
			break;
		case '9':
			rank = Rank9;
			break;
		case 'T':
		case 't':
			rank = RankTen;
			break;
		case 'J':
		case 'j':
			rank = RankJack;
			break;
		case 'Q':
		case 'q':
			rank = RankQueen;
			break;
		case 'K':
		case 'k':
			rank = RankKing;
			break;
		case 'A':
		case 'a':
			rank = RankAce;
			break;
		default:
			return -2;
		}
	}
	else
	{
		return -2;
	}

	if ((size_t)index < cards.length())
	{
		switch (cards[index++])
		{
		case 'H':
		case 'h':
			suit = Hearts;
			break;
		case 'D':
		case 'd':
			suit = Diamonds;
			break;
		case 'C':
		case 'c':
			suit = Clubs;
			break;
		case 'S':
		case 's':
			suit = Spades;
			break;
		default:
			return -2;
		}
	}
	else
	{
		return -2;
	}

	return rank + (suit * 13);
}


string CHandEval::DescriptionFromMask(unsigned long long cards)
{
	int numberOfCards = BitCount(cards);


	assert((numberOfCards > 0 && numberOfCards < 8));

	// Seperate out by suit
	unsigned int sc = (unsigned int)((cards >> (CLUB_OFFSET)) & 0x1fffULL);
	unsigned int sd = (unsigned int)((cards >> (DIAMOND_OFFSET)) & 0x1fffULL);
	unsigned int sh = (unsigned int)((cards >> (HEART_OFFSET)) & 0x1fffULL);
	unsigned int ss = (unsigned int)((cards >> (SPADE_OFFSET)) & 0x1fffULL);

	unsigned int handvalue = Evaluate(cards, numberOfCards);

	switch ((HandTypes)HandType(handvalue))
	{
	case HandTypes::HighCard:
	case HandTypes::Pair:
	case HandTypes::TwoPair:
	case HandTypes::Trips:
	case HandTypes::Straight:
	case HandTypes::FullHouse:
	case HandTypes::FourOfAKind:
		return DescriptionFromHandValueInternal(handvalue);
	case HandTypes::Flush:
		if (nBitsTable[ss] >= 5)
		{
			return "Flush (Spades) with " + ranktbl[TopCard(handvalue)] + " high";
		}
		else if (nBitsTable[sc] >= 5)
		{
			return "Flush (Clubs) with " + ranktbl[TopCard(handvalue)] + " high";
		}
		else if (nBitsTable[sd] >= 5)
		{
			return "Flush (Diamonds) with " + ranktbl[TopCard(handvalue)] + " high";
		}
		else if (nBitsTable[sh] >= 5)
		{
			return "Flush (Hearts) with " + ranktbl[TopCard(handvalue)] + " high";
		}
		break;
	case HandTypes::StraightFlush:
		if (nBitsTable[ss] >= 5)
		{
			return "Straight Flush (Spades) with " + ranktbl[TopCard(handvalue)] + " high";
		}
		else if (nBitsTable[sc] >= 5)
		{
			return "Straight (Clubs) with " + ranktbl[TopCard(handvalue)] + " high";
		}
		else if (nBitsTable[sd] >= 5)
		{
			return "Straight (Diamonds) with " + ranktbl[TopCard(handvalue)] + " high";
		}
		else if (nBitsTable[sh] >= 5)
		{
			return "Straight  (Hearts) with " + ranktbl[TopCard(handvalue)] + " high";
		}
		break;
	}

	assert(false); // Should never get here
	return "";
}

string CHandEval::DescriptionFromHandValueInternal(unsigned int handValue)
{

	string b = "";

	switch ((HandTypes)HandType(handValue))
	{
	case HandTypes::HighCard:
		b.append("High card: ");
		b.append(ranktbl[TopCard(handValue)]);
		return b;
	case HandTypes::Pair:
		b.append("One pair, ");
		b.append(ranktbl[TopCard(handValue)]);
		return b;
	case HandTypes::TwoPair:
		b.append("Two pair, ");
		b.append(ranktbl[TopCard(handValue)]);
		b.append("'s and ");
		b.append(ranktbl[SECOND_CARD(handValue)]);
		b.append("'s with a ");
		b.append(ranktbl[THIRD_CARD(handValue)]);
		b.append(" for a kicker");
		return b;
	case HandTypes::Trips:
		b.append("Three of a kind, ");
		b.append(ranktbl[TopCard(handValue)]);
		b.append("'s");
		return b;
	case HandTypes::Straight:
		b.append("A straight, ");
		b.append(ranktbl[TopCard(handValue)]);
		b.append(" high");
		return b;
	case HandTypes::Flush:
		b.append("A flush");
		return b;
	case HandTypes::FullHouse:
		b.append("A fullhouse, ");
		b.append(ranktbl[TopCard(handValue)]);
		b.append("'s and ");
		b.append(ranktbl[SECOND_CARD(handValue)]);
		b.append("'s");
		return b;
	case HandTypes::FourOfAKind:
		b.append("Four of a kind, ");
		b.append(ranktbl[TopCard(handValue)]);
		b.append("'s");
		return b;
	case HandTypes::StraightFlush:
		b.append("A straight flush");
		return b;
	}
	assert(false); // Should never get here
	return "";
}

string CHandEval::MaskToString(unsigned long long mask)
{
	string builder = "";
	int count = 0;
	for (int i = 51; i >= 0; i--)
	{
		if (((1ULL << i) & mask) != 0)
		{
			if (count != 0)
			{
				builder.append(" ");
			}
			builder.append(CardTable[i]);
			count++;
		}
	}
	return builder;
}

CHandEval::HandTypes CHandEval::EvaluateType(unsigned long long mask, int cards)
{
	HandTypes is_st_or_fl = HandTypes::HighCard;

	unsigned int ss = (unsigned int)((mask >> (SPADE_OFFSET)) & 0x1fffULL);
	unsigned int sc = (unsigned int)((mask >> (CLUB_OFFSET)) & 0x1fffULL);
	unsigned int sd = (unsigned int)((mask >> (DIAMOND_OFFSET)) & 0x1fffULL);
	unsigned int sh = (unsigned int)((mask >> (HEART_OFFSET)) & 0x1fffULL);

	unsigned int ranks = sc | sd | sh | ss;
	unsigned int rankinfo = nBitsAndStrTable[ranks];
	unsigned int n_dups = (unsigned int)(cards - (rankinfo >> 2));

	if ((rankinfo & 0x01U) != 0U)
	{
		if ((rankinfo & 0x02) != 0U)
			is_st_or_fl = HandTypes::Straight;

		unsigned int t = (unsigned int)(nBitsAndStrTable[ss] | nBitsAndStrTable[sc] | nBitsAndStrTable[sd] | nBitsAndStrTable[sh]);

		if ((t & 0x01u) != 0U)
		{
			if ((t & 0x02u) != 0U)
				return HandTypes::StraightFlush;
			else
				is_st_or_fl = HandTypes::Flush;
		}

		if (is_st_or_fl != 0 && n_dups < 3)
			return is_st_or_fl;
	}

	switch (n_dups)
	{
	case 0:
		return HandTypes::HighCard;
	case 1:
		return HandTypes::Pair;
	case 2:
		return ((ranks ^ (sc ^ sd ^ sh ^ ss)) != 0) ? HandTypes::TwoPair : HandTypes::Trips;
	default:
		if (((sc & sd) & (sh & ss)) != 0) return HandTypes::FourOfAKind;
		else if ((((sc & sd) | (sh & ss)) & ((sc & sh) | (sd & ss))) != 0) return HandTypes::FullHouse;
		else if (is_st_or_fl != 0) return is_st_or_fl;
		else return HandTypes::TwoPair;
	}
}

unsigned int CHandEval::Evaluate(unsigned long long cards, int numberOfCards)
{
	unsigned int retval = 0, four_mask, three_mask, two_mask;

	// This functions supports 1-7 cards
	assert(numberOfCards > 0 && numberOfCards <= 8);

	// Seperate out by suit
	unsigned int sc = (unsigned int)((cards >> (CLUB_OFFSET)) & 0x1fffULL);
	unsigned int sd = (unsigned int)((cards >> (DIAMOND_OFFSET)) & 0x1fffULL);
	unsigned int sh = (unsigned int)((cards >> (HEART_OFFSET)) & 0x1fffULL);
	unsigned int ss = (unsigned int)((cards >> (SPADE_OFFSET)) & 0x1fffULL);

	unsigned int ranks = sc | sd | sh | ss;
	unsigned int n_ranks = nBitsTable[ranks];
	unsigned int n_dups = ((unsigned int)(numberOfCards - n_ranks));

	/* Check for straight, flush, or straight flush, and return if we can
	determine immediately that this is the best possible hand
	*/
	if (n_ranks >= 5)
	{
		if (nBitsTable[ss] >= 5)
		{
			if (straightTable[ss] != 0)
				return HANDTYPE_VALUE_STRAIGHTFLUSH + (unsigned int)(straightTable[ss] << TOP_CARD_SHIFT);
			else
				retval = HANDTYPE_VALUE_FLUSH + topFiveCardsTable[ss];
		}
		else if (nBitsTable[sc] >= 5)
		{
			if (straightTable[sc] != 0)
				return HANDTYPE_VALUE_STRAIGHTFLUSH + (unsigned int)(straightTable[sc] << TOP_CARD_SHIFT);
			else
				retval = HANDTYPE_VALUE_FLUSH + topFiveCardsTable[sc];
		}
		else if (nBitsTable[sd] >= 5)
		{
			if (straightTable[sd] != 0)
				return HANDTYPE_VALUE_STRAIGHTFLUSH + (unsigned int)(straightTable[sd] << TOP_CARD_SHIFT);
			else
				retval = HANDTYPE_VALUE_FLUSH + topFiveCardsTable[sd];
		}
		else if (nBitsTable[sh] >= 5)
		{
			if (straightTable[sh] != 0)
				return HANDTYPE_VALUE_STRAIGHTFLUSH + (unsigned int)(straightTable[sh] << TOP_CARD_SHIFT);
			else
				retval = HANDTYPE_VALUE_FLUSH + topFiveCardsTable[sh];
		}
		else
		{
			unsigned int st = straightTable[ranks];
			if (st != 0)
				retval = HANDTYPE_VALUE_STRAIGHT + (st << TOP_CARD_SHIFT);
		};

		/*
		Another win -- if there can't be a FH/Quads (n_dups < 3),
		which is true most of the time when there is a made hand, then if we've
		found a five card hand, just return.  This skips the whole process of
		computing two_mask/three_mask/etc.
		*/
		if (retval != 0 && n_dups < 3)
			return retval;
	}

	/*
	* By the time we're here, either:
	1) there's no five-card hand possible (flush or straight), or
	2) there's a flush or straight, but we know that there are enough
	duplicates to make a full house / quads possible.
	*/
	switch (n_dups)
	{
	case 0:
		/* It's a no-pair hand */
		return HANDTYPE_VALUE_HIGHCARD + topFiveCardsTable[ranks];
	case 1:
	{
		/* It's a one-pair hand */
		unsigned int t, kickers;

		two_mask = ranks ^ (sc ^ sd ^ sh ^ ss);

		retval = (unsigned int)(HANDTYPE_VALUE_PAIR + (topCardTable[two_mask] << TOP_CARD_SHIFT));
		t = ranks ^ two_mask;      /* Only one bit set in two_mask */
								   /* Get the top five cards in what is left, drop all but the top three
								   * cards, and shift them by one to get the three desired kickers */
		kickers = (topFiveCardsTable[t] >> CARD_WIDTH) & ~FIFTH_CARD_MASK;
		retval += kickers;
		return retval;
	}

	case 2:
		/* Either two pair or trips */
		two_mask = ranks ^ (sc ^ sd ^ sh ^ ss);
		if (two_mask != 0)
		{
			unsigned int t = ranks ^ two_mask; /* Exactly two bits set in two_mask */
			retval = (unsigned int)(HANDTYPE_VALUE_TWOPAIR
				+ (topFiveCardsTable[two_mask]
					& (TOP_CARD_MASK | SECOND_CARD_MASK))
				+ (topCardTable[t] << THIRD_CARD_SHIFT));

			return retval;
		}
		else
		{
			unsigned int t, second;
			three_mask = ((sc & sd) | (sh & ss)) & ((sc & sh) | (sd & ss));
			retval = (unsigned int)(HANDTYPE_VALUE_TRIPS + (topCardTable[three_mask] << TOP_CARD_SHIFT));
			t = ranks ^ three_mask; /* Only one bit set in three_mask */
			second = topCardTable[t];
			retval += (second << SECOND_CARD_SHIFT);
			t ^= (1U << (int)second);
			retval += (unsigned int)(topCardTable[t] << THIRD_CARD_SHIFT);
			return retval;
		}

	default:
		/* Possible quads, fullhouse, straight or flush, or two pair */
		four_mask = sh & sd & sc & ss;
		if (four_mask != 0)
		{
			unsigned int tc = topCardTable[four_mask];
			retval = (unsigned int)(HANDTYPE_VALUE_FOUR_OF_A_KIND
				+ (tc << TOP_CARD_SHIFT)
				+ ((topCardTable[ranks ^ (1U << (int)tc)]) << SECOND_CARD_SHIFT));
			return retval;
		};

		/* Technically, three_mask as defined below is really the set of
		bits which are set in three or four of the suits, but since
		we've already eliminated quads, this is OK */
		/* Similarly, two_mask is really two_or_four_mask, but since we've
		already eliminated quads, we can use this shortcut */

		two_mask = ranks ^ (sc ^ sd ^ sh ^ ss);
		if (nBitsTable[two_mask] != n_dups)
		{
			/* Must be some trips then, which really means there is a
			full house since n_dups >= 3 */
			unsigned int tc, t;
			three_mask = ((sc & sd) | (sh & ss)) & ((sc & sh) | (sd & ss));
			retval = HANDTYPE_VALUE_FULLHOUSE;
			tc = topCardTable[three_mask];
			retval += (tc << TOP_CARD_SHIFT);
			t = (two_mask | three_mask) ^ (1U << (int)tc);
			retval += (unsigned int)(topCardTable[t] << SECOND_CARD_SHIFT);
			return retval;
		};

		if (retval != 0) /* flush and straight */
			return retval;
		else
		{
			/* Must be two pair */
			unsigned int top, second;

			retval = HANDTYPE_VALUE_TWOPAIR;
			top = topCardTable[two_mask];
			retval += (top << TOP_CARD_SHIFT);
			second = topCardTable[two_mask ^ (1 << (int)top)];
			retval += (second << SECOND_CARD_SHIFT);
			retval += (unsigned int)((topCardTable[ranks ^ (1U << (int)top) ^ (1 << (int)second)]) << THIRD_CARD_SHIFT);
			return retval;
		}
	}
}

void CHandEval::UpdateHandMask()
{
	int cards = 0;
	handmask = this->ParseHand(pocket, board, cards);
	handval = this->Evaluate(handmask, cards);
}

int CHandEval::BitCount(unsigned long long bitField)
{
	return
		bits[(int)(bitField & 0x00000000000000FFULL)] +
		bits[(int)((bitField & 0x000000000000FF00ULL) >> 8)] +
		bits[(int)((bitField & 0x0000000000FF0000ULL) >> 16)] +
		bits[(int)((bitField & 0x00000000FF000000ULL) >> 24)] +
		bits[(int)((bitField & 0x000000FF00000000ULL) >> 32)] +
		bits[(int)((bitField & 0x0000FF0000000000ULL) >> 40)] +
		bits[(int)((bitField & 0x00FF000000000000ULL) >> 48)] +
		bits[(int)((bitField & 0xFF00000000000000ULL) >> 56)];
}

