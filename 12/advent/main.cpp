// Open and read input file.
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iterator>

#include <bitset>


// DIFFICULTY
static constexpr int PUZZLE_PART = 1;


// EVERGREEN FUNCS //

// Get and store input as lines of strings.
const int parseInputFile(std::string& filename, std::vector<std::string>& list)
{
	std::ifstream inf{ "../" + filename + ".txt" };
	//std::ifstream inf{ "../fake.txt" };

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
const int64_t convertCharToInt(std::string intAsChars)
{
	int64_t value{ 0 };

	bool isNegative = intAsChars[0] == '-';

	if (isNegative)
	{
		intAsChars = intAsChars.substr(1);
	}

	for (int64_t index{ 0 }, exponent{ static_cast<int64_t>(intAsChars.length() - 1) }; index < intAsChars.length(); ++index, --exponent)
	{
		int64_t num{ intAsChars[index] - '0' };

		value += static_cast<int64_t>(num * pow(10, exponent));
	}

	value *= isNegative ? -1 : 1;
	
	return value;
}


// PUZZLE FUNCS //

std::vector<size_t> bars;
std::vector<size_t> zeroes;

const bool advanceBar(size_t barToMove, size_t downstreamBar)
{
	if (barToMove == 0)
	{
		return false;
	}

	// Only the last real bar is allowed to be equal to another bar (the end bar).
	bool causeCascade = false;
	if (barToMove == bars.size() - 2)
	{
		causeCascade = bars[barToMove] + 1 > bars[downstreamBar];
	}
	else
	{
		causeCascade = bars[barToMove] + 1 >= bars[downstreamBar];
	}

	// Make upstream bar advance downstream 1 step.
	if (causeCascade)
	{
		size_t upstreamBar = barToMove - 1;

		// At end of movement area. Try to advance the upstream bar.
		if (advanceBar(upstreamBar, barToMove))
		{
			// Successful. Set to be 1 after upstream bar.
			bars[barToMove] = bars[upstreamBar] + 1;
		}
		else
		{
			// Failed. Most upstream bar (1st real bar) has reached end of movement area. All permutations checked.
			return false;
		}
	}
	else
	{
		++bars[barToMove];
	}

	return true;
}




auto solvePuzzle1()
{ 

}


auto solvePuzzle2()
{

}


// MAIN //

int main()
{
	// PARSE INPUT FILE //

	std::string filename = "fake";
	std::vector<std::string> list;

	if (parseInputFile(filename, list) == 1)
	{
		return 1;
	}

	// SOLVE PUZZLE //

	auto answer = 0;

	for (int i = 0; i < list.size(); ++i)
	{
		std::string& line = list[i];

		// Get grouping info.
		std::vector<int> groupings;
		{		
			int pos = line.find(' ') + 1;

			while (true)
			{
				int comma = line.find(',', pos);

				if (comma == std::string::npos)
				{
					groupings.push_back(convertCharToInt(line.substr(pos)));
					break;
				}
				else
				{
					groupings.push_back(convertCharToInt(line.substr(pos, (comma - pos))));
					pos = comma + 1;
				}
			}
		}


		// Isolate puzzle.
		line = line.substr(0, line.find(' '));
		
		if (PUZZLE_PART == 2)
		{
			std::string originalLine = line;
			int originalGroupingSize = groupings.size();

			for (int i = 0; i < 4; ++i)
			{
				for (int g = 0; g < originalGroupingSize; ++g)
				{
					groupings.push_back(groupings[g]);
				}

				line.append("?" + originalLine);
			}
		}

		// Confirm it matches the mandatory/given values.
		size_t brokeMask = 0;
		size_t goodMask = 0;

		for (int l = 0; l < line.length(); ++l)
		{
			if (line[l] == '#')
			{
				int shift = line.length() - 1 - l;
				brokeMask |= ((size_t)1 << shift);
			}

			if (line[l] == '.')
			{
				int shift = line.length() - 1 - l;
				goodMask |= ((size_t)1 << shift);
			}
		}

		//std::bitset<32> b(brokeMask);
		//std::bitset<32> g(goodMask);
		//std::cout << "# mask: " << b << '\n';
		//std::cout << ". mask: " << g << '\n';
		////std::cout << '\n' << '\n';
		

		// Find all permutations.
		std::vector<size_t> permutations;	
		{
			// Get groupings total size.
			size_t groupingTotalSize = 0;
			for (int g = 0; g < groupings.size(); ++g)
			{
				groupingTotalSize += groupings[g];
			}

			// Total 
			int spaces = line.length() - groupingTotalSize;


			// Bars are separations between the sets of zeros.
			// ##....#..#. = (h)_|_|_|_(e)
			bars.clear();
			bars.push_back(0);		// First bar is head anchor and does not move.
			for (int g = 0; g < groupings.size(); ++g)
			{
				bars.push_back(g);
			}			
			bars.push_back(spaces);	// Last bar is end anchor and does not move.

			zeroes.clear();			
			for (int b = 0; b < bars.size() - 1; ++b)
			{
				zeroes.push_back(bars[b + 1] - bars[b]);
			}
				
			// Find all permutations for the line.
			size_t endBar = bars.size() - 1;
			do
			{
				//// Get zero sets.
				//for (size_t z = 0; z < zeroes.size(); ++z)
				//{
				//	zeroes[z] = bars[z + 1] - bars[z];
				//}
				auto zIT = zeroes.begin();
				auto bIT = bars.begin();
				size_t bSize = bars.size();
				for (size_t b = 1; b < bSize; ++b)
				{ 
					*zIT = *(bIT + b) - *bIT;
					++zIT;
				}

				// Set head/anchor.
				size_t permutation = 0;

				size_t start = line.length() - 1;
				size_t pos = 0;

				// Create new permutation
				size_t gSize = groupings.size();
				auto gIT = groupings.begin();
				zIT = zeroes.begin();
				bIT = bars.begin();
				for (size_t s = 0; s < gSize; ++s)
				{
					//pos += zeroes[s];
					pos += *zIT; //*(bIT + 1) - *bIT; // bars[s + 1] - bars[s];
					size_t shift = start - pos;

					for (size_t g = 0; g < *gIT; ++g)
					{
						permutation |= ((size_t)1 << shift--);
					}

					pos += *gIT;
					++zIT;
					++bIT;
					++gIT;
				}
						
				// Check permutation.
				if ((permutation & brokeMask) == brokeMask
					&& (~permutation & goodMask) == goodMask)
				{
					/*
					std::bitset<32> p(permutation);
					std::cout << "keeper: " << p << '\n';*/
					answer += 1;
				}

				//permutations.push_back(permutation);

			} while (advanceBar(endBar - 1, endBar));

		} // End of finding all permutations.	

		std::cout << "Next input line...\n";

		//// Display found permutations.
		//for (int p = 0; p < permutations.size(); ++p)
		//{
		//	std::bitset<32> x(permutations[p]);
		//	std::cout << x << '\n';
		//}			
		//std::cout << '\n\n';
		
		//// Check found permutations against required pattern.
		//for (int p = 0; p < permutations.size(); ++p)
		//{
		//	size_t permutation = permutations[p];

		//	if ((permutation & brokeMask) == brokeMask
		//		&& (~permutation & goodMask) == goodMask)
		//	{
		//		std::bitset<32> p(permutation);
		//		std::cout << "keeper: " << p << '\n';
		//		answer += 1;
		//	}		
		//}

		//std::cout << '\n' << '\n';

	} // End of input line.


	std::cout << "The answer to part " << PUZZLE_PART << " is: " << answer << std::endl;
	
	return 0;
}


