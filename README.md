DemTools
==========

A collection of tools that operate GoldSource demo files.
- DemoSanitizer: neutralizes malicious demo frames which may lead to infection of your PC.
- FixYaw: fixes the view yaw to the given value.
- Listdemo: prints some info about the demo (game, map, time, FPS).

#Building
####Windows
- Get [Boost](http://www.boost.org/) and [Boost.Nowide](http://cppcms.com/files/nowide/html/) and build the latter.
- Create a build directory along the *src* directory.
- Run `cmake -DBOOST_ROOT=path\to\boost\base\dir ..` from the build directory.
- Compile the **ALL_BUILD** project from the generated VS solution.

####Linux
- Get Boost.Nowide.
- Create a build directory along the *src* directory.
- Run `cmake ..` from the build directory.
- Run `make` from the build directory.
