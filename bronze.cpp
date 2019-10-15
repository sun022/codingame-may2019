/*
wood2 strat:
- rapid greedy expansion with t1
- killable enemy (directly adjancent) units killed

future: 
avoid leaving vulnerable units
aggressive 'strikes' covering multiple squares
*/

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>

using namespace std;

const vector<pair<int,int>> d4 = {{0,1},{0,-1},{1,0},{-1,0}};

struct Building {
    int owner;
    int buildingType;
    int x;
    int y;
    Building(int owner, int buildingType, int x, int y):owner(owner),buildingType(buildingType),x(x),y(y){}
};

struct Unit {
    int unitId;
    int level;
    int x;
    int y;
    Unit(int unitId, int level, int x, int y):unitId(unitId),level(level),x(x),y(y){}
};

int main()
{
    int numberMineSpots;
    cin >> numberMineSpots; cin.ignore();
    for (int i = 0; i < numberMineSpots; i++) {
        int x, y;
        cin >> x >> y; cin.ignore();
        cerr << "MINE " << x << "," << y << endl;
    }

    for(int tick=1;;tick++) {
        int gold, income, opponentGold, opponentIncome;
        vector<vector<int>> grid(14, vector<int>(14, -1));
        vector<Unit> units;
        vector<Unit> enemy_units;

        cin >> gold >> income >> opponentGold >> opponentIncome; cin.ignore();

        for (int i = 0; i < 12; i++) {
            string line;
            cin >> line; cin.ignore();
            // cerr << line << endl;
            for(int j=0;j<12;j++){
                if(line[j] == 'O')          grid[i+1][j+1] = 1;
                else if(line[j] == '#')     grid[i+1][j+1] = -1;
                else                        grid[i+1][j+1] = 0;
            }
        }

        for (int i = 0; i < 14; i++) {
            for(int j=0;j<14;j++){
                char c = '?';
                if(grid[i][j] == 1) c = 'X';
                if(grid[i][j] == -1) c = ' ';
                if(grid[i][j] == 0) c = '.';
                cerr << c;
            }
            cerr << endl;
        }

        int buildingCount;
        cin >> buildingCount; cin.ignore();
        for (int i = 0; i < buildingCount; i++) {
            int owner, buildingType, x, y;
            cin >> owner >> buildingType >> x >> y; cin.ignore();
            x++; y++;
        }
        int unitCount;
        cin >> unitCount; cin.ignore();

        for (int i = 0; i < unitCount; i++) {
            int owner, unitId, level, x, y;
            cin >> owner >> unitId >> level >> x >> y; cin.ignore();
            x++; y++;
            if (owner == 0)
                units.emplace_back(unitId, level, x, y);
            else
                enemy_units.emplace_back(unitId, level, x, y);
            
        }

        string t="";

        // for each unit, move to random unoccupied adjancent space
        for(auto u : units){
            vector<pair<int,int>> move_opt;
            for(auto [dx, dy] : d4){
                int x1 = u.x + dx;
                int y1 = u.y + dy;
                if(grid[y1][x1] == 0)
                    move_opt.push_back({x1,y1});
            }
            if(move_opt.size() != 0){
                auto [x1, y1] = move_opt[rand()%move_opt.size()];
                t += "MOVE " + to_string(u.unitId) + " " + to_string(x1-1) + " " + to_string(y1-1) + ";";
                cerr << "moving to " << x1-1 << "," << y1-1 << " currently " << grid[y1][x1] << endl;
                grid[y1][x1] = 1;
            }
        }

        while(gold >= 10){
            vector<pair<int,int>> new_unit_opt;
            for(int i=0;i<14;i++){      // y dir
                for(int j=0;j<14;j++){  // x dir
                    if(grid[i][j] != 1) continue;
                    for(auto [dx, dy] : d4){
                        int x1 = j + dx;
                        int y1 = i + dy;
                        if(grid[y1][x1] == 0)
                            new_unit_opt.push_back({x1,y1});
                    }
                }
            }
            if(new_unit_opt.size() == 0)
                break;
            auto [x1, y1] = new_unit_opt[rand()%new_unit_opt.size()];
            t += "TRAIN 1 " + to_string(x1-1) + " " + to_string(y1-1) + ";";
            grid[y1][x1] = 1;
            gold -= 10;
            income -= 1;
        }
        
        if(t=="") t = "WAIT;";

        cout << t << endl;

    }
}