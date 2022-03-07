#pragma once

#include <string>
#include <functional>

#include <TFIQFeed/HistoryQuery.h>

#include <TFTimeSeries/DatedDatum.h>

class DailyHistory: ou::tf::iqfeed::HistoryQuery<DailyHistory> {
  friend ou::tf::iqfeed::HistoryQuery<DailyHistory>;
public:

  using fConnected_t = std::function<void(void)>;
  using fBar_t = std::function<void(const ou::tf::Bar&)>;
  using fDone_t = std::function<void()>;

  DailyHistory( fConnected_t&&, fBar_t&&, fDone_t&& );
  ~DailyHistory() {}
  void Connect();
  void Request( const std::string& sSymbol, unsigned int nDays );
  void Disconnect();
protected:
  void OnHistoryConnected();
  void OnHistorySummaryData( structSummary* pDP );
  void OnHistoryRequestDone();
  void OnHistorySendDone();
  void OnHistoryError( size_t e );
private:
  fConnected_t m_fConnected;
  fBar_t m_fBar;
  fDone_t m_fDone;
};
