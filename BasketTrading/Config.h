/*
 * File:    Config.h
 * Project: BestExTrading
 * Created on Feb 24, 2022, 21:52
 */

#include <string>
#include <vector>

#include <boost/date_time/gregorian/gregorian.hpp>

namespace config {

using vSymbol_t = std::vector<std::string>;

struct Options {
  boost::gregorian::date dateHistory;
  boost::gregorian::date dateTrading;
  vSymbol_t vSymbol;
};

bool Load( Options& );

} // namespace config
