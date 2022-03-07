
#include <map>
#include <string>

namespace ou {
namespace tf {
namespace Merrill {

enum class EPattern {
  UnDefined = 0,
  DownTrend, InvertedHeadAndShoulders,
  UpTrend,   HeadAndShoulders,
  Broadening, Uninteresting, Triangle
};

using result_t = std::pair<EPattern,const std::string&>;

void Validate();
result_t Classify( double, double, double, double, double );
const std::string& Name( EPattern );

} // namespace Merrill
} // namespace tf
} // namespace ou
