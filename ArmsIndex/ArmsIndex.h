#pragma once

// Started 02/22/2022

#include <TFBitsNPieces/FrameWork01.h>

// may need to inherit and add more functionality to the class:
#include <TFTrading/DBOps.h>

#include <TFVuTrading/FrameMain.h>
#include <TFVuTrading/PanelLogging.h>

#include "EventProviderConnected.h"
#include "PanelArmsIndex.h"

class AppArmsIndex:
  public wxApp, public ou::tf::FrameWork01<AppArmsIndex> {
    friend ou::tf::FrameWork01<AppArmsIndex>;
public:
protected:
private:

  //typedef ou::tf::IBTWS::pInstrument_t pInstrument_t;
  //typedef ou::tf::Instrument::pInstrument_t pInstrument_t;

  bool m_bData1Connected;
  bool m_bExecConnected;
  bool m_bStarted;

  std::string m_sDbName;

  wxTimer m_timerGuiRefresh;

  FrameMain* m_pFrameMain;

  ou::tf::PanelArmsIndex* m_pPanelArmsIndex;
  ou::tf::PanelLogging* m_pPanelLogging;

  ou::tf::DBOps m_db;

  virtual bool OnInit();
  virtual int OnExit();
  void OnClose( wxCloseEvent& event );

  void Start( void );

  void OnIQFeedConnected( int );

  void OnData1Connected( int );
  void OnExecConnected( int );
  void OnData1Disconnected( int );
  void OnExecDisconnected( int );

  void HandlePopulateDatabase( void );
  void HandleLoadDatabase( void );

  void HandleRegisterTables( ou::db::Session& session );
  void HandleRegisterRows( ou::db::Session& session );

  void HandleGuiRefresh( wxTimerEvent& event );
  void HandleProviderConnected( EventProviderConnected& );

};

// Implements MyApp& wxGetApp()
DECLARE_APP(AppArmsIndex)
