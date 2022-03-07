/*
 * File:    BuildInstrument.h
 * Project: BestExTrading
 * Created on Feb 24, 2022, 21:52
 */

#include <set>
#include <map>
#include <functional>

#include <TFIQFeed/Provider.h>
#include <TFIQFeed/MarketSymbol.h>

#include <TFInteractiveBrokers/IBTWS.h>

#include "AcquireFundamentals.h"

#pragma once

class BuildInstrument {
public:

  using pInstrument_t = ou::tf::ib::TWS::pInstrument_t;

  using pProviderIBTWS_t = ou::tf::ib::TWS::pProvider_t;
  using pProviderIQFeed_t = ou::tf::iqfeed::IQFeedProvider::pProvider_t;

  using fInstrument_t = std::function<void(pInstrument_t)>;

  using trd_t = ou::tf::iqfeed::MarketSymbol::TableRowDef;
  using fGetTableRowDef_t = std::function<const trd_t&(const std::string& sIQFeedSymbolName)>;

  BuildInstrument( pProviderIQFeed_t, pProviderIBTWS_t, fGetTableRowDef_t&& );

  void Add( const std::string& sIQFeedSymbol, fInstrument_t&& );
  void Clear();

protected:
private:

  using pAcquireFundamentals_t = AcquireFundamentals::pAcquireFundamentals_t;

  using setSymbol_t = std::set<std::string>;

  using mapSymbol_t = std::map<std::string,fInstrument_t>;

  // TODO: need a completion function?
  struct InProgress {
    pAcquireFundamentals_t pAcquireFundamentals;
    fInstrument_t fInstrument;
    InProgress( fInstrument_t&& fInstrument_ )
    : fInstrument( std::move( fInstrument_ ) ) {}
  };

  using mapInProgress_t = std::map<std::string,InProgress>;

  using fInstrumentDone_t = std::function<void(const std::string&)>;

  std::mutex m_mutexMap;

  mapSymbol_t m_mapSymbol;  // contains symbols waiting to be built
  mapInProgress_t m_mapInProgress; // waiting for fundamentals, contract

  pProviderIQFeed_t m_pIQ;
  pProviderIBTWS_t m_pIB;

  fGetTableRowDef_t m_fGetTableRowDef;

  void Update();

  void Build( mapInProgress_t::iterator );
};
