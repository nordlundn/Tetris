#include <iostream>
#include <typeinfo>
#include <stdio.h>
#include <iterator>
#include <vector>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include "boost/random.hpp"
#include "boost/generator_iterator.hpp"

#include <boost/python.hpp>
#include "boost/python/numpy.hpp"
#include "vec2np.h"
#include "Tetronimos.h"

// #include <boost/python.hpp>

const int board_rows = 20;
const int board_cols = 10;
const int board_size = board_rows*board_cols;

namespace bp = boost::python;
namespace bn = boost::python::numpy;

typedef enum {
  COMMIT,
  SEARCH,
  EXPLORE
} State;

class Board{
  int board_array[board_size];
  public:
    Board();
    ~Board();
    int get(int, int);
    void set(int, int, int);
    void print_board();
};
Board::Board(){
  for (int i=0; i<board_size; i++){
    board_array[i] = 0;
  }
}
Board::~Board(){
}
int Board::get(int i, int j){
  return(board_array[10*i + j]);
}
void Board::set(int i, int j, int val){
  board_array[10*i + j] = val;
}
void Board::print_board(){
  for (int i = 0; i<board_rows; i++){
    for (int j = 0; j<board_cols; j++){
      printf("%d ", get(i,j));
    }
    printf("\n");
  }
  printf("\n");
}

struct action{
  int orientation_idx;
  int col_idx;
};

class Moves{
  std::vector<action *> I;
  std::vector<action *> J;
  std::vector<action *> L;
  std::vector<action *> O;
  std::vector<action *> S;
  std::vector<action *> Z;
  std::vector<action *> T;

  std::vector<std::vector<action *> *> move_set;
  std::vector<int> num_actions;

  public:
    Moves(Tetronimos *);
    ~Moves();
    void fill_array(std::vector<action *> *, Tetronimos *, int tetronimo_idx);
    action * get_action(int tetronimo_idx, int action_idx);
    int max_action_num;
    int get_num_actions(int);
};
Moves::Moves(Tetronimos * t_set){
  max_action_num = 0;
  fill_array(&I, t_set, 0);
  fill_array(&J, t_set, 1);
  fill_array(&L, t_set, 2);
  fill_array(&O, t_set, 3);
  fill_array(&S, t_set, 4);
  fill_array(&Z, t_set, 5);
  fill_array(&T, t_set, 6);

  move_set.push_back(&I);
  move_set.push_back(&J);
  move_set.push_back(&L);
  move_set.push_back(&O);
  move_set.push_back(&S);
  move_set.push_back(&Z);
  move_set.push_back(&T);

  int action_num;
  for(int i = 0; i < (int)(move_set.size()); i++){
    action_num = move_set[i]->size();
    // printf("The number of actions is %d\n", action_num);
    num_actions.push_back(action_num);
    if (action_num > max_action_num){
      max_action_num = action_num;
    }
  }
  // printf("\n");
}
Moves::~Moves(){
  for(int i = 0; i < (int)(move_set.size()); i++){
    for (int j = 0; j < (int)((*move_set[i]).size()); j++){
      delete (*move_set[i])[j];
    }
  }
}
void Moves::fill_array(std::vector<action *> * v, Tetronimos * t_set, int tetronimo_idx){
  int num_orientations;
  int tetronimo_width;
  // I actions
  num_orientations = t_set->get_num_orientation(tetronimo_idx);
  // printf("the number of orientations is %d\n", num_orientations);
  int num_action =0;
  // printf("filling the array with %d actions\n", num_orientations * )
  for (int i = 0; i<num_orientations; i++){
    tetronimo_width = t_set->get_tetronimo_width(tetronimo_idx,i);
    for (int j = 0; j<(board_cols - tetronimo_width + 1); j++){
      action * a = new action;
      // action * a;
      a->orientation_idx = i;
      a->col_idx = j;
      v->push_back(a);
      num_action++;
    }
  }
  // printf("filling the array with %d actions\n", num_action);
}
action * Moves::get_action(int tetronimo_idx, int action_idx){
  // printf("%d, %d\n", tetronimo_idx, action_idx);
  // printf("%lu\n", move_set.size());
  // printf("%lu\n", (*move_set[tetronimo_idx]).size());
  return (*move_set[tetronimo_idx])[action_idx];
}
int Moves::get_num_actions(int tetronimo_idx){
  return num_actions[tetronimo_idx];
}

class Tetris{
  int length;
  std::vector<int> tetronimos;
  public:
    Tetris(bn::ndarray, int);
    ~Tetris();
    Tetronimos * t_set;

    Board * committed_board;
    Board * search_board;
    Board * exploration_board;

    int committed_move_num;
    int search_move_num;
    int exploration_move_num;

    Moves * move_set;
    int max_action_num;

    int get_num_actions(int);
    void board_cpy(Board *, Board *);
    bool can_place(Board *, Tetronimo *, int, int);
    bool place_tetronimo(Board *, int, int);
    bool take_action(int, int);
    std::vector<int> * make_state(Board *, int);
    std::vector<int> * get_state(int);
    bn::ndarray make_nd_state(Board *, int);
    bn::ndarray get_nd_state(int);
    int get_reward();
    void reset_search();
    void reset_explore();
    bn::ndarray explore();
    void print_board();
};
Tetris::Tetris(bn::ndarray arr, int l) {

  t_set = new Tetronimos();

  committed_board = new Board();
  search_board = new Board();
  exploration_board = new Board();

  committed_move_num = 0;
  search_move_num = 0;
  exploration_move_num = 0;
  move_set = new Moves(t_set);
  max_action_num = move_set->max_action_num;

  length = l;
  tetronimos = np2vec(&arr, l);

  for (int i = 0; i < length; i++){
    // tetronimos[i] = rand()%7;
    // printf("The tetronimo idx is %d\n", tetronimos[i]);
  }
  // printf("\n");
}
Tetris::~Tetris(){
  delete t_set;
  delete committed_board;
  delete search_board;
  delete exploration_board;
  delete move_set;
}
int Tetris::get_num_actions(int state){
  int tetronimo_idx;
  switch (state) {
    case COMMIT:
      tetronimo_idx = tetronimos[committed_move_num];
      return move_set->get_num_actions(tetronimo_idx);
    case SEARCH:
      tetronimo_idx = tetronimos[search_move_num];
      return move_set->get_num_actions(tetronimo_idx);
    case EXPLORE:
      tetronimo_idx = tetronimos[exploration_move_num];
      return move_set->get_num_actions(tetronimo_idx);
    default:
      return -1;
  }
}
void Tetris::board_cpy(Board * src, Board * dst){
  bool all_zeros = false;
  for (int i = 0; i < board_rows; i++){
    all_zeros = false;
    for (int j = 0; j < board_cols; j++){
      all_zeros &= (src->get(i,j) == 0 && dst->get(i,j) == 0);
      dst->set(i,j, src->get(i,j));
    }
    if (all_zeros){
      return;
    }
  }
  return;
}
bool Tetris::can_place(Board * board, Tetronimo * tetronimo, int row_idx, int col_idx){
  int height = tetronimo->height;
  int width = tetronimo->width;
  bool conflict = false;

  for (int i = 0; i<height; i++){
    for (int j = 0; j<width; j++){
      conflict |= ((board->get(row_idx+i, col_idx+j) == 1) && tetronimo->get_tile(i,j));
    }
  }

  if (!conflict){
    for (int i = 0; i<height; i++){
      for (int j = 0; j<width; j++){
        board->set(row_idx+i,col_idx+j, tetronimo->get_tile(i,j)|board->get(row_idx+i,col_idx+j));
      }
    }
    return true;
  }

  return false;
}
bool Tetris::place_tetronimo(Board * board, int tetronimo_idx, int action_idx){

  action * a = move_set->get_action(tetronimo_idx, action_idx);
  Tetronimo * tetronimo = t_set->get_tetronimo(tetronimo_idx, a->orientation_idx);
  // tetronimo->print_tile();
  // printf("%d, %d\n", a->orientation_idx, a->col_idx);
  int col_idx = a->col_idx;
  int row_idx = 0;
  // printf("%d, %d\n", row_idx, col_idx);
  while(true){

    if ((row_idx+tetronimo->height) > board_rows){
      return true;
    }
    else if (can_place(board, tetronimo, row_idx, col_idx)){
      return false;
    }
    else{
      row_idx+=1;
    }
  }
}
bool Tetris::take_action(int action_idx, int state){
  // printf("Tetris::take_action\n");
  bool game_over = false;
  int tetronimo_idx;
  switch (state) {
    case COMMIT:
      tetronimo_idx = tetronimos[committed_move_num];
      // printf("Tetris::Commit take action %d with tetronimo idx %d\n", action_idx, tetronimo_idx);
      // if (action_idx > get_num_actions(0)){printf("Invalid action");}
      game_over = place_tetronimo(committed_board, tetronimo_idx, action_idx);
      committed_move_num++;
      search_move_num = committed_move_num;
      exploration_move_num = committed_move_num;

      if (committed_move_num >= length){
        game_over = true;
      }
      // printf("Tetris::Commit take_action: success\n");
      return game_over;
    case SEARCH:
      tetronimo_idx = tetronimos[search_move_num];
      // printf("Tetris::Search take action %d with tetronimo idx %d\n", action_idx, tetronimo_idx);
      // if (action_idx > get_num_actions(1)){printf("Invalid action");}
      game_over = place_tetronimo(search_board, tetronimo_idx, action_idx);
      search_move_num++;
      exploration_move_num = search_move_num;
      if (search_move_num >= length){
        game_over = true;
      }
      // printf("Tetris::Search take_action: success\n");
      return game_over;

    case EXPLORE:
      tetronimo_idx = tetronimos[exploration_move_num];
      // printf("Tetris::Explore move num %d\n", exploration_move_num);
      // printf("Tetris::Explore take action %d with tetronimo idx %d\n", action_idx, tetronimo_idx);
      // if (action_idx > get_num_actions(1)){printf("Invalid action");}
      game_over = place_tetronimo(exploration_board, tetronimo_idx, action_idx);
      exploration_move_num++;
      if (exploration_move_num >= length){
        game_over = true;
      }
      // printf("Tetris::Explore take_action: success\n");
      return game_over;
    default:
      // printf("Tetris::take_action: success\n");
      return true;
  }
}
bn::ndarray Tetris::make_nd_state(Board * board, int tetronimo_idx){

  int num_tetronimos = t_set->get_num_tetronimos();
  // board->print_board();
  // printf("The num tetronimos is %d\n", num_tetronimos);
  std::vector<int> state;
  state.resize(board_cols + num_tetronimos);
  int j;
  for(int i = 0; i < board_cols; i++){
    j = board_rows-1;
    while(j>=0 && board->get(j,i) == 0){
      j--;
    }
    // printf("The state is %d\n", j+1);
    state[i] = j+1;
  }
  if (tetronimo_idx >= 0){
    state[board_cols+tetronimo_idx] = 1;
    // printf("The state at %d is %d\n", tetronimo_idx, 1);
  }
  // printf("make_state thinks the size of the vector is %lu\n", state.size());
  return vec2np(&state);
}
bn::ndarray Tetris::get_nd_state(int state){
  int tetronimo_idx;
  switch (state) {
    case COMMIT:
      tetronimo_idx = tetronimos[committed_move_num];
      return make_nd_state(committed_board, tetronimo_idx);
    case SEARCH:
      tetronimo_idx = tetronimos[search_move_num];
      return make_nd_state(search_board, tetronimo_idx);
    case EXPLORE:
      tetronimo_idx = tetronimos[exploration_move_num];
      return make_nd_state(exploration_board, tetronimo_idx);
    default:
      return make_nd_state(exploration_board, -1);
  }
}
std::vector<int> * Tetris::make_state(Board * board, int tetronimo_idx){

  int num_tetronimos = t_set->get_num_tetronimos();
  // board->print_board();
  // printf("The num tetronimos is %d\n", num_tetronimos);
  std::vector<int> * state = new std::vector<int>;
  state->resize(board_cols + num_tetronimos);
  int j;
  for(int i = 0; i < board_cols; i++){
    j = board_rows-1;
    while(j>=0 && board->get(j,i) == 0){
      j--;
    }
    // printf("The state is %d\n", j+1);
    (*state)[i] = j+1;
  }
  if (tetronimo_idx >= 0){
    (*state)[board_cols+tetronimo_idx] = 1;
    // printf("The state at %d is %d\n", tetronimo_idx, 1);
  }
  // printf("make_state thinks the size of the vector is %lu\n", state.size());
  return state;
}
std::vector<int> * Tetris::get_state(int state){
  int tetronimo_idx;
  switch (state) {
    case COMMIT:
      tetronimo_idx = tetronimos[committed_move_num];
      return make_state(committed_board, tetronimo_idx);
    case SEARCH:
      tetronimo_idx = tetronimos[search_move_num];
      return make_state(search_board, tetronimo_idx);
    case EXPLORE:
      tetronimo_idx = tetronimos[exploration_move_num];
      return make_state(exploration_board, tetronimo_idx);
    default:
      return make_state(exploration_board, -1);
  }
}
int Tetris::get_reward(){
  int reward = 0;
  int height = board_rows;
  int width = board_cols;
  bool finished = false;
  for(int i = 0; i < height; i++){
    finished = true;
    for (int j = 0; j < width-1; j++){
      reward += committed_board->get(i,j)^committed_board->get(i,j+1);
      finished &= (committed_board->get(i,j) == 0) && (committed_board->get(i,j+1) == 0);
    }
    if (finished){
      return reward;
    }
  }
  return reward;
}
void Tetris::reset_search(){
  // printf("Tetris::reset_search\n");
  // printf("Committed num = %d\n", committed_move_num);
  board_cpy(committed_board, search_board);
  search_move_num = committed_move_num;
  exploration_move_num = committed_move_num;
  // printf("Tetris::reset_search: success\n");
}
void Tetris::reset_explore(){
  // printf("Tetris::exploration_search\n");
  board_cpy(search_board, exploration_board);
  exploration_move_num = search_move_num;
  // printf("Tetris::exploration_search: success\n");
}
bn::ndarray Tetris::explore(){
  // printf("Tetris::explore\n");
  int state_size = 17;
  // printf("%d, %d, %d\n", committed_move_num, search_move_num, exploration_move_num);
  int num_actions = get_num_actions(1); // num children
  // printf("%d, %d, %d\n", tetronimos[search_move_num], move_set->get_num_actions(tetronimos[search_move_num]), num_actions);
  std::vector<int> states;
  states.reserve((state_size+2)*num_actions);
  int game_over;
  int num_children;
  for (int i = 0; i<num_actions; i++){
    game_over = (int)(take_action(i,2));
    num_children = get_num_actions(2);
    if (game_over == 1){num_children = 0;}
    else{num_children = get_num_actions(2);}
    std::vector<int> * state = get_state(2);
    states.push_back(game_over);
    states.push_back(num_children);
    std::copy(state->begin(), state->end(), std::back_inserter(states));
    delete state;
    reset_explore();
  }
  // printf("Tetris::explore: success\n");
  return vec2np(&states);
}
void Tetris::print_board(){
  committed_board->print_board();
}

BOOST_PYTHON_MODULE(Tetris)
{
  using namespace boost::python;
  numpy::initialize();
  class_<Tetris>("Tetris", init<numpy::ndarray, int>())
    .def("take_action", &Tetris::take_action)
    .def("get_state", &Tetris::get_nd_state)
    .def("explore", &Tetris::explore)
    .def("print", &Tetris::print_board)
    .def("get_num_actions", &Tetris::get_num_actions)
    .def("reset_search", &Tetris::reset_search);
}
