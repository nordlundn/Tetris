#include "Node.h"

class Tree{
  Node * root; // pointer to the root of the tree
  Node * current_node; // most recently visited node when making path
  std::vector<Node *> path; // needed for back propagation
  int tree_size;

  public:
    Tree(bn::ndarray, double, int);
    ~Tree();
    void set_values(bp::list, bp::list, bp::list, bp::list); // set policies and values for all children of current node in the path
    int step(); // interface with python for searching the tree
    void back_prop(); // update node values
    bn::ndarray get_policy(double tau);
};
Tree::Tree(bn::ndarray policy, double value, int num_children){
  root = new Node();
  tree_size = 1+num_children;
  root->set_children(num_children);
  root->set_values(&policy, value, num_children); // initialize root parameters
  current_node = root; // initialize tree search at the root.
}
Tree::~Tree(){
  delete root;
}

void Tree::set_values(bp::list policies, bp::list values, bp::list game_over, bp::list num_chil){ // update p and v for all children of node at end of path
  Node * node = path.back(); // get the last node in the tree search
  node->set_child_values(&policies, &values, &game_over, &num_chil);
  if (bp::len(num_chil) > 0){
    tree_size += (int)(bp::extract<double>(num_chil[0]));
  }
  back_prop();
  path.clear();
  current_node = root;
}
int Tree::step(){
    path.push_back(current_node); // add current node to path
    // printf("Current node terminal: %d\n", current_node->get_terminal());
    // printf("Current node children: %d\n", current_node->num_children);
    // printf("Current node visited: %d\n", current_node->get_visited());
    if (current_node->get_terminal()){
      return -2;
    }

    else if (!(current_node->get_visited())){
      return -1;
    }
    int next_child = current_node->select_action();
    current_node = current_node->children[next_child];
    return next_child;
}
void Tree::back_prop(){
  Node * new_child = path.back();
  double val = new_child->Q;
  for (int i = 0; i<(path.size()-1); i++){
    new_child = path[i];
    new_child->W += val;
    new_child->N += 1;
    new_child->Q = (new_child->W)/(new_child->N);
  }
}
bn::ndarray Tree::get_policy(double tau){
  return root->get_policy(tau);
}

BOOST_PYTHON_MODULE(Tree)
{
  using namespace boost::python;
  Py_Initialize();
  numpy::initialize();
  class_<Tree, boost::noncopyable>("Tree", init<numpy::ndarray, double, int>())
    .def("set_values", &Tree::set_values)
    .def("get_policy", &Tree::get_policy)
    .def("step", &Tree::step);
}
