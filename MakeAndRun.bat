
set ProjectRoot=Z:\Users\ardiandemiri\Desktop\Module\M121\avr\liftdoor
set ProjectBuild=%ProjectRoot%\Build
set ToolsRoot=Z:\Users\ardiandemiri\Desktop\Module\M121\avr\ToolChain
set PyScripts=%ToolsRoot%\PyScripts
set AvrGcc=%ToolsRoot%\avr8-gnu-toolchain-win32_x86
set AvrDude=%ToolsRoot%\avrdude-6.3-mingw32
set DotNetLib=%ToolsRoot%\DotNetLib
python %PyScripts%\GenerateMake.py %ProjectRoot% %ProjectBuild% elf
pushd %ProjectBuild%
call make.exe -f "%ProjectBuild%\Makefile" flash
popd
