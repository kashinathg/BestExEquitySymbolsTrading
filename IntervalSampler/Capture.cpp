#include "Capture.h"

Capture::Capture()
: m_nSequence {}
, m_bQuoteReady( false )
, m_bTradeReady( false )
, m_bBarReady( false )
{
}

Capture::~Capture() {
  m_bf.SetOnBarComplete( nullptr );
  if ( m_pWatch ) {
    m_pWatch->StopWatch();
    m_pWatch->OnTrade.Remove( MakeDelegate( this, &Capture::HandleTrade ) );
    m_pWatch->OnQuote.Remove( MakeDelegate( this, &Capture::HandleQuote ) );
    m_pWatch.reset(); // TODO: need to wait for queue to flush
  }
}

void Capture::Assign(
  ou::tf::BarFactory::duration_t duration,
  pWatch_t pWatch,
  fBarComplete_t&& fBarComplete
) {
  assert( nullptr != fBarComplete );
  m_fBarComplete = std::move( fBarComplete );
  m_bf.SetBarWidth( duration );
  m_pWatch = pWatch;
  m_bf.SetOnBarComplete( MakeDelegate( this, &Capture::HandleBarComplete ) );
  m_pWatch->RecordSeries( false );
  m_pWatch->OnQuote.Add( MakeDelegate( this, &Capture::HandleQuote ) );
  m_pWatch->OnTrade.Add( MakeDelegate( this, &Capture::HandleTrade ) );
  m_pWatch->StartWatch();
}

void Capture::Pull(
  bool& bBarReady, ou::tf::Bar& bar,
  bool& bQuoteReady, ou::tf::Quote& quote,
  bool& bTradeReady, ou::tf::Trade& trade
) {
  {
    m_spinlock.lock();
    bQuoteReady = m_bQuoteReady;
    quote = m_quote;
    if ( bQuoteReady ) {
      m_bQuoteReady = false;
    }
    m_spinlock.unlock();
  }
  {
    m_spinlock.lock();
    bTradeReady = m_bTradeReady;
    trade = m_trade;
    if ( bTradeReady ) {
      m_bTradeReady = false;
    }
//    m_spinlock.unlock();
//  }
//  {
//    m_spinlock.lock();
    bBarReady = m_bBarReady;
    bar = ou::tf::Bar( m_dtBarStart, m_dblOpen, m_dblHigh, m_dblLow, m_dblClose, m_nVolume );
    if ( bBarReady ) {
      m_bBarReady = false;
    }
    m_spinlock.unlock();
  }
}

void Capture::HandleQuote( const ou::tf::Quote& quote ) {
  m_spinlock.lock();
  m_quote = quote;
  m_bQuoteReady = true;
  m_spinlock.unlock();
}

void Capture::HandleTrade( const ou::tf::Trade& trade ) {
  m_spinlock.lock();
  m_trade = trade;
  m_bTradeReady = true;
//  if ( nullptr != m_fBarComplete ) {
//    m_bf.Add( trade );
//  }
  BarAddTrade( trade );
  m_spinlock.unlock();
}

void Capture::HandleBarComplete( const ou::tf::Bar& bar ) {
  m_nSequence++;
  if ( nullptr != m_fBarComplete ) {
    m_fBarComplete(
      m_pWatch->GetInstrument()->GetInstrumentName(),
      m_nSequence,
      bar,
      m_pWatch->LastQuote(),
      m_pWatch->LastTrade()
      );
  }
}

void Capture::BarAddTrade( const ou::tf::Trade& trade ) {
//  m_spinlock.lock();
  if ( !m_bBarReady ) {
    m_dtBarStart = trade.DateTime();
    m_dblHigh = m_dblLow = m_dblOpen = m_dblClose = trade.Price();
    m_nVolume = trade.Volume();
    m_bBarReady = true;
  }
  else {
    double price = trade.Price();
    if ( price > m_dblHigh ) m_dblHigh = price;
    if ( price < m_dblLow ) m_dblLow = price;
    m_dblClose = price;
    m_nVolume += trade.Volume();
  }
//  m_spinlock.unlock();
}
