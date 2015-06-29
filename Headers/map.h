#ifndef _MAP_BASE_H_
#define _MAP_BASE_H_

#include "node.h"
#include "path.h"
#include "problem_object.h"
#include <vector>

using std::cerr;
using std::endl;
using std::string;
using Utilities::Node;
using Utilities::Path;

namespace Utilities {
	class Map {
	private:
		vector<vector<Node*> > map;
		int num_connections;
		bool found_end;    // added, used to end wave expansion and bactracing
		vector<Path*> paths;
		vector<Connection> connections;     // added, easy access to p_o connections

	public:
		/* Constructors/Destructors */
		Map(ProblemObject* problem_object);
		~Map();

		/* Accessors */
		int get_width();
		int get_height();
		int get_num_connections();
		Node* get_node(int x, int y);
		Node* get_node(Point coord);
		vector<Path*> get_paths();
		Path* get_path(int i);

		/* Mutators */
		void replace_node(Node* replacement_node);
		void set_paths(vector<Path*> paths);
		bool validate_blockers(Blocker block, int max_width, int max_height); // added
		void set_blockers(vector<Blocker> blockers);    // added
		bool validate_connections(Connection connections);    //added first
		// void set_connections(vector<Connection> connections); // don't need...?
		void wave_expansion(Node* cur_node);	// added
		Path* backtrace(Node* sink, Path* path);    // added
        bool traceable(Node* sink);    // added
		void add_path(Path* path);
		void replace_path(int i, Path* path);
		void remove_path(int i);
		void print_map();    // added

		/* Algorithms */
		vector<Path*> lee();
		vector<Path*> test_algorithm();
	};
}



#endif //_MAP_BASE_H_
