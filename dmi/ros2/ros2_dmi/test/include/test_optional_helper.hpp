#pragma once

#include <gtest/gtest.h>
#include <boost/optional.hpp>

namespace ros2_dmi {
namespace test_util {

template <typename T, typename U>
void ExpectOptionalEq(
    const boost::optional<T>& actual,
    const U& expected) {

  ASSERT_TRUE(actual);
  EXPECT_EQ(*actual, expected);
}
template <typename T, typename U>
void ExpectValueEq(
    const T& actual,
    const U& expected, int idx = -1) {

  EXPECT_EQ(actual, expected) << idx;
}


}  // namespace test_util
}  // namespace ros2_dmi