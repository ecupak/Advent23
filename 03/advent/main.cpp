// Open and read input file.
#include <iostream>
#include <fstream>
#include <string>
#include <vector>


const int parseInputFile(std::vector<std::string>& list);


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

	int totalPartSum = 0;

	for (int y = 0; y < list.size(); ++y)	// Check row by row,
	{
		for (int x = 0; x < list[y].length(); ++x)	// and character by character.
		{
			char focus = list[y][x];

			// Skip if current focus is a digit or period.
			if (std::isdigit(focus) || focus == '.')
			{
				continue;
			}

			if (PUZZLE_PART == 1)
			{
				// Otherwise it is a symbol. Get the total values around it.
				totalPartSum += CheckAroundSymbol(list, x, y);
			}
			else
			{
				// We only care about gears (*).
				if (focus == '*')
				{
					totalPartSum += CheckAroundGear(list, x, y);
				}
			}
		}
	}

	std::cout << "The answer to part " << PUZZLE_PART << " is: " << totalPartSum << std::endl;
	
	return 0;
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