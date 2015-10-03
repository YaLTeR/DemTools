#include <chrono>
#include <cstdio>
#include <cstring>
#include <boost/nowide/args.hpp>
#include <boost/nowide/iostream.hpp>

#include "DemoFile.hpp"

namespace nowide = boost::nowide;

void usage()
{
	nowide::cerr << "Usage:"
		"\n\tDemoSanitizer <path to demo.dem>"
		"\n\t\t- Sanitize the given demo, save the result into <demo>_sanitized.dem."
		"\n\tDemoSanitizer <path to demo.dem> -o <path to output.dem>"
		"\n\t\t- Sanitize the given demo, save the result into output.dem."
		<< std::endl;
}

int main(int argc, char *argv[])
{
	nowide::args a(argc, argv);

	if ((argc != 2 && argc != 4) || (argc == 4 && std::memcmp(argv[2], "-o", 3))) {
		usage();
		return 1;
	}

	try {
		DemoFile demo(argv[1]);
		nowide::cout << "Sanitizing " << argv[1] << "..." << std::endl;
		demo.ReadFrames();

		// Some of the incorrect or malicious frames were filtered out on the demo reading stage.
		// Check the ones that got through.
		for (auto& entry : demo.directoryEntries) {
			for (auto& frame : entry.frames) {
				if (frame->type == DemoFrameType::SOUND) {
					auto f = reinterpret_cast<SoundFrame*>(frame.get());
					// The engine has a 256 byte long buffer
					// and additionally inserts a \0 in the end after reading into it.
					auto s = f->sample.size();
					if (s > 255) {
						f->sample.resize(255);
						nowide::cout << "Sanitized a sound frame, sample size was: " << s << "; maximum allowed is: 255." << std::endl;
					}
				} else if (frame->type == DemoFrameType::DEMO_BUFFER) {
					auto f = reinterpret_cast<DemoBufferFrame*>(frame.get());
					// The engine has a 32768 byte long buffer.
					auto s = f->buffer.size();
					if (s > 32768) {
						f->buffer.resize(32768);
						nowide::cout << "Sanitized a demo buffer frame, buffer size was: " << s << "; maximum allowed is: 32768." << std::endl;
					}
				}
			}
		}

		if (argc == 4) {
			demo.Save(argv[3]);
		} else {
			auto filename = std::string{ argv[1] };
			auto dot = filename.rfind('.');
			if (dot != std::string::npos) {
				filename = filename.substr(0, dot) + "_sanitized" + filename.substr(dot);
			} else {
				filename += "_sanitized";
			}

			demo.Save(filename);
		}

		nowide::cout << "Done." << std::endl;
	} catch (const std::exception& ex) {
		nowide::cerr << "Error: " << ex.what() << std::endl;
	}

	return 0;
}
