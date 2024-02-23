// 
// 234218 Data Structures 1.
// Semester: 2024A (winter).
// Wet Exercise #1.
// 
// Recommended TAB size to view this file: 8.
// 
// The following main file is necessary to link and run your code.
// This file is READ ONLY: even if you submit something else, the compiler ..
// .. WILL use our file.
// 

#include "Olympicsa1.h"
#include <string>
#include <iostream>

using namespace std;

void print(string cmd, StatusType res);
void print(string cmd, output_t<int> res);

int main()
{
	
    int d1, d2, d3, g1;
    string b1;
    bool b;

    // Init
    Olympics *obj = new Olympics();
	
    // Execute all commands in file
	string op;
	while (cin >> op)
    {
        if (!op.compare("add_country")) {
            cin >> d1 >> d2;
            print(op, obj->add_country(d1, d2));
        } else if (!op.compare("remove_country")) {
            cin >> d1;
            print(op, obj->remove_country(d1));
        } else if (!op.compare("add_team")) {
            cin >> d1 >> d2 >> d3;
            print(op, obj->add_team(d1, d2, (Sport)d3));
        } else if (!op.compare("remove_team")) {
            cin >> d1;
            print(op, obj->remove_team(d1));
        } else if (!op.compare("add_contestant")) {
            cin >> d1 >> d2 >> d3 >> g1;
            print(op, obj->add_contestant(d1, d2, (Sport)d3, g1));
        } else if (!op.compare("remove_contestant")) {
            cin >> d1;
            print(op, obj->remove_contestant(d1));
        } else if (!op.compare("add_contestant_to_team")) {
            cin >> d1 >> d2;
            print(op, obj->add_contestant_to_team(d1, d2));
        } else if (!op.compare("remove_contestant_from_team")) {
            cin >> d1 >> d2;
            print(op, obj->remove_contestant_from_team(d1, d2));
        } else if (!op.compare("update_contestant_strength")) {
            cin >> d1 >> d2;
            print(op, obj->update_contestant_strength(d1, d2));
        } else if (!op.compare("get_strength")) {
            cin >> d1;
            print(op, obj->get_strength(d1));
        } else if (!op.compare("get_medals")) {
            cin >> d1;
            print(op, obj->get_medals(d1));
        } else if (!op.compare("get_team_strength")) {
            cin >> d1;
            print(op, obj->get_team_strength(d1));
        } else if (!op.compare("unite_teams")) {
            cin >> d1 >> d2;
            print(op, obj->unite_teams(d1, d2));
        } else if (!op.compare("play_match")) {
            cin >> d1 >> d2;
            print(op, obj->play_match(d1, d2));
        } else if (!op.compare("austerity_measures")) {
            cin >> d1;
            print(op, obj->austerity_measures(d1));
        } else {
            cout << "Unknown command: " << op << endl;
            return -1;
        }
        // Verify no faults
        if (cin.fail()){
            cout << "Invalid input format" << endl;
            return -1;
        }
    }

    // Quit 
	delete obj;
	return 0;
}

// Helpers
static const char *StatusTypeStr[] =
{
   	"SUCCESS",
	"ALLOCATION_ERROR",
	"INVALID_INPUT",
	"FAILURE"
};

void print(string cmd, StatusType res) 
{
	cout << cmd << ": " << StatusTypeStr[(int) res] << endl;
}

void print(string cmd, output_t<int> res)
{
    if (res.status() == StatusType::SUCCESS) {
	    cout << cmd << ": " << StatusTypeStr[(int) res.status()] << ", " << res.ans() << endl;
    } else {
	    cout << cmd << ": " << StatusTypeStr[(int) res.status()] << endl;
    }
}
