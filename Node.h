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
  std::vector<double> p;
  double v; // intrinsic value of node state from neural network
  public:
    double Q; // average value updated according to MCTS
    double N; // number of visits to this node
    double W; // unweighted value of this path;
    std::vector<Node *> children; // vector containing addresses of all the child nodes
    int num_children; // number of children this node has
    Node(); // constructor
    ~Node(); // destructor
    void set_children(int); // sets num_children and creates node instances
    int arg_max(std::vector<double>); // arg_max
    int select_action(); // policy for traversing the tree
    bn::ndarray get_policy(double tau); // policy returned by the MCTS
    void set_terminal(bool); // no valid moves from this board state
    void set_values(bn::ndarray *, double, int);
    void set_child_values(bp::list *, bp::list *, bp::list *, bp::list *);
    bool get_visited(); // visited getter
    bool get_terminal(); // terminal getter
    Node * get_child(int);

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
  // printf("Selecting action\n");
  double s = 0;

  for (int i = 0; i < num_children; i++){
    u[i] = p[i]/(1+children[i]->N);
    q[i] = children[i]->Q;
    s += u[i];
    // printf("(%f, %f) ", u[i], q[i]);
  }

  for (int i = 0; i < num_children; i++){
    u[i] = u[i]/s + q[i];
    // printf("%f ", u[i]);
  }
  // printf("\n");
  // printf("printed action!\n");
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

void Node::set_values(bn::ndarray * policy, double value, int size){
  np2doublevec(policy, &p, size);
  v = value;
  Q = value;
  W = Q/N;
}
void Node::set_child_values(bp::list * policies, bp::list * values, bp::list * game_over, bp::list * num_chil){
  // printf("Node:: terminal %d\n", terminal);
  if (terminal){
    return;
  }
  for (int i = 0; i<num_children; i++){
    int terminal_child = (int)(bp::extract<double>((*game_over)[i]));
    if (terminal_child == 1){ // if no valid moves from this state
      children[i]->set_terminal(true);
      // printf("Node:: my child is terminal %d\n", terminal_child);
    }
    else{
      // printf("Node:: my child is not terminal %d\n", terminal_child);
      bn::ndarray policy = bp::extract<bn::ndarray>((*policies)[i]);
      double value = bp::extract<double>((*values)[i]);
      int num = (int)(bp::extract<double>((*num_chil)[i]));
      children[i]->set_children(num);
      children[i]->set_values(&policy, value, num);
    }
  }
  visited = true;
}
void Node::set_children(int num_children){
  this->num_children = num_children;
  children.resize(num_children);
  for(int i = 0; i<num_children; i++){
    children[i] = new Node();
  }
}
void Node::set_terminal(bool val){
  terminal = val;
}
Node * Node::get_child(int idx){
  return(children[idx]);
}
bool Node::get_visited(){
  return visited;
}
bool Node::get_terminal(){
  return terminal;
}


// BOOST_PYTHON_MODULE(Node)
// {
//   using namespace boost::python;
//   numpy::initialize();
//   class_<Node, boost::noncopyable>("Node")
//     .def("set_values", &Node::set_values)
//     .def("set_child_values", &Node::set_child_values)
//     .def("set_children", &Node::set_children)
//     .def("get_terminal", &Node::get_terminal)
//     .def("get_visited", &Node::get_visited)
//     .def("get_child", &Node::get_visited)
//     .def("select_action", &Node::select_action)
//     .def_readwrite("W", &Node::W)
//     .def_readwrite("N", &Node::N)
//     .def_readwrite("Q", &Node::Q);
// }
