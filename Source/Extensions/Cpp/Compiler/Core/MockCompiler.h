﻿// <copyright file="MockCompiler.h" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

#pragma once

namespace Soup::Cpp::Compiler::Mock
{
	/// <summary>
	/// A mock compiler interface implementation
	/// TODO: Move into test projects
	/// </summary>
	export class Compiler : public ICompiler
	{
	public:
		/// <summary>
		/// Initializes a new instance of the <see cref='MockCompiler'/> class.
		/// </summary>
		Compiler() :
			_compileRequests(),
			_linkRequests()
		{
		}

		/// <summary>
		/// Get the compile requests
		/// </summary>
		const std::vector<SharedCompileArguments>& GetCompileRequests() const
		{
			return _compileRequests;
		}

		/// <summary>
		/// Get the link requests
		/// </summary>
		const std::vector<LinkArguments>& GetLinkRequests() const
		{
			return _linkRequests;
		}

		/// <summary>
		/// Gets the unique name for the compiler
		/// </summary>
		std::string_view GetName() const override final
		{
			return "MockCompiler";
		}

		/// <summary>
		/// Gets the object file extension for the compiler
		/// </summary>
		std::string_view GetObjectFileExtension() const override final
		{
			return "mock.obj";
		}

		/// <summary>
		/// Gets the module file extension for the compiler
		/// </summary>
		std::string_view GetModuleFileExtension() const override final
		{
			return "mock.bmi";
		}

		/// <summary>
		/// Gets the static library file extension for the compiler
		/// TODO: This is platform specific
		/// </summary>
		std::string_view GetStaticLibraryFileExtension() const override final
		{
			return "mock.lib";
		}

		/// <summary>
		/// Gets the dynamic library file extension for the compiler
		/// TODO: This is platform specific
		/// </summary>
		std::string_view GetDynamicLibraryFileExtension() const override final
		{
			return "mock.dll";
		}

		/// <summary>
		/// Compile
		/// </summary>
		std::vector<Build::Utilities::BuildOperation> CreateCompileOperations(
			const SharedCompileArguments& arguments) const override final
		{
			_compileRequests.push_back(arguments);

			auto result = std::vector<Build::Utilities::BuildOperation>();
			if (arguments.InterfaceUnit.has_value())
			{
				result.push_back(
					Build::Utilities::BuildOperation(
						"MockCompileModule: " + std::to_string(_compileRequests.size()),
						Path("MockWorkingDirectory"),
						Path("MockCompiler.exe"),
						"Arguments",
						std::vector<Path>({
							Path("InputFile.in"),
						}),
						std::vector<Path>({
							Path("OutputFile.out"),
						})));
			}

			for (auto& fileArguments : arguments.ImplementationUnits)
			{
				result.push_back(
					Build::Utilities::BuildOperation(
						"MockCompile: " + std::to_string(_compileRequests.size()),
						Path("MockWorkingDirectory"),
						Path("MockCompiler.exe"),
						"Arguments",
						std::vector<Path>({
							fileArguments.SourceFile,
						}),
						std::vector<Path>({
							fileArguments.TargetFile,
						})));
			}

			return result;
		}

		/// <summary>
		/// Link
		/// </summary>
		Build::Utilities::BuildOperation CreateLinkOperation(
			const LinkArguments& arguments) const override final
		{
			_linkRequests.push_back(arguments);
			return Build::Utilities::BuildOperation(
				"MockLink: " + std::to_string(_linkRequests.size()),
				Path("MockWorkingDirectory"),
				Path("MockLinker.exe"),
				"Arguments",
				std::vector<Path>({
					Path("InputFile.in"),
				}),
				std::vector<Path>({
					Path("OutputFile.out"),
				}));
		}

	private:
		mutable std::vector<SharedCompileArguments> _compileRequests;
		mutable std::vector<LinkArguments> _linkRequests;
	};
}
