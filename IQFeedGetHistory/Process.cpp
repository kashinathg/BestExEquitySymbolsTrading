
#include "stdafx.h"

#include <algorithm>

#include <boost/foreach.hpp>

#include <TFIQFeed/LoadMktSymbols.h>

#include <TFHDF5TimeSeries/HDF5DataManager.h>
#include <TFHDF5TimeSeries/HDF5WriteTimeSeries.h>

#include "Process.h"

//
// Process
//

#ifdef _MSC_VER
#pragma message( "** Note:  for msvc, compile in release mode, buffer checks make it slow in debug" )
#endif

Process::Process(
  ou::tf::iqfeed::InMemoryMktSymbolList& list,
  const std::string& sPrefixPath,
	size_t nDatums )
: ou::tf::iqfeed::HistoryBulkQuery<Process>(),
	m_list( list ),
  m_sPrefixPath( sPrefixPath ), m_nDatums( nDatums )
  //m_cntBars( 25 )
//  m_cntBars( 0 ) // 2013/09/17
{
  m_vExchanges.insert( "NYSE" );
  m_vExchanges.insert( "NYSE_AMERICAN" );
  m_vExchanges.insert( "NYSE,NYSE_ARCA" );
  m_vExchanges.insert( "NASDAQ,NGSM" );
  //m_vExchanges.push_back( "NASDAQ,NMS" );
  //m_vExchanges.push_back( "NASDAQ,SMCAP" );
  //m_vExchanges.push_back( "NASDAQ,OTCBB" );
  //m_vExchanges.push_back( "NASDAQ,OTC" );
  //m_vExchanges.insert( "CANADIAN,TSE" );  // don't do yet, simplifies contract creation for IB
}

Process::~Process(void) {
}

void Process::Start( void ) {

  //ou::tf::iqfeed::InMemoryMktSymbolList list;

  static const std::string sSymbols( "../symbols.ser" );
/*
  if (false) {
//  if (false) {
    std::cout << "Downloading File ... ";
    ou::tf::iqfeed::LoadMktSymbols( list, ou::tf::iqfeed::MktSymbolLoadType::Download, true );  // put this into a thread
  //  ou::tf::iqfeed::LoadMktSymbols( m_list, ou::tf::iqfeed::MktSymbolLoadType::LoadTextFromDisk, false );  // put this into a thread
    std::cout << "Saving File " << sSymbols << " ... ";
    list.SaveToFile( sSymbols );
  }
  else {
    std::cout << "Loading From File " << sSymbols << " ...";
    list.LoadFromFile( sSymbols );
  }
  std::cout << " done." << std::endl;
*/

  typedef std::set<std::string> SymbolList_t;
  SymbolList_t setSelected;

  struct SelectSymbols {
    SelectSymbols( SymbolList_t& set ): m_selected( set ) {  };
    SymbolList_t& m_selected;
    void operator() ( const ou::tf::iqfeed::InMemoryMktSymbolList::trd_t& trd ) {
      if ( ou::tf::iqfeed::ESecurityType::Equity == trd.sc ) {
        if ( trd.bHasOptions ) {
          m_selected.insert( trd.sSymbol );
        }
      }
    }
  };

  m_list.SelectSymbolsByExchange( m_vExchanges.begin(), m_vExchanges.end(), SelectSymbols( setSelected ) );
  std::cout << "# symbols selected: " << setSelected.size() << std::endl;

  SetMaxSimultaneousQueries( 15 );
  SetSymbols( setSelected.begin(), setSelected.end() );
  DailyBars( m_nDatums );
  Block();

  std::cout << "Process complete." << std::endl;

}

void Process::OnBars( inherited_t::structResultBar* bars ) {

  // warning:  this section is re-entrant from multiple threads

  // save the data

  boost::mutex::scoped_lock lock( m_mutexProcessResults );

  assert( bars->sSymbol.length() > 0 );

  std::cout << bars->sSymbol << ": " << bars->bars.Size();

  if ( 0 != bars->bars.Size() ) {

    std::string sPath;

    ou::tf::HDF5DataManager::DailyBarPath( bars->sSymbol, sPath );  // build hierarchical path based upon symbol name

    ou::tf::HDF5DataManager dm( ou::tf::HDF5DataManager::RDWR );
    ou::tf::HDF5WriteTimeSeries<ou::tf::Bars> wts( dm, false, true, 0, 64 );
    wts.Write( sPath, &bars->bars );
  }

  ReQueueBars( bars );

  std::cout << "." << std::endl;

}

void Process::OnTicks( inherited_t::structResultTicks* ticks ) {

  boost::mutex::scoped_lock lock( m_mutexProcessResults );

  assert( ticks->sSymbol.length() > 0 );

  ou::tf::HDF5DataManager dm( ou::tf::HDF5DataManager::RDWR );

  if ( 0 != ticks->trades.Size() ) {
    std::string sPath( "/optionables/trade/" + ticks->sSymbol );
    ou::tf::HDF5WriteTimeSeries<ou::tf::Trades> wtst( dm );
    wtst.Write( sPath, &ticks->trades );
  }

  if ( 0 != ticks->quotes.Size() ) {
    std::string sPath( "/optionables/quote/" + ticks->sSymbol );
    ou::tf::HDF5WriteTimeSeries<ou::tf::Quotes> wtsq( dm );
    wtsq.Write( sPath, &ticks->quotes );
  }

  ReQueueTicks( ticks );
}

void Process::OnCompletion( void ) {
  std::cout << "Downloads complete." << std::endl;
}
