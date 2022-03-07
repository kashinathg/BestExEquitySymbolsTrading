

#include <fstream>
#include <exception>

#include <boost/log/trivial.hpp>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include "Config.h"

namespace config {

bool Load( Options& options ) {

  bool bOk( true );

  try {

    static const std::string sOption_SymbolName( "symbol" );

    po::options_description config( "dom" );
    config.add_options()
      ( sOption_SymbolName.c_str(), po::value<std::string>(&options.sSymbolName ), "symbol name")
      ;
    po::variables_map vm;
    //po::store( po::parse_command_line( argc, argv, config ), vm );

    static const std::string sFilename( "dom.cfg" );

    std::ifstream ifs( sFilename.c_str() );

    if ( !ifs ) {
      BOOST_LOG_TRIVIAL(error) << "depth of market config file " << sFilename << " does not exist";
      bOk = false;
    }
    else {
      po::store( po::parse_config_file( ifs, config), vm );
    }

    if ( 0 < vm.count( sOption_SymbolName ) ) {
      options.sSymbolName = std::move( vm[sOption_SymbolName].as<std::string>() );
      BOOST_LOG_TRIVIAL(info) << "symbol name: " << options.sSymbolName;
    }
    else {
      BOOST_LOG_TRIVIAL(error) << sFilename << " missing 'symbol='";
      bOk = false;
    }

  }
  catch( std::exception& e ) {
    BOOST_LOG_TRIVIAL(error) << "options parse error: " << e.what();
    bOk = false;
  }

  return bOk;

}

} // namespace config
