#pragma once

#include "afxcmn.h"

#include "GeneratePeriodicRefresh.h"

#include "PositionOptionDeltasModel.h"

class CPositionOptionDeltasVu :  public CListCtrl  {
  DECLARE_DYNCREATE(CPositionOptionDeltasVu)
public:
  CPositionOptionDeltasVu(void);
  virtual ~CPositionOptionDeltasVu(void);
  void SetModel( CPositionOptionDeltasModel *pModel );

  void HandlePeriodicRefresh( CGeneratePeriodicRefresh* );  // called via PositionOptionDeltasWnd
protected:
	DECLARE_MESSAGE_MAP()

  afx_msg int OnCreate( LPCREATESTRUCT lpCreateStruct );
  afx_msg void OnDestroy( );
  afx_msg void OnClose( );  // The default implementation calls DestroyWindow.
  afx_msg void OnCustomDraw( NMHDR* pNMHDR, LRESULT* pResult );
  afx_msg void OnGetDispInfo( NMHDR* pNMHDR, LRESULT* pResult );

  void HandleInstrumentCountChanged(  CPositionOptionDeltasModel::vDeltaRows_t::size_type cnt  );
  void HandleRowUpdate( CPositionOptionDeltasModel::vDeltaRows_t::size_type ixRow );  // ix starts at 0

private:
  CPositionOptionDeltasModel *m_pModel;
  bool m_bActive;
};
