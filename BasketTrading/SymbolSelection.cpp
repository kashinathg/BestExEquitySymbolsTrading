// Project: BestExTrading

#include <cmath>
#include <set>
#include <map>
#include <math.h>
#include <vector>
#include <iostream>

#include <TFBitsNPieces/InstrumentFilter.h>
#include <TFBitsNPieces/ReadCboeWeeklyOptionsCsv.h>

#include <TFIndicators/Darvas.h>

#include <TFStatistics/Pivot.h>
#include <TFStatistics/HistoricalVolatility.h>

#include "SymbolSelection.h"

// typically, not cut and dried:
// for $25 or less: strike intervals are 2.5
// for $25 or more: strike interval is 5
// for futures option, typicall 1 or 2
// for $200 or more: strike intervals are 10

struct AverageVolume {
private:
  ou::tf::Bar::volume_t m_nTotalVolume;
  unsigned long m_nNumberOfValues;
protected:
public:
  AverageVolume() : m_nTotalVolume( 0 ), m_nNumberOfValues( 0 ) {};
  void operator() ( const ou::tf::Bar& bar ) {
    m_nTotalVolume += bar.Volume();
    ++m_nNumberOfValues;
  }
  operator ou::tf::Bar::volume_t() { return m_nTotalVolume / m_nNumberOfValues; };
};

struct VolumeEma {
private:
  bool bFirstFound;
  double dblEmaVolume;
protected:
public:
  VolumeEma() : bFirstFound( false ), dblEmaVolume {} {};
  void operator() ( const ou::tf::Bar& bar ) {
    static const double dblEmaFactor1( 2.0 / ( 21.0 + 1.0 ) ); // 21 days, could use standard 20 days
    static const double dblEmaFactor2( 1.0 - dblEmaFactor1 );
    if ( bFirstFound ) {
      dblEmaVolume = ( dblEmaFactor1 * (double)bar.Volume() ) + ( dblEmaFactor2 * dblEmaVolume );
    }
    else {
      dblEmaVolume = (double)bar.Volume();
      bFirstFound = true;
    }
  }
  operator ou::tf::Bar::volume_t() { return std::floor( dblEmaVolume ); };
};

template<typename Scenario, typename Function>
void Process( ptime dtBegin, ptime dtEnd, size_t nMinBars, const setSymbols_t& setSymbols, Function fCheck ) {

  struct data_t {
    ou::tf::Bar::volume_t volumeEma;
    size_t nEnteredFilter;
    size_t nPassedFilter;
    data_t()
      : volumeEma {}, nEnteredFilter {}, nPassedFilter {}
      {}
  };

  data_t data;

  try {
    ou::tf::InstrumentFilter<data_t,ou::tf::Bars> filter(
      "/bar/86400/",
      dtBegin, dtEnd,
      200, data, //  need to figure out where 200 comes from, and the relation to nMinBars (=> 200sma)
      []( data_t&, const std::string& sPath, const std::string& sGroup )->bool{ // Use Group
        return true;
      },
      [nMinBars, dtEnd, &setSymbols]( data_t& data, const std::string& sObject, const ou::tf::Bars& bars )->bool{ // Filter
        //  std::cout << sObjectName << std::endl;
        data.nEnteredFilter++;
        bool bReturn( false );
        if ( nMinBars <= bars.Size() ) {
            ou::tf::Bars::const_iterator iterVolume = bars.end() - nMinBars;
            data.volumeEma = std::for_each( iterVolume, bars.end(), VolumeEma() );
            if ( ( 1000000 < data.volumeEma )
              && ( 30.0 <=  bars.last().Close() )
              && ( 500.0 >= bars.last().Close() )  // provides SPY at 4xx
              && ( dtEnd.date() == bars.last().DateTime().date() )
              && ( 120 < bars.Size() )
              ) {
              data.nPassedFilter++;
              bReturn = true;
            }
        }
        if ( !bReturn ) {
          setSymbols_t::const_iterator iterSymbol = setSymbols.find( sObject );
          if ( setSymbols.end() != iterSymbol ) {
            bReturn = true;
          }
        }
        return bReturn;
      },
      [&fCheck]( data_t& data, const std::string& sPath, const std::string& sObjectName, const ou::tf::Bars& bars ){ // Result

        double hv = std::for_each( bars.at( bars.Size() - 20 ), bars.end(), ou::HistoricalVolatility() );
        Scenario ii( sObjectName, sPath, bars.last(), data.volumeEma, hv );
        //CheckForDarvas( bars.begin(), bars.end(), ii, fSelected );
        //if ( "GLD" == sObjectName ) {
          fCheck( bars, ii );
        //}
        //          CheckFor10Percent( ii, bars.end() - 20, bars.end() );
        //          CheckForVolatility( ii, bars.end() - 20, bars.end() );
        //          CheckForRange( ii, bars.end() - m_nMinPivotBars, bars.end() );
      }
      );

    std::cout << "Items Checked: " << data.nEnteredFilter << ", Items passed: " << data.nPassedFilter << std::endl;
  }
  catch ( std::runtime_error& e ) {
    std::cout << "SymbolSelection - InstrumentFilter - " << e.what() << std::endl;
  }
  catch (... ) {
    std::cout << "SymbolSelection - Unknown Error - " << std::endl;
  }

  std::cout << "SymbolSelection - History Scanned." << std::endl;

}

SymbolSelection::SymbolSelection( const ptime dtLast )
  : m_dtLast( dtLast ), m_nMinBars( 20 )
{
  namespace gregorian = boost::gregorian;

  m_dtOneYearAgo = m_dtLast - gregorian::date_duration( 52 * 7 );
  m_dt26WeeksAgo = m_dtLast - gregorian::date_duration( 26 * 7 );
  m_dtDateOfFirstBar = m_dt26WeeksAgo;
}

SymbolSelection::SymbolSelection( const ptime dtLast, const setSymbols_t& setSymbols, fSelectedDarvas_t fSelected )
: SymbolSelection( dtLast )
{
  std::cout << "Darvas: AT=Aggressive Trigger, CT=Conservative Trigger, BO=Break Out Alert, stop=recommended stop" << std::endl;

  m_dtDarvasTrigger = m_dtLast - gregorian::date_duration( 8 );

  namespace ph = std::placeholders;
  Process<IIDarvas>(
    m_dtOneYearAgo, m_dtLast, m_nMinBars, setSymbols,
    std::bind( &SymbolSelection::CheckForDarvas, this, ph::_1, ph::_2, fSelected )
  );

}

// should just be returning candidates which pass the filter
// purpose here is to provide the probabilities for the weeklies
SymbolSelection::SymbolSelection( const ptime dtLast, const setSymbols_t& setSymbols, fSelectedPivot_t fSelected )
: SymbolSelection( dtLast )
{

  using mapSymbol_t = std::map<std::string,std::string>;
  mapSymbol_t mapSymbol;

  std::cout << "SignalGenerator parsing cboe spreadsheet ..." << std::endl;

  bool bOk( false );
  try {
    ou::tf::cboe::csv::ReadCboeWeeklyOptions(
      [&mapSymbol]( const ou::tf::cboe::csv::SymbolEntry& se ){
        mapSymbol[ se.sSymbol ] = se.sDescription;
      } );
    bOk = true;
  }
  catch( std::runtime_error& e ) {
    std::cout << "SignalGenerator - ReadCboeWeeklyOptions error: " << e.what() << std::endl;
  }
  catch(...) {
    std::cout << "SignalGenerator - unknown error during ReadCboeWeeklyOptions" << std::endl;
  }

  if ( bOk ) {

    std::cout << "SignalGenerator running eod and building output ..." << std::endl;

    size_t nSelected {};

    namespace ph = std::placeholders;
    Process<IIPivot>(
      m_dtOneYearAgo, m_dtLast, m_nMinBars, setSymbols,
      [&fSelected,&mapSymbol,&nSelected](const ou::tf::Bars& bars, IIPivot& ii){
        mapSymbol_t::const_iterator citer = mapSymbol.find( ii.sName );
        if ( mapSymbol.end() != citer ) {
          //const ou::tf::cboe::csv::UnderlyingInfo& ui( citer->second );
          ou::tf::statistics::Pivot pivot( bars );
          pivot.Points( ii.dblR2, ii.dblR1, ii.dblPV, ii.dblS1, ii.dblS2 );
          ii.dblProbabilityAboveAndUp   = pivot.ItemOfInterest( ou::tf::statistics::Pivot::EItemsOfInterest::BtwnPVR1_X_Up );
          ii.dblProbabilityAboveAndDown = pivot.ItemOfInterest( ou::tf::statistics::Pivot::EItemsOfInterest::BtwnPVR1_X_Down );
          ii.dblProbabilityBelowAndUp   = pivot.ItemOfInterest( ou::tf::statistics::Pivot::EItemsOfInterest::BtwnPVS1_X_Up );
          ii.dblProbabilityBelowAndDown = pivot.ItemOfInterest( ou::tf::statistics::Pivot::EItemsOfInterest::BtwnPVS1_X_Down );
          nSelected++;
          fSelected( ii );
        }
      }
    );

    std::cout << "SignalGenerator Complete, " << nSelected << " selected." << std::endl;
  }

}

SymbolSelection::~SymbolSelection(void) {
}

class AveragePrice {
private:
  double m_dblSumOfPrices;
  size_t m_nNumberOfValues;
protected:
public:
  AveragePrice() : m_dblSumOfPrices( 0 ), m_nNumberOfValues( 0 ) {};
  void operator() ( const ou::tf::Bar& bar ) {
    m_dblSumOfPrices += bar.Close();
    ++m_nNumberOfValues;
  }
  operator double() { return m_dblSumOfPrices / m_nNumberOfValues; };
};

void SymbolSelection::CheckForRange( citerBars begin, citerBars end, const InstrumentInfo& ii ) {
  citerBars iter1( begin );
  int cnt( 0 );
  int cntAbove( 0 );
  int cntBelow( 0 );
  int cntRising( 0 );
  int cntFalling( 0 );
  double dblAbove( 0 );
  double dblBelow( 0 );
  double dblRange( 0 );
  while ( iter1 != end ) {
    dblRange += ( iter1->High() - iter1->Low() );
    if ( iter1->High() > iter1->Open() ) {
      ++cntAbove;
      dblAbove += ( iter1->High() - iter1->Open() );
    }
    if ( iter1->Low() < iter1->Open() ) {
      ++cntBelow;
      dblBelow += ( iter1->Open() - iter1->Low() );
    }
    if ( iter1->Open() > iter1->Close() ) ++cntFalling;
    if ( iter1->Open() < iter1->Close() ) ++cntRising;
    ++cnt;
    ++iter1;
  }

  double avgAbove = dblAbove / cntAbove;
  double avgBelow = dblBelow / cntBelow;

  int diffCnt = cntAbove - cntBelow;  // minimize this
  double dblRatioAboveBelow = 0.5 - ( avgAbove / ( avgAbove + avgBelow ) ); // minimize this

  //m_mapRangeRanking.insert( mapRangeRanking_t::value_type( diffCnt, ii ) );
  std::cout
    << ii.sName << ","
    << diffCnt << ","
    << dblRatioAboveBelow << ","
    << avgAbove << ","
    << avgBelow << ","
//    << avgAbove + avgBelow << ","
    << dblRange
    << std::endl;
}

struct CalcMaxDate: public std::unary_function<ou::tf::Bar&, void> {
public:
  CalcMaxDate( void ) : dtMax( boost::date_time::special_values::min_date_time ), dblMax( 0 ) { };
  void operator() ( const ou::tf::Bar &bar ) {
    if ( bar.Close() >= dblMax ) {
      dblMax = bar.Close();
      dtMax = bar.DateTime();
    }
  }
  operator ptime() { return dtMax; };
private:
  ptime dtMax;
  double dblMax;
};

//
// ProcessPivots
//

// taken from scanner

void SymbolSelection::WrapUpPivots( setInstrumentInfo_t& selected ) {
  int cnt( 35 );  // select 35 symbols ( => < 3% weighting by symbol )
  for ( mapPivotRanking_t::const_reverse_iterator iter = m_mapPivotRanking.rbegin(); iter!= m_mapPivotRanking.rend(); ++iter ) {
    selected.insert( iter->second );
    --cnt;
    if ( 0 == cnt ) break;
  }
  m_mapPivotRanking.clear();
}

//
// ProcessDarvas
//

class ProcessDarvas: public ou::tf::Darvas<ProcessDarvas> {
  friend ou::tf::Darvas<ProcessDarvas>;
public:
  ProcessDarvas( std::stringstream& ss, size_t ix );
  ~ProcessDarvas( void ) {};
  bool Calc( const ou::tf::Bar& );
  double StopValue( void );  // should only be called once
protected:
  // CRTP from CDarvas<CProcess>
  void ConservativeTrigger( void );
  void AggressiveTrigger( void );
  void SetStop( double stop ) { m_dblStop = stop; };
//  void StopTrigger( void ) {};
  void BreakOutAlert( size_t );

private:

  std::stringstream& m_ss;

  size_t m_ix; // keeps track of index of trigger bar
  bool m_bTriggered;  // set when last bar has trigger
  double m_dblStop;

};

ProcessDarvas::ProcessDarvas( std::stringstream& ss, size_t ix )
: ou::tf::Darvas<ProcessDarvas>(), m_ss( ss ),
  m_bTriggered( false ), m_dblStop( 0 ), m_ix( ix )
{
}

bool ProcessDarvas::Calc( const ou::tf::Bar& bar ) {
  ou::tf::Darvas<ProcessDarvas>::Calc( bar );
  --m_ix;
  bool b = m_bTriggered;
  m_bTriggered = false;
  return b;
}

void ProcessDarvas::ConservativeTrigger( void ) {
  m_ss << " CT(" << m_ix << ")";
  m_bTriggered = true;
}

void ProcessDarvas::AggressiveTrigger( void ) {
  m_ss << " AT(" << m_ix << ")";
  m_bTriggered = true;
}

void ProcessDarvas::BreakOutAlert( size_t cnt ) {
  m_ss << " BO(" << m_ix << ")";
  m_bTriggered = true;
}

double ProcessDarvas::StopValue( void ) {
  m_ss << " stop(" << m_dblStop << ")";
  return m_dblStop;
}

void SymbolSelection::CheckForDarvas( const ou::tf::Bars& bars, IIDarvas& ii, fSelectedDarvas_t& fSelected ) {
  size_t nTriggerWindow( 10 );
  ptime dtDayOfMax = std::for_each( bars.begin(), bars.end(), CalcMaxDate() );
  //citerBars iterLast( end - 1 );
  if ( dtDayOfMax >= m_dtDarvasTrigger ) {
    std::stringstream ss;
    ss << "Darvas max for " << ii.sName
      << " on " << dtDayOfMax
      << ", close=" << bars.last().Close()
      << ", volume=" << bars.last().Volume();

    ProcessDarvas darvas( ss, nTriggerWindow );
    //size_t ix = end - nTriggerWindow;
    citerBars iterTriggerBegin = bars.end() - nTriggerWindow;
    bool bTrigger;  // wait for trigger on final day
    for ( citerBars iter = iterTriggerBegin; iter != bars.end(); ++iter ) {
      bTrigger = darvas.Calc( *iter );  // final day only is needed
    }

    if ( bTrigger ) {
      ii.dblStop = darvas.StopValue();
      //m_psetSymbols->insert( ii );
      fSelected( ii );
      std::cout << ss.str() << std::endl;
    }
  }
}

void SymbolSelection::CheckFor10Percent( citerBars begin, citerBars end, const InstrumentInfo& ii ) {
  double dblAveragePrice = std::for_each( begin, end, AveragePrice() );
  citerBars iterLast( end - 1 );
  if ( 25.0 < dblAveragePrice ) {
    mapRankingPos_t::iterator iterPos;
    mapRankingNeg_t::iterator iterNeg;
    //double dblReturn = ( m_bars.Last()->m_dblClose - m_bars.Last()->m_dblOpen ) / m_bars.Last()->m_dblClose;
    double dblReturn = ( iterLast->Close() - iterLast->Open() ) / iterLast->Close();
    if ( m_nMaxInList > m_mapMaxPositives.size() ) {
      m_mapMaxPositives.insert( pairRanking_t( dblReturn, ii ) );  // need to insert the path instead
    }
    else {
      iterPos = m_mapMaxPositives.begin();
      if ( dblReturn > iterPos->first ) {
        m_mapMaxPositives.erase( iterPos );
        m_mapMaxPositives.insert( pairRanking_t( dblReturn, ii ) ); // need to insert the path instead
      }
    }
    if ( m_nMaxInList > m_mapMaxNegatives.size() ) {
      m_mapMaxNegatives.insert( pairRanking_t( dblReturn, ii ) );// need to insert the path instead
    }
    else {
      iterNeg = m_mapMaxNegatives.begin();
      if ( dblReturn < iterNeg->first ) {
        m_mapMaxNegatives.erase( iterNeg );
        m_mapMaxNegatives.insert( pairRanking_t( dblReturn, ii ) );// need to insert the path instead
      }
    }
  }
}

void SymbolSelection::WrapUp10Percent( setInstrumentInfo_t& selected ) {
  std::cout << "Positives: " << std::endl;
  mapRankingPos_t::iterator iterPos;
  for ( iterPos = m_mapMaxPositives.begin(); iterPos != m_mapMaxPositives.end(); ++iterPos ) {
//    size_t pos = (iterPos->second).find_last_of( "/" );
//    std::string sSymbolName = (iterPos->second).substr( pos + 1 );
//    std::cout << " " << sSymbolName << "=" << iterPos->first << std::endl;
    std::cout << iterPos->second.sName << ": " << iterPos->first << std::endl;
//    if ( NULL != OnAddSymbol ) OnAddSymbol( sSymbolName, iterPos->second, "10%+" );
    selected.insert( iterPos->second );
  }
  m_mapMaxPositives.clear();

  std::cout << "Negatives: " << std::endl;
  mapRankingNeg_t::iterator iterNeg;
  for ( iterNeg = m_mapMaxNegatives.begin(); iterNeg != m_mapMaxNegatives.end(); ++iterNeg ) {
//    size_t pos = (iterNeg->second).find_last_of( "/" );
//    std::string sSymbolName = (iterNeg->second).substr( pos + 1 );
//    std::cout << " " << sSymbolName << "=" << iterNeg->first << std::endl;
    std::cout << " " << iterNeg->second.sName << ": " << iterNeg->first << std::endl;
//    if ( NULL != OnAddSymbol ) OnAddSymbol( sSymbolName, iterNeg->second, "10%-" );
    selected.insert( iterNeg->second );
  }
  m_mapMaxNegatives.clear();
}

class AverageVolatility {
  // volatility is a misnomer, in actual fact, looking for measure highest returns in period of time
  // therefore may want to maximize the mean, and minimize the standard deviation
  // therefore normalize based upon pg  86 of Black Scholes and Beyond,
  // then choose the one with
private:
  double m_dblSumReturns;
  size_t m_nNumberOfValues;
//  std::vector<double> m_returns;
protected:
public:
  AverageVolatility() : m_dblSumReturns( 0 ), m_nNumberOfValues( 0 ) {};
  void operator() ( const ou::tf::Bar& bar ) {
    //m_dblVolatility += ( bar.High() - bar.Low() ) / bar.Close;
    //m_dblVolatility += ( bar.High() - bar.Low() ) / bar.Low();
    //m_dblVolatility += ( bar.Open() - bar.Close() ) / bar.Close();
    //m_dblVolatility += ( bar.m_dblHigh - bar.m_dblLow ) ;
    double return_ = log( bar.Close() / bar.Open() );
//    m_returns.push_back( return_ );
    m_dblSumReturns += return_;
    ++m_nNumberOfValues;
  }
  operator double() {
    double mean = m_dblSumReturns / ( m_nNumberOfValues - 1 );
//    double dblDiffsSquared = 0;
//    double diff;
//    for ( std::vector<double>::const_iterator iter = m_returns.begin(); iter != m_returns.end(); iter++ ) {
//      diff = *iter - mean;
//      dblDiffsSquared += mean * mean;
//    }
//    double volatility = sqrt( dblDiffsSquared / m_nNumberOfValues );
//    return mean * volatility; // a measure of best trading range
    return mean;
  };
};

void SymbolSelection::CheckForVolatility( citerBars begin, citerBars end, const InstrumentInfo& ii ) {
  double dblAveragePrice = std::for_each( begin, end, AveragePrice() );
  citerBars iterLast( end - 1 );
  if ( 25.0 < dblAveragePrice ) {
    double dblAverageVolatility = std::for_each( begin, end, AverageVolatility() );
    mapRankingPos_t::iterator iterPos;
    if ( m_nMaxInList > m_mapMaxVolatility.size() ) {
      m_mapMaxVolatility.insert( pairRanking_t( dblAverageVolatility, ii ) );
    }
    else {
      iterPos = m_mapMaxVolatility.begin();
      if ( dblAverageVolatility > iterPos->first ) {
        m_mapMaxVolatility.erase( iterPos );
        m_mapMaxVolatility.insert( pairRanking_t( dblAverageVolatility, ii ) );
      }
    }
  }
}

void SymbolSelection::WrapUpVolatility( setInstrumentInfo_t& selected ) {
  std::cout << "Volatiles: " << std::endl;
  mapRankingPos_t::iterator iterPos;
  for ( iterPos = m_mapMaxVolatility.begin(); iterPos != m_mapMaxVolatility.end(); ++iterPos ) {
//    size_t pos = (iterPos->second).find_last_of( "/" );
//    std::string sSymbolName = (iterPos->second).substr( pos + 1 );;
//    cout << " " << sSymbolName << "=" << iterPos->first << endl;
    std::cout << " " << iterPos->second.sName << ": " << iterPos->first << std::endl;
//    if ( NULL != OnAddSymbol ) OnAddSymbol( sSymbolName, iterPos->second, "Volatile" );
    selected.insert( iterPos->second );
  }
  m_mapMaxVolatility.clear();
}
