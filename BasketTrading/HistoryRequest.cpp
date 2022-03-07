#include "DailyHistory.h"
#include "HistoryRequest.h"

HistoryRequest::HistoryRequest(
  fConnected_t&& fConnected
)
: m_bInProcess( false ),
  m_fConnected( std::move( fConnected ) )
{
  m_pHistory = std::make_unique<DailyHistory>(
    [this](){ // fConnected_t
      m_fConnected();
    },
    [this]( const ou::tf::Bar& bar ){ // fBar_t
      m_entryCurrent.fBar( bar );
    },
    [this](){ // fDone_t
      m_entryCurrent.fDone();

      std::lock_guard<std::mutex> lock( m_mutexHistorySlots );
      if ( 0 != m_vEntry.size() ) {
        NextRequest( std::move( m_vEntry.back() ) );
        m_vEntry.pop_back();
      }
      else {
        m_bInProcess = false;
        m_entryCurrent.Clear();
      }
    }
  );
  m_pHistory->Connect(); // start the process
}

HistoryRequest::~HistoryRequest() {
  m_pHistory->Disconnect();
}

void HistoryRequest::Request( const std::string& sSymbol_, uint16_t nBar, fBar_t&& fBar, fDone_t&& fDone ) {
  const std::string sSymbol( sSymbol_ );
  Entry entry( std::move( sSymbol ), nBar, std::move( fBar ), std::move( fDone ) );
  std::lock_guard<std::mutex> lock( m_mutexHistorySlots );
  if ( m_bInProcess ) {
    m_vEntry.emplace_back( std::move( entry ) );
  }
  else {
    m_bInProcess = true;
    NextRequest( std::move( entry ) );
  }
}

void HistoryRequest::NextRequest( Entry&& entry ) {
  m_entryCurrent = std::move( entry );
  m_pHistory->Request( m_entryCurrent.sSymbol, m_entryCurrent.nBar );
}
