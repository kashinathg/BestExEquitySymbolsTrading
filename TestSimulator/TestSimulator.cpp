
// TestSimulator.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "CreateDataSet.h"
#include "RunSequence.h"

int _tmain(int argc, _TCHAR* argv[]) {

//  CreateDataSet cds;
//  cds.Save();

  RunSequence seq(date( 2012, 7, 22 ));
  seq.Run();

	return 0;
}
