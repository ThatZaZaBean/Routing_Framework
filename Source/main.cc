
#include "../Headers/map.h"
#include "../Headers/problem_object.h"
#include <time.h>
#include <cstdlib>
#include <iostream>

using std::cerr;
using std::cout;
using std::endl;

int main(int argc,char* argv[]) {

	// DO NOT CHANGE THIS SECTION OF CODE
	if(argc < 2) { 
		cout << "Usage: ./Map_router <test_file>" << endl; 
		exit(1);
	}
	Utilities::ProblemObject* first_problem = new Utilities::ProblemObject(std::string(argv[1]));
	// EDIT FROM HERE DOWN

	//Create your problem map object (in our example, we use a simple Map, you should create your own)
	Utilities::Map g(first_problem);

	/*
	Note: we do not take into account the connections or blockers that exist in the Project Object
	You should be accouting for these in your problem map objects (you should not be using Map). You
	should be using both the Node and Edge classes as the background for whatever problem map object
	you create.
	*/

	/*
	Run your algorithm after creating your problem map object. You should be returning from your algorithm 
	either a Path or a Netlist

	Path: a series of straight line segments, with a single source and a single sink
	Netlist: a series of stright line segments, with a single source and more than one sink
	*/
	vector<Path*> paths = g.lee();

	//Print the paths/netlists that you return from your algorithm
	for(unsigned i = 0; i < paths.size(); i++) {
		cout << "Path " << i << ": ";
		paths.at(i)->print();
        printf("Path length: %d (sanity check)", paths.at(i)->size());
		Path* temp = paths.at(i);
		delete temp;
        printf("\n\n");
	}

	paths.clear();

	delete first_problem;

	return 0;
}
