#include <iostream>
#include "CModelLoader.h"

int main()
{
	CModelLoader ModelLoader;

	ModelLoader.Initialize();

	ModelLoader.Tick();

	cout << "== Done ==" << endl;

	system("pause");
}