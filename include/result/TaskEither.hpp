//
// Created by Austin Hanson on 8/12/25.
//

#ifndef TASKEITHER_HPP
#define TASKEITHER_HPP
#include "Either.hpp"
#include "async/task.hpp"

namespace lcl {
  template <typename TResult, ErrorType TError>
  using TaskEither = async::Task<Either<TResult, TError>>;
}

#endif //TASKEITHER_HPP
