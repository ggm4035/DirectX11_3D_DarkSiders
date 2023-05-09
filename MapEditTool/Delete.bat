rmdir /s /q "ipch"
rmdir /s /q "Debug"
rmdir /s /q "Release"
rmdir /s /q "x64"
rmdir /s /q ".vs"

rmdir /s /q .\MapEditTool\Debug
rmdir /s /q .\MapEditTool\ipch
rmdir /s /q .\MapEditTool\Release
rmdir /s /q .\MapEditTool\x64
rmdir /s /q .\MapEditTool\.vs

rmdir /s /q .\Engine\System\Include\Debug
rmdir /s /q .\Engine\Utility\Include\Debug

del /f /q .\Client\Bin\Client.exe
del /f /q .\Client\Bin\Client.ilk
del /f /q .\Client\Bin\Client.pdb
del /f /q Framework.VC.db
