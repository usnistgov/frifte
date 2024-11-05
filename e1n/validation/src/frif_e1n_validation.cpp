/*
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 */

#include <algorithm>
#include <fstream>
#include <iostream>
#include <random>
#include <numeric>
#include <string_view>
#include <sstream>
#include <stdexcept>
#include <thread>
#include <tuple>

#include <getopt.h>
#include <sys/wait.h>
#include <unistd.h>

#include <frif/e1n.h>
#include <frif/util.h>

#include "frif_e1n_data.h"
#include "frif_e1n_validation.h"

void
FRIF::Evaluations::Exemplar1N::Validation::dispatchCreateTemplates(
    const Arguments &args)
{
	if (args.operation &&
	    !(*args.operation == Operation::CreateTemplates))
		throw std::runtime_error{"Unsupported operation was sent to "
		    "dispatchCreateTemplates()"};
	if (!args.templateType.has_value())
		throw std::runtime_error{"Template type not set"};

	std::vector<Data::Dataset> setList{};
	switch (*args.templateType) {
	case TemplateType::Probe:
		for (const auto &name : Data::ProbeNames) {
			const std::string csvName{name + ".csv"};
			setList.emplace_back(Data::readCSV(args.imageDir /
			    csvName));
		}
		break;
	case TemplateType::Reference:
		for (const auto &name : Data::ReferenceNames) {
			const std::string csvName{name + ".csv"};
			setList.emplace_back(Data::readCSV(args.imageDir /
			    csvName));
		}
		break;
	}

	for (const auto &dataset : setList) {
		makeCreateTemplatesDirs(args, dataset);

		const auto indices = Util::randomizeIndices(
		    std::get<std::vector<Data::ImageSet>>(
		    dataset).size(), args.randomSeed);

		forkOperation(args, dataset, indices,
		    &runPartialCreateTemplates);
	}
}

void
FRIF::Evaluations::Exemplar1N::Validation::dispatchCreateDatabases(
    const Arguments &args)
{
	if (args.operation &&
	    !(*args.operation == Operation::CreateDatabases))
		throw std::runtime_error{"Unsupported operation was sent to "
		    "dispatchCreateDatabases()"};

	auto iv = instantiateImplementation(args);
	std::shared_ptr<ExtractionInterface> impl{};
	if (!std::get_if<std::shared_ptr<ExtractionInterface>>(&iv))
		throw std::runtime_error{"Failure to obtain implementation of "
		    "ExtractionInterface"};
	impl = std::get<std::shared_ptr<ExtractionInterface>>(iv);
	if (!impl)
		throw std::runtime_error{"ExtractionInterface implementation "
		    "was null"};

	for (const auto &datasetName : Data::ReferenceNames)
		singleCreateDatabase(impl, datasetName, args);
}

void
FRIF::Evaluations::Exemplar1N::Validation::dispatchSearch(
    const Arguments &args)
{
	if (args.operation &&
	    !((*args.operation == Operation::SearchSubject) ||
	      (*args.operation == Operation::SearchSubjectPosition)))
		throw std::runtime_error{"Unsupported operation was sent to "
		    "dispatchSearch()"};

	std::vector<std::pair<std::string, std::string>> searchSets{};
	if (*args.operation == Operation::SearchSubject)
		searchSets.assign(Data::SubjectSearchSets.begin(),
		     Data::SubjectSearchSets.end());
	else
		searchSets.assign(Data::SubjectPositionSearchSets.begin(),
		     Data::SubjectPositionSearchSets.end());

	for (const auto &[probeName, dbName] : searchSets) {
		const std::string csvName{probeName + ".csv"};
		const auto probes = Data::readCSV(args.imageDir / csvName);
		const auto indices = Util::randomizeIndices(
		    std::get<std::vector<Data::ImageSet>>(probes).size(),
		    args.randomSeed);

		/* FIXME: Override dbDir for each database */
		Arguments argsOverride{args};
		argsOverride.dbDir = *args.dbDir / dbName;

		forkOperation(argsOverride, probes, indices, &runPartialSearch);
	}
}

std::tuple<std::chrono::steady_clock::time_point,
    std::chrono::steady_clock::time_point, FRIF::ReturnStatus,
    std::optional<FRIF::SearchSubjectResult>>
FRIF::Evaluations::Exemplar1N::Validation::executeSingleSearchSubject(
    std::shared_ptr<SearchInterface> impl,
    const std::vector<std::byte> &probe,
    const uint16_t maxCandidates)
{
	std::chrono::steady_clock::time_point start{}, stop{};
	try {
		start = std::chrono::steady_clock::now();
		const auto ret = impl->searchSubject(probe,
		    maxCandidates);
		stop = std::chrono::steady_clock::now();

		return {start, stop, std::get<ReturnStatus>(ret),
		    std::get<std::optional<FRIF::SearchSubjectResult>>(ret)};
	} catch (const std::exception &e) {
		throw std::runtime_error{"Exception from searchSubject(max = " +
		    std::to_string(maxCandidates) + "): " + e.what()};
	} catch (...) {
		throw std::runtime_error{"Unknown exception from searchSubject("
		    "max = " + std::to_string(maxCandidates) + ")"};
	}
}

std::tuple<std::chrono::steady_clock::time_point,
    std::chrono::steady_clock::time_point, FRIF::ReturnStatus,
    std::optional<FRIF::SearchSubjectPositionResult>>
FRIF::Evaluations::Exemplar1N::Validation::executeSingleSearchSubjectPosition(
    std::shared_ptr<SearchInterface> impl,
    const std::vector<std::byte> &probe,
    const uint16_t maxCandidates)
{
	std::chrono::steady_clock::time_point start{}, stop{};
	try {
		start = std::chrono::steady_clock::now();
		const auto ret = impl->searchSubjectPosition(probe,
		    maxCandidates);
		stop = std::chrono::steady_clock::now();

		return {start, stop, std::get<ReturnStatus>(ret),
		    std::get<std::optional<FRIF::SearchSubjectPositionResult>>(
		    ret)};
	} catch (const std::exception &e) {
		throw std::runtime_error{"Exception from searchSubjectPosition("
		    "max = " +  std::to_string(maxCandidates) + "): " +
		    e.what()};
	} catch (...) {
		throw std::runtime_error{"Unknown exception from searchSubject"
		    "Position(max = " + std::to_string(maxCandidates) + ")"};
	}
}

void
FRIF::Evaluations::Exemplar1N::Validation::forkOperation(
    const Arguments &args,
    const Data::Dataset &dataset,
    const std::vector<uint64_t> &indices,
    const std::function<void(InterfaceVariant, const Data::Dataset&,
        const std::vector<uint64_t>&, const Arguments&)> &fn)
{
	auto impl = instantiateImplementation(args);

	if (args.numProcs == 1) {
		fn(impl, dataset, indices, args);
		return;
	}

	const auto splits = Util::splitSet(indices, args.numProcs);
	for (const auto &split : splits) {
		const auto pid = fork();
		switch (pid) {
		case 0:		/* Child */
			try {
				fn(impl, dataset, split, args);
			} catch (const std::exception &e) {
				std::cerr << e.what() << '\n';
				std::exit(EXIT_FAILURE);
			} catch (...) {
				std::cerr << "Caught unknown exception\n";
				std::exit(EXIT_FAILURE);
			}

			std::exit(EXIT_SUCCESS);
		case -1: 	/* Error */
			throw std::runtime_error{"Error during fork()"};
		default:	/* Parent */
			break;
		}
	}

	/* Parent only */
	waitForExit(args.numProcs);
}

int
FRIF::Evaluations::Exemplar1N::Validation::dispatchOperation(
    const Arguments &args)
{
	int rv{EXIT_FAILURE};

	switch (args.operation.value_or(Operation::Usage)) {
	case Operation::CreateDatabases:
		try {
			dispatchCreateDatabases(args);
			rv = EXIT_SUCCESS;
		} catch (const std::exception &e) {
			std::cerr << "dispatchCreateDatabases(): " <<
			    e.what() << '\n';
		} catch (...) {
			std::cerr << "dispatchCreateDatabases(): Non-standard "
			    "exception\n";
		}
		break;
	case Operation::CreateTemplates:
		try {
			dispatchCreateTemplates(args);
			rv = EXIT_SUCCESS;
		} catch (const std::exception &e) {
			std::cerr << "dispatchCreateTemplates(): " <<
			    e.what() << '\n';
		} catch (...) {
			std::cerr << "dispatchCreateTemplates(): Non-standard "
			    "exception\n";
		}
		break;
	case Operation::SearchSubject:
		[[ fallthrough ]];
	case Operation::SearchSubjectPosition:
		try {
			dispatchSearch(args);
			rv = EXIT_SUCCESS;
		} catch (const std::exception &e) {
			std::cerr << "dispatchSearch(): " <<
			    e.what() << '\n';
		} catch (...) {
			std::cerr << "dispatchSearch(): Non-standard "
			    "exception\n";
		}
		break;
	case Operation::Usage:
		std::cout << getUsageString(args.executableName) << '\n';
		rv = EXIT_SUCCESS;
		break;
	case Operation::IdentifyExtraction:
		try {
			std::cout <<
			    getExtractionInterfaceIdentificationString() <<
			    '\n' <<
			    getExtractionInterfaceCompatibilityString() << '\n';
			rv = EXIT_SUCCESS;
		} catch (const std::exception &e) {
			std::cerr << "ExtractionInterface::"
			    "getProductIdentifier(): " << e.what() << '\n';
		} catch (...) {
			std::cerr << "ExtractionInterface::"
			    "getProductIdentifier(): Non-standard exception\n";
		}
		break;
	case Operation::IdentifyLibrary:
		try {
			std::cout << getLibraryIdentifierString() << '\n';
			rv = EXIT_SUCCESS;
		} catch (const std::exception &e) {
			std::cerr << "Evaluations::getProductIdentifier(): " <<
			    e.what() << '\n';
		} catch (...) {
			std::cerr << "Evaluations::getProductIdentifier(): "
			    "Non-standard exception\n";
		}
		break;
	case Operation::IdentifySearch:
		try {
			std::cout << getSearchInterfaceIdentificationString() <<
			    '\n' << getSearchInterfaceCompatibilityString() <<
			    '\n';
			rv = EXIT_SUCCESS;
		} catch (const std::exception &e) {
			std::cerr << "SearchInterface::"
			    "getProductIdentifier(): " << e.what() << '\n';
		} catch (...) {
			std::cerr << "SearchInterface::getProductIdentifier(): "
			    "Non-standard exception\n";
		}
		break;
	}

	return (rv);
}

std::string
FRIF::Evaluations::Exemplar1N::Validation::
    getExtractionInterfaceCompatibilityString()
{
	const auto compat = ExtractionInterface::getCompatibility();
	const auto thisVersion = FRIF::Evaluations::Exemplar1N::
	    getLibraryIdentifier().versionNumber;

	std::stringstream ss{};

	ss << "Exemplar Feature Extraction Algorithm Supports Probe Template "
	    "Introspection? = " << std::boolalpha <<
	    compat.supportsProbeTemplateIntrospection << '\n' <<
	    "Exemplar Feature Extraction Algorithm Supports Reference Template "
	    "Introspection? = " << std::boolalpha <<
	    compat.supportsReferenceTemplateIntrospection << '\n' <<
	    "Exemplar Feature Extraction Algorithm Supported Probe Template "
	    "Versions = ";
	if (!compat.probeTemplateVersions.contains(thisVersion))
		ss << "0x" << std::setw(4) << std::hex << std::setfill('0') <<
		    std::uppercase << thisVersion << ',';
	for (const auto &version : compat.probeTemplateVersions)
		ss << "0x" << std::setw(4) << std::hex << std::setfill('0') <<
		    std::uppercase << version << ',';

	/* Remove trailing comma */
	ss.seekp(-1, std::ios_base::end);

	ss << "Exemplar Feature Extraction Algorithm Supported Reference "
	    "Template Versions = ";
	if (!compat.referenceTemplateVersions.contains(thisVersion))
		ss << "0x" << std::setw(4) << std::hex << std::setfill('0') <<
		    std::uppercase << thisVersion << ',';
	for (const auto &version : compat.referenceTemplateVersions)
		ss << "0x" << std::setw(4) << std::hex << std::setfill('0') <<
		    std::uppercase << version << ',';

	/* Remove trailing comma */
	ss.seekp(-1, std::ios_base::end);

	ss << "\nExemplar Feature Extraction Algorithm Supported Reference "
	    "Database Versions = ";
	if (!compat.referenceDatabaseVersions.contains(thisVersion))
		ss << "0x" << std::setw(4) << std::hex << std::setfill('0') <<
		    std::uppercase << thisVersion << ',';
	for (const auto &version : compat.referenceDatabaseVersions)
		ss << "0x" << std::setw(4) << std::hex << std::setfill('0') <<
		    std::uppercase << version << ',';

	/* Remove trailing comma */
	ss.seekp(-1, std::ios_base::end);
	ss << '\0';

	return (ss.str());
}

std::string
FRIF::Evaluations::Exemplar1N::Validation::
    getExtractionInterfaceIdentificationString()
{
	const auto id = ExtractionInterface::getProductIdentifier();

	std::stringstream ss{};

	if (id) {
		ss << "Exemplar Feature Extraction Algorithm Marketing "
		    "Identifier =";
		if (id->marketing)
			ss << ' ' << *(id->marketing);
		ss << '\n';

		if (id->cbeff) {
			ss << "Exemplar Feature Extraction Algorithm CBEFF "
			    "Owner = 0x" << std::setw(4) << std::hex <<
			    std::setfill('0') <<
			    std::uppercase << id->cbeff->owner << '\n';

			ss << "Exemplar Feature Extraction Algorithm "
			    "CBEFF Identifier =";
			if (id->cbeff->algorithm)
				ss << " 0x" << *(id->cbeff->algorithm);
		} else {
			ss << "Exemplar Feature Extraction Algorithm CBEFF "
			    "Owner =\n"
			    "Exemplar Feature Extraction Algorithm CBEFF "
			    "Identifier =";
		}
	}

	return (ss.str());
}

std::string
FRIF::Evaluations::Exemplar1N::Validation::getLibraryIdentifierString()
{
	const auto id = FRIF::Evaluations::Exemplar1N::getLibraryIdentifier();

	std::stringstream ss{};
	ss << "Identifier = " << id.identifier << '\n' <<
	    "Version = 0x" << std::setw(4) << std::hex << std::setfill('0') <<
	    std::uppercase << id.versionNumber;

	return (ss.str());
}

std::string
FRIF::Evaluations::Exemplar1N::Validation::
    getSearchInterfaceCompatibilityString()
{
	const auto compat = SearchInterface::getCompatibility();
	const auto thisVersion = FRIF::Evaluations::Exemplar1N::
	    getLibraryIdentifier().versionNumber;

	std::stringstream ss{};

	ss << "Search Algorithm Supports Correspondence? = " << std::boolalpha <<
	    compat.supportsCorrespondence << '\n' <<
	    "Search Algorithm Supported Probe Template Versions = ";
	if (!compat.probeTemplateVersions.contains(thisVersion))
		ss << "0x" << std::setw(4) << std::hex << std::setfill('0') <<
		    std::uppercase << thisVersion << ',';
	for (const auto &version : compat.probeTemplateVersions)
		ss << "0x" << std::setw(4) << std::hex << std::setfill('0') <<
		    std::uppercase << version << ',';

	/* Remove trailing comma */
	ss.seekp(-1, std::ios_base::end);

	ss << "\nSearch Algorithm Supported Reference Database Versions = ";
	if (!compat.referenceDatabaseVersions.contains(thisVersion))
		ss << "0x" << std::setw(4) << std::hex << std::setfill('0') <<
		    std::uppercase << thisVersion << ',';
	for (const auto &version : compat.referenceDatabaseVersions)
		ss << "0x" << std::setw(4) << std::hex << std::setfill('0') <<
		    std::uppercase << version << ',';

	/* Remove trailing comma */
	ss.seekp(-1, std::ios_base::end);
	ss << '\0';

	return (ss.str());
}

std::string
FRIF::Evaluations::Exemplar1N::Validation::
    getSearchInterfaceIdentificationString()
{
	const auto id = SearchInterface::getProductIdentifier();

	std::stringstream ss{};

	if (id) {
		ss << "Search Algorithm Marketing Identifier =";
		if (id->marketing)
			ss << ' ' << *(id->marketing);
		ss << '\n';

		ss << "Search Algorithm CBEFF Owner =";
		if (id->cbeff)
			ss << " 0x" << std::setw(4) << std::hex <<
			    std::setfill('0') <<
			    std::uppercase << id->cbeff->owner;
		ss << '\n';

		ss << "Search Algorithm CBEFF Identifier =";
		if (id->cbeff->algorithm)
			ss << " 0x" << std::setw(4) << std::hex <<
			    std::setfill('0') <<
			    std::uppercase << *(id->cbeff->algorithm);
	} else {
		ss << "Search Algorithm Marketing Identifier =\n"
		    "Search Algorithm CBEFF Owner =\n"
		    "Search Algorithm CBEFF Identifier =";
	}

	return (ss.str());
}

std::string
FRIF::Evaluations::Exemplar1N::Validation::getUsageString(
    const std::string &name)
{
	static const std::string usagePrompt{"Usage: "};
	const std::string prefix(usagePrompt.length(), ' ');

	std::string s{usagePrompt + name + " ...\n"};

	s += prefix + "# Identify\n" +
	    prefix + "-i l # library\n" +
	    prefix + "-i e # ExtractionInterface\n" +
	    prefix + "-i s # SearchInterface";

	s += "\n\n";

	s += prefix + "# Create Probe Templates + Extract Data\n" +
	    prefix + "-e p -z <config_dir> [-r <random_seed> -o <output_dir> "
	    "-I <image_dir>\n" + prefix + "-j <num_processes>]\n";
	s += prefix + "# Create Reference Templates + Extract Data\n" +
	    prefix + "-e r -z <config_dir> [-r <random_seed> -o <output_dir> "
	    "-I <image_dir>\n" + prefix + "-j <num_processes>]";

	s += "\n\n";

	s += prefix + "# Create Enrollment Databases\n" +
	    prefix + "-c -z <config_dir> [-d <database_dir> -o <output_dir>]";

	s += "\n\n";

	s += prefix + "# Search, returning subject ID + extract "
	    "correspondence\n" +
	    prefix + "-s s -z <config_dir> [-d <database_dir> -o "
	    "<output_dir>\n" + prefix + "-j <num_processes>]\n";

	s += prefix + "# Search, returning subject ID and position + "
	    "extract correspondence\n" +
	    prefix + "-s p -z <config_dir> [-d <database_dir> "
	    "-o <output_dir>\n" + prefix + "-j <num_processes>]";
	return (s);
}

FRIF::Evaluations::Exemplar1N::Validation::InterfaceVariant
FRIF::Evaluations::Exemplar1N::Validation::instantiateImplementation(
    const Arguments &args)
{
	if (!std::filesystem::exists(args.configDir))
		throw std::runtime_error{"Config dir does not exist: " +
		    args.configDir.string()};

	/* Instantiate only the appropriate interface */
	InterfaceVariant impl{};
	switch (args.operation.value()) {
	case Operation::CreateDatabases:
		[[fallthrough]];
	case Operation::CreateTemplates:
		impl = ExtractionInterface::getImplementation(
		    args.configDir);
		break;
	case Operation::SearchSubject:
		[[ fallthrough ]];
	case Operation::SearchSubjectPosition:
	{

		if (!args.dbDir || !std::filesystem::exists(*args.dbDir))
			throw std::runtime_error{"Database dir does not "
			    "exist: " + args.dbDir->string()};

		impl = SearchInterface::getImplementation(
		    args.configDir, args.outputDir / *args.dbDir);

		constexpr uint64_t oneGB{1 * (1024 * 1024 * 1024)};
		const auto status = std::get<std::shared_ptr<SearchInterface>>(
		    impl)->load(oneGB);
		if (!status) {
			std::string err{"Error on SearchInterface::load()"};
			if (status.message)
				err += ": " + *status.message;
			throw std::runtime_error(err);
		}
		break;
	}
	default:
		throw std::runtime_error("Unsupported operation was sent to "
		    "instantiateInterface()");
		break;
	}

	return (impl);
}

void
FRIF::Evaluations::Exemplar1N::Validation::makeCreateTemplatesDirs(
    const Arguments &args,
    const Data::Dataset &dataset)
{
	if (!args.templateType)
		throw std::runtime_error{"Did not provide template type "
		    "argument"};

	if (!std::filesystem::exists(args.outputDir) &&
	    !std::filesystem::create_directory(args.outputDir))
		throw std::runtime_error{"Could not create output directory: " +
		    args.outputDir.string()};
	std::filesystem::permissions(args.outputDir,
	    std::filesystem::perms::owner_all |
	    std::filesystem::perms::group_all);

	if (!std::filesystem::exists(args.outputDir / Data::TemplateDir) &&
	    !std::filesystem::create_directory(args.outputDir /
	    Data::TemplateDir, args.outputDir))
		throw std::runtime_error{"Could not create template "
		    "directory: " + (args.outputDir /
		    Data::TemplateDir).string()};

	if (!std::filesystem::exists(args.outputDir /
	    Data::getTemplateDir(args.templateType.value())) &&
	    !std::filesystem::create_directory(args.outputDir /
	    Data::getTemplateDir(args.templateType.value()), args.outputDir))
		throw std::runtime_error{"Could not create template "
		    "directory: " + (args.outputDir / Data::getTemplateDir(
		    args.templateType.value())).string()};

	const auto &datasetName = std::get<std::string>(dataset);
	if (!std::filesystem::exists(args.outputDir /
	    Data::getTemplateDir(args.templateType.value()) / datasetName) &&
	    !std::filesystem::create_directory(args.outputDir /
	    Data::getTemplateDir(args.templateType.value()) / datasetName,
	    args.outputDir))
		throw std::runtime_error{"Could not create template "
		    "directory: " + (args.outputDir / Data::getTemplateDir(
		    args.templateType.value()) / datasetName).string()};
}

std::string
FRIF::Evaluations::Exemplar1N::Validation::makeLog(
    const std::string &prefix,
    const std::string &header,
    const std::filesystem::path &outputDir)
{
	const std::string logName = std::filesystem::path{outputDir /
	    std::string{prefix + '-' + Util::ts(getpid()) + ".log"}}.string();
	std::ofstream file{logName, std::ios_base::out | std::ios_base::trunc};
	if (!file)
		throw std::runtime_error{Util::ts(getpid()) + ": Error "
		    "creating " + prefix + " log file"};

	file << header << '\n';
	if (!file)
		throw std::runtime_error{Util::ts(getpid()) + ": Error writing "
		    "to " + prefix + " log"};

	return (logName);
}

std::string
FRIF::Evaluations::Exemplar1N::Validation::makeExtractCorrespondenceLogLine(
    const std::string &identifier,
    const std::string &duration,
    const std::variant<SearchSubjectResult, SearchSubjectPositionResult>
        &searchResult,
    const Arguments &args)
{
	std::string logLine{};

	if (args.operation == Operation::SearchSubject) {
		const auto &correspondences = std::get<SearchSubjectResult>(
		    searchResult).correspondence;
		if (!correspondences.has_value())
			return {'"' + identifier + "\"," +
			   duration + ',' + Util::splice({14, Util::NA}, ",")};
		if (std::get<SearchSubjectResult>(searchResult).candidateList.
		    size() != correspondences->size())
			throw std::runtime_error{"There is not a "
			    "correspondence entry for each candidate in the "
			    "candidate list"};

		const std::string logLinePrefix{'"' + identifier + "\"," +
		   duration + ',' + Util::ts(correspondences->size()) + ','};

		std::unordered_map<std::string,FRIF::Correspondence>::size_type
		    correspondenceIdx{};
		for (const auto &entry : *correspondences) {
			++correspondenceIdx;

			const auto &[candidateID, correspondence] = entry;

			std::vector<EFS::CorrespondenceRelationship>::size_type
			    relIdx{};
			const auto numRels = correspondence.relationships.
			    size();
			for (const auto &rel : correspondence.relationships) {
				logLine += logLinePrefix +
				    Util::ts(correspondenceIdx) + ',' +
				    '"' + candidateID + "\"," +
				    Util::NA + ',' +
				    Util::ts(numRels) + ',' +
				    Util::ts(++relIdx) + ',' +
				    (correspondence.complex ?
					Util::ts(*correspondence.complex) :
					Util::NA) +
				    ',' + Util::e2i2s(rel.type) + ',' +
				    '"' + rel.probeIdentifier + "\"," +
				    Util::ts(rel.probeInputIdentifier) + ',' +
				    Util::splice({rel.probeMinutia}) + ',' +
				    '"' + rel.referenceIdentifier + "\"," +
				    Util::ts(rel.referenceInputIdentifier) +
				    ',';
				if ((rel.type == EFS::
				    CorrespondenceType::Definite) ||
				    (rel.type == EFS::
				    CorrespondenceType::Possible))
					logLine += Util::splice(
					    {rel.referenceMinutia}) + '\n';
				else
					logLine += Util::NA + '\n';
			}
		}
	} else if (args.operation == Operation::SearchSubjectPosition) {
		const auto &correspondences =
		    std::get<SearchSubjectPositionResult>(searchResult).
		    correspondence;
		if (!correspondences.has_value())
			return {'"' + identifier + "\"," +
			   duration + ',' + Util::splice({14, Util::NA}, ",")};
		if (std::get<SearchSubjectPositionResult>(searchResult).
		    candidateList.size() != correspondences->size())
			throw std::runtime_error{"There is not a "
			    "correspondence entry for each candidate in the "
			    "candidate list"};

		const std::string logLinePrefix{'"' + identifier + "\"," +
		   duration + ',' + Util::ts(correspondences->size()) + ','};

		std::unordered_map<std::string,FRIF::Correspondence>::size_type
		    correspondenceIdx{};
		for (const auto &entry : *correspondences) {
			++correspondenceIdx;

			const auto &[candidate, correspondence] = entry;

			std::vector<EFS::CorrespondenceRelationship>::size_type
			    relIdx{};
			const auto numRels = correspondence.relationships.
			    size();
			for (const auto &rel : correspondence.relationships) {
				logLine += logLinePrefix +
				    Util::ts(correspondenceIdx) + ',' +
				    '"' + candidate.identifier + "\"," +
				    Util::e2i2s(candidate.fgp) + ',' +
				    Util::ts(numRels) + ',' +
				    Util::ts(++relIdx) + ',' +
				    (correspondence.complex ?
					Util::ts(*correspondence.complex) :
					Util::NA) +
				    ',' + Util::e2i2s(rel.type) + ',' +
				    '"' + rel.probeIdentifier + "\"," +
				    Util::ts(rel.probeInputIdentifier) + ',' +
				    Util::splice({rel.probeMinutia}) + ',' +
				    '"' + rel.referenceIdentifier + "\"," +
				    Util::ts(rel.referenceInputIdentifier) +
				    ',';
				if ((rel.type == EFS::
				    CorrespondenceType::Definite) ||
				    (rel.type == EFS::
				    CorrespondenceType::Possible))
					logLine += Util::splice(
					    {rel.referenceMinutia}) + '\n';
				else
					logLine += Util::NA + '\n';
			}
		}
	} else
		throw std::runtime_error{"Unsupported operation sent to "
		    "makeExtractCorrespondenceLogLine()"};

	/* Remove last newline */
	logLine.pop_back();

	return (logLine);
}

std::string
FRIF::Evaluations::Exemplar1N::Validation::makeExtractTemplateDataLogLine(
    const std::string &identifier,
    const std::string &duration,
    const std::optional<std::vector<TemplateData>> &extractedData,
    const std::optional<ReturnStatus> &rs,
    const Arguments &args)
{
	const std::string logLinePrefix{'"' + identifier + "\"," +  duration +
	    ',' + Util::e2i2s(*args.templateType) + ','};

	if (!rs.has_value() || !extractedData.has_value() ||
	    (rs.has_value() && !rs.value()) ||
	    (extractedData.has_value() && extractedData->empty())) {
		static const uint8_t numElements{20};
		static const std::string NAFull = Util::splice(
		    std::vector<std::string>(numElements, Util::NA), ",");
		return (logLinePrefix + NAFull);
	}

	std::string logLine{};
	for (std::vector<TemplateData>::size_type i{};
	    i < extractedData->size(); ++i) {
		const auto &td = extractedData->at(i);
		if (td.identifier != identifier)
			throw std::runtime_error{"Inconsistency in identifier"};

		logLine += logLinePrefix + Util::ts(i) + ',' +
		    Util::ts(extractedData->size()) + ',' +
		    Util::ts(td.inputIdentifier) + ',';
		logLine += (td.imageQuality ?
		   Util::ts(*td.imageQuality) : Util::NA) + ',';

		static const uint8_t efsElements{16};
		static const std::string NAEFS = Util::splice(
		    std::vector<std::string>(efsElements, Util::NA), ",");
		if (!td.features) {
			logLine += NAEFS + '\n';
			continue;
		}

		const auto &efs = td.features.value();
		logLine += Util::e2i2s(efs.imp) + ',' + Util::e2i2s(efs.frct) +
		    ',' + Util::e2i2s(efs.frgp) + ',';
		if (efs.orientation) {
			logLine += Util::ts(efs.orientation->direction) +
			    ';' + (efs.orientation->uncertainty ?
			    Util::ts(*efs.orientation->uncertainty) :
			    Util::NA) + ',';
		} else
			logLine += Util::NA + ',';
		logLine += (efs.lpm ? Util::splice(*efs.lpm) : Util::NA) + ',';
		logLine += (efs.valueAssessment ?
		    Util::e2i2s(*efs.valueAssessment) : Util::NA) + ',';
		logLine += (efs.lsb ? Util::e2i2s(*efs.lsb) : Util::NA) + ',';

		if (efs.pat) {
			logLine += Util::e2i2s(efs.pat->
			    getPatternGeneralClassification()) + ';';

			if (efs.pat->getPatternSubclassification()) {
				const auto ps = *efs.pat->
				    getPatternSubclassification();
				if (const auto *aps = std::get_if<
				    EFS::ArchPatternSubclassification>(&ps))
					logLine += Util::e2i2s(*aps);
				else if (const auto *wps = std::get_if<
				    EFS::WhorlPatternSubclassification>(&ps))
					logLine += Util::e2i2s(*wps);
				else
					logLine += Util::NA;
			} else
				logLine += Util::NA + ';' + Util::NA + ';' +
				    Util::NA;

			logLine += ';' + (efs.pat->getWhorlDeltaRelationship() ?
			    Util::e2i2s(*efs.pat->getWhorlDeltaRelationship()) :
			    Util::NA) + ',';
		} else
			logLine += Util::NA + ',';

		logLine += (efs.plr ? Util::ts(*efs.plr) : Util::NA) + ',';
		logLine += (efs.trv ? Util::ts(*efs.trv) : Util::NA) + ',';
		logLine += (efs.cores ?
		    '"' + Util::splice(*efs.cores) + '"' : Util::NA) + ',';
		logLine += (efs.deltas ?
		    '"' + Util::splice(*efs.deltas)  + '"': Util::NA) + ',';
		logLine += (efs.minutiae ?
		    '"' + Util::splice(*efs.minutiae) + '"' : Util::NA) + ',';
		logLine += (efs.roi ?
		    '"' + Util::splice(*efs.roi)  + '"': Util::NA) + ',';
		logLine += (efs.rqm ?
		    '"' + Util::splice(*efs.rqm)  + '"': Util::NA) + ',';
		logLine += (efs.complex ? Util::ts(*efs.complex) : Util::NA);

		logLine += '\n';
	}

	/* Remove last newline */
	logLine.pop_back();

	return (logLine);
}

void
FRIF::Evaluations::Exemplar1N::Validation::makeReferenceTemplateArchive(
    const std::string &databaseName,
    const Arguments &args)
{
	const auto dir = args.outputDir / Data::getTemplateDir(
	    TemplateType::Reference) / databaseName;

	if (std::filesystem::exists(dir / Data::TemplateArchiveArchiveName))
		throw std::runtime_error{
		    (dir / Data::TemplateArchiveArchiveName).string() + " "
		        "already exists"};
	std::ofstream archive{dir / Data::TemplateArchiveArchiveName,
	    std::ios_base::out | std::ios_base::binary |
	    std::ios_base::app};
	if (!archive)
		throw std::runtime_error{"Could not open " +
		    (dir / Data::TemplateArchiveArchiveName).string()};

	if (std::filesystem::exists(dir / Data::TemplateArchiveManifestName))
		throw std::runtime_error{
		    (dir / Data::TemplateArchiveManifestName).string() + " "
		        "already exists"};
	std::ofstream manifest{dir / Data::TemplateArchiveManifestName,
	    std::ios_base::out | std::ios_base::binary |
	    std::ios_base::app};
	if (!manifest)
		throw std::runtime_error{"Could not open " +
		    (dir / Data::TemplateArchiveManifestName).string()};

	/* Sort to ensure archive is always generated in the same order */
	std::vector<std::filesystem::directory_entry> entries{};
	for (const auto &entry : std::filesystem::recursive_directory_iterator(
	    dir)) {
		if ((entry.path().extension() != Data::TemplateSuffix) ||
		    !entry.is_regular_file())
			continue;
		entries.push_back(entry);
	}
	std::sort(entries.begin(), entries.end());

	for (const auto &entry : entries) {
		std::ifstream tmpl{entry.path(), std::ios_base::in |
		    std::ios_base::binary | std::ios_base::ate};
		if (!tmpl)
			throw std::runtime_error{"Could not open " +
			    entry.path().string()};

		const auto tmplDataSize = tmpl.tellg();
		if (!tmpl)
			throw std::runtime_error{"Could not get size of " +
			    entry.path().string()};

		tmpl.seekg(0);
		if (!tmpl)
			throw std::runtime_error{"Could not rewind " +
			    entry.path().string()};

		std::vector<std::byte> tmplData(
		    static_cast<std::vector<std::byte>::size_type>(
		    tmplDataSize));
		tmpl.read(reinterpret_cast<char*>(tmplData.data()),
		    tmplDataSize);
		if (!tmpl)
			throw std::runtime_error{"Could not read " +
			    entry.path().string()};

		const auto currentOffset = archive.tellp();
		if (!archive)
			throw std::runtime_error{"Could not get current "
			    "offset from " +
			    (dir / Data::TemplateArchiveArchiveName).string()};

		archive.write(reinterpret_cast<char*>(tmplData.data()),
		    tmplDataSize);
		if (!archive)
			throw std::runtime_error{"Could not write " +
			    (dir / Data::TemplateArchiveArchiveName).string()};

		manifest << entry.path().filename().replace_extension().
		    string() << ' ' << tmplDataSize << ' ' << currentOffset <<
		    '\n';
		if (!manifest)
			throw std::runtime_error{"Could not write " +
			    (dir / Data::TemplateArchiveManifestName).string()};
	}


}

std::vector<FRIF::Sample>
FRIF::Evaluations::Exemplar1N::Validation::makeSamples(
    const std::vector<Data::Input> &metadatas,
    const Arguments &args)
{
	std::vector<Sample> samples{};
	samples.reserve(metadatas.size());
	for (std::vector<Data::Input>::size_type sampleIndex{};
	    sampleIndex < metadatas.size(); ++sampleIndex) {
		auto md = metadatas[sampleIndex];
		if (md.features)
			md.features->identifier = static_cast<uint8_t>(
			    sampleIndex);

		if (!md.image && !md.features)
			throw std::runtime_error{"No filename or features "
			    "provided"};

		if (md.image) {
			if (!md.image->filename || !md.image->width ||
			    !md.image->height || !md.image->ppi ||
			    !md.image->bpc || !md.image->bpp ||
			    !md.image->colorspace)
				throw std::runtime_error{"Missing image meta"
				    "data"};

			samples.emplace_back(Image(
			    static_cast<uint8_t>(sampleIndex),
			    *md.image->width, *md.image->height, *md.image->ppi,
			    *md.image->colorspace,
			    *md.image->bpc, *md.image->bpp, Util::readFile(
			    args.imageDir / *md.image->filename)), md.features);

			const auto &image = std::get<std::optional<Image>>(
			    samples.back());
			if (image)
				image->sanityCheck();
		} else
			samples.emplace_back(std::nullopt, md.features);
	}

	return (samples);
}

FRIF::Evaluations::Exemplar1N::Validation::Arguments
FRIF::Evaluations::Exemplar1N::Validation::parseArguments(
    const int argc,
    char * const argv[])
{
	static const char options[] {"cd:e:i:j:m:r:o:s:z:I:"};
	Arguments args{};
	args.executableName = argv[0];

	int c{};
	while ((c = getopt(argc, argv, options)) != -1) {
		switch (c) {
		case 'c':	/* Create databases */
			if (args.operation)
				throw std::logic_error{"Multiple operations "
				    "specified"};
			args.operation = Operation::CreateDatabases;
			break;

		case 'd':	/* Enrollment database directory */
			args.dbDir = optarg;
			break;

		case 'e':	/* Extract */
			if (std::string(optarg).length() != 1)
				throw std::logic_error{"Invalid -i argument"};
			if (args.operation)
				throw std::logic_error{"Multiple operations "
				    "specified"};
			args.operation = Operation::CreateTemplates;

			switch (optarg[0]) {
			case 'p':
				args.templateType = TemplateType::Probe;
				break;
			case 'r':
				args.templateType = TemplateType::Reference;
				break;
			default:
				throw std::logic_error{"Invalid -e argument"};
			}
			break;

		case 'i':	/* Identification */
			if (std::string(optarg).length() != 1)
				throw std::logic_error{"Invalid -i argument"};
			if (args.operation)
				throw std::logic_error{"Multiple operations "
				    "specified"};

			switch (optarg[0]) {
			case 'e':
				args.operation = Operation::IdentifyExtraction;
				break;
			case 'l':
				args.operation = Operation::IdentifyLibrary;
				break;
			case 's':
				args.operation = Operation::IdentifySearch;
				break;
			default:
				throw std::logic_error{"Invalid -i argument"};
			}
			break;

		case 'j': 	/* Number of processes */
			try {
				args.numProcs = static_cast<uint8_t>(
				    std::stoul(optarg));
			} catch (const std::exception&) {
				throw std::invalid_argument{"Number of "
				    "processes (-j): an error occurred when "
				    "parsing \"" + std::string(optarg) + "\""};
			}

			/* Need to test 2 procs, even if only 1 core */
			if (const auto threadCount = std::max(
			    static_cast<unsigned int>(2), std::thread::
			    hardware_concurrency());
			    (threadCount == 0 && args.numProcs > 4) ||
			    (args.numProcs > threadCount))
				throw std::invalid_argument{"Number of "
				    "processes (-f): Asked to spawn " +
				    Util::ts(args.numProcs) + " processes, but "
				    "refusing"};
			break;
		case 'm':	/* Maximum size */
			try {
				args.maximum = std::stoull(optarg);
			} catch (const std::exception&) {
				throw std::invalid_argument{"Maximum size "
				    "(-m): an error occurred when parsing \"" +
				    std::string(optarg) + "\""};
			}
			break;
		case 's':
			if (std::string(optarg).length() != 1)
				throw std::logic_error{"Invalid -s argument"};
			if (args.operation)
				throw std::logic_error{"Multiple operations "
				    "specified"};

			switch (optarg[0]) {
			case 'p':
				args.operation =
				    Operation::SearchSubjectPosition;
				break;
			case 's':
				args.operation = Operation::SearchSubject;
				break;
			default:
				throw std::logic_error{"Invalid -s argument"};
			}
			break;

		case 'o':	/* Output directory */
			args.outputDir = optarg;
			break;

		case 'r':	/* Random seed */
			try {
				args.randomSeed = std::stoull(optarg);
			} catch (const std::exception&) {
				throw std::invalid_argument{"Random seed (-r): "
				    "an error occurred when parsing \"" +
				    std::string(optarg) + "\""};
			}
			break;

		case 'z':	/* Config dir */
			args.configDir = optarg;
			break;
		case 'I':
			args.imageDir = optarg;
			break;
		}
	}

	if (!args.operation)
		args.operation = Operation::Usage;

	if (args.configDir.empty() && !(
	    (args.operation == Operation::Usage) ||
	    (args.operation == Operation::IdentifyExtraction) ||
	    (args.operation == Operation::IdentifyLibrary) ||
	    (args.operation == Operation::IdentifySearch)))
		throw std::invalid_argument{"Must provide path to "
		     "configuration directory"};

	if ((args.operation == Operation::SearchSubject) ||
	    (args.operation == Operation::SearchSubjectPosition)) {
		if (args.maximum > std::numeric_limits<uint16_t>::max())
			throw std::invalid_argument{"Value for -m is too high ("
			    "must be lower than " + std::to_string(
			    std::numeric_limits<uint16_t>::max())};

		if (!args.dbDir)
			throw std::invalid_argument{"No database set with -d"};
	}

	return (args);
}

void
FRIF::Evaluations::Exemplar1N::Validation::runPartialCreateTemplates(
    const InterfaceVariant &iv,
    const Data::Dataset &dataset,
    const std::vector<uint64_t> &indices,
    const Arguments &args)
{
	std::shared_ptr<ExtractionInterface> impl{};
	if (!std::get_if<std::shared_ptr<ExtractionInterface>>(&iv))
		throw std::runtime_error{"Failure to obtain implementation of "
		    "ExtractionInterface"};
	impl = std::get<std::shared_ptr<ExtractionInterface>>(iv);
	if (!impl)
		throw std::runtime_error{"ExtractionInterface implementation "
		    "was null"};

	static const std::string header{"\"identifier\",elapsed,result,"
	    "\"message\",type,num_images,size"};
	static std::string typeStr = (*args.templateType ==
	    TemplateType::Probe ? "probe" : "reference");

	std::string prefix{"createTemplate-" + typeStr + '-' +
	    std::get<std::string>(dataset)};
	const auto createLogPath = makeLog(prefix, header, args.outputDir);

	std::ofstream createLog{createLogPath,
	    std::ios_base::out | std::ios_base::app};
	if (!createLog)
		throw std::runtime_error{Util::ts(getpid()) + ": Error "
		    "opening create log file: " + createLogPath};

	const bool doExtract = *args.templateType ==
	    TemplateType::Probe ? impl->getCompatibility().
	    supportsProbeTemplateIntrospection : impl->getCompatibility().
	    supportsReferenceTemplateIntrospection;

	std::string extractDataLogPath{};
	std::ofstream extractDataLog{};
	if (doExtract) {
		static const std::string extractHeader{"\"template_filename\","
		    "elapsed,type,index,num_templates_in_buffer,"
		    "image_identifier,quality,imp,frct,frgp,orientation,lpm,"
		    "value_assessment,lsb,pat,plr,trv,\"cores\",\"deltas\","
		    "\"minutia\",\"roi\",\"rqm\",complex"};

		std::string extractPrefix{"extractTemplateData-" + typeStr +
		    '-' + std::get<std::string>(dataset)};
		extractDataLogPath = makeLog(extractPrefix, extractHeader,
		    args.outputDir);
		extractDataLog = std::ofstream{extractDataLogPath,
		    std::ios_base::out | std::ios_base::app};
		if (!extractDataLog)
			throw std::runtime_error{Util::ts(getpid()) + ": Error "
			    "opening extract template data log file: " +
			    extractDataLogPath};
	}

	for (const auto &n : indices) {
		const auto [createLogLine,  extractDataLogLine] =
		    singleCreateTemplate(impl, dataset, n, args);
		createLog << createLogLine << '\n';
		if (!createLog)
			throw std::runtime_error{Util::ts(getpid()) + ": Error "
			    "writing to create log"};

		/*
		 * Perform template data extraction.
		 */
		if (doExtract && extractDataLogLine) {
			extractDataLog << *extractDataLogLine << '\n';
			if (!extractDataLog)
				throw std::runtime_error{Util::ts(getpid()) +
				    ": Error writing to extract template data "
				    "log"};
		}
	}
}

void
FRIF::Evaluations::Exemplar1N::Validation::runPartialSearch(
    const InterfaceVariant &iv,
    const Data::Dataset &probes,
    const std::vector<uint64_t> &probeIndicies,
    const Arguments &args)
{
	std::shared_ptr<SearchInterface> impl{};
	if (!std::get_if<std::shared_ptr<SearchInterface>>(&iv))
		throw std::runtime_error{"Failure to obtain implementation of "
		    "SearchInterface"};
	impl = std::get<std::shared_ptr<SearchInterface>>(iv);
	if (!impl)
		throw std::runtime_error{"SearchInterface implementation "
		    "was null"};

	const std::string searchType =
	    args.operation == Operation::SearchSubject ? "subject" :
	    "subject+position";
	const std::string databaseName{args.dbDir->filename().string()};
	const std::string searchPrefix = "search-" + searchType + '-' +
	    std::get<std::string>(probes) + "_v_" + databaseName;
	static const std::string searchHeader = "\"probe_identifier\","
	    "max_candidates,elapsed,result,\"message\",include_fgp,cl_present,"
	    "decision,num_candidates,rank,\"candidate_id\",candidate_fgp,"
	    "similarity";
	const auto searchLogPath = makeLog(searchPrefix, searchHeader,
	    args.outputDir);
	std::ofstream searchLog{searchLogPath,
	    std::ios_base::out | std::ios_base::app};
	if (!searchLog)
		throw std::runtime_error{Util::ts(getpid()) + ": Error opening "
		    "search log file: " + searchLogPath};

	const bool doCorrespondence =
	    impl->getCompatibility().supportsCorrespondence;

	std::string correspondenceLogPath{};
	std::ofstream correspondenceLog{};
	if (doCorrespondence) {
		static const std::string header{"\"search_probe_identifier\","
		    "elapsed,num_entries,entry_num,\"candidate_identifier\","
		    "candidate_fgp,num_relationships,relationship_num,complex,"
		    "type,\"probe_identifier\",probe_input_identifier,"
		    "probe_minutia,\"reference_identifier\","
		    "reference_input_identifier,reference_minutia"};

		const std::string correspondencePrefix{
		    "extractCorrespondence-" + searchType + '-' +
		    std::get<std::string>(probes) + "_v_" + databaseName};
		correspondenceLogPath = makeLog(correspondencePrefix, header,
		    args.outputDir);
		correspondenceLog = std::ofstream{correspondenceLogPath,
		    std::ios_base::out | std::ios_base::app};
		if (!correspondenceLog)
			throw std::runtime_error{Util::ts(getpid()) + ": Error "
			    "opening extract template data log file: " +
			    correspondenceLogPath};
	}

	for (const auto &n : probeIndicies) {
		const auto [searchLogLine, correspondenceLogLine] =
		    singleSearch(impl, probes, n, args);
		searchLog << searchLogLine << '\n';
		if (!searchLog)
			throw std::runtime_error{Util::ts(getpid()) + ": Error "
			    "writing to search log"};

		/*
		 * Perform correspondence extraction.
		 */
		if (doCorrespondence && correspondenceLogLine) {
			correspondenceLog << *correspondenceLogLine << '\n';
			if (!correspondenceLog)
				throw std::runtime_error{Util::ts(getpid()) +
				    ": Error writing to correspondence log"};
		}
	}
}

void
FRIF::Evaluations::Exemplar1N::Validation::singleCreateDatabase(
    std::shared_ptr<ExtractionInterface> impl,
    const std::string &datasetName,
    const Arguments &args)
{
	const auto archive = Data::getTemplateArchive(datasetName,
	    args.outputDir);
	if (!std::filesystem::exists(archive.archive) ||
	    !std::filesystem::exists(archive.manifest))
		makeReferenceTemplateArchive(datasetName, args);

	const std::filesystem::path logFilePath = args.outputDir /
	    ("createDatabase-" + datasetName + ".log");
	auto logFile = std::ofstream{logFilePath,
	    std::ios_base::out | std::ios_base::trunc};
	logFile << "\"databaseName\",duration,result,\"message\"\n";
	if (!logFile)
		throw std::runtime_error{"Failed to write create database "
		    "header to " + logFilePath.string()};

	ReturnStatus rs{};
	std::chrono::steady_clock::time_point start{}, stop{};
	try {

		start = std::chrono::steady_clock::now();
		rs = impl->createReferenceDatabase(archive,
		    args.outputDir / Data::DatabaseDir / datasetName,
		    args.maximum);
		stop = std::chrono::steady_clock::now();
	} catch (const std::exception &e) {
		throw std::runtime_error{"Exception while creating database "
		    "for dataset = " + datasetName + " (" + e.what() +
		    ")"};
	} catch (...) {
		throw std::runtime_error{"Exception while creating database "
		    "for dataset = " + datasetName};
	}

	logFile << '"' << datasetName << "\"," <<
	    Util::duration(start, stop) << ',' << Util::e2i2s(rs.result) <<
	    ',' << Util::sanitizeMessage(rs.message ? *rs.message : "");
	if (!logFile)
		throw std::runtime_error{"Failed to write to create database "
		    "log " + logFilePath.string()};

	if (!rs)
		throw std::runtime_error{"Failed to create database for "
		    "dataset = " + datasetName + (rs.message ?
		    ": (" + *rs.message + ")" : "")};
}

std::pair<std::string, std::optional<std::string>>
FRIF::Evaluations::Exemplar1N::Validation::singleCreateTemplate(
    std::shared_ptr<ExtractionInterface> impl,
    const Data::Dataset &dataset,
    const uint64_t datasetIndex,
    const Arguments &args)
{
	const std::string &datasetName{std::get<std::string>(dataset)};
	const auto &[identifier, metadatas] =
	    std::get<std::vector<Data::ImageSet>>(dataset).at(datasetIndex);

	std::vector<Sample> samples{};
	try {
		samples = makeSamples(metadatas, args);
	} catch (const std::exception &e) {
		throw std::runtime_error{"Exception while creating samples "
		    "from ID = " + identifier + ",  dataset = " + datasetName +
		    ", index = " + Util::ts(datasetIndex) + " (" + e.what() +
		    ")"};
	}

	std::tuple<ReturnStatus, std::optional< CreateTemplateResult>> ret{};
	std::chrono::steady_clock::time_point start{}, stop{};
	try {

		start = std::chrono::steady_clock::now();
		ret = impl->createTemplate(*args.templateType, identifier,
		    samples);
		stop = std::chrono::steady_clock::now();
	} catch (const std::exception &e) {
		throw std::runtime_error{"Exception while creating template "
		    "from ID = " + identifier + ",  dataset = " + datasetName +
		    ", index = " + Util::ts(datasetIndex) + " (" + e.what() +
		    ")"};
	} catch (...) {
		throw std::runtime_error{"Unknown exception while creating "
		    "template from ID = " + identifier + ",  dataset = " +
		    datasetName + ", index = " + Util::ts(datasetIndex)};
	}

	const auto &[rv, ctr] = ret;
	std::string logLineCreate{'"' + identifier + "\"," +
	    Util::duration(start, stop) + ',' + Util::e2i2s(rv.result) + ',' +
	    Util::sanitizeMessage(rv.message ? *rv.message : "") + ',' +
	    Util::e2i2s(*args.templateType) + ',' + Util::ts(samples.size()) +
	    ','};
	std::optional<std::string> logLineExtractData{};

	const bool doExtract = *args.templateType ==
	    TemplateType::Probe ? impl->getCompatibility().
	    supportsProbeTemplateIntrospection : impl->getCompatibility().
	    supportsReferenceTemplateIntrospection;

	/* Write template */
	const auto dir = args.outputDir /
	    Data::getTemplateDir(*args.templateType) / datasetName;
	if (rv && ctr) {
		Util::writeFile(ctr->data,
		    dir / (identifier + Data::TemplateSuffix));
		logLineCreate += Util::ts(ctr->data.size());

		if (ctr->extractedData) {
			logLineExtractData = makeExtractTemplateDataLogLine(
			    identifier, Util::duration(start, stop),
			    ctr->extractedData, rv, args);
		} else if (doExtract) {
			logLineExtractData = singleExtractTemplateData(impl,
			    dataset, datasetIndex, ctr.value(), args);
		}

	} else {
		Util::writeFile({}, dir / (identifier + Data::TemplateSuffix));
		logLineCreate += Util::NA;
	}

	return {logLineCreate, logLineExtractData};
}

std::string
FRIF::Evaluations::Exemplar1N::Validation::singleExtractTemplateData(
    std::shared_ptr<ExtractionInterface> impl,
    const Data::Dataset &dataset,
    const uint64_t datasetIndex,
    const CreateTemplateResult &templateResult,
    const Arguments &args)
{
	if (!args.templateType)
		throw std::runtime_error{"Template type argument not set"};
	if ((*args.templateType == TemplateType::Probe) &&
	    !impl->getCompatibility().supportsProbeTemplateIntrospection)
		throw std::runtime_error{"Implementation does not support "
		    "probe template introspection"};
	if ((*args.templateType == TemplateType::Reference) &&
	    !impl->getCompatibility().supportsReferenceTemplateIntrospection)
		throw std::runtime_error{"Implementation does not support "
		    "reference template introspection"};

	const std::string &datasetName = std::get<std::string>(dataset);
	const std::string &identifier = std::get<std::string>(
	    std::get<std::vector<Data::ImageSet>>(dataset).at(datasetIndex));

	std::optional<std::tuple<ReturnStatus, std::vector<TemplateData>>>
	    ret{};
	std::chrono::steady_clock::time_point start{}, stop{};
	try {

		start = std::chrono::steady_clock::now();
		ret = impl->extractTemplateData(*args.templateType,
		    templateResult);
		stop = std::chrono::steady_clock::now();
	} catch (const std::exception &e) {
		throw std::runtime_error{"Exception while extracting template "
		    "data from ID = " + identifier + ",  dataset = " +
		    datasetName + ", index = " + Util::ts(datasetIndex) + " (" +
		    e.what() + ")"};
	} catch (...) {
		throw std::runtime_error{"Unknown exception while extracting "
		    "template data from ID = " + identifier + ",  dataset = " +
		    datasetName + ", index = " + Util::ts(datasetIndex)};
	}

	return (makeExtractTemplateDataLogLine(
	    identifier,
	    Util::duration(start, stop),
	    ret ? std::optional<std::vector<TemplateData>>{
	        std::get<std::vector<TemplateData>>(*ret)} : std::nullopt,
	    ret ? std::optional<ReturnStatus>{
	        std::get<ReturnStatus>(*ret)} : std::nullopt,
	    args));
}

std::pair<std::string, std::optional<std::string>>
FRIF::Evaluations::Exemplar1N::Validation::singleSearch(
    std::shared_ptr<SearchInterface> impl,
    const Data::Dataset &dataset,
    const uint64_t datasetIndex,
    const Arguments &args)
{
	const std::string &datasetName{std::get<std::string>(dataset)};
	const auto identifier = std::get<std::string>(
	    std::get<std::vector<Data::ImageSet>>(dataset).at(datasetIndex));

	std::vector<std::byte> probe{};
	try {
		const std::string tmplName{identifier + ".tmpl"};
		probe = Util::readFile(args.outputDir / Data::ProbeTemplateDir /
		    datasetName / tmplName);
	} catch (const std::exception &e) {
		throw std::runtime_error{"Could not read probe template for "
		    "id = " + identifier + " in dataset = " + datasetName +
		    ": " + e.what()};
	}

	/*
	 * NOTE: We don't search 0-byte templates, even if that's what was
	 *       returned, but we also don't want your submission to crash.
	 *       Hopefully passing a 0-byte template during validation will
	 *       encourage you to validate templates first.
	 */
// 	if (probeTemplate.size() == 0) {
// 	}

	/* Checked in parseArguments */
	const uint16_t maxCandidates = static_cast<uint16_t>(args.maximum);

	const bool doCorrespondence =
	    impl->getCompatibility().supportsCorrespondence;

	std::chrono::steady_clock::time_point start{}, stop{};
	ReturnStatus rs{};
	std::variant<std::optional<SearchSubjectResult>,
	    std::optional<SearchSubjectPositionResult>> result{};
	if (args.operation == Operation::SearchSubject) {
		auto ret = executeSingleSearchSubject(impl, probe,
		    maxCandidates);
		rs = std::get<ReturnStatus>(ret);
		result = std::get<std::optional<SearchSubjectResult>>(ret);
		start = std::get<0>(ret);
		stop = std::get<1>(ret);
	} else if (args.operation == Operation::SearchSubjectPosition) {
		auto ret = executeSingleSearchSubjectPosition(impl, probe,
		    maxCandidates);
		rs = std::get<ReturnStatus>(ret);
		result = std::get<std::optional<SearchSubjectPositionResult>>(
		    ret);
		start = std::get<0>(ret);
		stop = std::get<1>(ret);
	} else
		throw std::runtime_error{"Unsupported operation sent to "
		    "singleSearch()"};

	const std::string logLinePrefix{'"' + identifier + "\"," +
	    Util::ts(args.maximum) + ',' + Util::duration(start, stop) + ',' +
	    Util::e2i2s(rs.result) + ',' +
	    Util::sanitizeMessage(rs.message ? *rs.message : "") + ','};

	std::string logLine{};
	const bool includeFGP{
	    args.operation == Operation::SearchSubjectPosition};
	if (!rs) {
		logLine += logLinePrefix + std::to_string(includeFGP) + ',' +
		    Util::splice(std::vector<std::string>(7, Util::NA), ",");
		return {logLine, std::nullopt};
	}

	if (args.operation == Operation::SearchSubject) {
		const auto &optRes = std::get<std::optional<
		    SearchSubjectResult>>(result);
		if (!optRes.has_value()) {
			logLine += logLinePrefix + std::to_string(includeFGP) +
			    ',' + std::to_string(optRes.has_value()) + ',' +
			    Util::splice(std::vector<std::string>(6, Util::NA),
			    ",");
			return {logLine, std::nullopt};
		}
		if (optRes->candidateList.empty()) {
			logLine += logLinePrefix + std::to_string(includeFGP) +
			    ',' + std::to_string(optRes.has_value()) + ',' +
			    Util::splice(std::vector<std::string>(6, Util::NA),
			    ",");
			return {logLine, std::nullopt};
		}

		/* Order by descending similarity */
		std::vector<std::pair<std::string, double>> sorted{};
		for (const auto &[k,v] : optRes->candidateList)
			sorted.emplace_back(k, v);
		std::stable_sort(sorted.begin(), sorted.end(), [](
		    const SubjectCandidateList::value_type &a,
		    const SubjectCandidateList::value_type &b) {
		    	return (std::get<double>(a) > std::get<double>(b));
		});

		std::vector<SubjectCandidateList::value_type>::size_type rank{};
		for (const auto &[candidateID, similarity] : sorted) {
			logLine += logLinePrefix + std::to_string(includeFGP) +
			    ',' + std::to_string(optRes.has_value()) + ',' +
			    Util::ts(optRes->decision) + ',' +
			    Util::ts(optRes->candidateList.size()) + ',' +
			    Util::ts(++rank) + ",\"" + candidateID + "\"," +
			    Util::NA + "," + Util::ts(similarity);
			if (rank < optRes->candidateList.size())
				logLine += '\n';
		}

		/* Log or call correspondence */
		if (optRes->correspondence.has_value())
			return {logLine, makeExtractCorrespondenceLogLine(
			    identifier, Util::duration(start, stop), *optRes,
			    args)};
		else if (doCorrespondence)
			return {logLine, singleExtractCorrespondence(
			    impl, dataset, datasetIndex, *optRes, args)};
		else
			return {logLine, std::nullopt};
	} else if (args.operation == Operation::SearchSubjectPosition) {
		const auto &optRes = std::get<std::optional<
		    SearchSubjectPositionResult>>(result);
		if (!optRes.has_value()) {
			logLine += logLinePrefix + std::to_string(includeFGP) +
			    ',' + std::to_string(optRes.has_value()) + ',' +
			    Util::splice(std::vector<std::string>(6, Util::NA),
			    ",");
			return {logLine, std::nullopt};
		}
		if (optRes->candidateList.empty()) {
			logLine += logLinePrefix + std::to_string(includeFGP) +
			    ',' + std::to_string(optRes.has_value()) + ',' +
			    Util::splice(std::vector<std::string>(6, Util::NA),
			    ",");
			return {logLine, std::nullopt};
		}

		/* Order by descending similarity */
		std::vector<std::pair<SubjectPositionCandidate, double>>
		    sorted{};
		for (const auto &[k, v] : optRes->candidateList)
			sorted.emplace_back(k, v);
		std::stable_sort(sorted.begin(), sorted.end(), [](
		    const SubjectPositionCandidateList::value_type &a,
		    const SubjectPositionCandidateList::value_type &b) {
		    	return (std::get<double>(a) > std::get<double>(b));
		});

		std::vector<SubjectPositionCandidateList::value_type>::size_type
		    rank{};
		for (const auto &[c, similarity] : sorted) {
			logLine += logLinePrefix + std::to_string(includeFGP) +
			    ',' + std::to_string(optRes.has_value()) + ',' +
			    Util::ts(optRes->decision) + ',' +
			    Util::ts(optRes->candidateList.size()) + ',' +
			    Util::ts(++rank) + ",\"" + c.identifier + "\"," +
			    Util::e2i2s(c.fgp) + "," + Util::ts(similarity);
			if (rank < optRes->candidateList.size())
				logLine += '\n';
		}

		/* Log or call correspondence */
		if (optRes->correspondence.has_value())
			return {logLine, makeExtractCorrespondenceLogLine(
			    identifier, Util::duration(start, stop), *optRes,
			    args)};
		else if (doCorrespondence)
			return {logLine, singleExtractCorrespondence(
			    impl, dataset, datasetIndex, *optRes, args)};
		else
			return {logLine, std::nullopt};
	} else
		throw std::runtime_error{"Unsupported operation sent to "
		    "singleSearch()"};
}

std::string
FRIF::Evaluations::Exemplar1N::Validation::singleExtractCorrespondence(
    std::shared_ptr<SearchInterface> impl,
    const Data::Dataset &dataset,
    const uint64_t datasetIndex,
    const std::variant<SearchSubjectResult, SearchSubjectPositionResult>
        &result,
    const Arguments &args)
{
	if (!impl->getCompatibility().supportsCorrespondence)
		throw std::runtime_error{"Implementation does not support "
		    "correspondence"};

	const std::string &datasetName{std::get<std::string>(dataset)};
	const auto identifier = std::get<std::string>(
	    std::get<std::vector<Data::ImageSet>>(dataset).at(datasetIndex));

	std::vector<std::byte> probe{};
	try {
		probe = Util::readFile(args.outputDir / Data::TemplateDir /
		    datasetName / identifier);
	} catch (const std::exception &e) {
		throw std::runtime_error{"Could not read probe template for "
		    "id = " + identifier + " in dataset = " + datasetName +
		    ": " + e.what()};
	}

	/*
	 * NOTE: We don't search 0-byte templates, even if that's what was
	 *       returned, but we also don't want your submission to crash.
	 *       Hopefully passing a 0-byte template during validation will
	 *       encourage you to validate templates first.
	 */
// 	if (probeTemplate.size() == 0) {
// 	}

	std::chrono::steady_clock::time_point start{}, stop{};
	std::variant<std::optional<SubjectCandidateListCorrespondence>,
	    std::optional<SubjectPositionCandidateListCorrespondence>> corr{};
	if (args.operation == Operation::SearchSubject) {
		auto ssr = std::get<SearchSubjectResult>(result);
		try {
			start = std::chrono::steady_clock::now();
			corr = impl->extractCorrespondenceSubject(
			    probe, ssr);
			stop = std::chrono::steady_clock::now();
		} catch (const std::exception &e) {
			throw std::runtime_error{"Exception from extract"
			    "CorrespondenceSubject(): " +
			    std::string{e.what()}};
		} catch (...) {
			throw std::runtime_error{"Unknown exception from "
			    "extractCorrespondenceSubject()"};
		}
	} else if (args.operation == Operation::SearchSubjectPosition) {
		auto ssr = std::get<SearchSubjectPositionResult>(result);
		try {
			start = std::chrono::steady_clock::now();
			corr = impl->extractCorrespondenceSubjectPosition(
			    probe, ssr);
			stop = std::chrono::steady_clock::now();
		} catch (const std::exception &e) {
			throw std::runtime_error{"Exception from extract"
			    "CorrespondenceSubjectPosition(): " +
			    std::string{e.what()}};
		} catch (...) {
			throw std::runtime_error{"Unknown exception from "
			    "extractCorrespondenceSubjectPosition()"};
		}
	} else
		throw std::runtime_error{"Unsupported operation sent to "
		    "singleExtractCorrespondence()"};

	return (makeExtractCorrespondenceLogLine(identifier,
	    Util::duration(start, stop), result, args));
}

void
FRIF::Evaluations::Exemplar1N::Validation::waitForExit(
    const uint8_t numChildren)
{
	pid_t pid{-1};
	bool stop{false};
	uint8_t exitedChildren{0};
	int status{};
	while (exitedChildren != numChildren) {
		stop = false;
		while (!stop) {
			pid = ::wait(&status);
			switch (pid) {
			case 0:		/* Status not available */
				break;
			case -1:	/* Delivery of signal */
				switch (errno) {
				case ECHILD:	/* No child processes remain */
					stop = true;
					break;
				case EINTR:	/* Interruption, try again */
					break;
				default:
					throw std::runtime_error{"Error while "
					    "reaping: " + std::system_error(
					        errno, std::system_category()).
					        code().message()};
				}
				break;
			default:	/* Child exited */
				++exitedChildren;
				break;
			}
		}
	}
}

int
main(
    int argc,
    char *argv[])
{
	/*
	 * Check API version.
	 */
	static const uint16_t expectedMajor{0};
	static const uint16_t expectedMinor{0};
	static const uint16_t expectedPatch{1};
	if (!((FRIF::API_MAJOR_VERSION == expectedMajor) &&
	    (FRIF::API_MINOR_VERSION == expectedMinor) &&
	    (FRIF::API_PATCH_VERSION == expectedPatch))) {
		std::cerr << "Incompatible API version encountered.\n "
		    "- Validation: " << expectedMajor << '.' << expectedMinor <<
		    '.' << expectedPatch << "\n - Participant: " <<
		    FRIF::API_MAJOR_VERSION << '.' <<
		    FRIF::API_MINOR_VERSION << '.' <<
		    FRIF::API_PATCH_VERSION << '\n';
		std::cerr << "Rebuild your core library with the latest FRIF "
		    "header files\n";
		return (EXIT_FAILURE);
	}

	FRIF::Evaluations::Exemplar1N::Validation::Arguments args{};
	try {
		args = FRIF::Evaluations::Exemplar1N::Validation::
		    parseArguments(argc, argv);
	} catch (const std::exception &e) {
		std::cerr << "[ERROR] " << e.what() << '\n';
		std::cerr << FRIF::Evaluations::Exemplar1N::Validation::
		    getUsageString(argv[0]) << '\n';
		return (EXIT_FAILURE);
	}

	try {
		return (FRIF::Evaluations::Exemplar1N::Validation::
		    dispatchOperation(args));
	} catch (const std::exception &e) {
		std::cerr << "[ERROR] " << e.what() << '\n';
		return (EXIT_FAILURE);
	} catch (...) {
		std::cerr << "[ERROR] Caught non-standard exception." << '\n';
		return (EXIT_FAILURE);
	}
}
