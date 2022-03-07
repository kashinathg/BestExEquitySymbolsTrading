#pragma once

#include <vector>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <TFOptions/Bundle.h>

// pass in vector of ptime for option expiry dates

class BundleTracking {
public:

  // need to be able to detect futures vs equities (pull from iqfeed?)
  struct BundleDetails {
    // 20151115 this isn't going to work, will need the instrument instead
    //   will also requre the UTC expiry times for the options required
    std::string sIBUnderlyingName;  // eg, IB is gc
    std::string sIQUnderlyingName;  // eg, IQ is qgc
    std::vector<boost::gregorian::date> vOptionExpiryDay;
    BundleDetails( const std::string& sIb, const std::string& sIq ):
      sIBUnderlyingName( sIb ), sIQUnderlyingName( sIq ) {}
  };

  BundleTracking( const std::string& sName );
  virtual ~BundleTracking();

  void SetBundleParameters( const BundleDetails& bd );

private:
  ou::tf::option::MultiExpiryBundle m_bundle;
};
