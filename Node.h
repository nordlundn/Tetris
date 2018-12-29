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
    double * policy_arr;

    double Q; // average value updated according to MCTS
    double N; // number of visits to this node
    double W; // unweighted value of this path;
    std::vector<std::shared_ptr<Node>> children; // vector containing addresses of all the child nodes
    int num_children; // number of children this node has
    Node(); // constructor
    ~Node(); // destructor
    int arg_max(std::vector<double>); // arg_max
    int select_action(); // policy for traversing the tree
    bn::ndarray get_pi(double tau); // policy returned by the MCTS
    void set_children(int); // sets num_children and creates node instances
    void set_terminal(bool); // no valid moves from this board state
    bool get_visited(); // visited getter
    bool get_terminal(); // terminal getter
    Node * get_child(int);
    bn::ndarray get_policy();
    void set_params(double, int);
    void set_visited(bool);
    void print_policy();
    void set_grandchildren(int, int);
    void set_child_params(double, int, int);
};
Node::Node(){
  // printf("Made it here\n");
  Q = -1.0; // initial value for the weighted average
  N = 1; // number of visits is 1
  visited = false;
  terminal = false;
  num_children = 0;
}
Node::~Node(){
  if (num_children > 0){
    delete [] policy_arr;
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
    u[i] = policy_arr[i]/(1+children[i]->N);
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
bn::ndarray Node::get_pi(double tau){
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
void Node::set_children(int num_children){
  // printf("setting children %d\n", num_children);
  this->num_children = num_children;
  policy_arr = new double[num_children];
  children.resize(num_children);
  for(int i = 0; i<num_children; i++){
    policy_arr[i] = 0.0;
    children[i] = std::make_shared<Node>();
  }
}
void Node::set_terminal(bool val){
  terminal = val;
}
Node * Node::get_child(int idx){
  return((children[idx]).get());
}
bool Node::get_visited(){
  return visited;
}
bool Node::get_terminal(){
  return terminal;
}
bn::ndarray Node::get_policy(){
  return bn::from_data(policy_arr, bn::dtype::get_builtin<double>(), bp::make_tuple(num_children), bp::make_tuple(sizeof(double)), bp::object());
}
void Node::set_params(double value, int game_over){
  v = value;
  Q = value;
  W = Q/N;
  if (game_over == 1){terminal = true;}
}
void Node::set_visited(bool val){
  visited = val;
}
void Node::print_policy(){
  for (int i = 0; i < num_children; i++){
    printf("%f, ", policy_arr[i]);
  }
  printf("\n");
}
void Node::set_grandchildren(int num_children, int idx){
  children[idx]->set_children(num_children);
}
void Node::set_child_params(double value, int game_over, int idx){
  children[idx]->set_params(value, game_over);
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
