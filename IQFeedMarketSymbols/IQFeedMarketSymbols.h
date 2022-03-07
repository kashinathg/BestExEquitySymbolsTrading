
#pragma once


#include <map>

#include <OUCommon/Worker.h>

#include <TFBitsNPieces/FrameWork01.h>

#include <TFIQFeed/LoadMktSymbols.h>

#include <TFVuTrading/FrameMain.h>
#include <TFVuTrading/PanelLogging.h>

class AppIQFeedMarketSymbols:
  public wxApp, public ou::tf::FrameWork01<AppIQFeedMarketSymbols> {
    friend ou::tf::FrameWork01<AppIQFeedMarketSymbols>;
public:

protected:
private:

  typedef ou::tf::iqfeed::InMemoryMktSymbolList::trd_t trd_t;

  ou::action::Worker m_worker;

  FrameMain* m_pFrameMain;
  ou::tf::PanelLogging* m_pPanelLogging;

  ou::tf::iqfeed::InMemoryMktSymbolList m_listIQFeedSymbols;

  typedef std::map<boost::uint32_t, uint32_t> mapCounts_t;
  typedef mapCounts_t::const_iterator citerMapCounts_t;
  mapCounts_t m_mapSIC;
  mapCounts_t m_mapNAICS;

  virtual bool OnInit();
  virtual int OnExit();
  void OnClose( wxCloseEvent& event );

  void HandleMenuAction0ObtainNewIQFeedSymbolListRemote( void );
  void HandleMenuAction1ObtainNewIQFeedSymbolListLocal( void );
  void HandleMenuAction2LoadIQFeedSymbolList( void );
  void HandleMenuActionScanSymbolList( void );
  void HandleMenuActionLoadSICCodes( void );

  void HandleObtainNewIQFeedSymbolListRemote( void );
  void HandleObtainNewIQFeedSymbolListLocal( void );
  void HandleLoadIQFeedSymbolList( void );

};

// Implements MyApp& wxGetApp()
DECLARE_APP(AppIQFeedMarketSymbols)
