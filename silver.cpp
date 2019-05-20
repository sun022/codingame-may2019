#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <queue>
#include <climits>
#include <numeric>

using namespace std;

const vector<pair<int,int>> d4 = {{0,1},{0,-1},{1,0},{-1,0}};
const vector<pair<int,int>> diag4 = {{1,1},{1,-1},{-1,1},{-1,-1}};

const int SELF = 0;      // part of statement definition
const int ENEMY = 1;     // part of statement definition
const int NEUTRAL = 2;

const int HQ = 0;    // part of statement definition
const int MINE = 1;  // part of statement definition
const int TOWER = 2; // part of statement definition
const int UNIT = 3;
const int NONE = 4; // read: none of the above

struct Building {
    int owner;
    int buildingType;
    int y;
    int x;
    Building(int owner, int buildingType, int y, int x):owner(owner),buildingType(buildingType),y(y),x(x){}
};

struct Unit {
    int unitId;
    int level;
    int y;
    int x;
    Unit(int unitId, int level, int y, int x):unitId(unitId),level(level),y(y),x(x){}
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

bool adjancent_enemy(const vector<vector<bool>> &mask, const vector<vector<int>> &owner, const vector<vector<int>> &prot_tier, int y, int x) {
    for(auto [dy, dx] : d4){
        int y1 = y + dy;
        int x1 = x + dx;
        if(owner[y1][x1] == ENEMY && prot_tier[y1][x1] >= 1)
            return true;
    }
    return false;
}

int get_num_moves(const Unit &u, const vector<vector<bool>> &mask, const vector<vector<int>> &owner, const vector<vector<int>> &prot_tier, const vector<vector<int>> &contains){
    int ret=0;
    for(auto [dy, dx] : d4) {
        int y1 = u.y + dy;
        int x1 = u.x + dx;
        if(!mask[y1][x1]) continue;
        if(owner[y1][x1] == NEUTRAL)
            ret++;
        else if(owner[y1][x1] == ENEMY)
            ret += (prot_tier[y1][x1] == 0); // assuming u is t1 unit
        else if(owner[y1][x1] == SELF)
            ret += (contains[y1][x1] == NONE);        
    }
    return ret;
}

bool enemy_threat(int y, int x, const vector<vector<int>> &owner, const vector<vector<int>> &contains, const vector<vector<bool>> &tower_reserved, const vector<vector<int>> &unit_tier){
    int prot = 2; // min tier of unit required to get in
    int threat = 0; // max tier of threatening unit
    for(auto [dy, dx] : d4){
        int y1 = y + dy;
        int x1 = x + dx;
        if(owner[y1][x1] == SELF && (contains[y1][x1] == TOWER || tower_reserved[y1][x1])){
            prot = 3;
        }
        if(owner[y1][x1] == ENEMY && contains[y1][x1] == UNIT)
            threat = max(threat, unit_tier[y1][x1]);
    }
    return threat >= prot;
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
        vector<vector<int>>  prot_tier(14, vector<int>(14, 0));
        vector<vector<int>>  contains(14, vector<int>(14, NONE));
        vector<vector<int>>  unit_tier(14, vector<int>(14,0));

        vector<vector<bool>> tower_reserved(14, vector<bool>(14,false));
        vector<Unit> units;
        vector<Unit> enemy_units;

        cin >> gold >> income >> opponentGold >> opponentIncome; cin.ignore();

        for (int i = 0; i < 12; i++) {
            string line;
            cin >> line; cin.ignore();
            // cerr << line << endl;
            for(int j=0;j<12;j++){
                if(line[j] == 'O')          owner[i+1][j+1] = SELF;
                else if(line[j] == '#')     owner[i+1][j+1] = NEUTRAL;
                else                        owner[i+1][j+1] = ENEMY;
                mask[i+1][j+1] = (line[j]!='#');
            }
        }

        int enemy_hq_y, enemy_hq_x;
        int self_hq_y, self_hq_x;
        int buildingCount;
        cin >> buildingCount; cin.ignore();
        for (int i = 0; i < buildingCount; i++) {
            int buildingOwner, buildingType, x, y;
            cin >> buildingOwner >> buildingType >> x >> y; cin.ignore();
            y++; x++;
            if(buildingType == HQ){
                contains[y][x] = HQ;
                if(buildingOwner == ENEMY) { enemy_hq_y = y; enemy_hq_x = x; }
                if(buildingOwner == SELF) { self_hq_y = y; self_hq_x = x; }
            }
            if(buildingType == TOWER){
                prot_tier[y][x] = 3;
                contains[y][x] = TOWER;
                for(auto [dy, dx] : d4){
                    int y1 = y + dy; int x1 = x + dx;
                    if(owner[y1][x1] == buildingOwner)
                        prot_tier[y1][x1] = 3;
                }
            }
        }

        int unitCount;
        cin >> unitCount; cin.ignore();
        for (int i = 0; i < unitCount; i++) {
            int owner, unitId, level, x, y;
            cin >> owner >> unitId >> level >> x >> y; cin.ignore();
            y++; x++;
            contains[y][x] = UNIT;
            unit_tier[y][x] = level;
            if (owner == SELF)
                units.emplace_back(unitId, level, y, x);
            else
                enemy_units.emplace_back(unitId, level, y, x);
            prot_tier[y][x] = max(prot_tier[y][x], level);
        }

        vector<vector<int>> dist_enemy_hq = calc_dists(mask, enemy_hq_y, enemy_hq_x);
        vector<vector<int>> dist_self_hq = calc_dists(mask, self_hq_y, self_hq_x);

        string t="";
        // look for enemy t2, unmatched
        for(auto u : enemy_units){
            if(u.level != 2) continue;
            for(auto [dy, dx] : diag4) {
                int y1 = u.y + dy;
                int x1 = u.x + dx;
                if(gold >= 15 && owner[y1][x1] == SELF && prot_tier[y1][x1] != 3){
                    gold -= 15;
                    if(contains[y1][x1] == NONE){
                        t += "BUILD TOWER " + to_string(x1-1) +  " " + to_string(y1-1) + ";";
                    }
                    else {
                        // then mark spot for tower
                        tower_reserved[y1][x1] = true;
                    }
                }
            }
        }

        // for each unit, move to random unoccupied adjancent space
        // EDIT: prio to spaces closest to enemy HQ
        // EDIT2: process units in order of no. adjacent movable squares
        vector<int> unit_num_init_moves(units.size());
        for(int i=0;i<units.size();i++) unit_num_init_moves[i] = get_num_moves(units[i], mask, owner, prot_tier, contains);
        vector<int> z(units.size()); iota(z.begin(), z.end(), 0);
        sort(z.begin(), z.end(), [&unit_num_init_moves](int i,int j){return unit_num_init_moves[i]<unit_num_init_moves[j];});

        for(int i=0;i<units.size();i++){
            const Unit u = units[z[i]];
            vector<pair<int,int>> move_opt;
            int best_pri = INT_MAX;
            for(auto [dy, dx] : d4){
                int y1 = u.y + dy;
                int x1 = u.x + dx;
                if(!mask[y1][x1]) continue;
                if(owner[y1][x1] == ENEMY && prot_tier[y1][x1] > 0) continue; // can't get through unit (yet)
                if(enemy_threat(y1,x1, owner, contains, tower_reserved, unit_tier)) continue;
                // if(tower_reserved[y1][x1]) continue;

                int pri = dist_enemy_hq[y1][x1];
                if(owner[y1][x1] == SELF) pri += 100;
                if(owner[y1][x1] == NEUTRAL) pri += 50;
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
                owner[y1][x1] = SELF;
                contains[y1][x1] = UNIT;
                contains[u.y][u.x] = NONE;

                if(tower_reserved[u.y][u.x]){
                    tower_reserved[u.y][u.x] = false;
                    t += "BUILD TOWER " + to_string(u.x-1) +  " " + to_string(u.y-1) + ";";
                    contains[u.y][u.x] = TOWER;
                }

                // maybe put tower
                if(gold >= 15
                    && adjancent_enemy(mask, owner, prot_tier, u.y, u.x)
                    && prot_tier[u.y][u.x] != 3
                ){
                    // put tower on (now-empty) square
                    t += "BUILD TOWER " + to_string(u.x-1) +  " " + to_string(u.y-1) + ";";
                    gold -= 15;
                }
            }
        }

        while(gold >= 10){
            vector<pair<int,int>> new_unit_opt;
            int best_pri = INT_MAX;
            for(int i=0;i<14;i++){      // y dir
                for(int j=0;j<14;j++){  // x dir
                    if(owner[i][j] != SELF) continue;
                    for(auto [dy, dx] : d4){
                        int y1 = i + dy;
                        int x1 = j + dx;
                        if(!mask[y1][x1] || owner[y1][x1] == SELF) continue;
                        if(owner[y1][x1] == ENEMY && prot_tier[y1][x1] > 0) continue; // can't get through unit (yet)
                        if(enemy_threat(y1,x1, owner, contains, tower_reserved, unit_tier)) continue;

                        int pri = dist_enemy_hq[y1][x1];
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
            owner[y1][x1] = SELF;
            gold -= 10;
            income -= 1;
        }
        
        if(t=="") t = "WAIT;";

        cout << t << endl;

    }
}