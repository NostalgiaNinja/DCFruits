#include "pch.h"
#include "SA2ModLoader.h"
#include <string>
#include "ModelInfo.h"

extern "C"
{
	//Non-SA2B structs - Need to be added for CWE purposes.
	struct BlackMarketItemAttributes
	{
		int PurchasePrice;
		int SalePrice;
		__int16 RequiredEmblems;
		__int16 Name;
		__int16 Descriptions;
		__int16 Unknown;
	};

	struct ItemChance
	{
		Sint8 item;
		Sint8 chance;
	};

	//NINJA types - Texture name and textlist (derives from texture name)
	NJS_TEXNAME texname_CUSTOMFRUIT[88];
	NJS_TEXLIST texlist_CUSTOMFRUIT = {arrayptrandlength(texname_CUSTOMFRUIT)};

	//Chao Fruit ID - Initialize to start the process.
	int ExampleFruitID;

	std::string pathStr;

	//registration functions - Handles registering the Chao Fruit to certain locations.  Rare fruit are handled like mushrooms, while general fruit are handled like shape fruit.
	int (*RegisterChaoFruit)(NJS_OBJECT *model, NJS_TEXLIST *texlist, ChaoItemStats *stats, void *attrib, void *Func, const char *name, const char *description);
	void (*RegisterDataFunc)(void *ptr);
	void (*RegisterBlackMarketGeneralFruit)(ItemChance *chance);
	void (*RegisterBlackMarketRareFruit)(ItemChance *chance);
	void (*RegisterChaoTexlistLoad)(const char *name, NJS_TEXLIST *load);

	ChaoItemStats ExampleFruitStats = {10, 10, 0, 0, 0, 0, 50, 0, 0, 0}; //mood, belly, swim, fly, run, power, stamina, luck, intel, unknown;

	//make a function for each fruit you add - gets called in RegisterFruit
	void __cdecl ExampleFruitFunc(ChaoData *data, ObjectMaster *fruit)
	{
		// data->data(data1) has some interesting functions that you can have a look at - See Intellisense update for more information.
	}

	// copied from Hat mod code.
	int RegisterFruit(const char *path, int rings, int sellPrice, ChaoItemStats stats, void *func, const char *name, const char *desc)
	{
		BlackMarketItemAttributes attrib = {rings, sellPrice, 0, -1, -1, 0};
		std::string finalPath = pathStr + "\\" + path;
		ModelInfo *file = new ModelInfo(finalPath);
		return RegisterChaoFruit(file->getmodel(), &texlist_CUSTOMFRUIT, &stats, &attrib, func, name, desc);
	}

	//Black Market Registration
	BlackMarketItemAttributes DragonFruit = {1000, 250, 0, -1, -1, 0};

	void CWELoad()
	{
		//Texture list - Change the name for making different custom textures.
		RegisterChaoTexlistLoad("examplefruit", &texlist_CUSTOMFRUIT);

		//Register the fruit ID to the fruit list - should start at 34 before DCFruits gets initialized, or 36 after.
		ExampleFruitID = RegisterFruit("example.sa2mdl", 1000, 250, ExampleFruitStats, ExampleFruitFunc, "Example Fruit", "Example fruit description");

		//give the fruit a chance to spawn, 50% is standard in DCFruits
		ItemChance ExampleFruitChance{ExampleFruitID, 50};

		//Register the fruit
		RegisterBlackMarketGeneralFruit(&ExampleFruitChance);
	}
	__declspec(dllexport) void Init(const char *path)
	{
		//IMPORTANT - ASSIGN THIS OTHERWISE THE IMPORT MODEL WON'T WORK!
		pathStr = std::string(path);

		HMODULE h = GetModuleHandle(L"CWE");
		RegisterChaoFruit = (int (*)(NJS_OBJECT * model, NJS_TEXLIST * texlist, ChaoItemStats * stats, void *attrib, void *Func, const char *, const char *)) GetProcAddress(h, "RegisterChaoFruit");
		RegisterDataFunc = (void (*)(void *ptr))GetProcAddress(h, "RegisterDataFunc");
		RegisterBlackMarketGeneralFruit = (void (*)(ItemChance * chance)) GetProcAddress(h, "RegisterBlackMarketGeneralFruit");
		RegisterBlackMarketRareFruit = (void (*)(ItemChance * chance)) GetProcAddress(h, "RegisterBlackMarketRareFruit");
		RegisterChaoTexlistLoad = (void (*)(const char *name, NJS_TEXLIST *load))GetProcAddress(h, "RegisterChaoTexlistLoad");

		RegisterDataFunc(CWELoad);
	}
	__declspec(dllexport) ModInfo SA2ModInfo = {ModLoaderVer};
}
