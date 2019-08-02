#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <utility>
#include <set>
#include <algorithm>

int randomInt(int start, int end) {
	return start + ( rand() % (end - start + 1) ); 
}

void roomgen(std::vector<std::vector<char>> &mapvec, int mapWidth, int mapHeight, int roomMaxWidth, int roomMaxHeight){
	//note: rooms must have minimum have width and height of 1 occupiable space, including walls have 3 spaces in length
	int numrooms = 5;
	int maxRoomgenAttempt = 10;
	int roomgenAttempt = 0;
	int tempy, tempx;
	int tempsizey, tempsizex;
	bool badroom = false;
	int mindim = 5;	
	int roomcount = 0;
	bool isnewroom = true;
	while(roomcount < numrooms && roomgenAttempt < maxRoomgenAttempt){
		isnewroom = true;
		badroom = false;
		//size of the room
		tempsizey = randomInt(mindim,roomMaxHeight);
		tempsizex = randomInt(mindim,roomMaxWidth);

		//top left corner of new room
		tempy = randomInt(1,mapHeight-1-tempsizey);
		tempx = randomInt(1,mapWidth-1-tempsizex);

		//check if theres any wall sharing
		for (int a = tempy; a < tempy+tempsizey; ++a){
			badroom = badroom || (mapvec[a][tempx] == '|');
			badroom = badroom || (mapvec[a][tempx+tempsizex-1] == '|');
		}
		for (int b = tempx; b < tempx+tempsizex; ++b){
			
			badroom = badroom || (mapvec[tempy][b] == '-');
			badroom = badroom || (mapvec[tempy+tempsizey-1][b] == '-');
		}

		if(!badroom){
			//draw box
			for (int a = tempy; a < tempy+tempsizey; ++a){
				for (int b = tempx; b < tempx+tempsizex; ++b){
					if (mapvec[a][b]=='.') isnewroom = false;
					if ((b == tempx || b == tempx+tempsizex-1) && mapvec[a][b] != '.'){
						mapvec[a][b] = '|';
					}else if ((a == tempy || a == tempy+tempsizey-1) && mapvec[a][b] != '.'){
						if(mapvec[a][b] != '|') mapvec [a][b] = '-';
					}else{
						mapvec[a][b] = '.';
					}
				}
			}
			++roomgenAttempt;
			if (isnewroom){ ++roomcount;}
			std::cout << "roomcount: " << roomcount << std::endl;
		}
	}
	/*special edgecase:
		|.
		.|
	or
		.|
		|.
	*/
	for (int a = 1; a < mapHeight-1; ++a){
		for (int b = 1; b < mapWidth-1; ++b){
			if (mapvec[a][b] == '|' && mapvec[a][b+1] == '.' && mapvec[a+1][b] == '.' && mapvec[a+1][b+1] == '|'){
				mapvec[a][b] = mapvec[a+1][b+1] = '.';
				mapvec[a-1][b-1] = mapvec[a][b-1] = mapvec[a+1][b+2] = mapvec[a+2][b+2];
			}else if (mapvec[a][b] == '.' && mapvec[a][b+1] == '|' && mapvec[a+1][b] == '|' && mapvec[a+1][b+1] == '.'){
				mapvec[a][b+1] = mapvec[a+1][b] = '.';
				mapvec[a-1][b+2] = mapvec[a][b+2] = mapvec[a+1][b-1] = mapvec[a+2][b-1] = '|';
			}
		}
	}
}

//helper for passagegen
std::pair<int,int> charsearch(std::vector<std::vector<char>> &mapvec, int mapWidth, int mapHeight, char target, std::set<std::pair<int,int>> ignorelist){
	for (int a = 1; a < mapHeight-1; ++a){
		for (int b = 1; b < mapWidth-1; ++b){
			if ((mapvec[a][b] == target) && not (ignorelist.find(std::make_pair(a,b)) != ignorelist.end())){
				return std::make_pair(a,b);
			}
		}
	}
	return std::make_pair(0,0);
}

void passagegen(std::vector<std::vector<char>> &mapvec, int mapWidth, int mapHeight, int thisx, int thisy, bool firstone = true){
	bool passageplaced = false;
	int validspots = 0;
	int whichspot = 0;
	std::vector<std::pair<int,int>> validcoords;
	
	//write the # if there are exactly 1 previous # tile (the previous one) or if it is the first placed on the board
	if (mapvec[thisy-1][thisx] == '#') ++validspots;
	if (mapvec[thisy][thisx-1] == '#') ++validspots;
	if (mapvec[thisy][thisx+1] == '#') ++validspots;
	if (mapvec[thisy+1][thisx] == '#') ++validspots;
	
	if (firstone || validspots == 1){
		mapvec[thisy][thisx] = '#';
		passageplaced = true;
	}

	if (passageplaced){
		//count adjacent ' ' spots
		validspots = 0;
		if (mapvec[thisy-1][thisx] == ' '){
			++validspots;
			validcoords.push_back({thisy-1,thisx});
		}
		if (mapvec[thisy][thisx-1] == ' '){
			++validspots;
			validcoords.push_back({thisy,thisx-1});
		}
		if (mapvec[thisy][thisx+1] == ' '){
			++validspots;
			validcoords.push_back({thisy,thisx+1});
		}
		if (mapvec[thisy+1][thisx] == ' '){
			++validspots;
			validcoords.push_back({thisy+1,thisx});
		}
		
		//recursively call passagegen on all adjacent ' ' spots in random order
		while (validspots){
			whichspot = randomInt(0,validspots-1);
			passagegen(mapvec, mapWidth, mapHeight, validcoords[whichspot].second, validcoords[whichspot].first, false);
			validcoords.erase(validcoords.begin()+whichspot);
			--validspots;
		}
	}
}

//helper for adddoor
void walldots(std::vector<std::vector<char>> &mapvec, int mapWidth, int mapHeight, int thisx, int thisy, std::set<std::pair<int,int>> &ignorelist, std::set<std::pair<int,int>> &potdoors){
	//if this dot is not in the ignorelist
	if (not (ignorelist.find(std::make_pair(thisy,thisx)) != ignorelist.end())){
		//if this dot is a candidate for being next to a door
		if (mapvec[thisy-1][thisx] == '-' && mapvec[thisy-2][thisx] == '#') potdoors.emplace(std::make_pair(thisy-1,thisx));
		if (mapvec[thisy][thisx-1] == '|' && mapvec[thisy][thisx-2] == '#') potdoors.emplace(std::make_pair(thisy,thisx-1));
		if (mapvec[thisy][thisx+1] == '|' && mapvec[thisy][thisx+2] == '#') potdoors.emplace(std::make_pair(thisy,thisx+1));
		if (mapvec[thisy+1][thisx] == '-' && mapvec[thisy+2][thisx] == '#') potdoors.emplace(std::make_pair(thisy+1,thisx));

		ignorelist.emplace(std::make_pair(thisy,thisx));
		
		if (mapvec[thisy-1][thisx] == '.') walldots(mapvec, mapWidth, mapHeight, thisx, thisy-1, ignorelist, potdoors);
		if (mapvec[thisy][thisx-1] == '.') walldots(mapvec, mapWidth, mapHeight, thisx-1, thisy, ignorelist, potdoors);
		if (mapvec[thisy][thisx+1] == '.') walldots(mapvec, mapWidth, mapHeight, thisx+1, thisy, ignorelist, potdoors);
		if (mapvec[thisy+1][thisx] == '.') walldots(mapvec, mapWidth, mapHeight, thisx, thisy+1, ignorelist, potdoors);
	}
}

void adddoor(std::vector<std::vector<char>> &mapvec, int mapWidth, int mapHeight){
	std::set<std::pair<int,int>> ignorelist; //set of coordinates of dots
	std::set<std::pair<int,int>> potdoors; //set of coordinates of potential doors (the actual walls)

	while (charsearch(mapvec, mapWidth, mapHeight, '.', ignorelist) != std::make_pair(0,0)){
		int tempy = charsearch(mapvec, mapWidth, mapHeight, '.', ignorelist).first;
		int tempx = charsearch(mapvec, mapWidth, mapHeight, '.', ignorelist).second;

		walldots(mapvec, mapWidth, mapHeight, tempx, tempy, ignorelist, potdoors);
		
		int potdoorssize = potdoors.size();
		int numdoors = randomInt(1, std::min(3, potdoorssize));//for a particular room
		
		for (int counter = 0; counter < numdoors; ++counter){
			potdoorssize = potdoors.size();
			int temprand = randomInt(0, potdoorssize-1);
			std::pair<int,int> temppair = *std::next(potdoors.begin(), temprand);
			int doory = temppair.first;
			int doorx = temppair.second;
			mapvec[doory][doorx] = '+';
			potdoors.erase(temppair);
		}
		potdoors.clear();
	}	
}

void removeends(std::vector<std::vector<char>> &mapvec, int mapWidth, int mapHeight, int thisx, int thisy){
	int adjcount = 0;
	if (mapvec[thisy-1][thisx] == '#' || mapvec[thisy-1][thisx] == '+') ++adjcount;
	if (mapvec[thisy][thisx-1] == '#' || mapvec[thisy][thisx-1] == '+') ++adjcount;
	if (mapvec[thisy][thisx+1] == '#' || mapvec[thisy][thisx+1] == '+') ++adjcount;
	if (mapvec[thisy+1][thisx] == '#' || mapvec[thisy+1][thisx] == '+') ++adjcount;

	if (adjcount == 1){
		mapvec[thisy][thisx] = ' ';
		
		if (mapvec[thisy-1][thisx] == '#') removeends(mapvec, mapWidth, mapHeight, thisx, thisy-1);
		if (mapvec[thisy][thisx-1] == '#') removeends(mapvec, mapWidth, mapHeight, thisx-1, thisy);
		if (mapvec[thisy][thisx+1] == '#') removeends(mapvec, mapWidth, mapHeight, thisx+1, thisy);
		if (mapvec[thisy+1][thisx] == '#') removeends(mapvec, mapWidth, mapHeight, thisx, thisy+1);
	}
}

std::vector<std::vector<char>> genmap(int mapWidth, int mapHeight){
	/*
	Steps:
	1) default set the map to spaces and border characters
	2) randomly generate rooms
	3) randomly fully generate passages to fully fill the board (recursively, by reference)
		a) new #s should be placed adjacent to the most recently placed #
		b) #s can be adjacent to a room
		c) newly placed #s should be adjacent to exactly 1 other #
		d) if when a # is placed, there exactly x places the # can be placed:
			call passagegen on all adjacent ' ' in random order
			x=1: place the # there
			x=2: place a # in both spots, call placeHash on both
			x=3: place a # in one of the 3 spots
	4) randomly connect the chambers to passages (change the chamber border to door('+'))
	5) remove 'dead end's of #s
	6) return as a string to be used in game (remember to include whitespace characters in the string)
	*/

	std::vector<std::vector<char>> mapvec;
	mapvec = std::vector<std::vector<char>>(mapHeight, std::vector<char>(mapWidth, ' '));
	int roomMaxWidth = 10;
	int roomMaxHeight = 10;

//1) default set the map to spaces and border characters
	for (int a = 0; a < mapHeight; ++a){
		for (int b = 0; b < mapWidth; ++b){
			if (b == 0 || b == mapWidth-1) mapvec[a][b] = '|';
			else if (a == 0 || a == mapHeight-1) mapvec [a][b] = '-';
		}
	}

//2) randomly generate rooms
	roomgen(mapvec, mapWidth, mapHeight, roomMaxWidth, roomMaxHeight);

//3) randomly fully generate passages to fully fill the board (recursively, by reference)
	int firstvalidx = charsearch(mapvec, mapWidth, mapHeight, ' ', {}).second;
	int firstvalidy = charsearch(mapvec, mapWidth, mapHeight, ' ', {}).first;
	passagegen(mapvec, mapWidth, mapHeight, firstvalidx, firstvalidy);

//4) randomly connect the chambers to passages (change the chamber border to door('+'))
	adddoor(mapvec, mapWidth, mapHeight);

//5) remove 'dead end's of #s
	for (int a = 0; a < mapHeight; ++a){
		for (int b = 0; b < mapWidth; ++b){
			if (mapvec[a][b] == '#'){
				removeends(mapvec, mapWidth, mapHeight, b, a);
			}
		}
	}

//6) return as a string to be used in game (remember to include whitespace characters in the string)
	return mapvec;
}

//just print to test if it works
int main(){
	srand(time(NULL));
	int mapWidth = 79;
	int mapHeight = 25;
	
	auto test = genmap(mapWidth,mapHeight);
	
	for (int a = 0; a < mapHeight; ++a){
		for (int b = 0; b < mapWidth; ++b){
			std::cout<<test[a][b];
		}
		std::cout<<std::endl;
	}

	return 0;
}
