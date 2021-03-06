﻿// <copyright file="BuildTask.h" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

#pragma once

namespace Soup::Cpp
{
	export using CreateCompiler = std::function<std::shared_ptr<Soup::Cpp::Compiler::ICompiler>(Soup::Build::Utilities::ValueTableWrapper&)>;
	export using CompilerFactory = std::map<std::string, CreateCompiler>;

	/// <summary>
	/// The build task
	/// </summary>
	export class BuildTask : public Memory::ReferenceCounted<Soup::Build::IBuildTask>
	{
	public:
		BuildTask(CompilerFactory compilerFactory) :
			_compilerFactory(std::move(compilerFactory))
		{
		}

		/// <summary>
		/// Get the task name
		/// </summary>
		const char* GetName() const noexcept override final
		{
			return "Build";
		}

		/// <summary>
		/// Get the run before list
		/// </summary>
		const Soup::Build::IReadOnlyList<const char*>& GetRunBeforeList() const noexcept override final
		{
			return _runBeforeList;
		}

		/// <summary>
		/// Get the run after list
		/// </summary>
		const Soup::Build::IReadOnlyList<const char*>& GetRunAfterList() const noexcept override final
		{
			return _runAfterList;
		}

		/// <summary>
		/// The Core build task
		/// </summary>
		Soup::Build::ApiCallResult TryExecute(
			Soup::Build::IBuildState& buildState) noexcept override final
		{
			auto buildStateWrapper = Soup::Build::Utilities::BuildStateWrapper(buildState);

			try
			{
				return Execute(buildStateWrapper);
			}
			catch (const std::exception& ex)
			{
				buildStateWrapper.LogError(ex.what());
				return Soup::Build::ApiCallResult::Error;
			}
			catch(...)
			{
				// Unknown error
				return Soup::Build::ApiCallResult::Error;
			}
		}

	private:
		/// <summary>
		/// The Core build task
		/// </summary>
		Soup::Build::ApiCallResult Execute(
			Soup::Build::Utilities::BuildStateWrapper& buildState)
		{
			auto activeState = buildState.GetActiveState();
			auto sharedState = buildState.GetSharedState();

			auto buildTable = activeState.GetValue("Build").AsTable();
			auto parametersTable = activeState.GetValue("Parameters").AsTable();

			auto arguments = Soup::Cpp::Compiler::BuildArguments();
			arguments.TargetArchitecture = parametersTable.GetValue("Architecture").AsString().GetValue();
			arguments.TargetName = buildTable.GetValue("TargetName").AsString().GetValue();
			arguments.TargetType = static_cast<Soup::Cpp::Compiler::BuildTargetType>(
				buildTable.GetValue("TargetType").AsInteger().GetValue());
			arguments.LanguageStandard = static_cast<Soup::Cpp::Compiler::LanguageStandard>(
				buildTable.GetValue("LanguageStandard").AsInteger().GetValue());
			arguments.WorkingDirectory = Path(buildTable.GetValue("WorkingDirectory").AsString().GetValue());
			arguments.ObjectDirectory = Path(buildTable.GetValue("ObjectDirectory").AsString().GetValue());
			arguments.BinaryDirectory = Path(buildTable.GetValue("BinaryDirectory").AsString().GetValue());

			if (buildTable.HasValue("ModuleInterfaceSourceFile"))
			{
				arguments.ModuleInterfaceSourceFile =
					Path(buildTable.GetValue("ModuleInterfaceSourceFile").AsString().GetValue());
			}

			if (buildTable.HasValue("Source"))
			{
				arguments.SourceFiles =
					buildTable.GetValue("Source").AsList().CopyAsPathVector();
			}

			if (buildTable.HasValue("IncludeDirectories"))
			{
				arguments.IncludeDirectories =
					buildTable.GetValue("IncludeDirectories").AsList().CopyAsPathVector();
			}

			if (buildTable.HasValue("PlatformLibraries"))
			{
				arguments.PlatformLinkDependencies =
					buildTable.GetValue("PlatformLibraries").AsList().CopyAsPathVector();
			}

			if (buildTable.HasValue("LinkLibraries"))
			{
				arguments.LinkDependencies =
					buildTable.GetValue("LinkLibraries").AsList().CopyAsPathVector();
			}

			if (buildTable.HasValue("LibraryPaths"))
			{
				arguments.LibraryPaths =
					buildTable.GetValue("LibraryPaths").AsList().CopyAsPathVector();
			}

			if (buildTable.HasValue("PreprocessorDefinitions"))
			{
				arguments.PreprocessorDefinitions =
					buildTable.GetValue("PreprocessorDefinitions").AsList().CopyAsStringVector();
			}

			if (buildTable.HasValue("OptimizationLevel"))
			{
				arguments.OptimizationLevel = static_cast<Soup::Cpp::Compiler::BuildOptimizationLevel>(
					buildTable.GetValue("OptimizationLevel").AsInteger().GetValue());
			}
			else
			{
				arguments.OptimizationLevel = Soup::Cpp::Compiler::BuildOptimizationLevel::None;
			}

			if (buildTable.HasValue("GenerateSourceDebugInfo"))
			{
				arguments.GenerateSourceDebugInfo =
					buildTable.GetValue("GenerateSourceDebugInfo").AsBoolean().GetValue();
			}
			else
			{
				arguments.GenerateSourceDebugInfo = false;
			}

			// Load the runtime dependencies
			if (buildTable.HasValue("RuntimeDependencies"))
			{
				arguments.RuntimeDependencies = MakeUnique(
					buildTable.GetValue("RuntimeDependencies").AsList().CopyAsPathVector());
			}

			// Load the link dependencies
			if (buildTable.HasValue("LinkDependencies"))
			{
				arguments.LinkDependencies = MakeUnique(
					buildTable.GetValue("LinkDependencies").AsList().CopyAsPathVector());
			}

			// Load the module references
			if (buildTable.HasValue("ModuleDependencies"))
			{
				arguments.ModuleDependencies = MakeUnique(
					buildTable.GetValue("ModuleDependencies").AsList().CopyAsPathVector());
			}

			// Load the list of disabled warnings
			if (buildTable.HasValue("EnableWarningsAsErrors"))
			{
				arguments.EnableWarningsAsErrors =
					buildTable.GetValue("EnableWarningsAsErrors").AsBoolean().GetValue();
			}
			else
			{
				arguments.GenerateSourceDebugInfo = false;
			}

			// Load the list of disabled warnings
			if (buildTable.HasValue("DisabledWarnings"))
			{
				arguments.DisabledWarnings =
					buildTable.GetValue("DisabledWarnings").AsList().CopyAsStringVector();
			}

			// Check for any custom compiler flags
			if (buildTable.HasValue("CustomCompilerProperties"))
			{
				arguments.CustomProperties =
					buildTable.GetValue("CustomCompilerProperties").AsList().CopyAsStringVector();
			}

			// Initialize the compiler to use
			auto compilerName = std::string(parametersTable.GetValue("Compiler").AsString().GetValue());
			auto findCompilerFactory = _compilerFactory.find(compilerName);
			if (findCompilerFactory == _compilerFactory.end())
			{
				buildState.LogError("Unknown compiler: " + compilerName);
				return Soup::Build::ApiCallResult::Error;
			}

			auto createCompiler = findCompilerFactory->second;
			auto compiler = createCompiler(activeState);

			auto buildEngine = Soup::Cpp::Compiler::BuildEngine(compiler);
			auto buildResult = buildEngine.Execute(buildState, arguments);

			// Pass along internal state for other stages to gain access
			buildTable.EnsureValue("InternalLinkDependencies").EnsureList().SetAll(buildResult.InternalLinkDependencies);

			// Always pass along required input to shared build tasks
			auto sharedBuildTable = sharedState.EnsureValue("Build").EnsureTable();
			sharedBuildTable.EnsureValue("ModuleDependencies").EnsureList().SetAll(buildResult.ModuleDependencies);
			sharedBuildTable.EnsureValue("RuntimeDependencies").EnsureList().SetAll(buildResult.RuntimeDependencies);
			sharedBuildTable.EnsureValue("LinkDependencies").EnsureList().SetAll(buildResult.LinkDependencies);

			if (!buildResult.TargetFile.IsEmpty())
			{
				sharedBuildTable.EnsureValue("TargetFile").SetValueString(buildResult.TargetFile.ToString());
			}

			// Register the build operations
			for (auto& operation : buildResult.BuildOperations)
			{
				buildState.CreateOperation(operation);
			}

			buildState.LogInfo("Build Generate Done");
			return Soup::Build::ApiCallResult::Success;
		}

		static std::vector<Path> MakeUnique(const std::vector<Path>& collection)
		{
			std::unordered_set<std::string> valueSet;
			for (auto& value : collection)
				valueSet.insert(value.ToString());

			std::vector<Path> result;
			for (auto& value : valueSet)
				result.push_back(Path(value));

			return result;
		}

	private:
		CompilerFactory _compilerFactory;
		Soup::Build::Utilities::ReadOnlyStringList _runBeforeList;
		Soup::Build::Utilities::ReadOnlyStringList _runAfterList;
	};
}
