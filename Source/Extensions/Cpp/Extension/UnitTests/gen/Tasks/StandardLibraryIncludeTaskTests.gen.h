#pragma once
#include "Tasks/StandardLibraryIncludeTaskTests.h"

TestState RunStandardLibraryIncludeTaskTests() 
 {
	auto className = "StandardLibraryIncludeTaskTests";
	auto testClass = std::make_shared<Soup::Cpp::UnitTests::StandardLibraryIncludeTaskTests>();
	TestState state = { 0, 0 };
	state += Soup::Test::RunTest(className, "Initialize_Success", [&testClass]() { testClass->Initialize_Success(); });
	state += Soup::Test::RunTest(className, "Execute", [&testClass]() { testClass->Execute(); });

	return state;
}