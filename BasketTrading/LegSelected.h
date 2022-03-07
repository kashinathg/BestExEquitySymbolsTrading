#ifndef LEGSELECTED_H
#define LEGSELECTED_H

#include <string>

#include <TFOptions/Option.h>

class LegSelected { // specific definition of each leg for trading
public:

  using pOption_t = ou::tf::option::Option::pOption_t;

  LegSelected();
  LegSelected( const LegSelected& ) = delete;
  LegSelected( const LegSelected&& );

  // return 0 if not changed, 1 if changed
  unsigned int Update( double spread, double strike, boost::gregorian::date dateExpiry, const std::string& sIQFeedOptionName );

  bool Changed() { bool bTemp = m_bChanged; m_bChanged = false; return bTemp; }
  double Spread() const { return m_dblSpread; }
  double Strike() const { return m_dblStrike; }
  boost::gregorian::date Expiry() const { return m_dateExpiry; }
  const std::string& IQFeedOptionName() const { return m_sIQFeedOptionName; }
  pOption_t& Option() { return m_pOption; } // cheating by returning a reference

  void Clear();

private:
  bool m_bChanged;
  double m_dblSpread;
  double m_dblStrike;
  boost::gregorian::date m_dateExpiry;
  std::string m_sIQFeedOptionName;
  pOption_t m_pOption; // set after initial construction
};

#endif /* LEGSELECTED_H */
