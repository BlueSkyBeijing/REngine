
class IModuleInterface
{
public:
	virtual ~IModuleInterface()
	{
	}

	virtual void Init()
	{
	}

	virtual void UnInit()
	{
	}
};

#define DLLEXPORT __declspec(dllexport)
#define DLLIMPORT __declspec(dllimport)

#define IMPLEMENT_MODULE( ModuleImplClass, ModuleName ) \
		extern "C" DLLEXPORT IModuleInterface* InitializeModule() \
		{ \
			return new ModuleImplClass(); \
		} \
