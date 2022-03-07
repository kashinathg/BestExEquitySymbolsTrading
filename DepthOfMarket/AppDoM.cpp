/*
TODO:
  * select symbol
  * start watch:
      obtain trades, match against orders
      obtain quotes, match against top of each book
  * start Market Depth
      validate against trade stream for actual orders (limits vs market)
*/

#include <wx/defs.h>
#include <wx/sizer.h>

#include "Config.h"

#include "AppDoM.h"

IMPLEMENT_APP(AppDoM)

bool AppDoM::OnInit() {

  int code = 1;

  config::Options options;

  if ( !Load( options ) ) {
    code = 0;
  }
  else {

    m_pFrameMain = new FrameMain( nullptr, wxID_ANY, "Depth of Market" );
    wxWindowID idFrameMain = m_pFrameMain->GetId();
    //m_pFrameMain->Bind( wxEVT_SIZE, &AppStrategy1::HandleFrameMainSize, this, idFrameMain );
    //m_pFrameMain->Bind( wxEVT_MOVE, &AppStrategy1::HandleFrameMainMove, this, idFrameMain );
    //m_pFrameMain->Center();
  //  m_pFrameMain->Move( -2500, 50 );

    wxBoxSizer* sizerMain = new wxBoxSizer(wxVERTICAL);
    m_pFrameMain->SetSizer( sizerMain );

    m_pFrameMain->SetSize( 675, 800 );
    SetTopWindow( m_pFrameMain );

    //Bind(
    //  wxEVT_SIZE,
    //  [this](wxSizeEvent& event){
    //    std::cout << "w=" << event.GetSize().GetWidth() << ",h=" << event.GetSize().GetHeight() << std::endl;
    //    event.Skip();
    //    }//,
    //  //idFrameMain
    //  );

    wxSize size;

    wxBoxSizer* sizerControls = new wxBoxSizer( wxHORIZONTAL );
    sizerMain->Add( sizerControls, 0, wxLEFT|wxTOP|wxRIGHT, 4 );

    m_pPanelProviderControl = new ou::tf::PanelProviderControl( m_pFrameMain, wxID_ANY );
    sizerControls->Add( m_pPanelProviderControl, 1, wxEXPAND|wxALIGN_LEFT|wxRIGHT, 4);
    m_pPanelProviderControl->Show( true );

    size = sizerMain->GetSize();

    LinkToPanelProviderControl();

    m_pFrameMain->Show( true );

    wxBoxSizer* sizerTrade = new wxBoxSizer( wxHORIZONTAL );
    sizerMain->Add( sizerTrade, 1, wxEXPAND|wxALL, 4 );

    m_pPanelTrade = new ou::tf::l2::PanelTrade( m_pFrameMain );
    sizerTrade->Add( m_pPanelTrade, 1, wxALL | wxEXPAND, 4 );
    m_pPanelTrade->Show( true );

    //wxBoxSizer* sizerStatus = new wxBoxSizer( wxHORIZONTAL );
    //sizerMain->Add( sizerStatus, 1, wxEXPAND|wxALL, 5 );

//    m_pPanelLogging = new ou::tf::PanelLogging( m_pFrameMain, wxID_ANY );
//    sizerStatus->Add( m_pPanelLogging, 1, wxALL | wxEXPAND|wxALIGN_LEFT|wxALIGN_RIGHT|wxALIGN_TOP|wxALIGN_BOTTOM, 0);
//    m_pPanelLogging->Show( true );

    m_pDispatch = std::make_unique<DoMDispatch>( options.sSymbolName );

    std::cout << "watching " << options.sSymbolName << std::endl;

    using mi = FrameMain::structMenuItem;  // vxWidgets takes ownership of the objects

  //  FrameMain::vpItems_t vItemsLoadSymbols;
  //  vItemsLoadSymbols.push_back( new mi( "New Symbol List Remote", MakeDelegate( this, &AppIQFeedGetHistory::HandleNewSymbolListRemote ) ) );
  //  vItemsLoadSymbols.push_back( new mi( "New Symbol List Local", MakeDelegate( this, &AppIQFeedGetHistory::HandleNewSymbolListLocal ) ) );
  //  vItemsLoadSymbols.push_back( new mi( "Local Binary Symbol List", MakeDelegate( this, &AppIQFeedGetHistory::HandleLocalBinarySymbolList ) ) );
  //  wxMenu* pMenuSymbols = m_pFrameMain->AddDynamicMenu( "Load Symbols", vItemsLoadSymbols );

  }

  return code;
}

void AppDoM::OnClose( wxCloseEvent& event ) {

  if ( m_bData1Connected ) {
    m_pDispatch->Disconnect();
  }

  m_pDispatch.reset();

  //if ( m_worker.joinable() ) m_worker.join();
  //m_timerGuiRefresh.Stop();
  DelinkFromPanelProviderControl();
//  if ( 0 != OnPanelClosing ) OnPanelClosing();
  // event.Veto();  // possible call, if needed
  // event.CanVeto(); // if not a

  //SaveState();

  //if ( m_db.IsOpen() ) m_db.Close();
  event.Skip();  // auto followed by Destroy();
}

int AppDoM::OnExit() {

//  if ( m_db.IsOpen() ) m_db.Close();

  return 0;
}

void AppDoM::OnData1Connected( int ) {
  std::cout << "Depth of Market connected" << std::endl;
  m_pDispatch->Connect();
}

void AppDoM::OnData1Disconnecting( int ) {
  m_pDispatch->Disconnect();
}

void AppDoM::OnData1Disconnected( int ) {
  std::cout << "Depth of Market disconnected" << std::endl;
}
