// Open and read input file.
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <queue>


//#pragma comment(linker, "/STACK:10000000")

// DIFFICULTY
static constexpr int PUZZLE_PART = 2;


// EVERGREEN FUNCS //

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

enum Connection
{
	LEFT	= 0b0001,
	RIGHT	= 0b0010,
	UP		= 0b0100,
	DOWN	= 0b1000
};


struct Pipe
{

};

struct int2
{
	int x{ 0 };
	int y{ 0 };
};


void CheckFirstPath(int x, int y);
bool CheckPath(int x, int y, int fromConnection);

void CheckPathNR(int x, int y, int fromConnection);

void FloodMap();
void FloodBigmap();
const int MinifloodMap(int xStart, int yStart, char fill);
bool IsValidCoord(int x, int y, int mWidth, int mHeight);


int width, height;
int2 start;
std::vector<std::vector<int>> maze;
std::vector<int2> correctPath;

std::vector<std::string> map;
std::vector<std::string> bigmap;


int main()
{
	// PARSE INPUT FILE //

	std::vector<std::string> list;

	if (parseInputFile(list) == 1)
	{
		return 1;
	}

	// SOLVE PUZZLE //

	size_t answer = 0;

	// Setup maze vectors.	
	height = list.size();
	width = list[0].length();
	maze.reserve(height);

	for (int y = 0; y < height; ++y)
	{
		std::vector<int> section(width, 0);
		maze.push_back(section);
	}

	// Assign maze elements a bitflag of exits.
	for (int y = 0; y < height; ++y)
	{
		std::vector<int>& section = maze[y];
		for (int x = 0; x < width; ++x)
		{
			char pipe = list[y][x];
			int connections = 0;
			switch (pipe)
			{
			case 'J':
				connections = LEFT | UP;
				break;
			case '7':
				connections = LEFT | DOWN;
				break;
			case 'L':
				connections = RIGHT | UP;
				break;
			case 'F':
				connections = RIGHT | DOWN;
				break;
			case '-':
				connections = LEFT | RIGHT;
				break;
			case '|':
				connections = UP | DOWN;
				break;
			case 'S':
				start = { x, y };
				connections = LEFT | RIGHT | UP | DOWN;
				break;
			default:
				break;
			}

			section[x] = connections;
		}
	}	

	// Find the correct path.
	//CheckFirstPath(start.x, start.y);
	CheckPathNR(start.x, start.y, 0);

	// Print result.
	map = list;
	{
		for (int y = 0; y < height; ++y)
		{
			for (int x = 0; x < width; ++x)
			{
				map[y][x] = '.';
			}
		}

		for (int i = 0; i < correctPath.size(); ++i)
		{
			int x = correctPath[i].x;
			int y = correctPath[i].y;
			
			map[y][x] = list[y][x];
		}

		std::ofstream ofs{ "map.txt" };

		for (int i = 0; i < map.size(); ++i)
		{
			ofs << map[i];
			ofs << '\n';
		}

		ofs.close();
	}

	if (PUZZLE_PART == 1)
	{
		std::cout << "The answer to part " << PUZZLE_PART << " is: " << correctPath.size() / 2 << std::endl;
		return 1;
	}

	// Continue on to part 2.
	
	bigmap.reserve(height * 2);

	// First, we make an expanded map so we can see the spaces betwee the pipes.
	// But before we do that, we need to assign the real connections to the S tile.
	{
		int connections = 0;

		if (maze[start.y - 1][start.x] & DOWN)
		{
			connections |= UP;
		}

		if (maze[start.y + 1][start.x] & UP)
		{
			connections |= DOWN;
		}

		if (maze[start.y][start.x - 1] & RIGHT)
		{
			connections |= LEFT;
		}

		if (maze[start.y][start.x + 1] & LEFT)
		{
			connections |= RIGHT;
		}

		maze[start.y][start.x] = connections;
	}

	// Now we expand the map.
	{
		for (int y = 0; y < height * 2; ++y)
		{
			bigmap.emplace_back(width * 2, '.');
		}

		for (int y = 0; y < height; ++y)
		{
			std::string& line = map[y];

			// For each pipe of the correct path, extend either downward or rightward a new straight pipe connection.
			// This will expand the map and allow us to search the "gaps between the pipes".
			for (int x = 0; x < width; ++x)
			{
				// Only get pipe info if there is a pipe at location.
				if (line[x] == '.')
				{
					continue;
				}

				// Copy original pipe data.
				bigmap[y * 2][x * 2] = list[y][x];

				// Add extended pipe data.
				int connection = maze[y][x];
				if (connection & RIGHT)
				{
					bigmap[y * 2][1 + x * 2] = '-';
				}

				if (connection & DOWN)
				{
					bigmap[1 + y * 2][x * 2] = '|';
				}
			}
		}

		std::ofstream ofs{ "big_map.txt" };

		for (int i = 0; i < bigmap.size(); ++i)
		{
			ofs << bigmap[i];
			ofs << '\n';
		}

		ofs.close();
	}

	// Next, we mark all the outside spaces with 'x' instead of '.'
	{
		FloodMap();

		std::ofstream ofs{ "flood_map.txt" };

		for (int i = 0; i < map.size(); ++i)
		{
			ofs << map[i];
			ofs << '\n';
		}

		ofs.close();


		FloodBigmap();

		ofs.open("big_flood_map.txt");

		for (int i = 0; i < bigmap.size(); ++i)
		{
			ofs << bigmap[i];
			ofs << '\n';
		}

		ofs.close();
	}

	// Now we lookup the remaining dots on the 'small' map and cross reference them to the big map.
	// If the big map location is also a dot, then that area is "inside" the loop.
	// Do a semi flood of that area on the 'small' map and count how many dots were touched. Sum this up for our answer.
	{
		for (int y = 0; y < height; ++y)
		{
			std::string& line = map[y];

			// This is the replacement for the inner x-loop.
			int pos = line.find_first_of('.');
			while (pos != std::string::npos)
			{
				// Lookup on big map.
				if (bigmap[y * 2][pos * 2] == '.')
				{
					// Confirmed inner zone. Do a miniflood and get count.
					answer += MinifloodMap(pos, y, '!');
				}
				else
				{
					// Part of outer zone. Flood it so we don't have any future hits on that zone.
					MinifloodMap(pos, y, 'x');
				}

				pos = line.find_first_of('.');
			}
		}

		std::ofstream ofs{ "answer_map.txt" };

		for (int i = 0; i < map.size(); ++i)
		{
			ofs << map[i];
			ofs << '\n';
		}

		ofs.close();
	}
 
	std::cout << "The answer to part " << PUZZLE_PART << " is: " << answer << std::endl;
	
	return 0;
}


// This + next method use recursive pattern to find path. Overflows the stack (see pragma at top of file for workaround).
void CheckFirstPath(int x, int y)
{
	int connection = maze[y][x];

	// Investigate
	if ((connection & LEFT) && x > 0)
	{
		int xNext = x - 1;

		if ((maze[y][xNext] & RIGHT) && CheckPath(xNext, y, RIGHT))
		{
			correctPath.push_back({ x, y });
			return;
		}
	}
	
	if ((connection & RIGHT) && x < width - 1)
	{
		int xNext = x + 1;

		if ((maze[y][xNext] & LEFT) && CheckPath(xNext, y, LEFT))
		{
			correctPath.push_back({ x, y });
			return;
		}
	}
	
	if ((connection & UP) && y > 0)
	{
		int yNext = y - 1;

		if ((maze[yNext][x] & DOWN) && CheckPath(x, yNext, DOWN))
		{
			correctPath.push_back({ x, y });
			return;
		}
	}
	
	if ((connection & DOWN) && y < height - 1)
	{
		int yNext = y + 1;

		if ((maze[yNext][x] & UP) && CheckPath(x, yNext, UP))
		{
			correctPath.push_back({ x, y });
			return;
		}
	}
}


bool CheckPath(int x, int y, int fromConnection)
{
	int connection = maze[y][x];

	// Prevent backtracking.
	connection = connection & ~fromConnection;

	// Check if we made a full loop.
	if (x == start.x && y == start.y)
	{
		// Do not store these coordinates - they are the start coordinates.
		return true;
	}

	// Investigate
	if ((connection & LEFT) && x > 0)
	{
		int xNext = x - 1;

		if ((maze[y][xNext] & RIGHT) && CheckPath(xNext, y, RIGHT))
		{
			correctPath.push_back({ x, y });
			return true;
		}
	}
	else if ((connection & RIGHT) && x < width - 1)
	{
		int xNext = x + 1;

		if ((maze[y][xNext] & LEFT) && CheckPath(xNext, y, LEFT))
		{
			correctPath.push_back({ x, y });
			return true;
		}
	}
	else if ((connection & UP) && y > 0)
	{
		int yNext = y - 1;

		if ((maze[yNext][x] & DOWN) && CheckPath(x, yNext, DOWN))
		{
			correctPath.push_back({ x, y });
			return true;
		}
	}
	else if ((connection & DOWN) && y < height - 1)
	{
		int yNext = y + 1;

		if ((maze[yNext][x] & UP) && CheckPath(x, yNext, UP))
		{
			correctPath.push_back({ x, y });
			return true;
		}
	}

	// If dead end.
	return false;
}


// Non-recursive version of finding the path. Does not overflow stack.
void CheckPathNR(int x, int y, int fromConnection)
{
	while (true)
	{
		int connection = maze[y][x];

		if (fromConnection != 0)
		{
			// Prevent backtracking.
			connection = connection & ~fromConnection;

			// Check if we made a full loop.
			if (x == start.x && y == start.y)
			{
				// Do not store these coordinates - they are the start coordinates.
				return;
			}
		}

		// Investigate
		if ((connection & LEFT) && x > 0)
		{
			int xNext = x - 1;
			
			if (maze[y][xNext] & RIGHT)
			{
				x = xNext;
				fromConnection = RIGHT;
				correctPath.push_back({ x, y });				
			}
		}
		else if ((connection & RIGHT) && x < width - 1)
		{
			int xNext = x + 1;

			if (maze[y][xNext] & LEFT)
			{
				x = xNext;

				fromConnection = LEFT;
				correctPath.push_back({ x, y });
			}
		}
		else if ((connection & UP) && y > 0)
		{
			int yNext = y - 1;

			if (maze[yNext][x] & DOWN)
			{
				y = yNext;
				fromConnection = DOWN;
				correctPath.push_back({ x, y });				
			}
		}
		else if ((connection & DOWN) && y < height - 1)
		{
			int yNext = y + 1;

			if (maze[yNext][x] & UP)
			{
				y = yNext;
				fromConnection = UP;
				correctPath.push_back({ x, y });
			}
		}
	}
}


void FloodMap()
{
	struct Part
	{
		Part(int x, int y) : x{ x }, y{ y } {}

		int x, y;
	};

	int mWidth = width;
	int mHeight = height;

	bool* beenVisited = new bool[mHeight * mWidth];
	memset(beenVisited, 0, sizeof(bool)* mHeight* mWidth);

	std::queue<Part> parts;
	parts.emplace(0, 0);

	beenVisited[0] = true;

	while (parts.empty() == false)
	{
		// Get top item.
		Part part = parts.front();
		int x = part.x;
		int y = part.y;
		char pipe = map[y][x];

		map[y][x] = 'x';

		// Remove top item.
		parts.pop();

		// Check surrounding cells.
		// Add to queue if valid and not visited yet.

		// Left
		if (IsValidCoord(x - 1, y, mWidth, mHeight)
			&& !beenVisited[(x - 1) + (y * width)]
			&& map[y][x - 1] == '.')
		{
			parts.emplace(x - 1, y);
			beenVisited[(x - 1) + (y * width)] = true;
		}

		// Right
		if (IsValidCoord(x + 1, y, mWidth, mHeight)
			&& !beenVisited[(x + 1) + (y * width)]
			&& map[y][x + 1] == '.')
		{
			parts.emplace(x + 1, y);
			beenVisited[(x + 1) + (y * width)] = true;
		}

		// Up
		if (IsValidCoord(x, y - 1, mWidth, mHeight)
			&& !beenVisited[x + ((y - 1) * width)]
			&& map[y - 1][x] == '.')
		{
			parts.emplace(x, y - 1);
			beenVisited[x + ((y - 1) * width)] = true;
		}

		// Down
		if (IsValidCoord(x, y + 1, mWidth, mHeight)
			&& !beenVisited[x + ((y + 1) * width)]
			&& map[y + 1][x] == '.')
		{
			parts.emplace(x, y + 1);
			beenVisited[x + ((y + 1) * width)] = true;
		}
	}

	delete[] beenVisited;
}


const int MinifloodMap(int xStart, int yStart, char fill)
{
	struct Part
	{
		Part(int x, int y) : x{ x }, y{ y } {}

		int x, y;
	};

	int count = 0;

	int mWidth = width;
	int mHeight = height;

	bool* beenVisited = new bool[mHeight * mWidth];
	memset(beenVisited, 0, sizeof(bool) * mHeight * mWidth);

	std::queue<Part> parts;
	parts.emplace(xStart, yStart);

	beenVisited[0] = true;

	while (parts.empty() == false)
	{
		// Get top item.
		Part part = parts.front();
		int x = part.x;
		int y = part.y;
		char pipe = map[y][x];

		map[y][x] = fill;
		++count;

		// Remove top item.
		parts.pop();

		// Check surrounding cells.
		// Add to queue if valid and not visited yet.

		// Left
		if (IsValidCoord(x - 1, y, mWidth, mHeight)
			&& !beenVisited[(x - 1) + (y * width)]
			&& map[y][x - 1] == '.')
		{
			parts.emplace(x - 1, y);
			beenVisited[(x - 1) + (y * width)] = true;
		}

		// Right
		if (IsValidCoord(x + 1, y, mWidth, mHeight)
			&& !beenVisited[(x + 1) + (y * width)]
			&& map[y][x + 1] == '.')
		{
			parts.emplace(x + 1, y);
			beenVisited[(x + 1) + (y * width)] = true;
		}

		// Up
		if (IsValidCoord(x, y - 1, mWidth, mHeight)
			&& !beenVisited[x + ((y - 1) * width)]
			&& map[y - 1][x] == '.')
		{
			parts.emplace(x, y - 1);
			beenVisited[x + ((y - 1) * width)] = true;
		}

		// Down
		if (IsValidCoord(x, y + 1, mWidth, mHeight)
			&& !beenVisited[x + ((y + 1) * width)]
			&& map[y + 1][x] == '.')
		{
			parts.emplace(x, y + 1);
			beenVisited[x + ((y + 1) * width)] = true;
		}
	}

	delete[] beenVisited;

	return count;
}


void FloodBigmap()
{
	struct Part
	{
		Part(int x, int y) : x{ x }, y{ y } {}
		
		int x, y;
	};

	int mWidth = width * 2;
	int mHeight = height * 2;

	bool* beenVisited = new bool[mHeight * mWidth];
	memset(beenVisited, 0, sizeof(bool) * mHeight * mWidth);

	std::queue<Part> parts;
	parts.emplace(0, 0);

	beenVisited[0] = true;

	while (parts.empty() == false)
	{
		// Get top item.
		Part part = parts.front();
		int x = part.x;
		int y = part.y;
		char pipe = bigmap[y][x];

		bigmap[y][x] = 'x';

		// Remove top item.
		parts.pop();

		// Check surrounding cells.
		// Add to queue if valid and not visited yet.
		
		// Left
		if (IsValidCoord(x - 1, y, mWidth, mHeight)
			&& !beenVisited[(x - 1) + (y * mWidth)]
			&& bigmap[y][x - 1] == '.')
		{
			parts.emplace(x - 1, y);
			beenVisited[(x - 1) + (y * mWidth)] = true;
		}

		// Right
		if (IsValidCoord(x + 1, y, mWidth, mHeight)
			&& !beenVisited[(x + 1) + (y * mWidth)]
			&& bigmap[y][x + 1] == '.')
		{
			parts.emplace(x + 1, y);
			beenVisited[(x + 1) + (y * mWidth)] = true;
		}

		// Up
		if (IsValidCoord(x, y - 1, mWidth, mHeight)
			&& !beenVisited[x + ((y - 1) * mWidth)]
			&& bigmap[y - 1][x] == '.')
		{
			parts.emplace(x, y - 1);
			beenVisited[x + ((y - 1) * mWidth)] = true;
		}

		// Down
		if (IsValidCoord(x, y + 1, mWidth, mHeight)
			&& !beenVisited[x + ((y + 1) * mWidth)]
			&& bigmap[y + 1][x] == '.')
		{
			parts.emplace(x, y + 1);
			beenVisited[x + ((y + 1) * mWidth)] = true;
		}
	}

	delete[] beenVisited;
}


bool IsValidCoord(int x, int y, int mWidth, int mHeight)
{
	if (x < 0 || x > mWidth - 1)
	{
		return false;
	}

	if (y < 0 || y > mHeight - 1)
	{
		return false;
	}

	return true;
}