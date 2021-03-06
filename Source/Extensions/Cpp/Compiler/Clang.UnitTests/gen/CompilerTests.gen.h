#pragma once
#include "CompilerTests.h"

TestState RunCompilerTests() 
 {
	auto className = "CompilerTests";
	auto testClass = std::make_shared<Soup::Cpp::Compiler::Clang::UnitTests::CompilerTests>();
	TestState state = { 0, 0 };
	state += Soup::Test::RunTest(className, "Initialize", [&testClass]() { testClass->Initialize(); });
	state += Soup::Test::RunTest(className, "Compile_Simple", [&testClass]() { testClass->Compile_Simple(); });
	state += Soup::Test::RunTest(className, "Compile_Module", [&testClass]() { testClass->Compile_Module(); });
	state += Soup::Test::RunTest(className, "LinkStaticLibrary_Simple", [&testClass]() { testClass->LinkStaticLibrary_Simple(); });
	state += Soup::Test::RunTest(className, "LinkExecutable_Simple", [&testClass]() { testClass->LinkExecutable_Simple(); });

	return state;
}