rmdir /s /q "ipch"
rmdir /s /q "Debug"
rmdir /s /q "Release"
rmdir /s /q "x64"
rmdir /s /q ".vs"

rmdir /s /q .\Framework\Debug
rmdir /s /q .\Framework\ipch
rmdir /s /q .\Framework\Release
rmdir /s /q .\Framework\x64
rmdir /s /q .\Framework\.vs

rmdir /s /q .\Engine\System\Include\Debug
rmdir /s /q .\Engine\Utility\Include\Debug

del /f /q .\Client\Bin\Client.exe
del /f /q .\Client\Bin\Client.ilk
del /f /q .\Client\Bin\Client.pdb
del /f /q Framework.VC.db
