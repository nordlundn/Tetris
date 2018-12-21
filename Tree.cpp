#include <stdlib.h>
#include <math.h>
#include <vector>
#include <array>
#include <boost/python.hpp>
#include "boost/python/numpy.hpp"
#include "vec2np.h"

namespace bp = boost::python;
namespace bn = boost::python::numpy;

class Node{
  bool visited;
  bool terminal;
  public:
    std::vector<double> p;
    double v; // intrinsic value of node state from neural network
    double Q; // average value updated according to MCTS
    double N; // number of visits to this node
    double W; // unweighted value of this path;
    int num_children; // number of children this node has
    std::vector<Node *> children; // vector containing addresses of all the child nodes
    Node(); // constructor
    ~Node(); // destructor
    void visit(int); // sets num_children and creates node instances
    void set(); // sets back_prop variables after node gets neural net params
    int arg_max(std::vector<double>); // arg_max
    int select_action(); // policy for traversing the tree
    bn::ndarray get_policy(double tau); // policy returned by the MCTS
    void set_terminal(bool); // no valid moves from this board state
    void set_visited(bool); // this node is visited
    bool get_visited(); // visited getter
    bool get_terminal(); // terminal getter
};
Node::Node(){
  Q = -1.0; // initial value for the weighted average
  N = 1; // number of visits is 1
  visited = false;
  terminal = false;
  num_children = 0;
}
Node::~Node(){
  for (int i = 0; i<num_children; i++){
    delete children[i];
  }
}
void Node::visit(int num_children){
  this->num_children = num_children;
  children.resize(num_children);
  for(int i = 0; i<num_children; i++){
    children[i] = new Node();
  }
}
void Node::set(){
  this->Q = this->v;
  W = Q/N;
}
int Node::arg_max(std::vector<double> arr){
  double max = arr[0];
  int max_idx = 0;
  for (int i = 0; i<arr.size(); i++){
    if (arr[i] > max){
      max = arr[i];
      max_idx = i;
    }
  }
  return max_idx;
}
int Node::select_action(){
  std::vector<double> u;
  std::vector<double> q;
  u.resize(num_children);
  q.resize(num_children);

  double s = 0;

  for (int i = 0; i < num_children; i++){
    u[i] = p[i]/(1+children[i]->N);
    q[i] = children[i]->Q;
  }

  for (int i = 0; i < num_children; i++){
    u[i] = u[i]/s + q[i];
  }
  return arg_max(u);
}
bn::ndarray Node::get_policy(double tau){
  std::vector<double> pi;
  pi.resize(num_children);
  double s = 0;
  for (int i = 0; i < num_children; i++){
    pi[i] = pow(children[i]->N, tau);
    s+= pi[i];
  }
  for (int i = 0; i < num_children; i++){
    pi[i]/=s;
  }
  return doublevec2np(&pi);

}
void Node::set_terminal(bool val){
  terminal = val;
}
void Node::set_visited(bool val){
  visited = val;
}
bool Node::get_visited(){
  return visited;
}
bool Node::get_terminal(){
  return terminal;
}

class Tree{
  Node * root; // pointer to the root of the tree
  Node * current_node; // most recently visited node when making path
  std::vector<Node *> path; // needed for back propagation

  public:
    Tree(bn::ndarray, double, int);
    ~Tree();
    void set_values(Node *, bn::ndarray *, double); // interface with python for neural network params
    void set_all_values(bp::list, bp::list, bp::list, bp::list); // set policies and values for all children of current node in the path
    int step(); // interface with python for searching the tree
    void back_prop(); // update node values
};
Tree::Tree(bn::ndarray p, double v, int num_children){
  root = new Node();
  root->visit(num_children);
  set_values(root, &p, v); // initialize root parameters
  current_node = root; // initialize tree search at the root.
}
Tree::~Tree(){
  delete root;
}
void Tree::set_values(Node * node, bn::ndarray * p, double v){
  np2doublevec(p, &(node->p), node->num_children);
  node->v = v;
  node->set();
}
void Tree::set_all_values(bp::list policies, bp::list values, bp::list game_over, bp::list num_chil){ // update p and v for all children of node at end of path
  // int num_children = end_of_path->num_children;
  Node * node = path.back(); // get the last node in the tree search
  if (node->get_terminal()){ // if there are no valid moves from this state, just back prop. No need to update children
    back_prop();
    path.clear(); // clear path for next search
    current_node = root; // reset start of path to root of the tree
    return;
  }
  int num_children = bp::len(game_over); // get the number of children for the node to be updated
  for (int i = 0; i < num_children; i++){ // iterate through all its children
    bn::ndarray policy = bp::extract<bn::ndarray>(policies[i]);
    int terminal = (int)(bp::extract<double>(game_over[i]));
    double val = bp::extract<double>(values[i]);
    int num = (int)(bp::extract<double>(num_chil[i]));
    set_values((node->children)[i], &policy, val);
    if (terminal == 1){ // if no valid moves from this state
      (node->children)[i]->set_terminal(true);
    }
    else{ // initialize children
      (node->children)[i]->visit(num);
    }
  }
  back_prop();
  node->set_visited(true);
  path.clear();
  current_node = root;
}
int Tree::step(){
    path.push_back(current_node); // add current node to path
    if (current_node->get_terminal()){ 
      return -2;
    }
    else if (!(current_node->get_visited())){
      return -1;
    }
    int next_child = current_node->select_action();
    current_node = (current_node->children)[next_child];
    return next_child;
}
void Tree::back_prop(){
  Node * new_child = path.back();
  double val = new_child->v;
  for (int i = 0; i<(path.size()-1); i++){
    new_child = path[i];
    new_child->W += val;
    new_child->N += 1;
    new_child->Q = new_child->W/new_child->N;
  }
}

BOOST_PYTHON_MODULE(Tree)
{
  using namespace boost::python;
  numpy::initialize();
  class_<Tree>("Tree", init<numpy::ndarray, double, int>())
    .def("set_all_values", &Tree::set_all_values)
    .def("step", &Tree::step);
}
