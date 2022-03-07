

// ImportIQFeedSymbolFile.cpp : Defines the entry point for the console application.
// http://www.dtniq.com/product/mktsymbols_v2.zip

// this project is deprecated.
// the file.Load below refers to symbols.db4.  db4 is no longer supported in this collection
// neither is the IQFeedSymbolFile.h include

#include "stdafx.h"

#include <TFIQFeed\IQFeedSymbolFile.h>

int _tmain(int argc, _TCHAR* argv[]) {

  ou::tf::CIQFeedSymbolFile file;

  file.Load( "mktsymbols_v2.txt", "symbols.db4" );

 	return 0;
}
