#include <mutex>
#include <string>
#include <functional>

#include <TFTimeSeries/TimeSeries.h>

class DailyHistory;

class HistoryRequest {
public:

  using fConnected_t = std::function<void()>;

  using fBar_t = std::function<void(const ou::tf::Bar&)>;
  using fDone_t = std::function<void()>;

  HistoryRequest( fConnected_t&& fConnected );
  ~HistoryRequest();

  void Request( const std::string& sSymbol_, uint16_t nBar, fBar_t&& fBar, fDone_t&& fDone );

protected:
private:

  fConnected_t m_fConnected;

  std::mutex m_mutexHistorySlots;
  std::unique_ptr<DailyHistory> m_pHistory;
  bool m_bInProcess;

  struct Entry {
    std::string sSymbol;
    uint16_t nBar;
    fBar_t fBar;
    fDone_t fDone;
    Entry() {}
    Entry( const Entry&& rhs )
    : fBar( std::move( rhs.fBar ) ), fDone( std::move( rhs.fDone ) ),
      sSymbol( std::move( rhs.sSymbol ) ), nBar( rhs.nBar )
    {}
    Entry& operator=( const Entry&& rhs ) {
      sSymbol = std::move( rhs.sSymbol );
      nBar = rhs.nBar;
      fBar = std::move( rhs.fBar );
      fDone = std::move( rhs.fDone );
      return *this;
    }
    Entry( const std::string&& sSymbol_, uint16_t nBar_, fBar_t&& fBar_, fDone_t&& fDone_ )
    : sSymbol( std::move( sSymbol_ ) ), nBar( nBar_ ),
      fBar( std::move( fBar_ ) ), fDone( std::move( fDone_ ) ) {}
    void Clear() {
      sSymbol.clear();
      fBar = nullptr;
      fDone = nullptr;
    }
  };

  using vEntry_t = std::vector<Entry>;
  vEntry_t m_vEntry;
  Entry m_entryCurrent;

  void NextRequest( Entry&& entry );

};
