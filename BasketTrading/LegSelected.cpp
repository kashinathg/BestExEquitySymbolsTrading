#include "LegSelected.h"

LegSelected::LegSelected()
: m_bChanged( false ),
  m_dblStrike {}
{}

LegSelected::LegSelected( const LegSelected&& rhs )
: m_bChanged( rhs.m_bChanged ),
  m_dblStrike( rhs.m_dblStrike ),
  m_dateExpiry( rhs.m_dateExpiry ),
  m_sIQFeedOptionName( std::move( rhs.m_sIQFeedOptionName ) ),
  m_pOption( std::move( rhs.m_pOption ) )
{
}

unsigned int LegSelected::Update( double spread, double strike, boost::gregorian::date dateExpiry, const std::string& sIQFeedOptionName ) {
  if ( sIQFeedOptionName.empty() ) {
    m_bChanged = true;
  }
  else {
    m_bChanged = ( ( strike != m_dblStrike ) || ( dateExpiry != m_dateExpiry ) );
  }

  if ( m_bChanged ) {
    m_dblSpread = spread;
    m_dblStrike = strike;
    m_dateExpiry = dateExpiry;
    m_sIQFeedOptionName = sIQFeedOptionName;
  }

  return ( m_bChanged ? 1 : 0 );
}

void LegSelected::Clear() {
  m_bChanged = false;
  m_dblSpread = 0.0;
  m_dblStrike = 0.0;
  m_sIQFeedOptionName.clear();
  m_pOption.reset();
}
