#include <boost/python/numpy.hpp>
#include <vector>
#include <stdio.h>
#include <stdlib.h>

namespace bp = boost::python;
namespace bn = boost::python::numpy;

bn::ndarray vec2np(std::vector<int> * v){
  // printf("The size of the state vector is %lu\n", v->size());
  Py_intptr_t shape[1] = {v->size()};
  bn::ndarray result = bn::zeros(1, shape, bn::dtype::get_builtin<double>());
  std::copy(v->begin(), v->end(), reinterpret_cast<double*>(result.get_data()));
  return result;
}

std::vector<int> np2vec(bn::ndarray * arr, int size){
  int * int_arr = reinterpret_cast<int*>(arr->get_data());
  // printf("The size of the array is %d\n", size);
  std::vector<int> result;
  result.resize(size);
  for (int i = 0; i<size*2;i+=2){
    // printf("NP array element is %d\n", int_arr[i]);
    result[i/2] = int_arr[i];
  }

  return result;
}
