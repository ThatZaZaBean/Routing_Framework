#include "../Headers/edge.h"
#include "../Headers/claim.h"
#include "../Headers/node.h"
#include "../Headers/map.h"
#include "../Headers/problem_object.h"

//Takes an x and y coordinate as input and creates a Map of that size filled with default nodes
Utilities::Map::Map(ProblemObject* problem_object) {
	this->connections = problem_object->get_connections();
	this->num_connections = problem_object->get_connections().size();
	int height = problem_object->get_height();
	int width = problem_object->get_width();
	this->found_end = false;
	for (int y = 0; y < height; y++) {
		vector<Node*> temp_row;
		for (int x = 0; x < width; x++) {
			Node* new_node = new Node(x, y);
			if (x > 0) {
				Edge* west = new Edge(new_node, temp_row.at(temp_row.size() - 1));
				new_node->add_connection(west);
			}
			if (y > 0) {
				Edge* north = new Edge(new_node, map.at(y - 1).at(x));
				new_node->add_connection(north);
			}
			temp_row.push_back(new_node);
		}
		this->map.push_back(temp_row);
	}
	this->set_blockers(problem_object->get_blockers());          // Sets the map block.
}

//Destructs the Map by deleting each node individually, the node destructors will delete their own set of edges
Utilities::Map::~Map() {
	int width = this->get_width();
	int height = this->get_height();
	for (int x = 0; x < width; x++) {
		for (int y = 0; y < width; y++) {
			delete map.at(y).at(x);
		}
	}
}

int Utilities::Map::get_width() {
	//Assumes a perfect rectangle
	return map.empty() ? 0 : map.at(0).size();
}

int Utilities::Map::get_height() {
	//Assumes a perfect rectangle
	return this->map.size();
}

int Utilities::Map::get_num_connections() {
	return this->num_connections;
}

Node* Utilities::Map::get_node(int x, int y) {
	if (y >= this->map.size()) {
		claim("Attemping to access a node outside of the Map's range (y-value out of range)", kError);
		return NULL;
	}
	if (x >= this->map.at(y).size()) {
		claim("Attemping to access a node outside of the Map's range (x-value out of range)", kError);
		return NULL;
	}
	return this->map.at(y).at(x);      //at(...) will throw an out of range exception
}

Node* Utilities::Map::get_node(Point coord) {
	return this->get_node(coord.x, coord.y);
}

vector<Path*> Utilities::Map::get_paths() {
	return this->paths;
}

Path* Utilities::Map::get_path(int i) {
	if (i >= paths.size()) {
		claim("Attempting to access a path outside of the path list's range", kError);
	}
	return this->paths.at(i);           //at(...) will throw an out of range exception
}

/*
Since each node has an x/y coordinate, if you pass a Node* into replace node, it will take the node at the passed in
Nodes location, delete it, and then place the passed in node into the Map at its proper location.
*/
void Utilities::Map::replace_node(Node* replacement_node) {
	delete this->map.at(replacement_node->get_y()).at(replacement_node->get_x());
	this->map.at(replacement_node->get_y()).at(replacement_node->get_x()) = replacement_node;
}

void Utilities::Map::set_paths(vector<Path*> paths) {
	this->paths = paths;
}

void Utilities::Map::add_path(Path* path) {
	this->paths.push_back(path);
}

void Utilities::Map::replace_path(int i, Path* path) {
	if (i >= this->paths.size()) {
		claim("Attemping to replace path outside of the path list's range", kError);
	}
	paths.at(i) = path;
}

void Utilities::Map::remove_path(int i) {
	if (i >= paths.size()) {
		claim("Attempting to remove a path outside of the path list's range", kError);
	}
	vector<Path*>::iterator it = this->paths.begin();
	it += i;
	paths.erase(it);
}

// Basic visual display of map using relative costs of nodes
void Utilities::Map::print_map()
{
	int max_height = this->get_height();
	int max_width = this->get_width();
    printf("\n\n");
	for (unsigned int x = 0; x < max_width; x++)
	{
		for (unsigned int y = 0; y < max_height; y++)
		{
			printf("%4d", this->map.at(x).at(y)->get_cost());
		}
		printf("\n");
	}
    printf("\n");
}

// New Mutator(s) Here

/*
    Parameter blockers (vector<Blocker>): Used to set blocker nodes within the map

    Return void: "removes" blocker nodes from map within function.
*/
void Utilities::Map::set_blockers(vector<Blocker> blockers) {

	int x = 0, y = 0;
	int max_height = this->get_height(), max_width = this->get_width();    // Used for blocker bounds checking
	unsigned int i = 0;

	block_loop:
	for (; i < blockers.size(); i++)
	{
		if (!(this->validate_blockers(blockers.at(i), max_width, max_height))) {
			++i;
			goto block_loop;    // moves on to the next loop if a bad block is found
		}

		x = blockers.at(i).location.x;
		y = blockers.at(i).location.y;
        
		// Remove block from map
		for (unsigned int x_coord = 0; x_coord < blockers.at(i).width; x_coord++)
		{
			for (unsigned int y_coord = 0; y_coord < blockers.at(i).height; y_coord++)
			{
				map.at(y + y_coord).at(x + x_coord)->set_cost(-1);   // we treat -1 as a wall          
			}
		}
	}
    this->print_map();
}

/*
	Parameter block (Blocker): The current block to validate
	          max_width/max_height: passed in so that they wouldn't have to be continuously calculated

	Return bool: Whether the current block is viable within out given map.
*/
bool Utilities::Map::validate_blockers(Blocker block, int max_width, int max_height) {
	/* save in case I mess up

	x = blockers.at(i).location.x;
	y = blockers.at(i).location.y;
	block_width = blockers.at(i).width;
	block_height = blockers.at(i).height;
	*/
	int x = block.location.x;
	int y = block.location.y;
	// check if block position is valid then if there is room for the block
	if (x > max_width)	{
		claim("The Blockers X coord is too large!", kError);
		return false;
	}
	else if (x < 0)	{
		claim("The Blockers X coord is too small!", kError);
		return false;
	}
	else if ((x + block.width) > max_width){
		claim("The current block doesn't fit along the X axis!", kError);
		return false;
	}

	if (y > max_height)	{
		claim("The Blockers Y coord is too large!", kError);
		return false;
	}
	else if (y < 0)	{
		claim("The Blockers Y coord is too small!", kError);
		return false;
	}
	else if ((y + block.height) > max_height){
		claim("The current block doesn't fit along the Y axis!", kError);
		return false;
	}
	return true;
}

//Note, we create random paths just as an example of how to create paths, netlists are created similarly
vector<Path*> Utilities::Map::test_algorithm() {
	vector<Path*> paths;
	srand(time(NULL));
	int number_paths = this->get_num_connections();
	for (int i = 0; i < number_paths; i++) {
		Path* new_path = new Path();
		int x = rand() % this->get_width();
		int y = rand() % this->get_height();
		int path_length = 1 + rand() % 10;
		for (unsigned j = 0; j < path_length; j++) {
			bool direction = rand() % 2;
			Point head(x, y);
			direction ? x += 1 : y += 1;
			Point tail(x, y);
			PathSegment* new_segment = new PathSegment(head, tail);
			new_path->add_segment(new_segment);
		}
		paths.push_back(new_path);
	}
	return paths;
}

/*

Parameter none: Implements the base lee's algorithm
Return vector<Path*>: Returns a vector of shortest paths from
their respective connections.

*/
vector<Path*> Utilities::Map::lee() {

	int max_connections = this->get_num_connections();
	
	int max_height = this->get_height(), max_width = this->get_width();
    unsigned int i = 0;
    
	connection_loop:
	for (; i < max_connections; i++)
	{		
		if (!(this->validate_connections(connections.at(i)))) { // checks if source/sink are valid
			++i;
			goto connection_loop;
		}

		// Reset map costs after each iteration
		for (unsigned int x = 0; x < max_width; x++)
		{
			for (unsigned int y = 0; y < max_height; y++)
			{
				if (this->map.at(x).at(y)->get_cost() != -1) {
					this->map.at(x).at(y)->set_cost(0);
				}
			}
		}

        this->print_map();
        
		//TODO check if source is also sink

		Node* source = this->map.at(this->connections.at(i).source.x).at(this->connections.at(i).source.y);
		source->set_cost(-2); 
		Node* sink = this->map.at(this->connections.at(i).sink.x).at(this->connections.at(i).sink.y);
		sink->set_cost(-3); // once we find -3. we're done
        this->print_map();

        if ( source == sink ) {    // Check if source is sink
            ++i;
            Path* new_path = new Path();
            new_path->set_source(Point(source->get_x(), source->get_y()));
            new_path->set_sink(Point(source->get_x(), source->get_y()));
            paths.push_back(new_path);
            goto connection_loop;
        }

		this->wave_expansion(source);    // Fills out map with all node costs
		found_end = false;
        this->print_map();

		Path* new_path = new Path();
		paths.push_back(this->backtrace(sink, new_path));    // Determines the lowest cost path and pushes it onto paths
	}
    return paths;
}

/*

Parameter connections (Connection): The current source and sink
Return Bool: Whether or not the source and sink are within bounds

*/
bool Utilities::Map::validate_connections(Connection connections) {

	if (connections.source.x < 0 || connections.source.x < 0) {
		claim("The connections source is invalid (negative) !!", kError);
		return false;
	}
	else if (connections.sink.x < 0|| connections.sink.y < 0) {
		claim("The connections sink is invalid (negative) !!", kError);
		return false;
	}
	else if (connections.source.x > this->get_width() || connections.source.y > this->get_height()) {
		claim("The connections source is invalid (out of bounds) !!", kError);
		return false;
	}
	else if (connections.sink.x > this->get_width() || connections.sink.y > this->get_height()) {
		claim("The connections sink is invalid (out of bounds) !!", kError);
		return false;
	}
	return true;
}

/*

Parameter cur_node (Node*): The current map position.
Used to recursively update all valid positions within the map
Return nothing.

*/
void Utilities::Map::wave_expansion(Node* cur_node){ // not quite right, pretty sure this infinite loops

    /*
     * I need a better way of determining when to end. As of right now it gives a single solution
     * before all solutions are calculated. My only current fix to this is is to disable the return
     * statement. The algorithm then calculates EVERY node, and that is far too slow for
     * practical usage, even though it gives a correct mapping.
     * */
	//if (found_end){ // done with wave expansion
		//return; 
	//}
    
    // Up
    if (cur_node->get_y() + 1 < this->get_height()) { // Check bounds
        
        Node* up = this->get_node(cur_node->get_x(), cur_node->get_y() + 1);        
        if (up->get_cost() == -1 || up->get_cost() == -2) { // check if part of a block
		    // do nothing, hit a wall
	    }
	    else if (up->get_cost() == -3) { // check if we hit the sink
		    found_end = true;
            up->set_cost(cur_node->get_cost() + 1); // replace sink's old cost w/ real cost
	    }
	    else if ((up->get_cost() <= (cur_node->get_cost())) && up->get_cost() > 0) { // check if a node with higher cost tries to overwrite another node
		    // do nothing, no need to update
	    }
	    else { 
            if (cur_node->get_cost() == -2 ) { // if cur node is the start node
                up->set_cost(1);
                wave_expansion(up);
            }
            else {                
                up->set_cost(cur_node->get_cost() + 1);
                wave_expansion(up);
            }
	    }
    }
    // Down
    if (cur_node->get_y() - 1 >= 0) {
        
        Node* down = this->get_node(cur_node->get_x(), cur_node->get_y() - 1);
        if (down->get_cost() == -1 || down->get_cost() == -2) { // check if part of a block or source
            // do nothing, hit the bottom or source
        }
        else if (down->get_cost() == -3) { // check if we hit the sink
            found_end = true;
            down->set_cost(cur_node->get_cost() + 1); // replace sink's old cost w/ real cost
        }
        else if ((down->get_cost() <= (cur_node->get_cost())) && down->get_cost() > 0) { // check if a node with higher cost tries to overwrite another node
            // do nothing, no need to update
        }
        else {
            if (cur_node->get_cost() == -2 ) { // if cur node is the start node
                down->set_cost(1);
                wave_expansion(down);
            }
            else {                
                down->set_cost(cur_node->get_cost() + 1);
                wave_expansion(down);
            }
        }
    }    
    // Right
    if (cur_node->get_x() + 1 < this->get_width()) {
        
        Node* right = this->get_node(cur_node->get_x() + 1, cur_node->get_y());
        if (right->get_cost() == -1 || right->get_cost() == -2) { // check if part of a block or source
            // do nothing, hit the bottom or source
        }
        else if (right->get_cost() == -3) { // check if we hit the sink
            found_end = true;
            right->set_cost(cur_node->get_cost() + 1); // replace sink's old cost w/ real cost
        }
        else if ((right->get_cost() <= (cur_node->get_cost())) && right->get_cost() > 0) { // check if a node with higher cost tries to overwrite another node
            // do nothing, no need to update
        }
        else {
            if (cur_node->get_cost() == -2 ) { // if cur node is the start node
                right->set_cost(1);
                wave_expansion(right);
            }
            else {                
                right->set_cost(cur_node->get_cost() + 1);
                wave_expansion(right);
            }
        }
    }
    // Left
    if (cur_node->get_x() - 1 >= 0) {
     
        Node* left = this->get_node(cur_node->get_x() - 1, cur_node->get_y());
        if (left->get_cost() == -1 || left->get_cost() == -2) { // check if part of a block or source
            // do nothing, hit a wall or source or source
        }
        else if (left->get_cost() == -3) { // check if we hit the sink
            found_end = true;
            left->set_cost(cur_node->get_cost() + 1); // replace sink's old cost w/ real cost
        }
        else if ((left->get_cost() <= (cur_node->get_cost())) && left->get_cost() > 0) { // check if a node with higher cost tries to overwrite another node
            // do nothing, no need to update
        }
        else {
            if (cur_node->get_cost() == -2 ) { // if cur node is the start node
                left->set_cost(1);
                wave_expansion(left);
            }
            else {                
                left->set_cost(cur_node->get_cost() + 1);
                wave_expansion(left);
            };
        }
    }
    
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
    /*
    srand(time(NULL));    
    while(!found_end)
    {
        int path_choice = rand() % 4;
        
        if (path_choice == 1) {    // Up            
            if (cur_node->get_y() + 1 < this->get_height()) { // Check bounds
                
                int old_cost = cur_node->get_cost(); // Save previous cost                
                cur_node = this->get_node(cur_node->get_x(), cur_node->get_y() + 1); 
                       
                if (cur_node->get_cost() == -1 || cur_node->get_cost() == -2) { // check if part of a block
                    // hit a wall, go back to old spot
                    cur_node = this->get_node(cur_node->get_x(), cur_node->get_y() - 1);
                }
                else if (cur_node->get_cost() == -3) { // check if we hit the sink
                    found_end = true;
                }
                else if ((cur_node->get_cost() <= old_cost ) && cur_node->get_cost() > 0) { // check if a node with higher cost tries to overwrite another node
                    // do nothing, no need to update
                }
                else { 
                    if (old_cost == -2 ) { // if cur node is the start node
                        cur_node->set_cost(1);
                    }
                    else {                
                        cur_node->set_cost(old_cost + 1);
                    }
                }
            } 
        }
        else if (path_choice == 2) {   // Down
        
            if (cur_node->get_y() - 1 >= 0) { // Check bounds
                
                int old_cost = cur_node->get_cost(); // Save previous cost                
                cur_node = this->get_node(cur_node->get_x(), cur_node->get_y() - 1); 
                       
                if (cur_node->get_cost() == -1 || cur_node->get_cost() == -2) { // check if part of a block
                    // hit a wall, go back to old spot
                    cur_node = this->get_node(cur_node->get_x(), cur_node->get_y() + 1);
                }
                else if (cur_node->get_cost() == -3) { // check if we hit the sink
                    found_end = true;
                }
                else if ((cur_node->get_cost() <= old_cost ) && cur_node->get_cost() > 0) { // check if a node with higher cost tries to overwrite another node
                    // do nothing, no need to update
                }
                else { 
                    if (old_cost == -2 ) { // if cur node is the start node
                        cur_node->set_cost(1);
                    }
                    else {                
                        cur_node->set_cost(old_cost + 1);
                    }
                }
            } 
        }
        else if (path_choice == 3) {   // Left
        
            if (cur_node->get_x() - 1 >= 0) { // Check bounds
                
                int old_cost = cur_node->get_cost(); // Save previous cost                
                cur_node = this->get_node(cur_node->get_x(), cur_node->get_y() - 1); 
                       
                if (cur_node->get_cost() == -1 || cur_node->get_cost() == -2) { // check if part of a block
                    // hit a wall, go back to old spot
                    cur_node = this->get_node(cur_node->get_x() + 1, cur_node->get_y());
                }
                else if (cur_node->get_cost() == -3) { // check if we hit the sink
                    found_end = true;
                }
                else if ((cur_node->get_cost() <= old_cost ) && cur_node->get_cost() > 0) { // check if a node with higher cost tries to overwrite another node
                    // do nothing, no need to update
                }
                else { 
                    if (old_cost == -2 ) { // if cur node is the start node
                        cur_node->set_cost(1);
                    }
                    else {                
                        cur_node->set_cost(old_cost + 1);
                    }
                }
            } 
        }
        else if (path_choice == 4) {   // Right
            
            if (cur_node->get_x() + 1 < this->get_width()) { // Check bounds
                
                int old_cost = cur_node->get_cost(); // Save previous cost                
                cur_node = this->get_node(cur_node->get_x(), cur_node->get_y() - 1); 
                       
                if (cur_node->get_cost() == -1 || cur_node->get_cost() == -2) { // check if part of a block
                    // hit a wall, go back to old spot
                    cur_node = this->get_node(cur_node->get_x() - 1, cur_node->get_y());
                }
                else if (cur_node->get_cost() == -3) { // check if we hit the sink
                    found_end = true;
                }
                else if ((cur_node->get_cost() <= old_cost ) && cur_node->get_cost() > 0) { // check if a node with higher cost tries to overwrite another node
                    // do nothing, no need to update
                }
                else { 
                    if (old_cost == -2 ) { // if cur node is the start node
                        cur_node->set_cost(1);
                    }
                    else {                
                        cur_node->set_cost(old_cost + 1);
                    }
                }
            }
        }
        
    }*/
    //return;
}

/*

Parameter sink (Node*): The point to begin backtracing
          path (Path*): The current list of (points)?
Return Path*: The shortest path as determined by lee's algorithm

*/
Path* Utilities::Map::backtrace(Node* sink, Path* path) {


	// first push cur_node onto path
	// figure out how to do this and we're done

	//if (cur_node->get_cost() == -2) {
	//	return path;
	//}	

    /* What used to be QQ
	Node* right = this->get_node(cur_node->get_x() + 1, cur_node->get_y());
	Node* left = this->get_node(cur_node->get_x() - 1, cur_node->get_y());
    */
    
    // all we are doing is looking for the current cost - 1, then backtracing from there
    // rewriting, too many edge cases, looks sloppy
    /*if (cur_node->get_cost() == -3) // At the sink
    {
        
    }
    else if ((cur_node->get_y() + 1 < this->get_height()) && 
        (this->get_node(cur_node->get_x(), cur_node->get_y() + 1)->get_cost() == cur_node->get_cost() -1 ||
         this->get_node(cur_node->get_x(), cur_node->get_y() + 1)->get_cost() == -2 ))
    {
            backtrace(this->get_node(cur_node->get_x(), cur_node->get_y() + 1), path);
    }    
    // Down
    else if ((cur_node->get_y() - 1 > 0 ) && 
        (this->get_node(cur_node->get_x(), cur_node->get_y() - 1)->get_cost() == cur_node->get_cost() -1 ||
         this->get_node(cur_node->get_x(), cur_node->get_y() - 1)->get_cost() == -2 ))
    {
            backtrace(this->get_node(cur_node->get_x(), cur_node->get_y() - 1), path);
    }
    // Right
    else if ((cur_node->get_x() + 1 < this->get_width()) && 
        (this->get_node(cur_node->get_x() + 1, cur_node->get_y())->get_cost() == cur_node->get_cost() -1 ||
         this->get_node(cur_node->get_x() + 1, cur_node->get_y())->get_cost() == -2 ))
    {
            backtrace(this->get_node(cur_node->get_x() + 1, cur_node->get_y()), path);
    }
    // Left
    else if ((cur_node->get_x() - 1 > 0 ) && 
        (this->get_node(cur_node->get_x() - 1, cur_node->get_y())->get_cost() == cur_node->get_cost() -1 ||
         this->get_node(cur_node->get_x() - 1, cur_node->get_y())->get_cost() == -2 ))
    {
            backtrace(this->get_node(cur_node->get_x() - 1, cur_node->get_y()), path);
    }
    else {
        claim("Unknown pathing error occured!", kError);
        return path;
    }*/
    
    // Check if surrounding of sink is all -1s/0s i.e. there is no solution
    if (!this->traceable(sink)) {
        return path;
    }
    
    // TODO: add sink to path
    int x = sink->get_x();
    int y = sink->get_y();
    
    while(!found_end)
    {
        // Up
        if( y + 1 < this->get_height() && (map.at(x).at(y + 1)->get_cost() == map.at(x).at(y)->get_cost() - 1 
        || map.at(x).at(y + 1)->get_cost() == -2 ))
        {
            // TODO: push node onto path
            if (map.at(x).at(y + 1)->get_cost() == -2) {
                found_end = true;
            }
            printf("Up\n");
            y++;
        }
        // Down
        else if( y - 1 > 0 && (map.at(x).at(y - 1)->get_cost() == map.at(x).at(y)->get_cost() - 1 
        || map.at(x).at(y - 1)->get_cost() == -2 ))
        {
            // TODO: push node onto path
            if (map.at(x).at(y - 1)->get_cost() == -2) {
                found_end = true;
            }
            printf("Down\n");
            y--;
        }
        // Left
        else if( x - 1 > 0 && (map.at(x - 1).at(y)->get_cost() == map.at(x).at(y)->get_cost() - 1 
        || map.at(x - 1).at(y)->get_cost() == -2 ))
        {
            // TODO: push node onto path
            if (map.at(x - 1).at(y)->get_cost() == -2) {
                found_end = true;
            }
            printf("Left\n");
            x--;
        }
        // Right
        else if( x + 1 < this->get_width() && (map.at(x + 1).at(y)->get_cost() == map.at(x).at(y)->get_cost() - 1 
        || map.at(x + 1).at(y)->get_cost() == -2 ))
        {
            // TODO: push node onto path
            if (map.at(x + 1).at(y)->get_cost() == -2) {
                found_end = true;
            }
            printf("Right\n");
            x++;
        }
        else {
            claim("Unknown pathing error occured!", kError);
            return path;
        }
    }   
}

/*

Parameter sink (Node*): The point to begin backtracing
Return bool: Whether or not there is a searchable path

*/
bool Utilities::Map::traceable(Node* sink) {

    int x = sink->get_x();
    int y = sink->get_y();
    // Goes through and checks if all points around sink are 0/-1
    if ( y + 1 == this->get_height() || ((map.at(x).at(y + 1)->get_cost() == 0 ||
       (map.at(x).at(y + 1)->get_cost() == -1)))) {        
        if ( y - 1 < 0 || ((map.at(x).at(y - 1)->get_cost() == 0 ||
           (map.at(x).at(y - 1)->get_cost() == -1)))) {
            if ( x + 1 == this->get_width() || ((map.at(x + 1).at(y)->get_cost() == 0 ||
               (map.at(x + 1).at(y)->get_cost() == -1)))) {
                if ( x - 1 < 0 || ((map.at(x - 1).at(y)->get_cost() == 0 ||
                   (map.at(x - 1).at(y)->get_cost() == -1)))) {
                    return false;
                }
            }
        }
    }
    return true;
}
