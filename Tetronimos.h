#include <iostream>
#include <typeinfo>
#include <stdio.h>
#include <iterator>
#include <vector>


const int nRow = 4; // default row size of tetronimo tile
const int nCol = 4; // default col size of tetronimo tile
const int arraySize = nRow*nCol;

class Tetronimo{
    int tile[arraySize]; // 1d representation of 2d tile array
  public:
    int height;
    int width;
    void build(int []);
    Tetronimo rotate();
    void print_tile();
    int get_idx(int, int);
    void set_tile(int, int, int);
    int get_tile(int, int);
    ~Tetronimo();
    Tetronimo();
};
int Tetronimo::get_idx(int i, int j){
  return(i*4 + j);
}
void Tetronimo::set_tile(int i, int j, int val){
  tile[get_idx(i,j)] = val;
}
int Tetronimo::get_tile(int i, int j){
  return tile[get_idx(i,j)];
}
void Tetronimo::build(int dimensions[4]){
  int i;
  int j;
  for(i = 0; i<4; i++){
    for(j = 0; j<dimensions[i]; j++){
      tile[get_idx(i,j)] = 1;
    }
    if (j > width){
      width = j;
    }
    if (j > 0){
      height = i+1;
    }
  }
}
void Tetronimo::print_tile(){
  for(int i = 0; i<height; i++){
    for(int j = 0; j<width; j++){
      printf("%d ", tile[get_idx(i,j)]);
    }
    printf("\n");
  }
}
Tetronimo Tetronimo::rotate(){ // returns rotated version of class
  Tetronimo rotated_Tetronimo = Tetronimo();
  rotated_Tetronimo.height = width;
  rotated_Tetronimo.width = height;
  for(int i = 0; i<width; i++){
    for(int j = height-1; j>=0; j--){
      // printf("Rotated index %d, %d\n", i,(height-j-1));
      // printf("Original index %d, %d\n", j,i);
      rotated_Tetronimo.set_tile(i,(height-j-1),get_tile(j,i));
    }
  }
  return rotated_Tetronimo;
}
Tetronimo::Tetronimo(){
  for (int i = 0; i < arraySize; i++){
    tile[i] = 0;
  }
  height = 0;
  width = 0;
}
Tetronimo::~Tetronimo(){
  // printf("Deleting tetronimo\n");
}

class Tetronimos{
    std::vector<Tetronimo *> I;
    std::vector<Tetronimo *> J;
    std::vector<Tetronimo *> L;
    std::vector<Tetronimo *> O;
    std::vector<Tetronimo *> S;
    std::vector<Tetronimo *> Z;
    std::vector<Tetronimo *> T;

  public:
    int size;
    void fill_tetronimo_array(std::vector<Tetronimo *> *, int[4], int);
    void init();
    Tetronimo * get_tetronimo(int, int);
    int get_num_orientation(int);
    int get_tetronimo_width(int, int);
    int get_num_tetronimos();
    Tetronimos();
    ~Tetronimos();

};
void Tetronimos::fill_tetronimo_array(std::vector<Tetronimo *> *shape, int dimensions[4], int sorz){ // when passing vector, pass pointer to vector
  size++;
  Tetronimo * tet = new Tetronimo();
  tet->build(dimensions);
  // shape->push_back(tet);
  (*shape)[0] = tet;
  if (sorz == 1){ // s tetronimo
    tet->set_tile(2,0,0);
  }
  if (sorz == 2){ // z tetronimo
    tet->set_tile(0,0,0);
  }

  // tet->print_tile();
  for(int i = 1; i < shape->size(); i++){
    Tetronimo * tet = new Tetronimo();
    *tet = (*shape)[i-1]->rotate();
    // shape->push_back(tet);
    (*shape)[i] = tet;
    // tet->print_tile();

  }

  // for (int i = 0; i < (*shape).size(); i++){
  //   (*shape)[i]->print_tile();
  //   printf("\n");
  // }

  return;
}
void Tetronimos::init(){

  I.resize(2);
  J.resize(4);
  L.resize(4);
  O.resize(1);
  S.resize(2);
  Z.resize(2);
  T.resize(4);

  int dimensions[4] = {4,0,0,0};
  fill_tetronimo_array(&I, dimensions,0);

  dimensions[0] = 1;
  dimensions[1] = 3;
  dimensions[2] = 0;
  dimensions[3] = 0;
  fill_tetronimo_array(&J, dimensions,0);

  dimensions[0] = 3;
  dimensions[1] = 1;
  dimensions[2] = 0;
  dimensions[3] = 0;
  fill_tetronimo_array(&L, dimensions,0);

  dimensions[0] = 2;
  dimensions[1] = 2;
  dimensions[2] = 0;
  dimensions[3] = 0;
  fill_tetronimo_array(&O, dimensions,0);

  dimensions[0] = 1;
  dimensions[1] = 2;
  dimensions[2] = 2;
  dimensions[3] = 0;
  fill_tetronimo_array(&S, dimensions,1);

  dimensions[0] = 2;
  dimensions[1] = 2;
  dimensions[2] = 1;
  dimensions[3] = 0;
  fill_tetronimo_array(&Z, dimensions,2);

  dimensions[0] = 1;
  dimensions[1] = 2;
  dimensions[2] = 1;
  dimensions[3] = 0;
  fill_tetronimo_array(&T, dimensions,0);
}
Tetronimo * Tetronimos::get_tetronimo(int tetronimo_idx, int orientation_idx){

  switch(tetronimo_idx){
    case 0:
      return I[orientation_idx];
    case 1:
      return J[orientation_idx];
    case 2:
      return L[orientation_idx];
    case 3:
      return O[orientation_idx];
    case 4:
      return S[orientation_idx];
    case 5:
      return Z[orientation_idx];
    case 6:
      return T[orientation_idx];
    default:
      return I[orientation_idx];
  }
}
int Tetronimos::get_num_orientation(int tetronimo_idx){
  switch(tetronimo_idx){
    case 0:
      return (int)(I.size());
    case 1:
      return (int)(J.size());
    case 2:
      return (int)(L.size());
    case 3:
      return (int)(O.size());
    case 4:
      return (int)(S.size());
    case 5:
      return (int)(Z.size());
    case 6:
      return (int)(T.size());
    default:
      return 0;
  }
}
int Tetronimos::get_tetronimo_width(int tetronimo_idx, int orientation_idx){
  switch(tetronimo_idx){
    case 0:
      return (*I[orientation_idx]).width;
    case 1:
      return (*J[orientation_idx]).width;
    case 2:
      return (*L[orientation_idx]).width;
    case 3:
      return (*O[orientation_idx]).width;
    case 4:
      return (*S[orientation_idx]).width;
    case 5:
      return (*Z[orientation_idx]).width;
    case 6:
      return (*T[orientation_idx]).width;
    default:
      return (*I[orientation_idx]).width;
  }
}
int Tetronimos::get_num_tetronimos(){
  return(size);
}
Tetronimos::Tetronimos(){ // constructor
  I.reserve(2);
  J.reserve(4);
  L.reserve(4);
  O.reserve(1);
  S.reserve(2);
  Z.reserve(2);
  T.reserve(4);
  size = 0;
  init();

}
Tetronimos::~Tetronimos(){ // destructor
  // printf("Destoying tetronimos\n");
  for(int i = 0; i<(int)(I.size()); i++){
    delete I[i];
  }
  for(int i = 0; i<(int)(J.size()); i++){
    delete J[i];
  }
  for(int i = 0; i<(int)(L.size()); i++){
    delete L[i];
  }
  for(int i = 0; i<(int)(O.size()); i++){
    delete O[i];
  }
  for(int i = 0; i<(int)(S.size()); i++){
    delete S[i];
  }
  for(int i = 0; i<(int)(Z.size()); i++){
    delete Z[i];
  }
  for(int i = 0; i<(int)(T.size()); i++){
    delete T[i];
  }
  // I.clear();
  // J.clear();
  // L.clear();
  // O.clear();
  // S.clear();
  // Z.clear();
  // T.clear();
  // printf("Finished destroying tetronimos\n");
}
