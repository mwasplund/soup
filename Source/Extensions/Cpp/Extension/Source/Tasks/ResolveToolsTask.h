﻿// <copyright file="ResolveToolsTask.h" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

#pragma once

namespace Soup::Cpp
{
	/// <summary>
	/// The recipe build task that knows how to build a single recipe
	/// </summary>
	export class ResolveToolsTask : public Memory::ReferenceCounted<Soup::Build::IBuildTask>
	{
	public:
		/// <summary>
		/// Get the task name
		/// </summary>
		const char* GetName() const noexcept override final
		{
			return "ResolveTools";
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
		/// The Core Execute task
		/// </summary>
		Soup::Build::ApiCallResult TryExecute(
			Soup::Build::IBuildState& buildState) noexcept override final
		{
			auto buildStateWrapper = Soup::Build::Utilities::BuildStateWrapper(buildState);

			try
			{
				Execute(buildStateWrapper);
				return Soup::Build::ApiCallResult::Success;
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
		/// The Core Execute task
		/// </summary>
		void Execute(Soup::Build::Utilities::BuildStateWrapper& buildState)
		{
			auto state = buildState.GetActiveState();
			auto parameters = state.GetValue("Parameters").AsTable();

			auto systemName = std::string(parameters.GetValue("System").AsString().GetValue());
			auto architectureName = std::string(parameters.GetValue("Architecture").AsString().GetValue());

			if (systemName != "win32")
				throw std::runtime_error("Win32 is the only supported system... so far.");

			// Check if skip platform includes was specified
			bool skipPlatform = false;
			if (state.HasValue("SkipPlatform"))
			{
				skipPlatform = state.GetValue("SkipPlatform").AsBoolean().GetValue();
			}

			// Find the location of the Windows SDK
			auto visualStudioInstallRoot = FindVSInstallRoot(buildState);
			buildState.LogInfo("Using VS Installation: " + visualStudioInstallRoot.ToString());

			// Use the default version
			auto visualCompilerVersion = FindDefaultVCToolsVersion(buildState, visualStudioInstallRoot);
			buildState.LogInfo("Using VC Version: " + visualCompilerVersion);

			// Calculate the final VC tools folder
			auto visualCompilerVersionFolder =
				visualStudioInstallRoot + Path("/VC/Tools/MSVC/") + Path(visualCompilerVersion);

			// Sey the VC tools binary folder
			Path vcToolsBinaryFolder;
			if (architectureName == "x64")
				vcToolsBinaryFolder = visualCompilerVersionFolder + Path("/bin/Hostx64/x64/");
			else if (architectureName == "x86")
				vcToolsBinaryFolder = visualCompilerVersionFolder + Path("/bin/Hostx64/x86/");
			else
				throw std::runtime_error("Unknown architecture.");

			auto clToolPath = vcToolsBinaryFolder + Path("cl.exe");
			auto linkToolPath = vcToolsBinaryFolder + Path("link.exe");
			auto libToolPath = vcToolsBinaryFolder + Path("lib.exe");

			// Save the build properties
			state.EnsureValue("MSVS.InstallRoot").SetValueString(visualStudioInstallRoot.ToString());
			state.EnsureValue("MSVC.Version").SetValueString(visualCompilerVersion);
			state.EnsureValue("MSVC.VCToolsRoot").SetValueString(visualCompilerVersionFolder.ToString());
			state.EnsureValue("MSVC.VCToolsBinaryRoot").SetValueString(vcToolsBinaryFolder.ToString());
			state.EnsureValue("MSVC.LinkToolPath").SetValueString(linkToolPath.ToString());
			state.EnsureValue("MSVC.LibToolPath").SetValueString(libToolPath.ToString());

			// Allow custom overrides for the compiler path
			if (!state.HasValue("MSVC.ClToolPath"))
				state.EnsureValue("MSVC.ClToolPath").SetValueString(clToolPath.ToString());

			// Calculate the windows kits directory
			auto windows10KitPath = Path("C:/Program Files (x86)/Windows Kits/10/");
			auto windows10KitIncludePath = windows10KitPath + Path("/include/");
			auto windows10KitLibPath = windows10KitPath + Path("/Lib/");

			auto windowsKitVersion = FindNewestWindows10KitVersion(buildState, windows10KitIncludePath);

			buildState.LogInfo("Using Windows Kit Version: " + windowsKitVersion);
			auto windows10KitVersionIncludePath = windows10KitIncludePath + Path(windowsKitVersion);
			auto windows10KitVersionLibPath = windows10KitLibPath + Path(windowsKitVersion);

			// Set the include paths
			auto platformIncludePaths = std::vector<Path>();
			if (!skipPlatform)
			{
				platformIncludePaths = std::vector<Path>({
					visualCompilerVersionFolder + Path("/include/"),
					windows10KitVersionIncludePath + Path("/ucrt/"),
					windows10KitVersionIncludePath + Path("/um/"),
					windows10KitVersionIncludePath + Path("/shared/"),
				});
			}

			// Set the include paths
			auto platformLibraryPaths = std::vector<Path>();
			if (!skipPlatform)
			{
				if (architectureName == "x64")
				{
					platformLibraryPaths.push_back(windows10KitVersionLibPath + Path("/ucrt/x64/"));
					platformLibraryPaths.push_back(windows10KitVersionLibPath + Path("/um/x64/"));
					platformLibraryPaths.push_back(visualCompilerVersionFolder + Path("/atlmfc/lib/x64/"));
					platformLibraryPaths.push_back(visualCompilerVersionFolder + Path("/lib/x64/"));
				}
				else if (architectureName == "x86")
				{
					platformLibraryPaths.push_back(windows10KitVersionLibPath + Path("/ucrt/x86/"));
					platformLibraryPaths.push_back(windows10KitVersionLibPath + Path("/um/x86/"));
					platformLibraryPaths.push_back(visualCompilerVersionFolder + Path("/atlmfc/lib/x86/"));
					platformLibraryPaths.push_back(visualCompilerVersionFolder + Path("/lib/x86/"));
				}
			}

			// Set the platform definitions
			auto platformPreprocessorDefinitions = std::vector<std::string>({
				// "_DLL", // Link against the dynamic runtime dll
				// "_MT", // Use multithreaded runtime
			});

			if (architectureName == "x86")
				platformPreprocessorDefinitions.push_back("WIN32");

			// Set the platform libraries
			auto platformLibraries = std::vector<Path>({
				Path("kernel32.lib"),
				Path("user32.lib"),
				Path("gdi32.lib"),
				Path("winspool.lib"),
				Path("comdlg32.lib"),
				Path("advapi32.lib"),
				Path("shell32.lib"),
				Path("ole32.lib"),
				Path("oleaut32.lib"),
				Path("uuid.lib"),
				// Path("odbc32.lib"),
				// Path("odbccp32.lib"),
				// Path("crypt32.lib"),
			});

			// if (_options.Configuration == "debug")
			// {
			// 	// arguments.PlatformPreprocessorDefinitions.push_back("_DEBUG");
			// 	arguments.PlatformLibraries = std::vector<Path>({
			// 		Path("msvcprtd.lib"),
			// 		Path("msvcrtd.lib"),
			// 		Path("ucrtd.lib"),
			// 		Path("vcruntimed.lib"),
			// 	});
			// }
			// else
			// {
			// 	arguments.PlatformLibraries = std::vector<Path>({
			// 		Path("msvcprt.lib"),
			// 		Path("msvcrt.lib"),
			// 		Path("ucrt.lib"),
			// 		Path("vcruntime.lib"),
			// 	});
			// }
	
			state.EnsureValue("PlatformIncludePaths").SetValuePathList(platformIncludePaths);
			state.EnsureValue("PlatformLibraryPaths").SetValuePathList(platformLibraryPaths);
			state.EnsureValue("PlatformLibraries").SetValuePathList(platformLibraries);
			state.EnsureValue("PlatformPreprocessorDefinitions").SetValueStringList(platformPreprocessorDefinitions);
		}

	private:
		Path FindVSInstallRoot(Soup::Build::Utilities::BuildStateWrapper& buildState)
		{
			// Find a copy of visual studio that has the required VisualCompiler
			auto executablePath = Path("C:/Program Files (x86)/Microsoft Visual Studio/Installer/vswhere.exe");
			auto workingDirectory = Path("./");
			auto argumentList = std::vector<std::string>({
				"-latest",
				"-products",
				"*",
				"-requires",
				"Microsoft.VisualStudio.Component.VC.Tools.x86.x64",
				"-property",
				"installationPath",
			});

			// Check if we should include pre-release versions
			bool includePrerelease = true;
			if (includePrerelease)
			{
				argumentList.push_back("-prerelease");
			}

			// Execute the requested target
			auto arguments = CombineArguments(argumentList);
			buildState.LogDebug(executablePath.ToString() + " " + arguments);
			auto process = System::IProcessManager::Current().CreateProcess(
				executablePath,
				arguments,
				workingDirectory);
			process->Start();
			process->WaitForExit();

			auto stdOut = process->GetStandardOutput();
			auto stdErr = process->GetStandardError();
			auto exitCode = process->GetExitCode();

			if (!stdErr.empty())
			{
				buildState.LogError(stdErr);
				throw std::runtime_error("VSWhere failed.");
			}

			if (exitCode != 0)
			{
				// TODO: Return error code
				buildState.LogError("FAILED");
				throw std::runtime_error("VSWhere failed.");
			}

			auto stream = std::istringstream(stdOut);

			// The first line is the path
			auto path = std::string();
			if (!std::getline(stream, path, '\r'))
			{
				buildState.LogError("Failed to parse vswhere output.");
				throw std::runtime_error("Failed to parse vswhere output.");
			}

			return Path(path);
		}

		std::string FindDefaultVCToolsVersion(
			Soup::Build::Utilities::BuildStateWrapper& buildState,
			const Path& visualStudioInstallRoot)
		{
			// Check the default tools version
			auto visualCompilerToolsDefaultVersionFile =
				visualStudioInstallRoot + Path("VC/Auxiliary/Build/Microsoft.VCToolsVersion.default.txt");
			if (!System::IFileSystem::Current().Exists(visualCompilerToolsDefaultVersionFile))
			{
				buildState.LogError("VisualCompilerToolsDefaultVersionFile file does not exist: " + visualCompilerToolsDefaultVersionFile.ToString());
				throw std::runtime_error("VisualCompilerToolsDefaultVersionFile file does not exist.");
			}

			// Read the entire file into a string
			auto file = System::IFileSystem::Current().OpenRead(visualCompilerToolsDefaultVersionFile, false);

			// The first line is the version
			auto version = std::string();
			if (!std::getline(file->GetInStream(), version, '\n'))
			{
				buildState.LogError("Failed to parse version from file.");
				throw std::runtime_error("Failed to parse version from file.");
			}

			return version;
		}

		std::string FindNewestWindows10KitVersion(
			Soup::Build::Utilities::BuildStateWrapper& buildState,
			const Path& windows10KitIncludePath)
		{
			// Check the default tools version
			buildState.LogDebug("FindNewestWindows10KitVersion: " + windows10KitIncludePath.ToString());
			auto currentVersion = SemanticVersion(0, 0, 0);
			for (auto& child : System::IFileSystem::Current().GetDirectoryChildren(windows10KitIncludePath))
			{
				auto name = child.Path.GetFileName();
				buildState.LogDebug("CheckFile: " + std::string(name));
				auto platformVersion = name.substr(0, 3);
				if (platformVersion == "10.")
				{
					// Parse the version string
					auto version = SemanticVersion::Parse(name.substr(3));
					if (version > currentVersion)
						currentVersion = version;
				}
				else
				{
					buildState.LogWarning("Unexpected Kit Version: " + std::string(name));
				}
			}

			if (currentVersion == SemanticVersion(0, 0, 0))
				throw std::runtime_error("Could not find a minimum Windows 10 Kit Version");

			// The first line is the version
			auto version = "10." + currentVersion.ToString();
			return version;
		}

		static std::string CombineArguments(const std::vector<std::string>& args)
		{
			auto argumentString = std::stringstream();
			bool isFirst = true;
			for (auto& arg : args)
			{
				if (!isFirst)
					argumentString << " ";

				argumentString << arg;
				isFirst = false;
			}

			return argumentString.str();
		}

	private:
		Soup::Build::Utilities::ReadOnlyStringList _runBeforeList;
		Soup::Build::Utilities::ReadOnlyStringList _runAfterList;
	};
}
