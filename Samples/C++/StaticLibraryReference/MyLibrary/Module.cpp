module;

// Include all standard library headers in the global module
#include <string>

export module MyLibrary;

export namespace MyLibrary
{
	class Helper
	{
	public:
		static std::string GetName()
		{
			return "Soup";
		}
	};
}
