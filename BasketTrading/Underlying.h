

#pragma once

#include <string>

#include <OUCharting/ChartDataView.h>
#include <OUCharting/ChartEntryBars.h>
#include <OUCharting/ChartEntryMark.h>
#include <OUCharting/ChartEntryVolume.h>
#include <OUCharting/ChartEntryIndicator.h>

#include <TFTimeSeries/BarFactory.h>

#include <TFOptions/Aggregate.h>
#include <TFOptions/GatherOptions.h>

#include <TFTrading/Watch.h>
#include <TFTrading/Portfolio.h>

#include <TFBitsNPieces/BollingerTransitions.h>

//#include "PivotCrossing.h"

 // contains:
 //  1) will contain active strategies,
 //  2) gex calcs
 //  3) structures common to assigned strategies

class Underlying {
public:

  using pWatch_t = ou::tf::Watch::pWatch_t;
  using pPortfolio_t = ou::tf::Portfolio::pPortfolio_t;
  using pChartDataView_t = ou::ChartDataView::pChartDataView_t;
  using fOption_t = ou::tf::option::fOption_t;
  using fGatherOptions_t = ou::tf::option::fGatherOptions_t;

  Underlying( pWatch_t, pPortfolio_t );
  ~Underlying();

  void ReadDailyBars( const std::string& sDailyBarPath );
  void SetPivots( double dblR2, double dblR1, double dblPV, double dblS1, double dblS2 );
  void PopulateChartDataView( pChartDataView_t ); // share data sets
  void PopulateChains( fGatherOptions_t&& );
  void WalkChains( fOption_t&& ) const;
  void WalkChain( boost::gregorian::date, fOption_t&& ) const;

  void UpdateChart( boost::posix_time::ptime );

  pWatch_t GetWatch() { return m_pWatch; }
  pPortfolio_t GetPortfolio() { return m_pPortfolio; }
  pChartDataView_t GetChartDataView() { return m_pChartDataView; }

  void SaveSeries( const std::string& sPrefix );

  // TODO: will need two mapChain types:
  //   1) basic for passing to strategy
  //   2) gex calcs

protected:
private:

  enum EChartSlot { Price, Volume, PL, Tick };

  using gex_t = ou::tf::option::Aggregate;

  pWatch_t m_pWatch;
  pPortfolio_t m_pPortfolio; // aggregates strategies associated with underlying

  gex_t m_GexCalc;

  pChartDataView_t m_pChartDataView;

  ou::tf::BarFactory m_bfTrades06Sec; // charting

  ou::ChartEntryBars m_cePrice;
  ou::ChartEntryVolume m_ceVolume;
  ou::ChartEntryMark m_cePivots;

  ou::ChartEntryIndicator m_cePLCurrent;
  ou::ChartEntryIndicator m_cePLUnRealized;
  ou::ChartEntryIndicator m_cePLRealized;
  ou::ChartEntryIndicator m_ceCommissionPaid;

  ou::tf::BollingerTransitions m_BollingerTransitions;

  void HandleQuote( const ou::tf::Quote& quote );
  void HandleTrade( const ou::tf::Trade& trade );
  void HandleBarTrades06Sec( const ou::tf::Bar& bar );

};
