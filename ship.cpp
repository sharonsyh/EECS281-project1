// Project Identifier: 950181F63D0A883F183EC0A5CC67B19928FE896A


#include <stdio.h>     /* for printf */
#include <stdlib.h>    /* for exit */
#include <getopt.h>
#include <iostream>
#include <vector>
#include <sstream>
#include "ship.h"
#include <deque>
#include <stack>

using namespace std;

struct Options{
    bool stack;
    bool queue;
    char output_mode ;
    char input_mode;
    size_t map_size;
    size_t num_levels;
} Opt;


void BackToShip::get_options(int argc, char** argv){
    
    int c;

    Opt.stack = false;
    Opt.queue = false;
    Opt.output_mode = 'M';
    Opt.input_mode = 0;
    Opt.map_size = 0;
    Opt.num_levels = 0;

    map_size = Opt.map_size;
    num_levels = Opt.num_levels;
    stack = Opt.stack;
    queue = Opt.queue;
    
    while (1) {

        int option_index = 0;
        static struct option long_options[] = {
            {"stack",     no_argument, 0, 's'},
            {"queue",  no_argument, 0, 'q'},
            {"output",  required_argument, 0, 'o'},
            {"help", no_argument, 0, 'h'}
        };

        c = getopt_long(argc, argv, "sqo:h",
                long_options, &option_index);
        if (c == -1)
            break;

        switch (c) {
        case 's':
            if (!stack && !queue){
                stack = true;
            }
            else{
                cerr << "Multiple routing modes specified"<<endl;
                exit(1);
            }
            break;
        case 'q':
            if (!stack && !queue){
                queue = true;
            }
            else{
                cerr << "Multiple routing modes specified"<<endl;
                exit(1);
            }
            break;
        case 'o':
            Opt.output_mode = *optarg;
            break;
        case 'h':
                cerr << "This program finds the way back to the old spacecraft of questionable space-worthiness to escape." <<endl
                << "Command line arguments are as follows: " <<endl
                << "'-s' or '--stack': The search container will be used like a stack." <<endl
                << "'-q' or '--queue': The search container will be used like a queue." <<endl
                << "'-o' or '--output' (M|L): Indicates the output file format. (default output format is map format)" <<endl
                << "'-h' or '--help': Prints the message about how to use the program and ends." <<endl;
                exit(0);

        default:
            exit(1);
        }
    }

    if (!stack && !queue){
        cerr << "No routing mode specified" <<endl;
        exit(1);
    }
}

bool BackToShip::readmap() {

    cin >> Opt.input_mode;

    cin >> num_levels;

    cin >> map_size;

    string new_line;

    getline(cin, new_line);

    property ways ;
    ways.indicator = '.';
    ways.visited = false;
    ways.direction_from = 'A';

    map = vector<vector<vector<property> >> (num_levels, vector<vector<property>>(map_size, vector<property> (map_size, ways)));

    vector<vector<int>> directions = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};

    size_t row = 0;

    if(Opt.input_mode == 'M') {
        
        while((getline(cin, new_line))) {
            
            int flag = 0;

            for(size_t i = 0 ; i < new_line.length() ; i ++) {
                
                if(new_line[i] == '/'){
                    flag = 1;
                }
                if(flag == 1) continue;

                else {
                    
                    if(new_line[i] != 'S' && new_line[i] != 'H' && new_line[i] != '#' && new_line[i] != '.' && new_line[i] != 'E') {
                        
                        cerr << "Invalid map character" << endl;
                        exit(1);
                    }
                    property elem;
                    elem.indicator = new_line[i];
                    elem.visited = false;
                    elem.direction_from = 'A';
                    
                    map[row/map_size][row%map_size][i] = elem;
                    
                    if(new_line[i] == 'S') {

                        starting_pos.level = row/map_size;
                        starting_pos.row = row%map_size;
                        starting_pos.column = i;
                    }

                    if(new_line[i] == 'H') {

                        ending_pos.level = row/map_size;
                        ending_pos.row = row%map_size;
                        ending_pos.column = i;
                    }

                }
            }
            if(flag == 1) flag = 0;
            else {
                row += 1;
            }
        }
    }

    else if(Opt.input_mode == 'L') {

        char first;
        char bracket2;
        size_t floor;
        size_t n_row;
        size_t n_column;
        char l_indicator;
        char comma;
        string somethingelse;

        // cout << "HERE\n";

        while(cin >> first){

            // cout << "FIRST _ " << first << "\n";

            if(first == '/'){
                getline(cin, new_line);
            }

            else{
                cin >> floor >> comma >> n_row >> comma >> n_column >> comma >> l_indicator >>  bracket2;

                if(l_indicator != 'S' && l_indicator != 'H'
                            && l_indicator != '#' && l_indicator != '.'
                            && l_indicator != 'E') {
                    cerr << "Invalid map character" <<endl;
                    exit(1);
                }

                if( floor >= num_levels){
                    cerr << "Invalid map level"<<endl;
                    exit(1);
                }

                if(n_row >= map_size){
                    cerr << "Invalid map row"<<endl;
                    exit(1);
                }

                if(n_column >= map_size){
                    cerr << "Invalid map column" << endl;
                    exit(1);
                }

                if(l_indicator == 'S') {

                    starting_pos.level = floor;
                    starting_pos.row = n_row;
                    starting_pos.column = n_column;
                }

                if(l_indicator == 'H') {

                    ending_pos.level = floor;
                    ending_pos.row = n_row;
                    ending_pos.column = n_column;
                }

                map[floor][n_row][n_column].indicator = l_indicator;
                map[floor][n_row][n_column].visited = false;
                map[floor][n_row][n_column].direction_from = 'A';

                // cout << "(" << floor << "," << n_row << "," << n_column << "," << l_indicator << ")\n";
            }

        }
    }

    return 1;
}

void BackToShip::search(){

    deque<element> search_container ;
    element curr_loc ;
    vector<vector<int>> directions = {{-1, 0}, {0, 1}, {1, 0}, {0, -1}};

    curr_loc = starting_pos;
    search_container.push_back(curr_loc);

    size_t s_row;
    size_t s_col;
    // int s_row_;
    // int static_cast<int>(s_col);

    if(stack == true){

        while(!search_container.empty()){

            curr_loc = search_container.back();
            search_container.pop_back();

            if(map[curr_loc.level][curr_loc.row][curr_loc.column].indicator == 'H'){
                is_solution = true;
                break;
            }
            
            // search_container.pop_back();
            element possible;

            for (const vector<int> &direction : directions) { //N, E, S, W.

                s_row = static_cast<size_t> (static_cast<int>(curr_loc.row) + direction[0]);
                s_col = static_cast<size_t> (static_cast<int>(curr_loc.column) + direction[1]);

                // static_cast<int>(s_row) = static_cast<int>(s_row);
                // static_cast<int>(s_col) = static_cast<int>(s_col);

                if (static_cast<int>(s_row) < 0 || static_cast<int>(s_row) >= static_cast<int>(map_size) || static_cast<int>(s_col) < 0 || static_cast<int>(s_col) >= static_cast<int>(map_size)) {
                    continue; // skips the current direction and moves on to the next one
                }

                if(map[curr_loc.level][s_row][s_col].visited == true
                || map[curr_loc.level][s_row][s_col].indicator == '#'){
                    continue;
                }

                if(direction[0] == -1){
                    map[curr_loc.level][s_row][s_col].direction_from = 'n';
                }
                else if(direction[0] == 1){
                    map[curr_loc.level][s_row][s_col].direction_from = 's';
                }
                else if(direction[1] == 1){
                    map[curr_loc.level][s_row][s_col].direction_from = 'e';
                }
                else if(direction[1] == -1){
                    map[curr_loc.level][s_row][s_col].direction_from = 'w';
                }

                possible.row = s_row;
                possible.column = s_col;
                possible.level = curr_loc.level;

                search_container.push_back(possible);
                map[possible.level][possible.row][possible.column].visited = true;
                
            }

            if(map[curr_loc.level][curr_loc.row][curr_loc.column].indicator == 'E'){
                    
                for(size_t i = 0; i < num_levels ; i++){

                    if(i != curr_loc.level && map[i][curr_loc.row][curr_loc.column].indicator == 'E'
                                                    && map[i][curr_loc.row][curr_loc.column].visited == false){

                        possible.row = curr_loc.row;
                        possible.column = curr_loc.column;
                        possible.level = i;
   
                        search_container.push_back(possible);
                        map[possible.level][possible.row][possible.column].visited = true;

                        map[possible.level][possible.row][possible.column].direction_from
                        = static_cast<char>(curr_loc.level);

                    }
                }
            }
        }
    }
    else if(queue == true){

        while(!search_container.empty()){

            curr_loc = search_container.front();
            search_container.pop_front();

            if(map[curr_loc.level][curr_loc.row][curr_loc.column].indicator == 'H'){
                is_solution = true;
                break;
            }
            element possible;

            for (const vector<int> &direction : directions) { //N, E, S, W.

                s_row = static_cast<size_t> (static_cast<int>(curr_loc.row) + direction[0]);
                s_col = static_cast<size_t> (static_cast<int>(curr_loc.column) + direction[1]);

                // static_cast<int>(s_row) = static_cast<int>(s_row);
                // static_cast<int>(s_col) = static_cast<int>(s_col);

                if (static_cast<int>(s_row) < 0 || static_cast<int>(s_row) >= static_cast<int>(map_size) || static_cast<int>(s_col) < 0 || static_cast<int>(s_col) >= static_cast<int>(map_size)) {
                    continue; // skips the current direction and moves on to the next one
                }

                if(map[curr_loc.level][s_row][s_col].visited == true
                || map[curr_loc.level][s_row][s_col].indicator == '#'){
                    continue;
                }

                if(direction[0] == -1){
                    map[curr_loc.level][s_row][s_col].direction_from = 'n';
                }
                else if(direction[0] == 1){
                    map[curr_loc.level][s_row][s_col].direction_from = 's';
                }
                else if(direction[1] == 1){
                    map[curr_loc.level][s_row][s_col].direction_from = 'e';
                }
                else if(direction[1] == -1){
                    map[curr_loc.level][s_row][s_col].direction_from = 'w';
                }

                possible.row = s_row;
                possible.column = s_col;
                possible.level = curr_loc.level;

                search_container.push_back(possible);
                map[possible.level][possible.row][possible.column].visited = true;

            }

            if(map[curr_loc.level][curr_loc.row][curr_loc.column].indicator == 'E'){
                    
                for(size_t i = 0; i < num_levels ; i++){

                    if(i != curr_loc.level && map[i][curr_loc.row][curr_loc.column].indicator == 'E'
                                                    && map[i][curr_loc.row][curr_loc.column].visited == false){

                        possible.row = curr_loc.row;
                        possible.column = curr_loc.column;
                        possible.level = i;

                        search_container.push_back(possible);
                        map[possible.level][possible.row][possible.column].visited = true;

                        map[possible.level][possible.row][possible.column].direction_from = static_cast<char>(curr_loc.level);
                       
                    }
                }
            }
        }
    }
}

void BackToShip::backtrack(){

    element curr_loc = ending_pos;
    size_t level = 0;

    if(is_solution == true){

        while(1){

            if(curr_loc.level == starting_pos.level
            && curr_loc.row == starting_pos.row
            && curr_loc.column == starting_pos.column){
                break;
            }

            if(map[curr_loc.level][curr_loc.row][curr_loc.column].direction_from == 'n'){
                curr_loc.row += 1;
                map[curr_loc.level][curr_loc.row][curr_loc.column].indicator = 'n';
            }
            else if(map[curr_loc.level][curr_loc.row][curr_loc.column].direction_from == 'e'){
                curr_loc.column -= 1;
                map[curr_loc.level][curr_loc.row][curr_loc.column].indicator = 'e';
            }
            else if(map[curr_loc.level][curr_loc.row][curr_loc.column].direction_from == 's'){
                curr_loc.row -= 1;
                map[curr_loc.level][curr_loc.row][curr_loc.column].indicator = 's';
            }
            else if(map[curr_loc.level][curr_loc.row][curr_loc.column].direction_from == 'w'){
                curr_loc.column += 1;
                map[curr_loc.level][curr_loc.row][curr_loc.column].indicator = 'w';
            }
            else{

                level = static_cast<size_t>(map[curr_loc.level][curr_loc.row][curr_loc.column].direction_from);
                
                map[level][curr_loc.row][curr_loc.column].indicator = static_cast<char>(curr_loc.level);

                curr_loc.level = static_cast<size_t>(map[curr_loc.level][curr_loc.row][curr_loc.column].direction_from);

            }
        }
    }
}

void BackToShip::print_output(){

    if(Opt.output_mode == 'M'){

        cout << "Start in level " << starting_pos.level << ", row " << starting_pos.row << ", column " << starting_pos.column << "\n";

        for(size_t k = 0; k < num_levels ; k ++){
            cout << "//level " << k << "\n";
            for(size_t i = 0; i < map_size ; i ++){
                for(size_t j = 0; j < map_size ; j ++){
                    //static_cast<size_t>
                    if(map[k][i][j].indicator != 'n'
                    && map[k][i][j].indicator != 'e'
                    && map[k][i][j].indicator != 's'
                    && map[k][i][j].indicator != 'w'
                    && map[k][i][j].indicator != '.'
                    && map[k][i][j].indicator != '#'
                    && map[k][i][j].indicator != 'H'
                    && map[k][i][j].indicator != 'E'
                    && map[k][i][j].indicator != 'S'){
                            cout << static_cast<size_t>(map[k][i][j].indicator) ;
                    }
                    else cout << map[k][i][j].indicator ;
                }
                cout << "\n";
            }
        }
    }
    else if(Opt.output_mode == 'L'){

        cout << "//path taken\n";
    
        if(is_solution == true){

            element curr_loc = starting_pos;
            
            while(1) {

                if(map[curr_loc.level][curr_loc.row][curr_loc.column].indicator == 'H'){
                    break;
                }

                if(map[curr_loc.level][curr_loc.row][curr_loc.column].indicator != 'n'
                && map[curr_loc.level][curr_loc.row][curr_loc.column].indicator != 'e'
                && map[curr_loc.level][curr_loc.row][curr_loc.column].indicator != 's'
                && map[curr_loc.level][curr_loc.row][curr_loc.column].indicator != 'w'
                && map[curr_loc.level][curr_loc.row][curr_loc.column].indicator != '.'
                && map[curr_loc.level][curr_loc.row][curr_loc.column].indicator != 'H'
                && map[curr_loc.level][curr_loc.row][curr_loc.column].indicator != '#'
                && map[curr_loc.level][curr_loc.row][curr_loc.column].indicator != 'E'
                && map[curr_loc.level][curr_loc.row][curr_loc.column].indicator != 'S'){
                    cout << "(" << curr_loc.level << "," << curr_loc.row << "," << curr_loc.column << ","
                    << static_cast<size_t>(map[curr_loc.level][curr_loc.row][curr_loc.column].indicator) << ")\n";
                }

                else cout << "(" << curr_loc.level << "," << curr_loc.row << ","
                << curr_loc.column << "," << map[curr_loc.level][curr_loc.row][curr_loc.column].indicator << ")\n";

                if(map[curr_loc.level][curr_loc.row][curr_loc.column].indicator == 'n'){
                    curr_loc.row -= 1;
                }
                else if(map[curr_loc.level][curr_loc.row][curr_loc.column].indicator == 'e'){
                    curr_loc.column += 1;
                }
                else if(map[curr_loc.level][curr_loc.row][curr_loc.column].indicator == 's'){
                    curr_loc.row += 1;
                }
                else if(map[curr_loc.level][curr_loc.row][curr_loc.column].indicator == 'w'){
                    curr_loc.column -= 1;
                }
                else{
                    curr_loc.level = static_cast<size_t>(map[curr_loc.level][curr_loc.row][curr_loc.column].indicator);
                }
            }
        }
    }
}



int
main(int argc, char *argv[])
{
    std::ios::sync_with_stdio(false);
    BackToShip bts;
    bts.get_options(argc, argv);
    bts.readmap();
    bts.search();
    bts.backtrack();
    bts.print_output();

    return 0;
}
