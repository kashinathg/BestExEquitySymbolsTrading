
#include <wx/timer.h>
#include <wx/treectrl.h>

#include <OUCommon/Worker.h>

#include <OUCharting/ChartDataView.h>
#include <OUCharting/ChartMaster.h>

#include <TFBitsNPieces/FrameWork01.h>

#include <TFTrading/DBOps.h>
#include <TFTrading/PortfolioManager.h>

#include <TFOptions/Bundle.h>
#include <TFOptions/NoRiskInterestRateSeries.h>

#include <TFIQFeed/LoadMktSymbols.h>

#include <TFVuTrading/FrameMain.h>
#include <TFVuTrading/PanelLogging.h>
//#include <TFVuTrading/PanelManualOrder.h>
#include <TFVuTrading/WinChartView.h>

#include "EventUpdateOptionTree.h"
#include "Strategy1.h"

class AppHedgedBollinger:
  public wxApp, public ou::tf::FrameWork01<AppHedgedBollinger> {
    friend ou::tf::FrameWork01<AppHedgedBollinger>;
public:
protected:
private:

  enum EProviderUsage { EProviderUsageUnknown, EProviderUsageRead, EProviderUsageReadTrade };

  typedef ou::tf::Instrument::pInstrument_t pInstrument_t;
  typedef ou::tf::Portfolio::pPortfolio_t pPortfolio_t;

  std::string m_sNameUnderlying;
  std::string m_sNameUnderlyingIQFeed;
  std::string m_sNameOptionUnderlying;

  boost::gregorian::date m_dateFrontMonthOption;
  boost::gregorian::date m_dateSecondMonthOption;

  ou::action::Worker m_worker;

  FrameMain* m_pFrameMain;
//  PanelOptionsParameters* m_pPanelOptionsParameters;
  ou::tf::PanelLogging* m_pPanelLogging;
//  ou::tf::PanelManualOrder* m_pPanelManualOrder;

  ou::ChartMaster m_chart;
  ou::tf::WinChartView* m_pWinChartView;

  Strategy* m_pStrategy;

  ou::tf::DBOps m_db;

  pPortfolio_t m_pPortfolioMaster;
  pPortfolio_t m_pPortfolioCurrencyUSD;

  pPortfolio_t m_pPortfolioGC;
  pPortfolio_t m_pPortfolioGCLongs;
  pPortfolio_t m_pPortfolioGCShorts;

  wxTimer m_timerGuiRefresh;
  ptime m_dtTopOfMinute;

  static const unsigned int m_nthIVCalc;
  unsigned int m_cntIVCalc;  // calc IV every nth gui refresh
  volatile bool m_bIVCalcActive;

  wxTreeCtrl* m_ptreeChartables;  // http://docs.wxwidgets.org/trunk/classwx_tree_ctrl.html

  boost::thread* m_pthreadIVCalc;
  ou::tf::LiborFromIQFeed m_libor;

  ou::tf::iqfeed::InMemoryMktSymbolList m_listIQFeedSymbols;

  EProviderUsage m_eProviderUsage;
  ou::tf::option::MultiExpiryBundle* m_pBundle;

  virtual bool OnInit();
  virtual int OnExit();
  void OnClose( wxCloseEvent& event );

  void HandleRegisterTables( ou::db::Session& session );
  void HandleRegisterRows( ou::db::Session& session );
  void HandlePopulateDatabase( void );

  void HandleGuiRefresh( wxTimerEvent& event );

  void UpdateTree( ou::tf::option::Option*, bool bWatching );

  //void AutoStartCollection( void );

  void OnData1Connected( int );
  void OnData2Connected( int );
  void OnExecConnected( int );
  void OnData1Disconnected( int );
  void OnData2Disconnected( int );
  void OnExecDisconnected( int );

  void CalcIV( ptime dt );

  void HandleMenuAction0ObtainNewIQFeedSymbolListRemote( void );
  void HandleMenuAction1ObtainNewIQFeedSymbolListLocal( void );
  void HandleMenuAction2LoadIQFeedSymbolList( void );
  void HandleMenuActionInitializeSymbolSet( void );
  void HandleMenuActionSaveSymbolSubset( void );
  void HandleMenuActionLoadSymbolSubset( void );
  void HandleMenuActionStartWatch( void );
  void HandleMenuActionStopWatch( void );
  void HandleMenuActionSaveValues( void );
  void HandleMenuActionEmitYieldCurve( void );
  void HandleMenuActionStartChart( void );
  void HandleMenuActionStopChart( void );
  void HandleMenuActionEmitStrategyValues( void );

  void HandleIBUnderlyingContractDetails( const ou::tf::ib::TWS::ContractDetails&, pInstrument_t& );
  void HandleIBUnderlyingContractDetailsDone( void );
  void FinishStrategyInitialization( pInstrument_t pInstrumentUnderlying );

  void HandleObtainNewIQFeedSymbolListRemote( void );
  void HandleObtainNewIQFeedSymbolListLocal( void );
  void HandleLoadIQFeedSymbolList( void );
  void HandleSaveValues( void );

//  void HandleDrawChart( const MemBlock& );
  void HandlePaint( wxPaintEvent& event );
  void HandleSize( wxSizeEvent& event );
  void HandleMouse( wxMouseEvent& event );

  void HandleStrikeWatchOn( ou::tf::option::Strike& );
  void HandleStrikeWatchOff( ou::tf::option::Strike& );

  void HandleGuiUpdateOptionTree( EventUpdateOptionTree& event );

};

// Implements MyApp& wxGetApp()
DECLARE_APP(AppHedgedBollinger)
