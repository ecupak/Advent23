// Open and read input file.
#include <iostream>
#include <fstream>
#include <string>
#include <vector>


const int parseInputFile(std::vector<std::string>& list);


void GetWinningNumbers(std::string& game, std::vector<int>& winningNumbers);
void GetRevealedNumbers(std::string& game, std::vector<int>& revealedNumbers);
const int GetScore(std::vector<int>& winningNumbers, std::vector<int>& revealedNumbers);
const int GetMatches(std::vector<int>& winningNumbers, std::vector<int>& revealedNumbers);

const int CheckAroundSymbol(std::vector<std::string>& list, const int xTarget, const int yTarget);
const int GetNumber(std::string& line, const int xTarget, const int yTarget, const int xCheck, const int yCheck);
const int GetDigitStartPosition(int positionToCheck, const std::string& line);
const int GetDigitEndPosition(int positionToCheck, const std::string& line);
const int ConvertCharToInt(std::string intAsChars);

const int CheckAroundGear(std::vector<std::string>& list, const int xTarget, const int yTarget);

const int ProcessEasyResults();
const int ProcessHardResults();


static constexpr int PUZZLE_PART = 2;


int main()
{
	// PARSE INPUT FILE //

	std::vector<std::string> list;

	if (parseInputFile(list) == 1)
	{
		return 1;
	}
		
	// SOLVE PUZZLE //

	int totalScore = 0;
	std::vector<int> winningNumbers;
	std::vector<int> revealedNumbers;

	// Part 2
	int* totalCards = new int[list.size()];
	for (int card = 0; card < list.size(); ++card)
	{
		totalCards[card] = 1;
	}

	for (int card = 0; card < list.size(); ++card)
	{
		std::string game = list[card];

		if (PUZZLE_PART == 1)
		{
			// Get winning numbers.
			winningNumbers.clear();
			GetWinningNumbers(game, winningNumbers);

			// Get your numbers.
			revealedNumbers.clear();
			GetRevealedNumbers(game, revealedNumbers);

			// Get score.
			totalScore += GetScore(winningNumbers, revealedNumbers);
		}
		else
		{
			// Get winning and revealed numbers.
			winningNumbers.clear();
			GetWinningNumbers(game, winningNumbers);

			revealedNumbers.clear();
			GetRevealedNumbers(game, revealedNumbers);

			// Get number of matches.
			int matches = GetMatches(winningNumbers, revealedNumbers);

			// For each match, increase the subsequent card count by the number of copies of the current card.
			for (int m = 0; m < matches; ++m)
			{
				totalCards[card + m + 1] += totalCards[card];
			}
		}
	}

	if (PUZZLE_PART == 1)
	{
		std::cout << "The answer to part " << PUZZLE_PART << " is: " << totalScore << std::endl;
	}
	else
	{
		int totalCardCount = 0;
		for (int card = 0; card < list.size(); ++card)
		{
			totalCardCount += totalCards[card];
		}

		std::cout << "The answer to part " << PUZZLE_PART << " is: " << totalCardCount << std::endl;
	}
	
	delete[] totalCards;

	return 0;
}


void GetWinningNumbers(std::string& game, std::vector<int>& winningNumbers)
{
	// Find where winning numbers start and end.
	int start = game.find(":") + 1;

	std::string winningSet = game.substr(start);

	int end = winningSet.find("|");


	int it = 0;
	char number[10];
	int numIt = 0;
	for (int it = 0; it < end; ++it)
	{
		// Find start of digit.
		if (std::isdigit(winningSet[it]))
		{
			number[numIt] = winningSet[it];
			++numIt;
		}
		else
		{
			// If we found and added any numbers to number, add them to list.
			if (numIt > 0)
			{
				// Null terminate the char array.
				number[numIt] = '\0';

				// Get and store number.
				winningNumbers.push_back(ConvertCharToInt(number));
			}

			numIt = 0;
		}
	}
}


void GetRevealedNumbers(std::string& game, std::vector<int>& revealedNumbers)
{
	// Find where winning numbers start and end.
	int start = game.find("|") + 1;

	std::string revealedSet = game.substr(start) + " ";

	int end = revealedSet.length();


	int it = 0;
	char number[10];
	int numIt = 0;
	for (int it = 0; it < end; ++it)
	{
		// Find start of digit.
		if (std::isdigit(revealedSet[it]))
		{
			number[numIt] = revealedSet[it];
			++numIt;
		}
		else
		{
			// If we found and added any numbers to number, add them to list.
			if (numIt > 0)
			{
				// Null terminate the char array.
				number[numIt] = '\0';

				// Get and store number.
				revealedNumbers.push_back(ConvertCharToInt(number));
			}

			numIt = 0;
		}
	}
}


const int GetScore(std::vector<int>& winningNumbers, std::vector<int>& revealedNumbers)
{
	int matches = 0;
	int score = 0;
	std::vector<int> checkedWinningNumbers;
	bool skip = false;

	for (int winningNumber : winningNumbers)
	{
		skip = false;

		// If a winning number is repeated, skip the repeats.
		for (int checkedWinningNumber : checkedWinningNumbers)
		{
			if (winningNumber == checkedWinningNumber)
			{
				skip = true;
				break;
			}
		}

		if (skip)
		{
			continue;
		}

		for (int revealedNumber : revealedNumbers)
		{
			if (winningNumber == revealedNumber)
			{
				++matches;
				break;
			}
		}
	}

	return matches > 0 ? 1 << (matches - 1) : 0;
}


const int GetMatches(std::vector<int>& winningNumbers, std::vector<int>& revealedNumbers)
{
	int matches = 0;
	int score = 0;
	std::vector<int> checkedWinningNumbers;
	bool skip = false;

	for (int winningNumber : winningNumbers)
	{
		skip = false;

		// If a winning number is repeated, skip the repeats.
		for (int checkedWinningNumber : checkedWinningNumbers)
		{
			if (winningNumber == checkedWinningNumber)
			{
				skip = true;
				break;
			}
		}

		if (skip)
		{
			continue;
		}

		for (int revealedNumber : revealedNumbers)
		{
			if (winningNumber == revealedNumber)
			{
				++matches;
				break;
			}
		}
	}

	return matches;
}


const int CheckAroundSymbol(std::vector<std::string>& list, const int xTarget, const int yTarget)
{
	int partialPartSums = 0;

	// Setup start and ends for x and y to avoid checking out of bounds.
	int yStart = yTarget == 0 ? 1 : 0;
	int yEnd = yTarget == list.size() - 1 ? 2 : 3;
	int xStart = xTarget == 0 ? 1 : 0;
	int xEnd = xTarget == list[yTarget].length() - 1 ? 2 : 3;

	for (int yCheck = yStart; yCheck < yEnd; ++yCheck) // Check row by row,
	{
		for (int xCheck = xStart; xCheck < xEnd; ++xCheck)	// and character by character.
		{
			// Shift x and y to be relative to target.
			int xRelative = xCheck - 1;
			int yRelative = yCheck - 1;

			// Get char from list relative to symbol.
			char listChar = list[yTarget + yRelative][xTarget + xRelative];

			// If it is a digit, get full value.
			if (std::isdigit(listChar))
			{				
				partialPartSums += GetNumber(list[yTarget + yRelative], xTarget, yTarget, xRelative, yRelative);
			}
		}
	}

	return partialPartSums;
}


const int CheckAroundGear(std::vector<std::string>& list, const int xTarget, const int yTarget)
{
	std::vector<int> parts;

	// Setup start and ends for x and y to avoid checking out of bounds.
	int yStart = yTarget == 0 ? 1 : 0;
	int yEnd = yTarget == list.size() - 1 ? 2 : 3;
	int xStart = xTarget == 0 ? 1 : 0;
	int xEnd = xTarget == list[yTarget].length() - 1 ? 2 : 3;

	for (int yCheck = yStart; yCheck < yEnd; ++yCheck) // Check row by row,
	{
		for (int xCheck = xStart; xCheck < xEnd; ++xCheck)	// and character by character.
		{
			// Shift x and y to be relative to target.
			int xRelative = xCheck - 1;
			int yRelative = yCheck - 1;

			// Get char from list relative to symbol.
			char listChar = list[yTarget + yRelative][xTarget + xRelative];

			// If it is a digit, get full value.
			if (std::isdigit(listChar))
			{
				parts.push_back(GetNumber(list[yTarget + yRelative], xTarget, yTarget, xRelative, yRelative));
			}
		}
	}

	// Return the parts multiplied (gear ratio) is exactly 2 were found. Otherwise return nothing.
	if (parts.size() == 2)
	{
		return parts[0] * parts[1];
	}
	else
	{
		return 0;
	}
}


const int GetNumber(std::string& line, const int xTarget, const int yTarget, const int xRelative, const int yRelative)
{
	// Check left and right of digit to find full number it is part of.
	int initialPosition = xTarget + xRelative;

	int digitStartPosition = GetDigitStartPosition(initialPosition, line);
	int digitEndPosition = GetDigitEndPosition(initialPosition, line);

	int digitLength = (digitEndPosition - digitStartPosition) + 1;

	// Extract full number.
	std::string partNumber = line.substr(digitStartPosition, digitLength);

	// Replace number with periods so subsequent checks do not copy same number.
	line.replace(digitStartPosition, digitLength, digitLength, '.');

	return ConvertCharToInt(partNumber);
}


// Recursive method. Keeps checking to the left until it cannot find anymore digits.
const int GetDigitStartPosition(int positionToCheck, const std::string& line)
{
	if (positionToCheck >= 0 && std::isdigit(line[positionToCheck]))
	{
		return GetDigitStartPosition(positionToCheck - 1, line);
	}
	else
	{
		// Previous position was last one with a digit.
		return positionToCheck + 1;
	}
}


// Recursive method. Keeps checking to the right until it cannot find anymore digits.
const int GetDigitEndPosition(int positionToCheck, const std::string& line)
{
	if (positionToCheck < line.length() && std::isdigit(line[positionToCheck]))
	{
		return GetDigitEndPosition(positionToCheck + 1, line);
	}
	else
	{
		// Previous position was last one with a digit.
		return positionToCheck - 1;
	}
}


// Convert an array of char's into an int of the expected value ("563" = 563).
const int ConvertCharToInt(std::string intAsChars)
{
	int value{ 0 };

	for (size_t index{ 0 }, exponent{ intAsChars.length() - 1}; index < intAsChars.length(); ++index, --exponent)
	{
		int num{ intAsChars[index] - '0' };

		value += static_cast<int>(num * pow(10, exponent));
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