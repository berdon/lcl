//
// Created by Austin Hanson on 8/12/25.
//

#ifndef TASKERREAN_HPP
#define TASKERREAN_HPP
#include "Errean.hpp"
#include "async/task.hpp"

namespace lcl {
  template <typename TResult>
  using TaskErrean = async::Task<Errean<TResult>>;
}

#endif //TASKERREAN_HPP
