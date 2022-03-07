/*
 * File:    DailyHistory.cpp
 * Project: BestExTrading
 * Created on Feb 24, 2022, 21:52
 */

 //#include <iostream>  // for test use, remove for production

 #include "DailyHistory.h"

DailyHistory::DailyHistory(
  fConnected_t&& fConnected, fBar_t&& fBar, fDone_t&& fDone
)
: ou::tf::iqfeed::HistoryQuery<DailyHistory>(),
  m_fConnected( std::move( fConnected ) ),
  m_fBar( std::move( fBar ) ),
  m_fDone( std::move( fDone ) )
{
  assert( m_fConnected );
  assert( m_fBar );
  assert( m_fDone );
}

void DailyHistory::Connect() {
  ou::tf::iqfeed::HistoryQuery<DailyHistory>::Connect();
}

void DailyHistory::Request( const std::string& sSymbol, unsigned int nDays ) {
  RetrieveNEndOfDays( sSymbol, nDays );
}

void DailyHistory::OnHistoryConnected() {
  //std::cout << "OnHistoryConnected" << std::endl;
  m_fConnected();
};

void DailyHistory::OnHistorySendDone() {
  //std::cout << "OnHistorySendDone" << std::endl;
  m_fDone();
}

void DailyHistory::OnHistorySummaryData( structSummary* pDP ) {
  //std::cout << "OnHistorySummaryData: " << pDP->Close << std::endl;
  ou::tf::Bar bar( pDP->DateTime, pDP->Open, pDP->High, pDP->Low, pDP->Close, pDP->PeriodVolume );
  m_fBar( bar );
  ou::tf::iqfeed::HistoryQuery<DailyHistory>::OnHistorySummaryData( pDP );
};

void DailyHistory::OnHistoryRequestDone() {
  //std::cout << "OnHistoryRequestDone" << std::endl;
  m_fDone();
};

void DailyHistory::OnHistoryError( size_t e ) {
  std::cout << "OnHistoryError: " << e << std::endl;
};

void DailyHistory::Disconnect() {
  ou::tf::iqfeed::HistoryQuery<DailyHistory>::Disconnect();
}
