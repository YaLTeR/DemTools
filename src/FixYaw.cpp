#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <boost/nowide/args.hpp>
#include <boost/nowide/iostream.hpp>

#include "DemoFile.hpp"

namespace nowide = boost::nowide;

void usage()
{
	nowide::cerr << "Usage:"
		"\n\tFixYaw <path to demo.dem> <yaw>"
		"\n\t\t- Fix the yaw to <yaw>, save the result into <demo>_fixyaw.dem."
		<< std::endl;
}

int main(int argc, char *argv[])
{
	nowide::args a(argc, argv);

	if (argc != 3) {
		usage();
		return 1;
	}

	auto yaw = std::atof(argv[2]);

	try {
		DemoFile demo(argv[1]);
		nowide::cout << "Fixing the yaw in " << argv[1] << "..." << std::endl;
		demo.ReadFrames();

		for (auto& entry : demo.directoryEntries) {
			for (auto& frame : entry.frames) {
				if (static_cast<int>(frame->type) < 2 || static_cast<int>(frame->type) > 9) {
					NetMsgFrame *f = static_cast<NetMsgFrame*>(frame.get());
					f->DemoInfo.RefParams.viewangles[1] = yaw;
					f->DemoInfo.RefParams.cl_viewangles[1] = yaw;
					f->DemoInfo.UserCmd.viewangles[1] = yaw;
				}
			}
		}
		
		auto filename = std::string{ argv[1] };
		auto dot = filename.rfind('.');
		if (dot != std::string::npos) {
			filename = filename.substr(0, dot) + "_fixyaw" + filename.substr(dot);
		} else {
			filename += "_fixyaw";
		}

		demo.Save(filename);

		nowide::cout << "Done." << std::endl;
	} catch (const std::exception& ex) {
		nowide::cerr << "Error: " << ex.what() << std::endl;
		char c;
		nowide::cin.getline(&c, 1);
	}

	return 0;
}
