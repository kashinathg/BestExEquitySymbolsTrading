#ifndef MANAGESTRATEGY_H
#define MANAGESTRATEGY_H

#include <string>
#include <memory>
#include <functional>

#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>

#include <OUCharting/ChartDataView.h>
#include <OUCharting/ChartEntryBars.h>
#include <OUCharting/ChartEntryMark.h>
#include <OUCharting/ChartEntryShape.h>
#include <OUCharting/ChartEntryVolume.h>
#include <OUCharting/ChartEntryIndicator.h>

#include <TFTimeSeries/BarFactory.h>

#include <TFIQFeed/MarketSymbol.h>

#include <TFOptions/Chain.h>
#include <TFOptions/Chains.h>
#include <TFOptions/Option.h>

#include <TFOptionCombos/Collar.h>
#include <TFOptionCombos/SpreadSpecs.h>

#include <TFTrading/Position.h>
#include <TFTrading/Portfolio.h>
#include <TFTrading/DailyTradeTimeFrames.h>

#include "ValidateOptions.h"

class OptionRepository;

class ManageStrategy:
  public ou::tf::DailyTradeTimeFrame<ManageStrategy>
{
  friend class boost::serialization::access;
  friend ou::tf::DailyTradeTimeFrame<ManageStrategy>;
public:

  using pInstrument_t = ou::tf::Instrument::pInstrument_t;
  using pWatch_t      = ou::tf::option::Option::pWatch_t;
  using pOption_t     = ou::tf::option::Option::pOption_t;

  using pPosition_t  = ou::tf::Position::pPosition_t;
  using pPortfolio_t = ou::tf::Portfolio::pPortfolio_t;

  using idPortfolio_t = ou::tf::Portfolio::idPortfolio_t;

  using pOrder_t = ou::tf::Position::pOrder_t;

  using fGatherOptions_t = ou::tf::option::fGatherOptions_t;

  //using fConstructedWatch_t  = std::function<void(pWatch_t)>;
  using fConstructedOption_t = std::function<void(pOption_t)>;

  //using fConstructWatch_t  = std::function<void(const std::string&, fConstructedWatch_t&&)>;
  using fConstructOption_t = std::function<void(const std::string&, fConstructedOption_t&&)>;  // source from IQFeed Symbol Name

  using fConstructPosition_t = std::function<pPosition_t( const ou::tf::Portfolio::idPortfolio_t&, pWatch_t, const std::string& )>; // string used for "leg=x" for second day

  using fConstructPortfolio_t = std::function<pPortfolio_t( const idPortfolio_t&, const idPortfolio_t&)>; // id of new, id of ManageStrategy portfolio

  using fAuthorizeSimple_t     = std::function<bool(const idPortfolio_t&, const std::string& /* sUnderlying */,bool /* has active positions */)>;
  using fAuthorizeUnderlying_t = std::function<bool(pOrder_t&,pPosition_t&,pPortfolio_t&)>;
  using fAuthorizeOption_t     = std::function<bool(pOrder_t&,pPosition_t&,pPortfolio_t&,pWatch_t&)>;

  using fRegisterOption_t = std::function<void(pOption_t&)>;
  using fStartCalc_t = std::function<void(pOption_t,pWatch_t)>; // option, underlying
  using fStopCalc_t =  std::function<void(pOption_t,pWatch_t)>; // option, underlying

  using fFirstTrade_t = std::function<void(ManageStrategy&,const ou::tf::Trade&)>;
  using fBar_t        = std::function<void(ManageStrategy&,const ou::tf::Bar&)>;

  using pChartDataView_t = ou::ChartDataView::pChartDataView_t;

  ManageStrategy(
    //const ou::tf::Bar& barPriorDaily,
    double dblSlopeUnderlying,
    // TODO: convert these to left assign
    pWatch_t, // underlying
    pPortfolio_t, // owning portfolio
    boost::gregorian::date dateTrading,
    const ou::tf::option::SpreadSpecs&,
    fGatherOptions_t&&,
    //fConstructWatch_t,
    fConstructOption_t &&,
    fConstructPosition_t&&,
    fConstructPortfolio_t&&,
    fRegisterOption_t&&,
    fStartCalc_t&&,
    fStopCalc_t&&,
    fFirstTrade_t,
    fAuthorizeUnderlying_t,
    fAuthorizeOption_t,
    fAuthorizeSimple_t,
    fBar_t
    );
  virtual ~ManageStrategy( );

  void Run();

  pPortfolio_t GetPortfolio() { return m_pCombo->GetPortfolio(); }

  ou::tf::DatedDatum::volume_t CalcShareCount( double dblAmount ) const;
  //void SetFundsToTrade( double dblFundsToTrade ) { m_dblFundsToTrade = dblFundsToTrade; };
  void ClosePositions( void );
  void SaveSeries( const std::string& sPrefix );

  void AddPosition( pPosition_t ); // add pre-existing position

  pChartDataView_t GetChartDataView() { return m_pChartDataView; }

  void Test( void );

  void TakeProfits();  // close any profitable leg
  void CloseExpiryItm( boost::gregorian::date );
  void CloseFarItm(); // if one leg is far otm, close the far itm leg (far otm < 0.10, profit > 0.20?
  void CloseForProfits(); // sum(positions) > 0.10 profit (slippage, commissions), not CloseFarItm.
  void CloseItmLeg(); // leg needs to be positive, but overall combo may not be in the profit

  double EmitInfo();

protected:
private:

  enum EChartSlot { Price, Volume, PL, Tick };

  enum ETradingState {
    TSInitializing, // set for duration of class initialization
    TSWaitForFirstTrade,  // wait for first trade during Regular Trading Hours
//    TSWaitForFundsAllocation,  // flagged, reached only after first trade has arrived
    TSWaitForEntry, // start of equity trading
    TSOptionEvaluation, // start of option trading
    TSComboMonitor,
    TSMonitorLong, TSMonitorShort,
    TSNoMore
  };
  ETradingState m_stateTrading;

  boost::gregorian::date m_dateTrading;

  enum class ETradeDirection { None, Up, Down };
  ETradeDirection m_eTradeDirection;

  enum class EmaState { EmaUnstable, EmaUp, EmaDown };
  EmaState m_stateEma;

  //enum class EBarDirection { None, Up, Down };
  //EBarDirection m_rBarDirection[ 3 ];

  size_t m_nConfirmationIntervals;

  const ou::tf::option::SpreadSpecs& m_specsSpread;

  using volume_t = ou::tf::DatedDatum::volume_t;

  //std::string m_sUnderlying;
  double m_dblSlope20DayUnderlying;

  pWatch_t m_pWatchUnderlying;

  double m_dblOpen;

  ou::tf::Trade m_TradeUnderlyingLatest;
  ou::tf::Quote m_QuoteUnderlyingLatest;

//  double m_cntUpReturn;
//  double m_cntDnReturn;

  using chain_t = ou::tf::option::Chain<ou::tf::option::chain::OptionName>;
  using mapChains_t = std::map<boost::gregorian::date, chain_t>;
  mapChains_t m_mapChains;

  //fConstructWatch_t m_fConstructWatch;
  fConstructOption_t m_fConstructOption;
  fConstructPosition_t m_fConstructPosition;
  fConstructPortfolio_t m_fConstructPortfolio;

  fAuthorizeSimple_t m_fAuthorizeSimple;
  fAuthorizeUnderlying_t m_fAuthorizeUnderlying;
  fAuthorizeOption_t m_fAuthorizeOption;

  std::unique_ptr<OptionRepository> m_pOptionRepository;

  fFirstTrade_t m_fFirstTrade;
  fBar_t m_fBar;

  //const ou::tf::Bar& m_barPriorDaily;

  pPortfolio_t m_pPortfolioOwning; // owning portfolio

  //PivotCrossing m_pivotCrossing;

  pChartDataView_t m_pChartDataView;

  size_t m_ixColour;  // index into rColour for assigning colours to leg p/l

  using pValidateOptions_t = std::unique_ptr<ValidateOptions>;
  pValidateOptions_t m_pValidateOptions;

  using pCombo_t = std::unique_ptr<ou::tf::option::Combo>;
  pCombo_t m_pCombo;

  ou::tf::BarFactory m_bfQuotes01Sec; // provides more frequent ticks for Order Monitoring

  ou::tf::BarFactory m_bfTrades01Sec; // ema calcs
  ou::tf::BarFactory m_bfTrades06Sec; // charting
  //ou::tf::BarFactory m_bfTrades60Sec; // sentiment analysis

  //ou::tf::BarFactory m_bfTicks06sec; // monitors liquidity, use to determine a minimum count for entry

  ou::ChartEntryVolume m_ceTickCount;

  ou::ChartEntryIndicator m_ceProfitLossPortfolio;

//  ou::ChartEntryIndicator m_ceDiffDelta;
//  ou::ChartEntryIndicator m_ceDiffGamma;
//  ou::ChartEntryIndicator m_ceDiffVega;
//  ou::ChartEntryIndicator m_ceDiffTheta;

  //ou::ChartEntryIndicator m_ceUpReturn;
  //ou::ChartEntryIndicator m_ceDnReturn;

  ou::ChartEntryShape m_ceShortEntries;
  ou::ChartEntryShape m_ceLongEntries;
  ou::ChartEntryShape m_ceShortFills;
  ou::ChartEntryShape m_ceLongFills;
  ou::ChartEntryShape m_ceShortExits;
  ou::ChartEntryShape m_ceLongExits;

  // https://stats.stackexchange.com/questions/111851/standard-deviation-of-an-exponentially-weighted-mean
  // http://people.ds.cam.ac.uk/fanf2/hermes/doc/antiforgery/stats.pdf
  struct EMA {

    double dblCoef1; // smaller - used on arriving value
    double dblCoef2; // 1 - dblCoef1 (larger), used on prior ema
    double dblEmaLatest;
    //double dblSD;
    //double dblSn;
    enum class State { falling, same, rising };
    State state;

    //ou::ChartEntryIndicator m_ceUpperBollinger;
    ou::ChartEntryIndicator m_ceEma;
    //ou::ChartEntryIndicator m_ceLowerBollinger;
    //ou::ChartEntryIndicator m_ceSD;
    pChartDataView_t pChartDataView;

    EMA( unsigned int nIntervals, pChartDataView_t pChartDataView_, ou::Colour::enumColour colour )
    : dblEmaLatest {}, pChartDataView( pChartDataView_ ), state( State::same )//, dblSD {}, dblSn {}
    {
      dblCoef1 = 2.0 / ( nIntervals + 1 );
      dblCoef2 = 1.0 - dblCoef1;
      //pChartDataView->Add( 0, &m_ceUpperBollinger );
      pChartDataView->Add( 0, &m_ceEma );
      //pChartDataView->Add( 0, &m_ceLowerBollinger );
      //pChartDataView->Add( 0, &m_ceUpperBollinger );
      //pChartDataView->Add( 3, &m_ceSD );
      //m_ceUpperBollinger.SetColour( colour );
      m_ceEma.SetColour( colour );
      //m_ceLowerBollinger.SetColour( colour );
      //m_ceSD.SetColour( colour );
    }

    ~EMA() {
      pChartDataView->Remove( 0, &m_ceEma ); // helps with non-deterministic cleanup
    }

    void SetName( const std::string& sName ) {
      m_ceEma.SetName( sName );
    }

    double First( boost::posix_time::ptime dt, double value ) {
      dblEmaLatest = value;
      //m_ceUpperBollinger.Append( dt, 0.0 );
      m_ceEma.Append( dt, dblEmaLatest );
      //m_ceLowerBollinger.Append( dt, 0.0 );
      //m_ceSD.Append( dt, 0.0 );
      return dblEmaLatest;
    }

    double Update( boost::posix_time::ptime dt, double value ) {
      double dblPrevious( dblEmaLatest );
      dblEmaLatest = ( dblCoef1 * value ) + ( dblCoef2 * dblEmaLatest );
      //double diff = ( value - dblPrevious );
      //dblSn = dblCoef2 * ( dblSn + dblCoef1 * diff * diff );
      // std dev is sqrt( dblSn ) ie, Sn is variance
      if ( dblEmaLatest == dblPrevious ) {
        state = State::same;
      }
      else {
        state = ( dblEmaLatest > dblPrevious ) ? State::rising : State::falling;
      }
      //dblSD = std::sqrt( dblSn ); // calc std dev
      //double range = 2.0 * dblSD; // calc constant * std dev
      //m_ceUpperBollinger.Append( dt, dblEmaLatest + range );
      m_ceEma.Append( dt, dblEmaLatest );
      //m_ceLowerBollinger.Append( dt, dblEmaLatest - range );
      //m_ceSD.Append( dt, range );

      return dblEmaLatest;
    }
  };

  using pEMA_t = std::shared_ptr<EMA>;
  using vEMA_t = std::vector<pEMA_t>;
  vEMA_t m_vEMA;

  void ComboPrepare( boost::gregorian::date );

  void HandleBarQuotes01Sec( const ou::tf::Bar& bar );

  void HandleBarTrades01Sec( const ou::tf::Bar& bar );
  void HandleBarTrades06Sec( const ou::tf::Bar& bar );
  void HandleBarTrades60Sec( const ou::tf::Bar& bar );
  void HandleBarTicks06Sec( const ou::tf::Bar& bar );

  void HandleQuoteUnderlying( const ou::tf::Quote& quote );
  void HandleTradeUnderlying( const ou::tf::Trade& trade );

  // one shot inherited states:
  void HandleBellHeard( boost::gregorian::date, boost::posix_time::time_duration );
  void HandleCancel( boost::gregorian::date, boost::posix_time::time_duration );
  void HandleGoNeutral( boost::gregorian::date, boost::posix_time::time_duration );
  void HandleAtRHClose( boost::gregorian::date, boost::posix_time::time_duration );

  void RHEquity( const ou::tf::Bar& bar );
  void RHOption( const ou::tf::Bar& bar );

  void HandleGoingNeutral( const ou::tf::Bar& bar );
  void HandleGoingNeutral( const ou::tf::Quote& quote ) {};
  void HandleGoingNeutral( const ou::tf::Trade& trade ) {};

  // Datum based inherited states:
  void HandleRHTrading( const ou::tf::Quote& quote );
  void HandleRHTrading( const ou::tf::Trade& trade );
  void HandleRHTrading( const ou::tf::Bar& bar );

  void HandleAfterRH( const ou::tf::Quote& quote );
  void HandleAfterRH( const ou::tf::Trade& trade );
  void HandleAfterRH( const ou::tf::Bar& bar );

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()
};

BOOST_CLASS_VERSION(ManageStrategy, 1)

#endif /* MANAGESTRATEGY_H */
