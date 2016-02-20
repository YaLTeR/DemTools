#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <boost/nowide/args.hpp>
#include <boost/nowide/iostream.hpp>

#include "DemoFile.hpp"

namespace nowide = boost::nowide;

void usage()
{
	nowide::cout << "Usage:"
		"\n\tListdemo <path to demo.dem>"
		"\n\t- Shows information about the demo."
		<< std::endl;
}

template<size_t N>
inline bool any_matches(const char* pattern, const char* const (&samples)[N])
{
	for (size_t i = 0; i < N; ++i) {
		if (!std::strcmp(pattern, samples[i]))
			return true;
	}
	return false;
}

int main(int argc, char *argv[])
{
	nowide::args a(argc, argv);

	if (argc != 2) {
		usage();
		nowide::cin.get();
		return 1;
	}

	try {
		DemoFile demo(argv[1]);
		nowide::cout << "Reading " << argv[1] << "...\n\n";

		nowide::cout << "Demo protocol: " << demo.header.demoProtocol << '\n';
		nowide::cout << "Net protocol: " << demo.header.netProtocol << '\n';
		nowide::cout << "Map name: " << demo.header.mapName << '\n';
		nowide::cout << "Game directory: " << demo.header.gameDir << "\n\n";

		size_t i = 0;
		for (const auto& entry : demo.directoryEntries) {
			++i;
			if (entry.type == 0) {
				// Don't print the start segment, no useful info.
				continue;
			}
			nowide::cout << i << ":\n";
			nowide::cout << "\tType: " << (entry.type ? "normal" : "start") << " segment\n";
			nowide::cout << "\tDescription: " << entry.description << '\n';
			nowide::cout << "\tTime (inaccurate): " << entry.trackTime << "s\n";
			nowide::cout << "\tFrames: " << entry.frameCount << '\n';
		}

		nowide::cout << "\nReading frames...\n" << std::endl;
		demo.ReadFrames();

		float frametime_min, frametime_max;
		double frametime_sum = 0.0;
		size_t count = 0;
		uint8_t msec_min, msec_max;
		long long msec_sum = 0;
		bool first = true;
		bool found_cam_commands = false;
		for (auto& entry : demo.directoryEntries) {
			for (auto& frame : entry.frames) {
				if (static_cast<int>(frame->type) < 2 || static_cast<int>(frame->type) > 9) {
					NetMsgFrame *f = static_cast<NetMsgFrame*>(frame.get());
					frametime_sum += f->DemoInfo.RefParams.frametime;
					msec_sum += f->DemoInfo.UserCmd.msec;
					count++;

					if (first) {
						first = false;
						frametime_min = f->DemoInfo.RefParams.frametime;
						frametime_max = f->DemoInfo.RefParams.frametime;
						msec_min = f->DemoInfo.UserCmd.msec;
						msec_max = f->DemoInfo.UserCmd.msec;
					} else {
						frametime_min = std::min(frametime_min, f->DemoInfo.RefParams.frametime);
						frametime_max = std::max(frametime_max, f->DemoInfo.RefParams.frametime);
						msec_min = std::min(msec_min, f->DemoInfo.UserCmd.msec);
						msec_max = std::max(msec_max, f->DemoInfo.UserCmd.msec);
					}
				}

				if (frame->type == DemoFrameType::CONSOLE_COMMAND) {
					const char* const camera_commands[] = {
						"+lookup",
						"+lookdown",
						"+left",
						"+right"
					};

					ConsoleCommandFrame *f = static_cast<ConsoleCommandFrame*>(frame.get());
					if (!found_cam_commands && any_matches(f->command.c_str(), camera_commands)) {
						found_cam_commands = true;
					}
				}
			}
		}

		if (first) {
			nowide::cout << "There are no demo frames.\n";
		} else {
			nowide::cout << "Highest FPS: " << (1 / frametime_min) << '\n';
			nowide::cout << "Lowest FPS: " << (1 / frametime_max) << '\n';
			nowide::cout << "Average FPS: " << (count / frametime_sum) << '\n';
			nowide::cout << "Lowest msec: " << static_cast<unsigned>(msec_min) << " (" << (1000.0 / msec_min) << " FPS)\n";
			nowide::cout << "Highest msec: " << static_cast<unsigned>(msec_max) << " (" << (1000.0 / msec_max) << " FPS)\n";
			nowide::cout << "Average msec: " << (msec_sum / static_cast<double>(count)) << " (" << (1000.0 / (msec_sum / static_cast<double>(count))) << " FPS)\n";

			if (found_cam_commands)
				nowide::cout << "\nFound camera movement commands.\n";
		}
	} catch (const std::exception& ex) {
		nowide::cout << "Error: " << ex.what() << std::endl;
	}
	nowide::cin.get();

	return 0;
}
