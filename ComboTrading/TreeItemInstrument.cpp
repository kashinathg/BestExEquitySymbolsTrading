
//  manual control of watch/unwatch in menu at some point
//  set colour on menu item for watch/unwatch mode
//  then start the watch chart
//  then provide shift/zoom capability on the chart
//   should override rename so it always has proper instrument name
//   is rename actually in the menu, if not, then nothing to do

#include <wx/menu.h>

#include "TreeItemInstrument.h"

TreeItemInstrument::TreeItemInstrument( wxTreeItemId id, ou::tf::TreeItemResources& baseResources, Resources& resources ):
  TreeItemResources( id, baseResources, resources ), m_InstrumentSelector( ou::tf::Allowed::Basic ) {
  //std::cout << "TreeItemInstrument::TreeItemInstrument" << std::endl;
  m_pInstrumentActions = m_resources.signalGetInstrumentActions( m_id );
  assert( 0 != m_pInstrumentActions.use_count() );
}

TreeItemInstrument::~TreeItemInstrument( void ) {
  //std::cout << "TreeItemInstrument::~TreeItemInstrument" << std::endl;
}

void TreeItemInstrument::HandleDelete( wxCommandEvent& event ) {
  //std::cout << "TreeItemInstrument::HandleDelete" << std::endl;
  m_pInstrumentActions->signalDelete( m_id );
  m_baseResources.signalDelete( m_id );
}

void TreeItemInstrument::HandleLiveChart( wxCommandEvent& event ) {
  HandleOnClick();
}

void TreeItemInstrument::HandleOnClick( void ) {
  m_pInstrumentActions->signalLiveChart( m_id );
}

void TreeItemInstrument::HandleMenuOptionList( wxCommandEvent& event ) {
  m_pInstrumentActions->signalOptionList( m_id );
}

void TreeItemInstrument::HandleDailyChart( wxCommandEvent& event ) {
  m_pInstrumentActions->signalDailyChart( m_id );
}

void TreeItemInstrument::HandleEmit( wxCommandEvent& event ) {
  m_pInstrumentActions->signalEmitValues( m_id );
}

//void TreeItemInstrument::HandleSaveData( wxCommandEvent& event ) {
//  m_pInstrumentActions->signalSaveData( m_id );
//}

void TreeItemInstrument::BuildContextMenu( wxMenu* pMenu ) {
  assert( 0 != pMenu );
  switch ( m_InstrumentSelector ) {
    case ou::tf::Allowed::All:
      pMenu->Append( MINewInstrument, "New Instrument" );
      pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemInstrument::HandleMenuNewInstrument, this, MINewInstrument );
      break;
    case ou::tf::Allowed::Options:
      pMenu->Append( MINewOption, "New Option" );
      pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemInstrument::HandleMenuAddOption, this, MINewOption );

      pMenu->Append( MIOptionList, "Option Chains" );
      pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemInstrument::HandleMenuOptionList, this, MIOptionList );
      break;
    case ou::tf::Allowed::FuturesOptions:
      pMenu->Append( MINewFuturesOption, "New Futures Option" );
      pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemInstrument::HandleMenuAddFuturesOption, this, MINewFuturesOption );

      pMenu->Append( MIOptionList, "Option Chains" );
      pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemInstrument::HandleMenuOptionList, this, MIOptionList );
      break;
    case ou::tf::Allowed::None:
      // no menu for options, future options, etc
      break;
    default:
      std::cout << "TreeItemInstrument::BuildContextMenu has unknown lockType: " << m_InstrumentSelector << std::endl;
      break;
  }

  // add watch/unwatch menu item?

  pMenu->Append( MILiveChart, "Live Chart" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemInstrument::HandleLiveChart, this, MILiveChart );

  pMenu->Append( MIDailyChart, "Daily Chart" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemInstrument::HandleDailyChart, this, MIDailyChart );

  pMenu->Append( MIEmit, "Emit" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemInstrument::HandleEmit, this, MIEmit );

//  pMenu->Append( MISaveData, "Save Data" );
//  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemInstrument::HandleSaveData, this, MISaveData );

  pMenu->Append( MIDelete, "Delete" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemInstrument::HandleDelete, this, MIDelete );
}

void TreeItemInstrument::ShowContextMenu( void ) {
  if ( 0 == m_pMenu ) {
    m_pMenu = new wxMenu();  // menu does not get deleted, so may need to reclaim afterwards.  put into a list somewhere?
    TreeItemInstrument::BuildContextMenu( m_pMenu );
  }
  m_baseResources.signalPopupMenu( m_pMenu );
}

/* todo:  mostly complete
 *   for following two handlers:
 *   the lock should be forcing the gui to show options only for the underlying instrument
 *     when adding sub-menus to the tree
 *   the dialog needs a lock for FuturesOption and Option
 *   then need NewOptionViaDialog, NewFuturesOptionViaDialog to force that setting in the dialog
 */

// from tree menu popup
void TreeItemInstrument::HandleMenuAddOption( wxCommandEvent& event ) {
  InstrumentViaDialog( ou::tf::Allowed::Options, "Option" );
}

// from tree menu popup
void TreeItemInstrument::HandleMenuAddFuturesOption( wxCommandEvent& event ) {
  InstrumentViaDialog( ou::tf::Allowed::FuturesOptions, "FuturesOption" );
}

void TreeItemInstrument::HandleMenuNewInstrument( wxCommandEvent& event ) {
  InstrumentViaDialog( ou::tf::Allowed::All, "Instrument Name" );
}

void TreeItemInstrument::InstrumentViaDialog( ou::tf::Allowed::enumInstrument selector, const std::string& sPrompt ) {
  TreeItemInstrument* p = AddTreeItem<TreeItemInstrument>( sPrompt, IdInstrument, m_resources );
  wxString wsx( m_baseResources.signalGetItemText( m_id ) );
  if ( p->NewInstrumentViaDialog( selector, wsx ) ) {  // wsx is used as sUnderlying to new instrument, when needed
    // continue with processing
  }
  else {
    // delete the menuitem if nothing chosen
    wxTreeItemId id( p->GetTreeItemId() );
    m_baseResources.signalDelete( id );
    ou::tf::TreeItemBase::DeleteMember( id );
  }
}

// called by:
//   InstrumentViaDialog (above)
//   TreeItemGroup::HandleAddInstrument
bool TreeItemInstrument::NewInstrumentViaDialog(
  ou::tf::Allowed::enumInstrument selector, const wxString& wxsUnderlying
  ) {
  // IQF underlying name, and IB underlying name will need to be provided for naming options during lock
  // need to assume/assert that this is a new dialog?  or communicate it is a replacement?
  bool bInstrumentNameAssigned( false );
  InstrumentActions::values_t values(
    m_pInstrumentActions->signalNewInstrument( this->m_id, selector, wxsUnderlying ) );
  if ( "" == values.name_ ) {
    // caller takes care of the deletion
  }
  else {
    m_baseResources.signalSetItemText( m_id, values.name_ );
    if ( !wxsUnderlying.empty() )
      m_sUnderlying = wxsUnderlying;
    m_InstrumentSelector = values.selector;
    bInstrumentNameAssigned = true;
  }
  return bInstrumentNameAssigned;
}
