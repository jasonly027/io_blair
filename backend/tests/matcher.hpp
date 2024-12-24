#pragma once

#include <memory>
#include <ostream>

#include "gtest/gtest.h"

namespace io_blair {

template <typename T>
class SharedPtrEqMatcher {
   public:
    using is_gtest_matcher = void;

    explicit SharedPtrEqMatcher(const std::shared_ptr<T>& expected)
        : expected_(expected) {}

    bool MatchAndExplain(const std::shared_ptr<T>& actual,
                         std::ostream*) const {
        return actual == expected_;
    }

    void DescribeTo(std::ostream* os) const {
        *os << "actual and expected shared_ptrs are equal";
    }

    void DescribeNegationTo(std::ostream* os) const {
        *os << "actual and expected shared_ptrs are not equal";
    }

   private:
    const std::shared_ptr<T>& expected_;
};

template <typename T>
::testing::Matcher<std::shared_ptr<T>> SharedPtrEq(
    const std::shared_ptr<T>& expected) {
    return SharedPtrEqMatcher(expected);
}
}  // namespace io_blair
