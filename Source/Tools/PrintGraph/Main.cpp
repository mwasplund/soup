#include <iostream>
#include <sstream>
#include <vector>

import Opal;
import Soup.Build.Runtime;

void PrintUsage()
{
	std::cout << "printgraph [path]" << std::endl;
}

std::string ToString(const std::vector<uint32_t>& valueList)
{
	auto builder = std::stringstream();
	builder << "[";
	bool isFirst = true;
	for (auto value : valueList)
	{
		if (!isFirst)
		{
			builder << ", ";
		}

		builder << value;
		isFirst = false;
	}

	builder << "]";
	return builder.str();
}

void PrintOperations(Soup::Build::Runtime::OperationGraph& graph)
{
	for (auto operation : graph.GetOperations())
	{
		const auto& operationInfo = operation.second;
		std::cout << "Operation: " << operationInfo.Id << std::endl;
		std::cout << "\tTitle: " << operationInfo.Title << std::endl;
		std::cout << "\tCommand-WorkingDirectory: " << operationInfo.Command.WorkingDirectory.ToString() << std::endl;
		std::cout << "\tCommand-Executable: " << operationInfo.Command.Executable.ToString() << std::endl;
		std::cout << "\tCommand-Arguments: " << operationInfo.Command.Arguments << std::endl;
		std::cout << "\tDeclaredInput: " << ToString(operationInfo.DeclaredInput) << std::endl;
		std::cout << "\tDeclaredOutput: " << ToString(operationInfo.DeclaredOutput) << std::endl;
		std::cout << "\tChildren: " << ToString(operationInfo.Children) << std::endl;
		std::cout << "\tDependencyCount: " << operationInfo.DependencyCount << std::endl;
		std::cout << "\tWasSuccessfulRun: " << operationInfo.WasSuccessfulRun << std::endl;
		std::cout << "\tObservedInput: " << ToString(operationInfo.ObservedInput) << std::endl;
		std::cout << "\tObservedOutput: " << ToString(operationInfo.ObservedOutput) << std::endl;
	}
}

void PrintGraph(
	const std::string& indent,
	const std::vector<Soup::Build::Runtime::OperationId>& operationIds,
	Soup::Build::Runtime::OperationGraph& graph)
{
	auto childIndent = indent + "  ";
	for (auto operationId : operationIds)
	{
		const auto& operationInfo = graph.GetOperationInfo(operationId);
		std::cout << indent << operationInfo.Title << std::endl;
		PrintGraph(childIndent, operationInfo.Children, graph);
	}
}

void PrintGraph(Soup::Build::Runtime::OperationGraph& graph)
{
	std::cout << "Graph:" << std::endl;
	PrintGraph("", graph.GetRootOperationIds(), graph);
}

void LoadAndPrintGraph(const Opal::Path& operationGraphFile)
{
	if (!Opal::System::IFileSystem::Current().Exists(operationGraphFile))
	{
		throw std::runtime_error("Operation graph file does not exist");
	}

	// Open the file to read from
	auto file = Opal::System::IFileSystem::Current().OpenRead(operationGraphFile, true);

	// Read the contents of the build state file
	auto graph = Soup::Build::Runtime::OperationGraphReader::Deserialize(file->GetInStream());

	PrintOperations(graph);
	PrintGraph(graph);
}

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		PrintUsage();
		return 1;
	}

	try
	{
		Opal::System::IFileSystem::Register(std::make_shared<Opal::System::STLFileSystem>());

		auto operationGraphFile = Opal::Path(argv[1]);
		LoadAndPrintGraph(operationGraphFile);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return 2;
	}

	return 0;
}