#pragma once

// For IQFeedGetHistory

#include <string>

#include <boost/thread/thread.hpp>
//#include <boost/noncopyable.hpp>

#include <TFIQFeed/InMemoryMktSymbolList.h>

class Worker {
public:
  Worker(
    ou::tf::iqfeed::InMemoryMktSymbolList&,
    const std::string& sPrefixPath, size_t nDatums );
  ~Worker(void);
  void operator()( void );
  void Join( void ) { m_thread.join(); };
protected:
private:
  ou::tf::iqfeed::InMemoryMktSymbolList& m_list;
  std::string m_sPrefixPath;
  const size_t m_nDatums;
  boost::thread m_thread;
};
