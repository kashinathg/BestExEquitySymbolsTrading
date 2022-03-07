#include <string>

namespace config {

struct Options {
  std::string sSymbolName;
};

bool Load( Options& );

} // namespace config
