#include <iostream>
#include <algorithm>

#include "Process.h"

Process::Process() {
}

Process::~Process() {
}

void Process::LoadWeeklies( void ) {

  bool bOk( true );
  try {
    ou::tf::cboe::csv::ReadCboeWeeklyOptions( m_weekly );
  }
  catch(...) {
    bOk = false;
    std::cout << "error loading weeklies" << std::endl;
  }

  std::cout << "LoadWeeklies done." << std::endl;

  if ( bOk ) {
    //std::sort( m_vUnderlyingInfo.begin(), m_vUnderlyingInfo.end() );
    //for ( ou::tf::cboe::csv::vUnderlyinginfo_t::const_iterator iter = m_vUnderlyingInfo.begin(); m_vUnderlyingInfo.end() != iter; ++iter ) {
    //}
  }

}
