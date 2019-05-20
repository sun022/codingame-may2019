#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <queue>
#include <climits>

using namespace std;

const vector<pair<int,int>> d4 = {{0,1},{0,-1},{1,0},{-1,0}};

struct Building {
    int owner;
    int buildingType;
    int y;
    int x;
    Building(int owner, int buildingType, int y, int x):owner(owner),buildingType(buildingType),y(y),x(x){}
    static int const HQ = 0;
    static int const MINE = 1;
    static int const TOWER = 2;
};

struct Unit {
    int unitId;
    int level;
    int y;
    int x;
    Unit(int unitId, int level, int y, int x):unitId(unitId),level(level),y(y),x(x){}
};

struct Player {
    static int const SELF = 0;
    static int const ENEMY = 1;
    static int const NEUTRAL = 2;
};

vector<vector<int>> calc_dists(vector<vector<bool>> &mask, int tar_y, int tar_x){
    vector<vector<int>> dist(14, vector<int>(14,INT_MAX));
    queue<tuple<int,int,int>> q;
    dist[tar_y][tar_x] = 0;
    q.push({tar_x, tar_y, 0});

    while(!q.empty()){
        auto [y, x, d] = q.front(); q.pop();
        for(auto [dy, dx] : d4){
            int y1 = y + dy;
            int x1 = x + dx;
            if(mask[y1][x1] && dist[y1][x1] == INT_MAX){
                dist[y1][x1] = d + 1;
                q.push({y1, x1, d+1});
            }
        }
    }
    return dist;
}

int main()
{
    int numberMineSpots;
    cin >> numberMineSpots; cin.ignore();
    for (int i = 0; i < numberMineSpots; i++) {
        int x, y;
        cin >> x >> y; cin.ignore();
    }

    for(int tick=1;;tick++) {
        int gold, income, opponentGold, opponentIncome;
        vector<vector<int>>  owner(14, vector<int>(14, 2));
        vector<vector<bool>> mask(14, vector<bool>(14, false));
        vector<vector<int>>  tier(14, vector<int>(14, 0));
        vector<Unit> units;
        vector<Unit> enemy_units;

        cin >> gold >> income >> opponentGold >> opponentIncome; cin.ignore();

        for (int i = 0; i < 12; i++) {
            string line;
            cin >> line; cin.ignore();
            // cerr << line << endl;
            for(int j=0;j<12;j++){
                if(line[j] == 'O')          owner[i+1][j+1] = Player::SELF;
                else if(line[j] == '#')     owner[i+1][j+1] = Player::NEUTRAL;
                else                        owner[i+1][j+1] = Player::ENEMY;
                mask[i+1][j+1] = (line[j]!='#');
            }
        }

        int enemy_hq_y, enemy_hq_x;
        int buildingCount;
        cin >> buildingCount; cin.ignore();
        for (int i = 0; i < buildingCount; i++) {
            int owner, buildingType, x, y;
            cin >> owner >> buildingType >> x >> y; cin.ignore();
            x++; y++;
            if(owner == Player::ENEMY && buildingType == Building::HQ) {
                enemy_hq_y = y; enemy_hq_x = x;
            }
        }

        int unitCount;
        cin >> unitCount; cin.ignore();
        for (int i = 0; i < unitCount; i++) {
            int owner, unitId, level, x, y;
            cin >> owner >> unitId >> level >> x >> y; cin.ignore();
            y++; x++;
            if (owner == Player::SELF)
                units.emplace_back(unitId, level, y, x);
            else
                enemy_units.emplace_back(unitId, level, y, x);
            tier[y][x] = level;
        }

        // distance to enemy hq
        vector<vector<int>> dist = calc_dists(mask, enemy_hq_y, enemy_hq_x);

        string t="";
        // for each unit, move to random unoccupied adjancent space
        // EDIT: prio to spaces closest to enemy HQ
        for(auto u : units){
            vector<pair<int,int>> move_opt;
            int best_pri = INT_MAX;
            for(auto [dy, dx] : d4){
                int y1 = u.y + dy;
                int x1 = u.x + dx;
                if(!mask[y1][x1]) continue;
                if(tier[y1][x1] > 0) continue; // can't get through unit (yet)

                int pri = dist[y1][x1];
                if(owner[y1][x1] == Player::SELF) pri += 100;
                if(owner[y1][x1] == Player::NEUTRAL) pri += 50;
                if(pri < best_pri){
                    best_pri = pri;
                    move_opt.clear();
                }
                if(pri == best_pri)
                    move_opt.push_back({y1,x1});
                
            }
            if(move_opt.size() != 0){
                auto [y1, x1] = move_opt[rand()%move_opt.size()];
                t += "MOVE " + to_string(u.unitId) + " " + to_string(x1-1) + " " + to_string(y1-1) + ";";
                owner[y1][x1] = Player::SELF;
            }
        }

        while(gold >= 10){
            vector<pair<int,int>> new_unit_opt;
            int best_pri = INT_MAX;
            for(int i=0;i<14;i++){      // y dir
                for(int j=0;j<14;j++){  // x dir
                    if(owner[i][j] != Player::SELF) continue;
                    for(auto [dy, dx] : d4){
                        int y1 = i + dy;
                        int x1 = j + dx;
                        if(!mask[y1][x1] || owner[y1][x1] == Player::SELF) continue;
                        if(tier[y1][x1] > 0) continue; // can't get through unit (yet)

                        int pri = dist[y1][x1];
                        if(pri < best_pri){
                            best_pri = pri;
                            new_unit_opt.clear();
                        }
                        if(pri == best_pri)
                            new_unit_opt.push_back({y1,x1});
                    }
                }
            }
            if(new_unit_opt.size() == 0)
                break;
            auto [y1, x1] = new_unit_opt[rand()%new_unit_opt.size()];
            t += "TRAIN 1 " + to_string(x1-1) + " " + to_string(y1-1) + ";";
            owner[y1][x1] = Player::SELF;
            gold -= 10;
            income -= 1;
        }
        
        if(t=="") t = "WAIT;";

        cout << t << endl;

    }
}