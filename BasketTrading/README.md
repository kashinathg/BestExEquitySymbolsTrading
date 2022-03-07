# BestEx Research

## Introduction

This is an environment for running an algorithm over a collection of instruments.  Positions
are maintained day to day.

* BestExTrading - main file for gui and starting the process
* PanelBestExTradingMain - GUI container for buttons, BestExTrading assigns the event handlers
* PanelPortfolioStats - GUI container for presenting elementary P/L during session
* MasterPortfolio - Maintains instances of ManageStrategy
* ManageStrategy - Instance created for each primary instrument chosen - header & 'using' chooses combo style
* SymbolSelection - Determines instruments to be traded by MasterPortfolio

Currently designed to test a collar strategy (not yet complete) on weekly options.


To run, you'll need to:

* Approximately weekly, Sunday night is best:
  * _IQFeedMarketSymbols_: obtain latest market symbols, which includes OPRA option symbols
  * _Weeklies_ (weekly options list) [not currently required in this flavour]:
    * download [weeklysmf.csv](https://www.cboe.com/available_weeklys/get_csv_download/) from [Available Weeklys](https://www.cboe.com/available_weeklys/) and place into ./x64 directory
    * run Weeklies to load and test the file
* When just starting out, run _IQFeedGetHistory_ with 0 for daily bars, this will load all available daily bars for the common US exchanges
* On a daily basis, run _IQFeedGetHistory_ late at night, or first thing in the morning with 10 for daily bars, to load the latest
* _BestExTrading_ can then be started prior to market hours
  * In the run directory, create a file called BestExTrading.cfg with two entries:
    * date_history = yyyy-mm-dd   // this is the last day for a daily history bar
    * date_trading = yyyy-mm-dd   // this is th date for trading for use by DailyTradeTimeFrames
  * Use Symbols -> Load List to load symbol list, it may take a little while, and wait for it to complete
  * Click 'Turn On' for IQF and IB, both need to be running, IQF for the data stream, and IB for execution (be sure to use paper trading to start)
  * Use Manage -> Load to select and load symbols
  * When playing around, delete ./x64/debug/BestExTrading.db to reset trades and lists

@Shikhar Mittal, If you have problems, let me know.  I need to re-run to be sure I havn't broken anything.
