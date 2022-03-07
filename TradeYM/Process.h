#pragma once

#include <vector>
#include <string>

#include <LibTimeSeries/TimeSeries.h>
#include <LibInteractiveBrokers/IBTWS.h>
//#include <LibIQFeed/IQFeed.h>

class Process
//  :
//  public CIQFeed<Process>
{
//  friend CIQFeed<Process>;
public:
  Process( void );
  ~Process( void );
  void IBConnect( void );
  void PlaceBuyOrder( void );
  void PlaceSellOrder( void );
  void IBDisconnect( void );

  void StartWatch( void );
  void StopWatch( void );
protected:

  void OnIQFeedConnected( void );
  void OnIQFeedDisConnected( void );
//  void OnIQFeedUpdateMessage( linebuffer_t* pBuffer, CIQFUpdateMessage* msg);
private:

  std::string m_sSymbolName;
  IBTWS m_tws;
  bool m_bIBConnected;
  IBSymbol* m_pSymbol;
  Quotes m_vQuotes;
  Trades m_vTrades;

  void HandleOnQuote( IBSymbol::quote_t );
  void HandleOnTrade( IBSymbol::trade_t );
};
