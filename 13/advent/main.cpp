// Open and read input file.
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <algorithm> // reverse
#include <numeric> // inner_product


// DIFFICULTY
static constexpr int PUZZLE_PART = 2;


// EVERGREEN FUNCS //

// Get and store input as lines of strings.
const int parseInputFile(std::string& filename, std::vector<std::string>& list)
{
	std::ifstream inf{ "../" + filename + ".txt" };

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

const int FindRegularSolution(std::vector<std::string>& map);
const int FindSmudgeSolution(std::vector<std::string>& map);

// MAIN //

int main()
{
	// PARSE INPUT FILE //

	std::string filename = "input";
	std::vector<std::string> list;

	if (parseInputFile(filename, list) == 1)
	{
		return 1;
	}

	// SOLVE PUZZLE //

	auto answer = 0;

	for (int i = 0; i < list.size(); ++i)
	{
		int hReflectionCount = 0;
		int vReflectionCount = 0;

		// Collect single map.
		std::vector<std::string> map;
		{
			for (int m = i; m < list.size(); ++m, ++i)
			{
				if (list[m].length() != 0)
				{
					map.push_back(list[m]);
				}
				else
				{
					break;
				}
			}
		}

		if (PUZZLE_PART == 1)
		{
			answer += FindRegularSolution(map);
		}
		else
		{
			answer += FindSmudgeSolution(map);
		}
	}

	std::cout << "The answer to part " << PUZZLE_PART << " is: " << answer << std::endl;
	
	return 0;
}


const int FindRegularSolution(std::vector<std::string>& map)
{
	int vValue = 0;
	int hValue = 0;

	// Find vertical reflection.
	{
		int length = map[0].length();

		for (int rp = 1; rp < map[0].length(); ++rp)
		{
			int matchCount = 0;

			for (int m = 0; m < map.size(); ++m)
			{
				// Compare by length of shortest side.
				int leftLength = rp;
				int rightLength = map[0].length() - rp;
				int compareLength = leftLength < rightLength ? leftLength : rightLength;

				std::string left = map[m].substr(rp - compareLength, compareLength);
				std::string right = map[m].substr(rp, compareLength);

				std::reverse(left.begin(), left.end());

				// Compare.					
				if (left.compare(right) == 0)
				{
					std::cout << "MATCH >> ";
					++matchCount;
				}
				else
				{
					break;
				}
				std::cout << left << " = " << right << '\n';
			}

			// Check for full reflection (must match all rows).
			if (matchCount == map.size())
			{
				vValue = rp;
				break;
			}
		}
	}


	// Find horizontal reflection.
	{
		int size = map.size();

		for (int rp = 1; rp < map.size(); ++rp)
		{
			int matchCount = 0;

			// Compare by size of smallest half.
			int upperHalf = rp;
			int lowerHalf = map.size() - rp;
			int compareLength = upperHalf < lowerHalf ? upperHalf : lowerHalf;

			// Compare.
			for (int c = 0; c < compareLength; ++c)
			{
				std::string& above = map[(rp - 1) - c];
				std::string& below = map[(rp - 1) + 1 + c];

				if (above.compare(below) == 0)
				{
					std::cout << "MATCH >> ";
					++matchCount;
				}
				else
				{
					break;
				}
				std::cout << above << " = " << below << '\n';
			}

			// Check for full reflection (must match for all comparisons).
			if (matchCount == compareLength)
			{
				hValue = rp;
				break;
			}
		}
	}

	return vValue + (hValue * 100);
}



const int FindSmudgeSolution(std::vector<std::string>& map)
{
	int vValue = 0;
	int hValue = 0;

	// Find new vertical reflection.
	{
		int length = map[0].length();

		for (int rp = 1; rp < map[0].length(); ++rp)
		{
			int matchCount = 0;

			bool usedSmudgeFix = false;
			int fixLine = 0;
			int fixPosition = 0;
			char fixedCharacter = ' ';

			for (int m = 0; m < map.size(); ++m)
			{
				// Compare by length of shortest side.
				int leftLength = rp;
				int rightLength = map[0].length() - rp;
				int compareLength = leftLength < rightLength ? leftLength : rightLength;

				std::string left = map[m].substr(rp - compareLength, compareLength);
				std::string right = map[m].substr(rp, compareLength);

				std::reverse(left.begin(), left.end());

				// Compare.					
				if (left.compare(right) == 0)
				{
					std::cout << "MATCH >> ";
					++matchCount;
				}
				else if (!usedSmudgeFix)
				{
					int diff = 0;
					int pos = 0;
					
					// Find number of differences and position of difference.
					for (int l = 0; l < compareLength; ++l)
					{
						if (left[l] != right[l])
						{
							++diff;
							
							if (diff > 1)
							{
								break;
							}
							else
							{
								pos = l;
							}							
						}
					}

					// If only 1 difference, change it (use the right side since it is not flipped).
					if (diff == 1)
					{
						std::cout << "Trying fix... ";

						// Make temp copy and apply fix.
						std::string fixedLine = right;
						fixedLine[pos] = fixedLine[pos] == '.' ? '#' : '.';

						// Recompare.
						if (left.compare(fixedLine) == 0)
						{
							std::cout << "MATCH >> ";
							++matchCount;
						
							// Can only smudge 1 spot per map.
							usedSmudgeFix = true;

							// Store fix. If fix creates full reflection, save fix to real map.
							fixLine = m;
							fixPosition = rp + pos;						
							fixedCharacter = fixedLine[pos];
						}
						else
						{
							break; // Stop checking this reflection line - even with a fix there is still a non-match.
						}
					}
					else
					{
						break; // Stop checking this reflection line - there are more than 1 symbols that don't match.
					}
				}
				else
				{
					break; // Stop checking this reflection line - fix has been used and a non-match was found.
				}
				std::cout << left << " = " << right << '\n';
			}

			// Check for full reflection (must match all rows) that used a smudge fix.
			if (matchCount == map.size() && usedSmudgeFix)
			{
				std::cout << "Found new vertical reflection.\n";
				return rp;
			}
		}
	}


	// Find horizontal reflection.
	{
		int size = map.size();

		for (int rp = 1; rp < map.size(); ++rp)
		{
			int matchCount = 0;

			bool usedSmudgeFix = false;
			int fixLine = 0;
			int fixPosition = 0;
			char fixedCharacter = ' ';

			// Compare by size of smallest half.
			int upperHalf = rp;
			int lowerHalf = map.size() - rp;
			int compareLength = upperHalf < lowerHalf ? upperHalf : lowerHalf;

			// Compare.
			for (int c = 0; c < compareLength; ++c)
			{
				std::string& above = map[(rp - 1) - c];
				std::string& below = map[(rp - 1) + 1 + c];

				if (above.compare(below) == 0)
				{
					std::cout << "MATCH >> ";
					++matchCount;
				}
				else if (!usedSmudgeFix)
				{
					int diff = 0;
					int pos = 0;

					// Find number of differences and position of difference.
					for (int l = 0; l < map[0].length(); ++l)
					{
						if (above[l] != below[l])
						{
							++diff;

							if (diff > 1)
							{
								break;
							}
							else
							{
								pos = l;
							}
						}
					}

					// If only 1 difference, change it (use the below half - arbitrary choice).
					if (diff == 1)
					{
						std::cout << "Trying fix... ";

						// Make temp copy and apply fix.
						std::string fixedLine = below;
						fixedLine[pos] = fixedLine[pos] == '.' ? '#' : '.';

						// Recompare.
						if (above.compare(fixedLine) == 0)
						{
							std::cout << "MATCH >> ";
							++matchCount;

							// Can only smudge 1 spot per map.
							usedSmudgeFix = true;

							// Store fix. If fix creates full reflection, save fix to real map.
							fixLine = (rp - 1) + 1 + c;
							fixPosition = rp + pos;
							fixedCharacter = fixedLine[pos];
						}
						else
						{
							break; // Stop checking this reflection line - even with a fix there is still a non-match.
						}
					}
					else
					{
						break; // Stop checking this reflection line - there are more than 1 symbols that don't match.
					}
				}
				else
				{
					break;
				}
				std::cout << above << " = " << below << '\n';
			}

			// Check for full reflection (must match for all comparisons) that used a smudge fix.
			if (matchCount == compareLength && usedSmudgeFix)
			{		
				std::cout << "Found new horizontal reflection.\n";				
				return rp * 100;
			}
		}
	}
}
