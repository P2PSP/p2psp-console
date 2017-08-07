//
//  splitter.cc -- Console version of a P2PSP peer
//
//  This code is distributed under the GNU General Public License (see
//  THE_GENERAL_GNU_PUBLIC_LICENSE.txt for extending this information).
//
//  Copyright (C) 2016, the P2PSP team.
//
//  http://www.p2psp.org
//

// {{{ includes

#include <iostream>
#include <memory>

#include "common.h"

#if defined __IMS__
#include "core/splitter_ims.cc"
#else /* __DBS__ */
#include "core/splitter_dbs.cc"
#if defined __ACS__
#include "core/splitter_acs.cc"
#endif
#if defined __LRS__
#include "core/splitter_lrs.cc"
#endif
#if defined __NTS__
#include "core/splitter_nts.cc"
#endif /* __NTS__ */
#if defined __EMS__
#include "core/splitter_ems.cc"
#endif /* __EMS__ */
#endif /* !__IMS__ */
//#include "core/splitter_acs.h"
//#include "core/splitter_lrs.h"
//#include "core/splitter_nts.h"

#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
#include <signal.h>

// Time
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/local_time_adjustor.hpp>
#include <boost/date_time/c_local_time_adjustor.hpp>
// }}}

#if defined __IMS__
p2psp::Splitter_IMS splitter;
#elif defined __DBS__
p2psp::Splitter_DBS splitter;
#elif defined __ACS__
p2psp::Splitter_ACS splitter;
#elif defined __LRS__
p2psp::Splitter_LRS splitter;
#elif defined __NTS__
p2psp::Splitter_NTS splitter;
#elif defined __EMS__
p2psp::Splitter_EMS splitter;
#endif

void HandlerCtrlC(int s) {
  // {{{

  INFO("Keyboard interrupt detected ... Exiting!");

  // Say to daemon threads that the work has been finished,
  splitter.SetAlive(false);

  // }}}
}

void HandlerEndOfExecution() {
  // {{{

  // Wake up the "moderate_the_team" daemon, which is waiting in a recvfrom().
  //splitter_ptr->SayGoodbye();

  // Wake up the "handle_arrivals" daemon, which is waiting in an accept().
  boost::asio::io_service io_service_;
  boost::system::error_code ec;
  boost::asio::ip::tcp::socket socket(io_service_);
  boost::asio::ip::tcp::endpoint endpoint(
      boost::asio::ip::address::from_string("127.0.0.1"),
      splitter.GetSplitterPort());

  socket.connect(endpoint, ec);

  // TODO: If "broken pipe" errors have to be avoided, replace the close method
  // with multiple receive calls in order to read the configuration sent by the
  // splitter
  socket.close();

  // }}}
}

int main(int argc, const char *argv[]) {

  // {{{ Argument parsing

  const char description[80] = "This is the splitter node of a P2PSP team.\n"
#if defined __IMS__
      "Using IMS.\n"
#elif defined __DBS__
      "Using DBS.\n"
#endif
         "Parameters";

  boost::program_options::options_description desc(description);

  //~ {

    int buffer_size = splitter.GetDefaultBufferSize();
    std::string channel = splitter.GetDefaultChannel();
    int chunk_size = splitter.GetDefaultChunkSize();
    int splitter_port = splitter.GetDefaultSplitterPort();
    std::string source_addr = splitter.GetDefaultSourceAddr();
    int source_port = splitter.GetDefaultSourcePort();
    int header_size = splitter.GetDefaultHeaderSize();
#if defined __IMS__
    std::string mcast_addr = splitter.GetDefaultMcastAddr();
    unsigned short mcast_port = splitter.GetDefaultMcastPort();
    int TTL = splitter.GetDefaultTTL();
#else
    int max_number_of_chunk_loss = splitter.GetDefaultMaxNumberOfChunkLoss();
    int number_of_monitors = splitter.GetDefaultNumberOfMonitors();
#endif

    // TODO: strpe option should expect a list of arguments, not bool
    desc.add_options()
      ("help,h", "Produces this help message and exits.")
      ("buffer_size", boost::program_options::value<int>()->default_value(buffer_size), "Length of the buffer in chunks.")
      ("channel", boost::program_options::value<std::string>()->default_value(channel), "Name of the channel served by the streaming source.")
      ("chunk_size", boost::program_options::value<int>()->default_value(chunk_size), "Chunk size in bytes.")
      ("header_size", boost::program_options::value<int>()->default_value(header_size), "Length of the header of the stream in bytes.")
#if not defined __IMS__
      ("max_number_of_chunk_loss", boost::program_options::value<int>()->default_value(max_number_of_chunk_loss), "Maximum number of lost chunks for an unsupportive peer.")
      ("number_of_monitors", boost::program_options::value<int>()->default_value(number_of_monitors), "Number of monitors in the team. The first connecting peers will automatically become monitors.")
#endif
#if defined __IMS__
      ("mcast_addr",boost::program_options::value<std::string>()->default_value(mcast_addr), "IP multicast address used to broadcast the chunks.")
      ("mcast_port",boost::program_options::value<int>()->default_value(mcast_port), "IP multicast used used to broadcast the chunks.")
#endif
      ("source_addr", boost::program_options::value<std::string>()->default_value(source_addr), "IP address or hostname of the streaming server.")
      ("source_port", boost::program_options::value<int>()->default_value(source_port), "Port where the streaming server is listening.")
#if defined __IMS__
      ("TTL", boost::program_options::value<int>()->default_value(TTL), "Time To Live of the multicast messages.")
#endif
      ("splitter_port", boost::program_options::value<int>()->default_value(splitter_port), "Port to serve the peers.")
      ("smart_source_client", boost::program_options::value<bool>()->default_value(false), "Connect to smart Source Client");


  boost::program_options::variables_map vm;
  try {
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
  } catch (std::exception &e) {

    // If the argument passed is unknown, print the list of available arguments
    std::cout << desc << "\n";
    return 1;
  }

  boost::program_options::notify(vm);

  if (vm.count("help")) {
    std::cout << desc << "\n";
    return 1;
  }


  // }}}

#if defined __IMS__
  std::cout << "Using Splitter_IMS" << std::endl;
#endif /* __IMS__ */

#if defined __DBS__
  std::cout << "Using Splitter_DBS" << std::endl;
#endif /* __DBS__  */

#if defined __LRS__
  std::cout << "Using Splitter_LRS" << std::endl;
#endif /* __LRS__ */

#if defined __NTS__
  std::cout << "Using Splitter_NTS" << std::endl;
#endif /* __NTS__ */

#if defined __EMS__
  std::cout << "Using Splitter_EMS" << std::endl;
#endif /* __EMS__ */

  if (vm.count("smart_source_client")) {
    // {{{

    splitter.setSmartSourceClient(vm["smart_source_client"].as<bool>());
    TRACE("smart_source_client = " << splitter.isSmartSourceClient());

    // }}}
  }

  if (vm.count("buffer_size")) {
    // {{{

    splitter.SetBufferSize(vm["buffer_size"].as<int>());
    TRACE("Buffer size = "
	  << splitter.GetBufferSize());

    // }}}
  }

  if (vm.count("channel")) {
    // {{{

    splitter.SetChannel(vm["channel"].as<std::string>());
    TRACE("Channel = "
	  << splitter.GetChannel());

    // }}}
  }

  if (vm.count("chunk_size")) {
    // {{{

    splitter.SetChunkSize(vm["chunk_size"].as<int>());
    TRACE("Chunk size = "
	  << splitter.GetChunkSize());

    // }}}
  }

  if (vm.count("header_size")) {
    // {{{

    splitter.SetHeaderSize(vm["header_size"].as<int>());
    TRACE("Header size = "
	  << splitter.GetHeaderSize());

    // }}}
  }

#if defined __IMS__

  // {{{

  if (vm.count("mcast_addr")) {
    // {{{

    splitter.SetMcastAddr(vm["mcast_addr"].as<std::string>());
    TRACE("IP multicast address = "
	  << splitter.GetMcastAddr());

    // }}}
  }

  if (vm.count("mcast_port")) {
    // {{{

    splitter.SetMcastPort(vm["mcast_port"].as<int>());
    TRACE("IP multicast port = "
	  << splitter.GetMcastPort());

    // }}}
  }

  // }}}

#endif

  if (vm.count("splitter_port")) {
    // {{{

    splitter.SetSplitterPort(vm["splitter_port"].as<int>());
    TRACE("Splitter port = "
	  << splitter.GetSplitterPort());

    // }}}
  }

  if (vm.count("source_addr")) {
    // {{{

    splitter.SetSourceAddr(vm["source_addr"].as<std::string>());
    TRACE("Source address = "
	  << splitter.GetSourceAddr());

    // }}}
  }

  if (vm.count("source_port")) {
    // {{{

    splitter.SetSourcePort(vm["source_port"].as<int>());
    TRACE("Source port = "
	  << splitter.GetSourcePort());

    // }}}
  }

#if not defined __IMS__

  // {{{

  if (vm.count("max_number_of_chunk_loss")) {
    // {{{

    splitter.SetMaxNumberOfChunkLoss(vm["max_number_of_chunk_loss"].as<int>());
    TRACE("Maximun number of lost chunks = "
	  << splitter.GetMaxNumberOfChunkLoss());
  }

  // }}}

  if (vm.count("number_of_monitors")) {
    // {{{

    splitter.SetNumberOfMonitors(vm["number_of_monitors"].as<int>());
    TRACE("Number of monitors = "
	  << splitter.GetNumberOfMonitors());

    // }}}
  }

  // }}}

#endif

  splitter.Init();
  splitter.Start();

  std::cout << _RESET_COLOR();

#if defined __IMS__
  std::cout << "                     | Received |     Sent |" << std::endl;
  std::cout << "                Time |   (kbps) |   (kbps) |" << std::endl;
  std::cout << "---------------------+----------+----------+" << std::endl;
#else
  std::cout << "                     | Received |     Sent | Team | Team description" << std::endl;
  std::cout << "                Time |   (kbps) |   (kbps) | size | (peer chunks-lost/sent)" << std::endl;
  std::cout << "---------------------+----------+----------+------+------------------..." << std::endl;
#endif

  int last_sendto_counter = splitter.GetSendToCounter();
  int last_recvfrom_counter = splitter.GetRecvFromCounter();

  int chunks_sendto = 0;
  int kbps_sendto = 0;
  int kbps_recvfrom = 0;
  int chunks_recvfrom = 0;
#if not defined __IMS__
  std::vector<boost::asio::ip::udp::endpoint> peer_list;
#endif

  // Listen to Ctrl-C interruption
  struct sigaction sigIntHandler;
  sigIntHandler.sa_handler = HandlerCtrlC;
  sigemptyset(&sigIntHandler.sa_mask);
  sigIntHandler.sa_flags = 0;
  sigaction(SIGINT, &sigIntHandler, NULL);

  while (splitter.isAlive()) {

    boost::this_thread::sleep(boost::posix_time::milliseconds(1000));

    { /* Print current time */
      using boost::posix_time::ptime;
      using boost::posix_time::second_clock;
      using boost::posix_time::to_simple_string;
      using boost::gregorian::day_clock;
      ptime todayUtc(day_clock::universal_day(), second_clock::universal_time().time_of_day());
      std::cout << to_simple_string(todayUtc);
    }

    chunks_sendto = splitter.GetSendToCounter() - last_sendto_counter;
    kbps_sendto = (chunks_sendto * splitter.GetChunkSize() * 8) / 1000;
    chunks_recvfrom = splitter.GetRecvFromCounter() - last_recvfrom_counter;
    kbps_recvfrom = (chunks_recvfrom * splitter.GetChunkSize() * 8) / 1000;
    last_sendto_counter = splitter.GetSendToCounter();
    last_recvfrom_counter = splitter.GetRecvFromCounter();
    std::cout << " |";
    std::cout << std::setw(9) << kbps_recvfrom;
    std::cout << " |";
    std::cout << std::setw(9) << kbps_sendto;
    std::cout << " |";

    // O(_SET_COLOR(_CYAN));
#if not defined __IMS__
    peer_list = splitter.GetPeerList();
    //O("Size peer list: " << peer_list.size());

    std::cout
      << std::setw(5)
      << peer_list.size()
      << " |";

    std::vector<boost::asio::ip::udp::endpoint>::iterator it;
    for (it = peer_list.begin(); it != peer_list.end(); ++it) {

      std::cout
	//<< _SET_COLOR(_BLUE)
      	<< ' ' << *it ;

      int gs = splitter.GetLoss(*it);
      if (gs>0)
	std::cout
	  << _SET_COLOR(_RED);
      std::cout
	<< ' '
	<< splitter.GetLoss(*it)
	<< "/"
	<< chunks_sendto
	<< _RESET_COLOR();

      /*if (splitter_dbs->GetMagicFlags() >= p2psp::Common::kACS) { // If is ACS
      // _SET_COLOR(_YELLOW);
      O(splitter_acs->GetPeriod(*it));
      // _SET_COLOR(_PURPLE)
      O((splitter_acs->GetNumberOfSentChunksPerPeer(*it) *
      splitter_acs->GetChunkSize() * 8) /
      1000);
      splitter_acs->SetNumberOfSentChunksPerPeer(*it, 0);
      }*/
    }
#endif
    std::cout << std::endl;
  }

  std::cout << "Ending ..." << std::endl;
  HandlerEndOfExecution();

  return 0;
}
