#include <TFTrading/Watch.h>

#include <functional>

class AcquireFundamentals {
public:

  using pWatch_t = ou::tf::Watch::pWatch_t;

  using pAcquireFundamentals_t = std::shared_ptr<AcquireFundamentals>;
  using fDone_t = std::function<void(pWatch_t)>;

  AcquireFundamentals( pWatch_t&& pWatch_, fDone_t&& fDone_ );
  ~AcquireFundamentals();

  void Start();

private:

  pWatch_t pWatch;
  fDone_t fDone;

  void HandleFundamentals( const ou::tf::Watch::Fundamentals& fundamentals );
  void HandleTrade( const ou::tf::Trade& trade );

};
