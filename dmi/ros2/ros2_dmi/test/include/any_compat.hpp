#pragma once

#if __cplusplus >= 201703L

  #include <any>

  namespace dm_any
  {
    using std::any;
    using std::any_cast;
  }

#else

  #include <experimental/any>

  namespace dm_any
  {
    using std::experimental::any;
    using std::experimental::any_cast;
  }

#endif