
#include "stdafx.h"

#include "Process.h"

#include "Worker.h"

// For IQFeedGetHistory

Worker::Worker(
  ou::tf::iqfeed::InMemoryMktSymbolList& list,
  const std::string& sPrefixPath, size_t nDatums ):
  m_list( list ),
  m_nDatums( nDatums ),
  m_sPrefixPath( sPrefixPath ),
  m_thread( boost::ref( *this ) )
{
}

Worker::~Worker(void) {
}

void Worker::operator()( void ) {

  Process process( m_list, m_sPrefixPath, m_nDatums );
  process.Start();
}
