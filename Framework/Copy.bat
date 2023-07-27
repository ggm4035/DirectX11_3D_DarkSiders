attrib -R .\Reference\Headers\*.*
xcopy				/y /s		.\Engine\Public\*.*							.\Reference\Headers\
attrib +R .\Reference\Headers\*.*
xcopy				/y /s		.\Engine\ThirdPartyLib\*.*					.\Reference\Librarys\

xcopy				/y			.\Engine\Bin\Engine.lib						.\Reference\Librarys\
xcopy				/y			.\Engine\Bin\Engine.dll						.\Client\Bin\
xcopy				/y			.\Engine\Bin\Engine.dll						.\IMGUI\Bin\
xcopy				/y			.\Engine\Bin\Engine.dll						.\ModelLoader\Bin\

xcopy				/y /s		.\Engine\Bin\ShaderFiles\*.hlsl				.\Client\Bin\ShaderFiles\
xcopy				/y /s		.\Engine\Bin\ShaderFiles\*.hlsl				.\IMGUI\Bin\ShaderFiles\