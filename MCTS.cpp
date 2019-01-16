#include <stdlib.h>
#include <math.h>
#include <vector>
#include <array>
#include <boost/python.hpp>
#include "boost/python/numpy.hpp"
#include "vec2np.h"

namespace bp = boost::python;
namespace bn = boost::python::numpy;

class MCTS{
  double * policy;
  double value;
  std::vector<MCTS *> children;
  int num_children;
  MCTS * current_node;

  public:
    MCTS * root;
    std::vector<MCTS *> path;
    double Q; // average value updated according to MCTS
    double N; // number of visits to this node
    double W; // unweighted value of this path;
    bool visited;
    bool terminal;
    MCTS();
    ~MCTS();
    void make_children(int);
    void make_grandchildren(int, int);
    void make_root();
    int arg_max(std::vector<double>);
    int select_action();
    void set_params(double, int);
    void set_child_params(int, double, int);
    int step();
    void reset();
    bn::ndarray get_policy();
    void back_prop();
    bn::ndarray get_child_policy(int);
    bn::ndarray get_pi(double);
};
MCTS::MCTS(){
  Q = -1.0; // initial value for the weighted average
  N = 1; // number of visits is 1
  visited = false;
  terminal = false;
  num_children = 0;
}
MCTS::~MCTS(){
  for (int i = 0; i < num_children; i++){
    delete children[i];
    if (i == 0 ){delete [] policy;}
  }
}
void MCTS::make_children(int num_children){
  current_node->num_children = num_children;
  current_node->children.resize(num_children);
  current_node->policy = new double[num_children];
  for(int i = 0; i<num_children; i++){
    current_node->children[i] = new MCTS();
    current_node->children[i]->root = root;
    current_node->policy[i] = 0.0;
  }
}
void MCTS::make_grandchildren(int idx, int num_children){
  MCTS * child = current_node->children[idx];
  child->num_children = num_children;
  child->children.resize(num_children);
  child->policy = new double[num_children];
  for (int i = 0; i<num_children; i++){
    child->children[i] = new MCTS();
    child->children[i]->root = root;
    child->policy[i] = 0.0;
  }
}
void MCTS::make_root(){
  root = this;
  current_node = root;
}
int MCTS::arg_max(std::vector<double> arr){
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
int MCTS::select_action(){
  std::vector<double> u;
  std::vector<double> q;
  u.resize(num_children);
  q.resize(num_children);
  double s = 0;

  for (int i = 0; i < num_children; i++){
    u[i] = policy[i]/(1+children[i]->N);
    q[i] = children[i]->Q;
    s += u[i];
  }

  for (int i = 0; i < num_children; i++){
    u[i] = u[i]/s + q[i];
  }
  return arg_max(u);
}
void MCTS::set_child_params(int idx, double value, int game_over){
  MCTS * child = current_node->children[idx];
  child->value = value;
  child->Q = value;
  child->W = Q/N;
  if (game_over == 1){child->terminal = true;}
}
void MCTS::set_params(double value, int game_over){
  this->value = value;
  this->Q = value;
  this->W = Q/N;
  if (game_over == 1){this->terminal = true;}
}
int MCTS::step(){
  path.push_back(current_node);
  if (current_node->terminal){
    return -2;
  }
  else if (!(current_node->visited)){
    current_node->visited = true;
    return -1;
  }
  else{
    int next_child = current_node->select_action();
    current_node = current_node->children[next_child];
    return next_child;
  }
}
void MCTS::reset(){
  //back_prop
  back_prop();
  current_node = root;
  path.clear();
}
bn::ndarray MCTS::get_policy(){
  return bn::from_data(current_node->policy, bn::dtype::get_builtin<double>(), bp::make_tuple(current_node->num_children), bp::make_tuple(sizeof(double)), bp::object());
}
bn::ndarray MCTS::get_child_policy(int idx){
  MCTS * child = current_node->children[idx];
  return bn::from_data(child->policy, bn::dtype::get_builtin<double>(), bp::make_tuple(child->num_children), bp::make_tuple(sizeof(double)), bp::object());
}
void MCTS::back_prop(){
  MCTS * new_child = path.back();
  double val = new_child->Q;
  for (int i = 0; i<(path.size()-1); i++){
    new_child = path[i];
    new_child->W += val;
    new_child->N += 1;
    new_child->Q = (new_child->W)/(new_child->N);
  }
}
bn::ndarray MCTS::get_pi(double tau){
  std::vector<double> pi;
  pi.resize(root->num_children);
  double s = 0;
  for(int i = 0; i < root->num_children; i++){
    pi[i] = pow(root->children[i]->N, tau);
    s+=pi[i];
  }
  for(int i = 0; i < root->num_children; i++){
    pi[i]/=s;
  }
  return doublevec2np(&pi);
}

BOOST_PYTHON_MODULE(MCTS)
{
  using namespace boost::python;
  Py_Initialize();
  numpy::initialize();
  class_<MCTS, boost::noncopyable>("MCTS")
    .def("make_children", &MCTS::make_children)
    .def("make_grandchildren", &MCTS::make_grandchildren)
    .def("make_root", &MCTS::make_root)
    .def("set_child_params", &MCTS::set_child_params)
    .def("set_params", &MCTS::set_params)
    .def("step", &MCTS::step)
    .def("reset", &MCTS::reset)
    .def("get_policy", &MCTS::get_policy)
    .def("get_child_policy", &MCTS::get_child_policy)
    .def("get_pi", &MCTS::get_pi)
  ;
}
