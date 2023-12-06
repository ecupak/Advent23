// Open and read input file.
#include <iostream>
#include <fstream>
#include <string>
#include <vector>


const int parseInputFile(std::vector<std::string>& list);

std::string ReplaceWrittenNumbers(const std::string& line);


static constexpr int PUZZLE_PART = 1;

int main()
{
	// PARSE INPUT FILE //

	std::vector<std::string> list;

	if (parseInputFile(list) == 1)
	{
		return 1;
	}
		
	// SOLVE PUZZLE //

	std::vector<int> values;
	
	for (auto& line : list)
	{
		int places[2]{ 0, 0 };
		int index = 0;

		int foundNum = 0;
		bool isFirstNum = true;

		if (line.size() == 0)
		{
			continue;
		}
		
		// Switch based on first or second part of puzzle.
		std::string puzzleString;

		if (PUZZLE_PART == 1)
		{
			puzzleString = line;
		}
		else
		{
			puzzleString = ReplaceWrittenNumbers(line);
		}

		// Continue solving.
		for (int i = 0; i < puzzleString.size(); ++i)
		{
			if (std::isdigit(puzzleString[i]))
			{
				foundNum = puzzleString[i] - '0';

				if (isFirstNum)
				{
					places[index] = foundNum;
					++index;

					isFirstNum = false;
				}
			}
		}

		places[index] = foundNum;

		// 0 is the tens place, 1 is the ones place.
		values.push_back(places[0] * 10 + places[1]);
	}

	int totalValue = 0;
	for (int i = 0; i < values.size(); ++i)
	{
		totalValue += values[i];
	}

	std::cout << "The answer to part " << PUZZLE_PART << " is: " << totalValue << std::endl;
	
	return 0;
}


std::string ReplaceWrittenNumbers(const std::string& line)
{
	const char* numbers[9]
	{
		"one",
		"two",
		"three",
		"four",
		"five",
		"six",
		"seven",
		"eight",
		"nine"
	};

	std::string justDigits{ "" };

	// Check character-by-character.
	for (int i = 0; i < line.size(); ++i)
	{
		// Find actual digit.
		if (std::isdigit(line[i]))
		{
			justDigits.push_back(line[i]);
			continue;
		}

		// Find written number.
		for (int n = 0; n < 9; ++n)
		{
			size_t pos = line.find(numbers[n], i);

			if (pos == i)
			{
				justDigits.push_back('1' + n);
				break;
			}
		}
	}

	return justDigits;
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
	inf.close();

	return 0;
}