
// TestGpFusion.cpp : Defines the entry point for the console application.
// Tests library use of boost::fusion with genetic programming
//

#include "stdafx.h"

#include <iostream>

#include <boost/fusion/container/vector.hpp>
#include <boost/fusion/include/vector.hpp>

#include <boost/fusion/view/transform_view.hpp>
#include <boost/fusion/include/transform_view.hpp>

#include <boost/fusion/sequence/io.hpp>
#include <boost/fusion/include/io.hpp>

#include <boost/fusion/include/size.hpp>

#include <boost/type_traits.hpp>

#include <lib/OUGP/Population.h>

using namespace boost::fusion;

  struct square {
    template<typename Sig>
    struct result;

    template<typename U>
    struct result<square(U)> : boost::remove_reference<U>     {};

    template <typename T>
    T operator()(T x) const
    {
        return x * x;
    }
  };



int _tmain(int argc, _TCHAR* argv[]) {

  std::stringstream ss;

  typedef boost::fusion::vector<int, short, double> vector_type;
  vector_type vec(2, 5, 3.3);

  boost::fusion::transform_view<vector_type, square> transform(vec, square());
  std::cout << transform << std::endl;

  typedef boost::fusion::result_of::size<vector_type> whatsit;

  std::cout << whatsit::value << std::endl;

  ou::gp::Population pop1;
  pop1.MakeNewGeneration( true );

	return 0;
}
