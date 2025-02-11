if not exist .\msys2\root\ucrt64.exe (
  call .\msys2\installer in -c --am --root "%~dp0\msys2\root"
  call .\msys2\root\msys2_shell.cmd -ucrt64 -here -no-start "%~dp0\msys2\update-system.sh"
  call .\msys2\root\msys2_shell.cmd -defterm -ucrt64 -here -no-start "%~dp0\msys2\install-packages.sh"
  timeout 5
)
call .\msys2\root\msys2_shell.cmd -ucrt64 -defterm -here -no-start "%~dp0\msys2\start-melonDS.sh"
