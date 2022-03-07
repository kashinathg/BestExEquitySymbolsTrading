// TradeYMView.cpp : implementation of the CTradeYMView class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "TradeYMView.h"

CTradeYMView::CTradeYMView( void )
: CDialogImpl<CTradeYMView>()
{
}

CTradeYMView::~CTradeYMView( void ) {
}

BOOL CTradeYMView::PreTranslateMessage(MSG* pMsg) {
	return CWindow::IsDialogMessage(pMsg);
}

LRESULT CTradeYMView::OnBnClickedBtnIbconnect(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  m_process.IBConnect();

  return 0;
}

LRESULT CTradeYMView::OnBnClickedBtnIbdisconnect(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  m_process.IBDisconnect();

  return 0;
}

LRESULT CTradeYMView::OnBnClickedBtnBuy(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  m_process.PlaceBuyOrder();

  return 0;
}

LRESULT CTradeYMView::OnBnClickedBtnSell(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  m_process.PlaceSellOrder();

  return 0;
}

LRESULT CTradeYMView::OnBnClickedBtnWatchStart(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  m_process.StartWatch();

  return 0;
}

LRESULT CTradeYMView::OnBnClickedBtnWatchStop(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  m_process.StopWatch();

  return 0;
}

LRESULT CTradeYMView::OnBnClickedBtnIqfeedConnnect(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  // TODO: Add your control notification handler code here

  return 0;
}

LRESULT CTradeYMView::OnBnClickedBtnIqfeedDisconnect(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  // TODO: Add your control notification handler code here

  return 0;
}

LRESULT CTradeYMView::OnBnClickedBtnIqfeedStartwatch(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  // TODO: Add your control notification handler code here

  return 0;
}

LRESULT CTradeYMView::OnBnClickedBtnIqfeedStopwatch(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  // TODO: Add your control notification handler code here

  return 0;
}
