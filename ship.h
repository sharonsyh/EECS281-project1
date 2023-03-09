// Project Identifier: 950181F63D0A883F183EC0A5CC67B19928FE896A
#include <vector>
#include <iostream>
#include <algorithm> 
#include <getopt.h>
#include <string>

using namespace std;

class BackToShip{

    public:

        BackToShip(){
            map_size = 0;
            num_levels = 0;
        }

        struct element{
            size_t level;
            size_t row;
            size_t column;
        } elements;

        struct property{
            char indicator;
            bool visited = false;
            char direction_from = 'A'; // which direction it was moved from the previous location
        } properties;

    void get_options(int argc, char** argv);
    bool readmap();
    void search();
    void backtrack();
    void print_output();

    private:

        vector<vector<vector<property> >> map;

        size_t map_size ;
        size_t num_levels ;
        bool stack = false ;
        bool queue = false ;
        bool is_solution = false;
        element starting_pos;
        element ending_pos;

};
