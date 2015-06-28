#include "../Headers/Map.h"
#include "../Headers/edge.h"
#include "../Headers/claim.h"

//Takes an x and y coordinate as input and creates a Map of that size filled with default nodes
/*Utilities::Map::Map(ProblemObject* problem_object) {
      this->num_connections = problem_object->get_connections().size();
      int height = problem_object->get_height();
      int width = problem_object->get_width();
      for(int y = 0; y < height; y++) {
		  vector<Node*> temp_row;
	      for(int x = 0; x < width; x++) {
			  Node* new_node = new Node(x,y);
			  if (x > 0) {
				  Edge* west = new Edge(new_node,temp_row.at(temp_row.size()-1));
				  new_node->add_connection(west);
			  }
			  if (y > 0) {
				  Edge* north = new Edge(new_node,Map.at(y-1).at(x));
				  new_node->add_connection(north);
			  }
			  temp_row.push_back(new_node);
          }
          this->Map.push_back(temp_row);
      }
}

//Destructs the Map by deleting each node individually, the node destructors will delete their own set of edges
Utilities::Map::~Map() {
      int width = this->get_width();
      int height = this->get_height();
      for(int x = 0; x < width; x++) {
            for(int y = 0; y < width; y++) {
                  delete Map.at(y).at(x);
            }
      }
}

int Utilities::Map::get_width() {
	//Assumes a perfect rectangle
	return Map.empty()?0:Map.at(0).size();
}

int Utilities::Map::get_height() {
	//Assumes a perfect rectangle
	return this->Map.size();
}

int Utilities::Map::get_num_connections() {
  return this->num_connections;
}

Node* Utilities::Map::get_node(int x, int y) {
      if(y >= this->Map.size()) {
        claim("Attemping to access a node outside of the Map's range (y-value out of range)", kError);
	    return NULL;
      }
      if(x >= this->Map.at(y).size()) {
        claim("Attemping to access a node outside of the Map's range (x-value out of range)", kError);
	    return NULL;
      }
      return this->Map.at(y).at(x);      //at(...) will throw an out of range exception
}

Node* Utilities::Map::get_node(Point coord) {
      return this->get_node(coord.x, coord.y);
}

vector<Path*> Utilities::Map::get_paths() {
      return this->paths;
}

Path* Utilities::Map::get_path(int i) {
      if(i >= paths.size()) {
            claim("Attempting to access a path outside of the path list's range", kError);
      }
      return this->paths.at(i);           //at(...) will throw an out of range exception
}

/*
Since each node has an x/y coordinate, if you pass a Node* into replace node, it will take the node at the passed in
Nodes location, delete it, and then place the passed in node into the Map at its proper location.
*/

/*
void Utilities::Map::replace_node(Node* replacement_node) {
      delete this->Map.at(replacement_node->get_y()).at(replacement_node->get_x());
      this->Map.at(replacement_node->get_y()).at(replacement_node->get_x()) = replacement_node;
}

void Utilities::Map::set_paths(vector<Path*> paths) {
      this->paths = paths;
}

void Utilities::Map::add_path(Path* path) {
      this->paths.push_back(path);
}

void Utilities::Map::replace_path(int i, Path* path) {
      if(i >= this->paths.size()) {
            claim("Attemping to replace path outside of the path list's range", kError);
      }
      paths.at(i) = path;
}

void Utilities::Map::remove_path(int i) {
      if(i >= paths.size()) {
      	claim("Attempting to remove a path outside of the path list's range", kError);
      }
      vector<Path*>::iterator it = this->paths.begin();
      it += i;
      paths.erase(it);
}

//Note, we create random paths just as an example of how to create paths, netlists are created similarly
vector<Path*> Utilities::Map::test_algorithm() {
    vector<Path*> paths;
    srand(time(NULL));
    int number_paths = this->get_num_connections();
    for (int i = 0;i < number_paths;i++) {
      Path* new_path = new Path();
      int x = rand() % this->get_width();
      int y = rand() % this->get_height();
      int path_length = 1+rand()%10;
      for (unsigned j = 0;j < path_length;j++) {
        bool direction = rand()%2;
        Point head(x,y);
        direction?x+=1:y+=1;
        Point tail(x,y);
        PathSegment* new_segment = new PathSegment(head,tail);
        new_path->add_segment(new_segment);
      }
      paths.push_back(new_path);
    }
    return paths;
}
*/