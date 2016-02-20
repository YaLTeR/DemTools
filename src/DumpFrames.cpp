#include <chrono>
#include <boost/nowide/args.hpp>
#include <boost/nowide/fstream.hpp>
#include <boost/nowide/iostream.hpp>

#include "DemoFile.hpp"

using namespace boost;

int main(int argc, char *argv[])
{
	nowide::args a(argc, argv);

	if (argc != 2) {
		nowide::cerr << "Usage:\n\tDumpFrames <path to demo.dem>" << std::endl;
		return 1;
	}

	try {
		DemoFile demo(argv[1]);
		demo.ReadFrames();

		nowide::cout.precision(8);
		nowide::cout.setf(std::ios::fixed);

		size_t i = 0;
		for (auto& entry : demo.directoryEntries) {
			nowide::cout << "Entry " << ++i << ":\n";
			for (auto& frame : entry.frames) {
				nowide::cout << "f: " << frame->frame << " t: " << frame->time << ' ';

				#define t(name) \
					if (frame->type == DemoFrameType::name) { \
						nowide::cout << #name; \
					}
				t(DEMO_START);
				t(CONSOLE_COMMAND);
				t(CLIENT_DATA);
				t(NEXT_SECTION);
				t(EVENT);
				t(WEAPON_ANIM);
				t(SOUND);
				t(DEMO_BUFFER);
				#undef t

				if (frame->type == DemoFrameType::CONSOLE_COMMAND){
					ConsoleCommandFrame *f = static_cast<ConsoleCommandFrame*>(frame.get());
					nowide::cout << " `" << f->command << '`';
				}

				if (static_cast<int>(frame->type) < 2 || static_cast<int>(frame->type) > 9) {
					NetMsgFrame *f = static_cast<NetMsgFrame*>(frame.get());
					nowide::cout << "NETMSG ft: " << f->DemoInfo.RefParams.frametime
						<< " ms: " << static_cast<uint16_t>(f->DemoInfo.UserCmd.msec);
				}

				nowide::cout << '\n';
			}
		}
	} catch (const std::exception& ex) {
		nowide::cerr << "Error: " << ex.what() << std::endl;
	}

	nowide::cout.flush();
	return 0;
}
