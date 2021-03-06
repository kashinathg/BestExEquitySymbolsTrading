#include "TreeItemGroup.h"

#include "TreeItemPortfolio.h"
#include "TreeItemPosition.h"

void TreeItemGroupCommon::BuildContextMenu( wxMenu* pMenu ) {
  assert( 0 != pMenu );
  pMenu->Append( MIAddInstrument, "Add &Instrument" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemGroupCommon::HandleAddInstrument, this, MIAddInstrument );
  pMenu->Append( MIAddPortfolio, "Add P&ortfolio" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemGroupCommon::HandleAddPortfolio, this, MIAddPortfolio );
  pMenu->Append( MIAddPosition, "Add Pos&ition" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemGroupCommon::HandleAddPosition, this, MIAddPosition );
  pMenu->Append( MIAddGroup, "Add &Group" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemGroupCommon::HandleAddGroup, this, MIAddGroup );
  pMenu->Append( MIRename, "&Rename" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemBase::HandleRename, this, MIRename );
}

void TreeItemGroupCommon::HandleAddInstrument( wxCommandEvent& event ) {
  TreeItemInstrument* p = AddTreeItem<TreeItemInstrument>( "Instrument", IdInstrument, m_resources );
  if ( p->NewInstrumentViaDialog( ou::tf::Allowed::Basic ) ) {
    // continue processing
  }
  else {
    // remove the place holder
    wxTreeItemId id( p->GetTreeItemId() );
    m_baseResources.signalDelete( id );
    ou::tf::TreeItemBase::DeleteMember( id );
  }
//  p->InstrumentViaDialog( InstrumentActions::NoLock, )
//  p->HandleMenuNewInstrument( event );
//  if ( 0 == p->GetInstrumentWatch().get() ) {
//    this->m_baseResources.signalDelete( p->GetTreeItemId() );
    //DeleteMember( p->GetTreeItemId() );
//  }
//  else {
//  }
}

void TreeItemGroupCommon::HandleAddPortfolio( wxCommandEvent& event ) {
  TreeItemPortfolio* p = AddTreeItem<TreeItemPortfolio>( "Portfolio", IdPortfolio, m_resources );
  p->Rename();
}

void TreeItemGroupCommon::HandleAddPosition( wxCommandEvent& event ) {
  TreeItemPosition* p = AddTreeItem<TreeItemPosition>( "Position", IdPosition, m_resources );
  p->Rename();
}

void TreeItemGroupCommon::HandleAddGroup( wxCommandEvent& event ) {
  TreeItemGroup* p = AddTreeItem<TreeItemGroup>( "Group", IdGroup, m_resources );
  p->Rename();
}

// ================

void TreeItemRoot::ShowContextMenu( void ) {
  if ( 0 == m_pMenu ) {
    m_pMenu = new wxMenu();  // menu does not get deleted, so may need to reclaim afterwards.  put into a list somewhere?
    TreeItemGroupCommon::BuildContextMenu( m_pMenu );
  }
  m_baseResources.signalPopupMenu( m_pMenu );
}

// ================

void TreeItemGroup::ShowContextMenu( void ) {
  if ( 0 == m_pMenu ) {
    m_pMenu = new wxMenu();  // menu does not get deleted, so may need to reclaim afterwards.  put into a list somewhere?
    TreeItemGroupCommon::BuildContextMenu( m_pMenu );
    m_pMenu->Append( MIDelete, "Delete" );
    m_pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemGroup::HandleDelete, this, MIDelete );
  }
  m_baseResources.signalPopupMenu( m_pMenu );
}

void TreeItemGroup::HandleDelete( wxCommandEvent& event ) {
  std::cout << "Delete: TreeItemGroup" << std::endl;
  m_baseResources.signalDelete( this->m_id );
}
