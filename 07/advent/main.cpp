// Open and read input file.
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <array>
#include <algorithm>


// DIFFICULTY
static constexpr int PUZZLE_PART = 2;

// QOL
enum HandType
{
	HIGH_CARD = 0,
	ONE_PAIR,
	TWO_PAIR,
	THREE_OF_A_KIND,
	FULL_HOUSE,
	FOUR_OF_A_KIND,
	FIVE_OF_A_KIND,
};


struct Hand
{
	std::array<int, 5> hand{ 0, 0, 0, 0, 0 };
	int strength = 0;
	int rank = 0;
	size_t bid = 0;

	int& operator[](int index)
	{
		return hand[index];
	}

	const int& operator[](int index) const
	{
		return hand[index];
	}	
};


struct lowest_strength_key
{
	bool operator()(const Hand& lh, const Hand& rh) const
	{
		return lh.strength < rh.strength;
	}
};


struct lowest_card_key
{
	bool operator()(const Hand& lh, const Hand& rh) const
	{
		for (int card = 0; card < 5; ++card)
		{
			if (lh[card] < rh[card])
			{
				return true;
			}
			else if (lh[card] > rh[card])
			{
				return false;
			}
		}

		return false;
	}
};



// EVERGREEN FUNC
const int parseInputFile(std::vector<std::string>& list);
const size_t convertCharToInt(std::string intAsChars);
const int ProcessEasyResults();
const int ProcessHardResults();

// PUZZLE FUNCS
void getHands(std::vector<std::string>& list, std::vector<Hand>& hands, bool useJoker);

void setStrength(std::vector<Hand>& hands);
void setStrengthWithJoker(std::vector<Hand>& hands);
const int getHandStrength(Hand& singleHand);

void sortByStrength(std::vector<Hand>& hands);
void sortByHighCard(std::vector<Hand>& hands);

//std::string GetNextNumber(int& pos, std::string& line);
//void GetWinningCombos(std::vector<event>&  races, std::vector<size_t>& winningCombos);
//const size_t GetMarginOfError(std::vector<size_t>& winningCombos);




int main()
{
	// PARSE INPUT FILE //

	std::vector<std::string> list;

	if (parseInputFile(list) == 1)
	{
		return 1;
	}
		
	size_t answer = 0;

	// SOLVE PUZZLE //

	std::vector<Hand> hands;

	if (PUZZLE_PART == 1)
	{
		getHands(list, hands, false);
		setStrength(hands);
		sortByStrength(hands);
		sortByHighCard(hands);
		
		for (int h = 0; h < hands.size(); ++h)
		{
			answer += hands[h].bid * (h + 1);
		}
	}
	else
	{
		getHands(list, hands, true);
		setStrengthWithJoker(hands);
		sortByStrength(hands);
		sortByHighCard(hands);

		for (int h = 0; h < hands.size(); ++h)
		{
			answer += hands[h].bid * (h + 1);
		}
	}

	std::cout << "The answer to part " << PUZZLE_PART << " is: " << answer << std::endl;

	return 0;
}


// Get and store input as lines of strings.
const int parseInputFile(std::vector<std::string>& list)
{
	std::ifstream inf{ "../input.txt" };

	if (!inf)
	{
		std::cerr << "Couldn't find file.\n";
		return 1;
	}

	std::string strInput;

	while (inf)
	{
		std::getline(inf, strInput);
		list.push_back(strInput);
	}

	// The while-loop always pulls a "blank" line at the end. Remove it here.
	list.pop_back();

	inf.close();

	return 0;
}


// Convert an array of char's into an int of the expected value ("563" = 563).
const size_t convertCharToInt(std::string intAsChars)
{
	size_t value{ 0 };

	for (size_t index{ 0 }, exponent{ intAsChars.length() - 1 }; index < intAsChars.length(); ++index, --exponent)
	{
		int num{ intAsChars[index] - '0' };

		value += static_cast<size_t>(num * pow(10, exponent));
	}

	return value;
}


const int ProcessEasyResults()
{
	return 0;
}


const int ProcessHardResults()
{
	return 0;
}


void getHands(std::vector<std::string>& list, std::vector<Hand>& hands, bool useJoker)
{
	hands.reserve(list.size());

	for (int play = 0; play < list.size(); ++play)
	{
		std::string& line = list[play];

		Hand hand;
		size_t bid = 0;

		// Find hand.
		{
			for (int cardIndex = 0; cardIndex < 5; ++cardIndex)
			{
				int cardValue = 0;

				// Convert face cards to value.			
				switch (line[cardIndex])
				{
				case 'T':
					cardValue = 10;
					break;
				case 'J':
					cardValue = useJoker ? 1 : 11;
					break;
				case 'Q':
					cardValue = 12;
					break;
				case 'K':
					cardValue = 13;
					break;
				case 'A':
					cardValue = 14;
					break;
				default:
					cardValue = convertCharToInt(std::string() + line[cardIndex]);
					break;
				}

				hand[cardIndex] = cardValue;
			}
		}

		// Find bid.
		{
			int start = line.find(' ') + 1;
			int length = line.length() - start;
						
			hand.bid = convertCharToInt(line.substr(start, length));
		}

		hands.push_back(hand);

	}// Next line/hand.
}


void setStrength(std::vector<Hand>& hands)
{
	for (int h = 0; h < hands.size(); ++h)
	{
		Hand hand = hands[h];

		std::sort(hand.hand.begin(), hand.hand.end(), std::greater<int>());

		hands[h].strength = getHandStrength(hand);
	}
}


void setStrengthWithJoker(std::vector<Hand>& hands)
{
	for (int h = 0; h < hands.size(); ++h)
	{
		Hand hand = hands[h];

		std::sort(hand.hand.begin(), hand.hand.end(), std::greater<int>());

		// Check for joker - will be at end if there is at least 1.
		if (hand[4] == 1)
		{
			// Find start of joker set.
			int start = 4;
			for (int card = 0; card < hand.hand.size(); ++card)
			{
				if (hand[card] == 1)
				{
					start = card;
					break;
				}
			}

			// If all cards are jokers, run normal (should be 5 of a kind).
			if (start == 0)
			{
				hands[h].strength = getHandStrength(hand);
				continue;
			}

			// Find best strength;
			int bestStrength = 0;
			for (int card = 0; card < start; ++card)
			{
				// Set jokers to equal card value, starting with first card and changing each loop.
				for (int joker = start; joker < hand.hand.size(); ++joker)
				{
					hand[joker] = hand[card];
				}

				// Find strength of test hand (must sort first - use copy not reference).
				Hand jokerHand = hand;
				std::sort(jokerHand.hand.begin(), jokerHand.hand.end(), std::greater<int>());
				int strength = getHandStrength(jokerHand);

				// Keep strength if it is the best.
				if (strength > bestStrength)
				{
					bestStrength = strength;
				}
			}

			hands[h].strength = bestStrength;
		}
		// No joker in hand. Normal check.
		else
		{
			hands[h].strength = getHandStrength(hand);
		}
	}
}


const int getHandStrength(Hand& hand)
{
	std::vector<int> matchCounts;

	// Find number of matches.
	{
		matchCounts.clear();

		int pos = 0;

		int handSize = hand.hand.size();

		while (pos < handSize)
		{
			int count = 0;
			int cardToMatch = hand[pos];

			for (; pos < handSize; ++pos)
			{
				if (hand[pos] == cardToMatch)
				{
					++count; // Always 1 since it will match itself.
				}
				else
				{
					break;
				}
			}

			if (count > 1)
			{
				matchCounts.push_back(count);
			}
		} // Next while loop.
	} // End match finding.


	// Find strength.
	{
		// If no matches, don't sort, don't look for types.
		if (matchCounts.size() == 0)
		{
			return 0;
		}
		// Sort types.
		else if (matchCounts.size() > 1)
		{
			std::sort(matchCounts.begin(), matchCounts.end());
		}

		// matchCounts should have at most 2 values (since high card matches are not saved and can only have 2 ONE_PAIR (smallest type) max).

		if (matchCounts[0] == 5)
		{
			return FIVE_OF_A_KIND;
		}
		else if (matchCounts[0] == 4)
		{
			return FOUR_OF_A_KIND;
		}
		else if (matchCounts[0] == 3)
		{
			if (matchCounts.size() == 2 && matchCounts[1] == 2)
			{
				return FULL_HOUSE;
			}
			else
			{
				return THREE_OF_A_KIND;
			}
		}
		else if (matchCounts[0] == 2)
		{
			if (matchCounts.size() == 2 && matchCounts[1] == 3)
			{
				return FULL_HOUSE;
			}
			else if (matchCounts.size() == 2 && matchCounts[1] == 2)
			{
				return TWO_PAIR;
			}
			else
			{
				return ONE_PAIR;
			}
		}
	} // End strength/type search.
}


void sortByStrength(std::vector<Hand>& hands)
{
	std::sort(hands.begin(), hands.end(), lowest_strength_key());
}


void sortByHighCard(std::vector<Hand>& hands)
{
	std::vector<Hand>::iterator startIT = hands.begin();
	std::vector<Hand>::iterator endIT = startIT;
		
	for (int strength = HIGH_CARD; strength < FIVE_OF_A_KIND; ++strength)
	{
		for (; endIT != hands.end(); ++endIT)
		{
			if ((*endIT).strength != strength)
			{
				std::sort(startIT, endIT, lowest_card_key());
				startIT = endIT;
				break;
			}
		}
	}

	// Sort the highest strength;
	std::sort(startIT, hands.end(), lowest_card_key());
}


/*
void GetRaceEvent(std::vector<std::string>& list, std::vector<event>& races)
{
	// Find the first line and create the events.
	{
		std::string& line = list[0];

		int pos = line.find(':');
		std::string strNumber = "";

		while (pos < line.length())
		{
			strNumber.append(GetNextNumber(pos, line));
		}

		if (strNumber.length() > 0)
		{
			races.push_back({ ConvertCharToInt(strNumber), 0 });
		}
	}

	// Find the secondline and add to the events.
	{
		int eventId = 0;

		std::string& line = list[1];

		int pos = line.find(':');
		std::string strNumber = "";

		while (pos < line.length())
		{
			strNumber.append(GetNextNumber(pos, line));
		}

		if (strNumber.length() > 0)
		{
			races[eventId++].second = ConvertCharToInt(strNumber);
		}
	}
}


void GetWinningCombos(std::vector<event>& races, std::vector<size_t>& winningCombos)
{
	for (int e = 0; e < races.size(); ++e)
	{	
		size_t shortWinTime = 0;
		size_t longWinTime = 0;

		size_t totalTime = races[e].first;
		size_t bestDistance = races[e].second;

		// Short win.
		for (size_t timePressed = 0; timePressed < totalTime; ++timePressed)
		{
			size_t timeLeft = totalTime - timePressed;

			size_t myDistance = timePressed * timeLeft; // timePressed = speed

			if (myDistance > bestDistance)
			{
				shortWinTime = timePressed;
				break;
			}
		}

		if (shortWinTime > 0)
		{
			// Long win.
			for (size_t timePressed = totalTime; timePressed >= shortWinTime; --timePressed)
			{
				size_t timeLeft = totalTime - timePressed;

				size_t myDistance = timePressed * timeLeft; // timePressed = speed

				if (myDistance > bestDistance)
				{
					longWinTime = timePressed;
					break;
				}
			}
		}

		if (shortWinTime > 0)
		{
			winningCombos.push_back(longWinTime - shortWinTime + 1);
		}
		else
		{
			winningCombos.push_back(0);
		}
	}
}


const size_t GetMarginOfError(std::vector<size_t>& winningCombos)
{
	size_t moe = 1;

	for (size_t& wins : winningCombos)
	{
		moe *= wins;
	}

	return moe;
}

*/

