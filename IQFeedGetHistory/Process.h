
#pragma once

/*
  handles the various messages for:
  * scanning a list of symbols from an exchange from the CInstrumentFile
  * obtaining history from IQFeed for each symbol
  * creating the appropriate structures
  * processing the structures looking for promising trades
*/

#include <set>
#include <string>

#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>

#include <TFIQFeed/HistoryBulkQuery.h>
#include <TFIQFeed/InMemoryMktSymbolList.h>

class Process:
  public ou::tf::iqfeed::HistoryBulkQuery<Process>
{
  friend ou::tf::iqfeed::HistoryBulkQuery<Process>;
public:

  typedef ou::tf::iqfeed::HistoryBulkQuery<Process> inherited_t;

  Process(
    ou::tf::iqfeed::InMemoryMktSymbolList&,
    const std::string& sPrefixPath,
    size_t nDatums
  );
  ~Process(void);
  void Start( void );

protected:

  // CRTP from HistoryBulkQuery<CProcess>
  void OnBars( inherited_t::structResultBar* bars );
  void OnTicks( inherited_t::structResultTicks* ticks );
  void OnCompletion( void );

  void OnBarsForDarvas( inherited_t::structResultBar* bars );

  // CRTP prototypes
  //void OnHistoryConnected( structQueryState* pqs );  // optional
  //void OnHistoryDisconnected( structQueryState* pqs ); // optional
  //void OnHistoryError( structQueryState* pqs, size_t e ); // optional
  //void OnHistorySendDone( structQueryState* pqs ); // otional
  //void OnHistoryTickDataPoint( structQueryState* pqs, ou::tf::iqfeed::HistoryStructs::structTickDataPoint* pDP ); // for per tick processing
  //void OnHistoryIntervalData( structQueryState* pqs, ou::tf::iqfeed::HistoryStructs::structInterval* pDP ); // for per bar processing
  //void OnHistorySummaryData( structQueryState* pqs, ou::tf::iqfeed::HistoryStructs::structSummary* pDP ); // for per bar processing
  //void OnHistoryRequestDone( structQueryState* pqs ); // for processing finished ticks, bars

private:

  ou::tf::iqfeed::InMemoryMktSymbolList& m_list;

  boost::mutex m_mutexProcessResults;

  std::string m_sPrefixPath;
  const size_t m_nDatums;

  std::set<std::string> m_vExchanges;  // list of exchanges to be scanned to create:
  std::set<std::string> m_vSymbols;  // list of symbols to be scanned

  static const size_t m_BarWindow = 20;  // number of bars to examine

  //const size_t m_cntBars;

};
