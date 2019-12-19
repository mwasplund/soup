// <copyright file="CompilerArgumentBuilderTests.h" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

#pragma once

namespace Soup::Compiler::MSVC::UnitTests
{
	class CompilerArgumentBuilderTests
	{
	public:
		[[Fact]]
		void EmptySourceFile_Throws()
		{
			CompileArguments arguments = {};
			arguments.SourceFile = Path("");
			arguments.TargetFile = Path("File.obj");
			auto toolsPath = Path("tools/");
			Assert::ThrowsRuntimeError([&arguments, &toolsPath]() {
				auto actual = ArgumentBuilder::BuildCompilerArguments(arguments, toolsPath);
			});
		}

		[[Fact]]
		void EmptyTargetFile_Throws()
		{
			CompileArguments arguments = {};
			arguments.SourceFile = Path("File.cpp");
			arguments.TargetFile = Path("");
			auto toolsPath = Path("tools/");
			Assert::ThrowsRuntimeError([&arguments, &toolsPath]() {
				auto actual = ArgumentBuilder::BuildCompilerArguments(arguments, toolsPath);
			});
		}

		[[Fact]]
		void DefaultParameters()
		{
			CompileArguments arguments = {};
			arguments.SourceFile = Path("File.cpp");
			arguments.TargetFile = Path("File.obj");
			auto toolsPath = Path("tools/");
			auto actual = ArgumentBuilder::BuildCompilerArguments(arguments, toolsPath);

			auto expected = std::vector<std::string>({
				"/nologo",
				"/std:c++11",
				"/Od",
				"/X",
				"/RTC1",
				"/EHsc",
				"/MTd",
				"/bigobj",
				"/c",
				"File.cpp",
				"/Fo\"File.obj\"",
			});

			Assert::AreEqual(expected, actual, "Verify generated arguments match expected.");
		}

		[[Fact]]
		void SingleArgument_GenerateIncludeTree()
		{
			CompileArguments arguments = {};
			arguments.SourceFile = Path("File.cpp");
			arguments.TargetFile = Path("File.obj");
			arguments.GenerateIncludeTree = true;
			auto toolsPath = Path("tools/");
			auto actual = ArgumentBuilder::BuildCompilerArguments(arguments, toolsPath);

			auto expected = std::vector<std::string>({
				"/nologo",
				"/showIncludes",
				"/std:c++11",
				"/Od",
				"/X",
				"/RTC1",
				"/EHsc",
				"/MTd",
				"/bigobj",
				"/c",
				"File.cpp",
				"/Fo\"File.obj\"",
			});

			Assert::AreEqual(expected, actual, "Verify generated arguments match expected.");
		}

		[[Theory]]
		[[InlineData(Soup::LanguageStandard::CPP11, "/std:c++11")]]
		[[InlineData(Soup::LanguageStandard::CPP14, "/std:c++14")]]
		[[InlineData(Soup::LanguageStandard::CPP17, "/clang:-std=c++17")]]
		void SingleArgument_LanguageStandard(LanguageStandard standard, std::string expectedFlag)
		{
			CompileArguments arguments = {};
			arguments.SourceFile = Path("File.cpp");
			arguments.TargetFile = Path("File.obj");
			arguments.Standard = standard;
			auto toolsPath = Path("tools/");
			auto actual = ArgumentBuilder::BuildCompilerArguments(arguments, toolsPath);

			auto expected = std::vector<std::string>({
				"/nologo",
				expectedFlag,
				"/Od",
				"/X",
				"/RTC1",
				"/EHsc",
				"/MTd",
				"/bigobj",
				"/c",
				"File.cpp",
				"/Fo\"File.obj\"",
			});

			Assert::AreEqual(expected, actual, "Verify generated arguments match expected.");
		}

		[[Fact]]
		void SingleArgument_LanguageStandard_CPP20()
		{
			CompileArguments arguments = {};
			arguments.SourceFile = Path("File.cpp");
			arguments.TargetFile = Path("File.obj");
			arguments.Standard = LanguageStandard::CPP20;
			auto toolsPath = Path("tools/");
			auto actual = ArgumentBuilder::BuildCompilerArguments(arguments, toolsPath);

			auto expected = std::vector<std::string>({
				"/nologo",
				"/std:c++latest",
				"/experimental:module",
				"/Od",
				"/X",
				"/RTC1",
				"/EHsc",
				"/MTd",
				"/bigobj",
				"/c",
				"File.cpp",
				"/Fo\"File.obj\"",
			});

			Assert::AreEqual(expected, actual, "Verify generated arguments match expected.");
		}

		[[Fact]]
		void SingleArgument_OptimizationLevel_Disabled()
		{
			CompileArguments arguments = {};
			arguments.SourceFile = Path("File.cpp");
			arguments.TargetFile = Path("File.obj");
			arguments.Standard = LanguageStandard::CPP17;
			arguments.Optimize = OptimizationLevel::None;
			auto toolsPath = Path("tools/");
			auto actual = ArgumentBuilder::BuildCompilerArguments(arguments, toolsPath);

			auto expected = std::vector<std::string>({
				"/nologo",
				"/std:c++17",
				"/Od",
				"/X",
				"/RTC1",
				"/EHsc",
				"/MTd",
				"/bigobj",
				"/c",
				"File.cpp",
				"/Fo\"File.obj\"",
			});

			Assert::AreEqual(expected, actual, "Verify generated arguments match expected.");
		}

		[[Theory]]
		[[InlineData(Soup::OptimizationLevel::Size, "/Os")]]
		[[InlineData(Soup::OptimizationLevel::Speed, "/Ot")]]
		void SingleArgument_OptimizationLevel(OptimizationLevel level, std::string expectedFlag)
		{
			CompileArguments arguments = {};
			arguments.SourceFile = Path("File.cpp");
			arguments.TargetFile = Path("File.obj");
			arguments.Standard = LanguageStandard::CPP17;
			arguments.Optimize = level;
			auto toolsPath = Path("tools/");
			auto actual = ArgumentBuilder::BuildCompilerArguments(arguments, toolsPath);

			auto expected = std::vector<std::string>({
				"/nologo",
				"/std:c++17",
				expectedFlag,
				"/X",
				"/RTC1",
				"/EHsc",
				"/MT",
				"/bigobj",
				"/c",
				"File.cpp",
				"/Fo\"File.obj\"",
			});

			Assert::AreEqual(expected, actual, "Verify generated arguments match expected.");
		}

		[[Fact]]
		void SingleArgument_GenerateDebugInformation()
		{
			CompileArguments arguments = {};
			arguments.SourceFile = Path("File.cpp");
			arguments.TargetFile = Path("File.obj");
			arguments.Standard = LanguageStandard::CPP17;
			arguments.Optimize = OptimizationLevel::None;
			arguments.GenerateSourceDebugInfo = true;
			auto toolsPath = Path("tools/");
			auto actual = ArgumentBuilder::BuildCompilerArguments(arguments, toolsPath);

			auto expected = std::vector<std::string>({
				"/nologo",
				"/Z7",
				"/std:c++17",
				"/Od",
				"/X",
				"/RTC1",
				"/EHsc",
				"/MTd",
				"/bigobj",
				"/c",
				"File.cpp",
				"/Fo\"File.obj\"",
			});

			Assert::AreEqual(expected, actual, "Verify generated arguments match expected.");
		}

		[[Fact]]
		void SingleArgument_IncludePaths()
		{
			CompileArguments arguments = {};
			arguments.SourceFile = Path("File.cpp");
			arguments.TargetFile = Path("File.obj");
			arguments.IncludeDirectories = std::vector<Path>({
				Path("C:/Files/SDK/"),
				Path("my files/")
			});
			auto toolsPath = Path("tools/");
			auto actual = ArgumentBuilder::BuildCompilerArguments(arguments, toolsPath);

			auto expected = std::vector<std::string>({
				"/nologo",
				"/std:c++11",
				"/Od",
				"/I\"C:/Files/SDK/\"",
				"/I\"my files/\"",
				"/X",
				"/RTC1",
				"/EHsc",
				"/MTd",
				"/bigobj",
				"/c",
				"File.cpp",
				"/Fo\"File.obj\"",
			});

			Assert::AreEqual(expected, actual, "Verify generated arguments match expected.");
		}

		[[Fact]]
		void SingleArgument_PreprocessorDefinitions()
		{
			CompileArguments arguments = {};
			arguments.SourceFile = Path("File.cpp");
			arguments.TargetFile = Path("File.obj");
			arguments.PreprocessorDefinitions = std::vector<std::string>({
				"DEBUG",
				"VERSION=1"
			});
			auto toolsPath = Path("tools/");
			auto actual = ArgumentBuilder::BuildCompilerArguments(arguments, toolsPath);

			auto expected = std::vector<std::string>({
				"/nologo",
				"/std:c++11",
				"/Od",
				"/DDEBUG",
				"/DVERSION=1",
				"/X",
				"/RTC1",
				"/EHsc",
				"/MTd",
				"/bigobj",
				"/c",
				"File.cpp",
				"/Fo\"File.obj\"",
			});

			Assert::AreEqual(expected, actual, "Verify generated arguments match expected.");
		}

		[[Fact]]
		void SingleArgument_Modules()
		{
			CompileArguments arguments = {};
			arguments.SourceFile = Path("File.cpp");
			arguments.TargetFile = Path("File.obj");
			arguments.IncludeModules = std::vector<Path>({
				Path("Module.pcm"),
				Path("Std.pcm")
			});
			auto toolsPath = Path("tools/");
			auto actual = ArgumentBuilder::BuildCompilerArguments(arguments, toolsPath);

			auto expected = std::vector<std::string>({
				"/nologo",
				"/std:c++11",
				"/Od",
				"/X",
				"/RTC1",
				"/EHsc",
				"/MTd",
				"/module:reference",
				"\"Module.pcm\"",
				"/module:reference",
				"\"Std.pcm\"",
				"/bigobj",
				"/c",
				"File.cpp",
				"/Fo\"File.obj\"",
			});

			Assert::AreEqual(expected, actual, "Verify generated arguments match expected.");
		}

		[[Fact]]
		void SingleArgument_ExportModule_SingleSource()
		{
			CompileArguments arguments = {};
			arguments.SourceFile = Path("module.cpp");
			arguments.TargetFile = Path("module.obj");
			arguments.ExportModule = true;
			auto toolsPath = Path("tools/");
			auto actual = ArgumentBuilder::BuildCompilerArguments(arguments, toolsPath);

			auto expected = std::vector<std::string>({
				"/nologo",
				"/std:c++11",
				"/Od",
				"/X",
				"/RTC1",
				"/EHsc",
				"/MTd",
				"/module:export",
				"/module:output",
				"\"module.ifc\"",
				"/bigobj",
				"/c",
				"module.cpp",
				"/Fo\"module.obj\"",
			});
			Assert::AreEqual(expected, actual, "Verify generated arguments match expected.");
		}
	};
}
