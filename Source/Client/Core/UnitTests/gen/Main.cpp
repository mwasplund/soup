#include <any>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <queue>
#include <sstream>
#include <string>
#include <vector>

import Opal;
import Opal.Extensions;
import Soup.Core;
import json11;
import Soup.Test.Assert;

using namespace Opal;
using namespace Opal::System;
using namespace Soup::Test;

#include "Api/SoupApiTests.gen.h"
#include "Api/SoupApiJsonModelsTests.gen.h"

#include "Config/LocalUserConfigExtensionsTests.gen.h"
#include "Config/LocalUserConfigJsonTests.gen.h"
#include "Config/LocalUserConfigTests.gen.h"

#include "Package/PackageManagerTests.gen.h"
#include "Package/RecipeBuilderTests.gen.h"
#include "Package/RecipeBuildManagerTests.gen.h"

int main()
{
	std::cout << "Running Tests..." << std::endl;

	TestState state = { 0, 0 };

	state += RunSoupApiTests();
	state += RunSoupApiJsonModelsTests();

	state += RunLocalUserConfigExtensionsTests();
	state += RunLocalUserConfigJsonTests();
	state += RunLocalUserConfigTests();

	state += RunPackageManagerTests();
	state += RunRecipeBuilderTests();
	state += RunRecipeBuildManagerTests();

	// Touch stamp file to ensure incremental builds work
	// auto testFile = std::fstream("TestHarness.stamp", std::fstream::out);
	// testFile << "TOUCH";

	std::cout << state.PassCount << " PASSED." << std::endl;
	std::cout << state.FailCount << " FAILED." << std::endl;

	if (state.FailCount > 0)
		return 1;
	else
		return 0;
}
