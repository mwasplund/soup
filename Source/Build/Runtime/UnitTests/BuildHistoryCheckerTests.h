// <copyright file="BuildHistoryCheckerTests.h" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

#pragma once
#include "Helpers.h"

namespace Soup::Build::Runtime::UnitTests
{
	class BuildHistoryCheckerTests
	{
	public:
		// [[Fact]]
		void IsOutdated_ZeroInput()
		{
			// Register the test listener
			auto testListener = std::make_shared<TestTraceListener>();
			auto scopedTraceListener = ScopedTraceListenerRegister(testListener);

			// Initialize the file system state
			auto fileSystemState = std::make_shared<FileSystemState>(
				2,
				std::unordered_map<FileId, Path>({
					{ 1, Path("C:/Root/Output.bin") },
				}),
				std::unordered_map<FileId, std::optional<time_t>>({
					{ 1, std::nullopt },
				}));

			// Setup the input parameters
			auto targetFiles = std::vector<FileId>({
				1,
			});
			auto inputFiles = std::vector<FileId>({});

			// Perform the check
			auto uut = BuildHistoryChecker(fileSystemState);
			bool result = uut.IsOutdated(targetFiles, inputFiles);

			// Verify the results
			Assert::IsFalse(result, "Verify the result is false.");

			// Verify expected logs
			Assert::AreEqual(
				std::vector<std::string>({}),
				testListener->GetMessages(),
				"Verify log messages match expected.");
		}

		// [[Fact]]
		void IsOutdated_SingleInput_UnknownTarget()
		{
			// Register the test listener
			auto testListener = std::make_shared<TestTraceListener>();
			auto scopedTraceListener = ScopedTraceListenerRegister(testListener);

			// Register the test file system
			auto fileSystem = std::make_shared<MockFileSystem>();
			auto scopedFileSystem = ScopedFileSystemRegister(fileSystem);

			// Initialize the file system state
			auto fileSystemState = std::make_shared<FileSystemState>(
				3,
				std::unordered_map<FileId, Path>({
					{ 1, Path("C:/Root/Output.bin") },
					{ 2, Path("C:/Root/Input.cpp") },
				}),
				std::unordered_map<FileId, std::optional<time_t>>({
					{ 2, std::nullopt },
				}));

			// Setup the input parameters
			auto targetFiles = std::vector<FileId>({
				1,
			});
			auto inputFiles = std::vector<FileId>({
				2,
			});

			// Perform the check
			auto uut = BuildHistoryChecker(fileSystemState);
			bool result = uut.IsOutdated(targetFiles, inputFiles);

			// Verify the results
			Assert::IsTrue(result, "Verify the result is true.");

			// Verify expected logs
			Assert::AreEqual(
				std::vector<std::string>({
					"INFO: Output target does not exist: C:/Root/Output.bin",
				}),
				testListener->GetMessages(),
				"Verify log messages match expected.");

			// Verify expected file system requests
			Assert::AreEqual(
				std::vector<std::string>({
					"Exists: C:/Root/Output.bin",
				}),
				fileSystem->GetRequests(),
				"Verify file system requests match expected.");
		}

		// [[Fact]]
		void IsOutdated_SingleInput_DeletedTarget()
		{
			// Register the test listener
			auto testListener = std::make_shared<TestTraceListener>();
			auto scopedTraceListener = ScopedTraceListenerRegister(testListener);

			// Initialize the file system state
			auto fileSystemState = std::make_shared<FileSystemState>(
				3,
				std::unordered_map<FileId, Path>({
					{ 1, Path("C:/Root/Output.bin") },
					{ 2, Path("C:/Root/Input.cpp") },
				}),
				std::unordered_map<FileId, std::optional<time_t>>({
					{ 1, std::nullopt },
					{ 2, std::nullopt },
				}));

			// Setup the input parameters
			auto targetFiles = std::vector<FileId>({
				1,
			});
			auto inputFiles = std::vector<FileId>({
				2,
			});

			// Perform the check
			auto uut = BuildHistoryChecker(fileSystemState);
			bool result = uut.IsOutdated(targetFiles, inputFiles);

			// Verify the results
			Assert::IsTrue(result, "Verify the result is true.");

			// Verify expected logs
			Assert::AreEqual(
				std::vector<std::string>({
					"INFO: Output target does not exist: C:/Root/Output.bin",
				}),
				testListener->GetMessages(),
				"Verify log messages match expected.");
		}

		// [[Fact]]
		void IsOutdated_SingleInput_TargetExists_UnknownInputFile()
		{
			// Register the test listener
			auto testListener = std::make_shared<TestTraceListener>();
			auto scopedTraceListener = ScopedTraceListenerRegister(testListener);

			// Register the test file system
			auto fileSystem = std::make_shared<MockFileSystem>();
			auto scopedFileSystem = ScopedFileSystemRegister(fileSystem);

			// Create the file state
			auto outputTime = CreateDateTime(2015, 5, 22, 9, 12);

			// Initialize the file system state
			auto fileSystemState = std::make_shared<FileSystemState>(
				3,
				std::unordered_map<FileId, Path>({
					{ 1, Path("C:/Root/Output.bin") },
					{ 2, Path("C:/Root/Input.cpp") },
				}),
				std::unordered_map<FileId, std::optional<time_t>>({
					{ 1, outputTime },
				}));

			// Setup the input parameters
			auto targetFiles = std::vector<FileId>({
				1,
			});
			auto inputFiles = std::vector<FileId>({
				2,
			});

			// Perform the check
			auto uut = BuildHistoryChecker(fileSystemState);
			bool result = uut.IsOutdated(targetFiles, inputFiles);
			Assert::IsTrue(result, "Verify the result is true.");

			// Verify expected logs
			Assert::AreEqual(
				std::vector<std::string>({
					"INFO: Input Missing [C:/Root/Input.cpp]",
				}),
				testListener->GetMessages(),
				"Verify log messages match expected.");

			// Verify expected file system requests
			Assert::AreEqual(
				std::vector<std::string>({
					"Exists: C:/Root/Input.cpp",
				}),
				fileSystem->GetRequests(),
				"Verify file system requests match expected.");
		}

		// [[Fact]]
		void IsOutdated_SingleInput_TargetExists_DeletedInputFile()
		{
			// Register the test listener
			auto testListener = std::make_shared<TestTraceListener>();
			auto scopedTraceListener = ScopedTraceListenerRegister(testListener);

			// Register the test file system
			auto fileSystem = std::make_shared<MockFileSystem>();
			auto scopedFileSystem = ScopedFileSystemRegister(fileSystem);

			// Create the file state
			auto outputTime = CreateDateTime(2015, 5, 22, 9, 12);

			// Initialize the file system state
			auto fileSystemState = std::make_shared<FileSystemState>(
				3,
				std::unordered_map<FileId, Path>({
					{ 1, Path("C:/Root/Output.bin") },
					{ 2, Path("C:/Root/Input.cpp") },
				}),
				std::unordered_map<FileId, std::optional<time_t>>({
					{ 1, outputTime },
					{ 2, std::nullopt },
				}));

			// Setup the input parameters
			auto targetFiles = std::vector<FileId>({
				1,
			});
			auto inputFiles = std::vector<FileId>({
				2,
			});

			// Perform the check
			auto uut = BuildHistoryChecker(fileSystemState);
			bool result = uut.IsOutdated(targetFiles, inputFiles);
			Assert::IsTrue(result, "Verify the result is true.");

			// Verify expected logs
			Assert::AreEqual(
				std::vector<std::string>({
					"INFO: Input Missing [C:/Root/Input.cpp]",
				}),
				testListener->GetMessages(),
				"Verify log messages match expected.");

			// Verify expected file system requests
			Assert::AreEqual(
				std::vector<std::string>({}),
				fileSystem->GetRequests(),
				"Verify file system requests match expected.");
		}

		// [[Fact]]
		void IsOutdated_SingleInput_TargetExists_Outdated()
		{
			// Register the test listener
			auto testListener = std::make_shared<TestTraceListener>();
			auto scopedTraceListener = ScopedTraceListenerRegister(testListener);

			// Create the file state
			auto outputTime = CreateDateTime(2015, 5, 22, 9, 12);
			auto inputTime = CreateDateTime(2015, 5, 22, 9, 13);

			// Initialize the file system state
			auto fileSystemState = std::make_shared<FileSystemState>(
				3,
				std::unordered_map<FileId, Path>({
					{ 1, Path("C:/Root/Output.bin") },
					{ 2, Path("C:/Root/Input.cpp") },
				}),
				std::unordered_map<FileId, std::optional<time_t>>({
					{ 1, outputTime },
					{ 2, inputTime },
				}));

			// Setup the input parameters
			auto targetFiles = std::vector<FileId>({
				1,
			});
			auto inputFiles = std::vector<FileId>({
				2,
			});

			// Perform the check
			auto uut = BuildHistoryChecker(fileSystemState);
			bool result = uut.IsOutdated(targetFiles, inputFiles);

			// Verify the results
			Assert::IsTrue(result, "Verify the result is true.");

			// Verify expected logs
			Assert::AreEqual(
				std::vector<std::string>({
					"INFO: Input altered after target [C:/Root/Input.cpp] -> [C:/Root/Output.bin]",
				}),
				testListener->GetMessages(),
				"Verify log messages match expected.");
		}

		// [[Fact]]
		void IsOutdated_SingleInput_TargetExists_UpToDate()
		{
			// Register the test listener
			auto testListener = std::make_shared<TestTraceListener>();
			auto scopedTraceListener = ScopedTraceListenerRegister(testListener);

			// Create the file state
			auto outputTime = CreateDateTime(2015, 5, 22, 9, 12);
			auto inputTime = CreateDateTime(2015, 5, 22, 9, 11);

			// Initialize the file system state
			auto fileSystemState = std::make_shared<FileSystemState>(
				3,
				std::unordered_map<FileId, Path>({
					{ 1, Path("C:/Root/Output.bin") },
					{ 2, Path("C:/Root/Input.cpp") },
				}),
				std::unordered_map<FileId, std::optional<time_t>>({
					{ 1, outputTime },
					{ 2, inputTime },
				}));

			// Setup the input parameters
			auto targetFiles = std::vector<FileId>({
				1,
			});
			auto inputFiles = std::vector<FileId>({
				2,
			});

			// Perform the check
			auto uut = BuildHistoryChecker(fileSystemState);
			bool result = uut.IsOutdated(targetFiles, inputFiles);

			// Verify the results
			Assert::IsFalse(result, "Verify the result is false.");

			// Verify expected logs
			Assert::AreEqual(
				std::vector<std::string>({}),
				testListener->GetMessages(),
				"Verify log messages match expected.");
		}

		// [[Fact]]
		void IsOutdated_MultipleInputs_RelativeAndAbsolute()
		{
			// Register the test listener
			auto testListener = std::make_shared<TestTraceListener>();
			auto scopedTraceListener = ScopedTraceListenerRegister(testListener);

			// Create the file state
			auto outputTime = CreateDateTime(2015, 5, 22, 9, 12);
			auto inputTime = CreateDateTime(2015, 5, 22, 9, 11);

			// Initialize the file system state
			auto fileSystemState = std::make_shared<FileSystemState>(
				4,
				std::unordered_map<FileId, Path>({
					{ 1, Path("C:/Root/Output.bin") },
					{ 2, Path("C:/Root/Input.cpp") },
					{ 3, Path("C:/Input.h") },
				}),
				std::unordered_map<FileId, std::optional<time_t>>({
					{ 1, outputTime },
					{ 2, inputTime },
					{ 3, inputTime },
				}));

			// Setup the input parameters
			auto targetFiles = std::vector<FileId>({
				1,
			});
			auto inputFiles = std::vector<FileId>({
				2,
				3,
			});

			// Perform the check
			auto uut = BuildHistoryChecker(fileSystemState);
			bool result = uut.IsOutdated(targetFiles, inputFiles);

			// Verify the results
			Assert::IsFalse(result, "Verify the result is false.");

			// Verify expected logs
			Assert::AreEqual(
				std::vector<std::string>({}),
				testListener->GetMessages(),
				"Verify log messages match expected.");
		}
	};
}
