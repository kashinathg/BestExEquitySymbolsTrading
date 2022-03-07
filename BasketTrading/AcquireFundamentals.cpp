#include "AcquireFundamentals.h"

AcquireFundamentals::AcquireFundamentals( pWatch_t&& pWatch_, fDone_t&& fDone_ )
: pWatch( std::move( pWatch_ ) ), fDone( std::move( fDone_ ) ) {
  assert( ou::tf::keytypes::EProviderIQF == pWatch->GetProvider()->ID() );
  //std::cout << "AcquireFundamentals::AcquireFundamentals(): " << pWatch->GetInstrumentName() << std::endl;
}

AcquireFundamentals::~AcquireFundamentals() {
    //std::cout << "AcquireFundamentals::~AcquireFundamentals(): " << pWatch->GetInstrumentName() << std::endl;
  }

void AcquireFundamentals::Start() {
  //std::cout << "AcquireFundamentals::Start(): " << pWatch->GetInstrumentName() << std::endl;
  pWatch->OnFundamentals.Add( MakeDelegate( this, &AcquireFundamentals::HandleFundamentals) );
  pWatch->OnTrade.Add( MakeDelegate( this, &AcquireFundamentals::HandleTrade ) );
  pWatch->StartWatch();
}

void AcquireFundamentals::HandleFundamentals( const ou::tf::Watch::Fundamentals& fundamentals ) {
  // the watch will retain variables from the fundamentals message
  //std::cout << "AcquireFundamentals::HandleFundamentals(): " << pWatch->GetInstrumentName() << std::endl;
  pWatch->StopWatch();
  pWatch->OnFundamentals.Remove( MakeDelegate( this, &AcquireFundamentals::HandleFundamentals) );
  pWatch->OnTrade.Remove( MakeDelegate(this, &AcquireFundamentals::HandleTrade ) );
  fDone( pWatch );  // fundamentals reside in watch
}

void AcquireFundamentals::HandleTrade( const ou::tf::Trade& trade ) {
  // a watch is required in order to obtain the fundamental
  // no action required, just a placeholder
}
