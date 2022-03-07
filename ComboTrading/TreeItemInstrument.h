#pragma once

#include <string>

#include <wx/string.h>

#include <TFTrading/TradingEnumerations.h>

#include "InstrumentActions.h"
#include "TreeItem.h"

class TreeItemInstrument: public TreeItemResources {
  friend class boost::serialization::access;
public:

  TreeItemInstrument( wxTreeItemId id, ou::tf::TreeItemResources& baseResources, Resources& resources );
  virtual ~TreeItemInstrument( void );

  virtual void ShowContextMenu( void );
  virtual void HandleOnClick( void );

  const std::string& GetUnderlying( void ) const { return m_sUnderlying; }  // might be able to optimize away

  void HandleMenuNewInstrument( wxCommandEvent& event );

  // todo: invocable only if no instrument already exists
  bool NewInstrumentViaDialog( ou::tf::Allowed::enumInstrument selector, const wxString& wxsUnderlying = "" );

protected:

  enum IdTreeItemType {
    IdGroup = 301, IdInstrument
  };

  enum {
    ID_Null = wxID_HIGHEST,
    MINewInstrument, MINewOption, MINewFuturesOption,
    MIOptionList,
    MILiveChart, MIDailyChart, MISaveData, MIEmit,
    MIDelete
  };

  void BuildContextMenu( wxMenu* pMenu );

  void HandleMenuAddFuturesOption( wxCommandEvent& event );
  void HandleMenuAddOption( wxCommandEvent& event );
  void HandleLiveChart( wxCommandEvent& event );
  void HandleDailyChart( wxCommandEvent& event );
  void HandleMenuOptionList( wxCommandEvent& event );
  //void HandleSaveData( wxCommandEvent& event );
  void HandleDelete( wxCommandEvent& event );
  void HandleEmit( wxCommandEvent& event );

private:

  typedef InstrumentActions::pInstrumentActions_t pInstrumentActions_t;
  pInstrumentActions_t m_pInstrumentActions;

  ou::tf::Allowed::enumInstrument m_InstrumentSelector;

  std::string m_sUnderlying;

  void InstrumentViaDialog( ou::tf::Allowed::enumInstrument selector, const std::string& sPrompt );

  // need to keep track of (load, save)
  //   * menu item text (as it can be changed)
  //   * instrument code(s)

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
    ar & boost::serialization::base_object<const TreeItemResources>(*this);

    ar & m_InstrumentSelector;
    ar & m_sUnderlying;

    const mapMembers_t::size_type n = m_mapMembers.size();
    ar << n;
    for ( mapMembers_t::const_iterator iter = m_mapMembers.begin(); iter != m_mapMembers.end(); ++iter ) {
      ar << ( iter->second.m_type );
      switch ( iter->second.m_type ) {
        case IdInstrument:
        {
          const TreeItemInstrument* p = dynamic_cast<TreeItemInstrument*>( iter->second.m_pTreeItemBase.get() );
          ar & *p;
        }
        break;
      }
    }
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {

    ar & boost::serialization::base_object<TreeItemResources>(*this);
    ar & m_InstrumentSelector;
    ar & m_sUnderlying;

    // this is going to cause problems if renamed, so prevent a rename, ... is rename even available?
    m_pInstrumentActions->signalLoadInstrument( m_id, m_baseResources.signalGetItemText( m_id ), m_sUnderlying );
    // call InstrumentActions::Startup here?

    mapMembers_t::size_type n;
    ar & n;
    std::cout
      << m_baseResources.signalGetItemText( m_id ) << ","
      << m_sUnderlying << "," << n << ","
      << m_InstrumentSelector
      << std::endl;
    for ( mapMembers_t::size_type ix = 0; ix < n; ++ix ) {
      unsigned int type;
      ar & type;
      switch ( type ) {
        case IdInstrument:
        {
          TreeItemInstrument* p = AddTreeItem<TreeItemInstrument,IdTreeItemType>( "Instrument", IdInstrument, m_resources );
          ar & *p;
        }
        break;
      }
    }

  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()

};
