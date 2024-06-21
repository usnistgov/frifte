/*
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 */

#include <iostream>
#include <sstream>

#include <getopt.h>

#include <frif/e1n.h>

#include "frif_e1n_validation.h"

int
FRIF::Evaluations::Exemplar1N::Validation::dispatchOperation(
    const Arguments &args)
{
	int rv{EXIT_FAILURE};

	// TODO: Add timer on each operation

	switch (args.operation.value_or(Operation::Usage)) {
	case Operation::Usage:
		std::cout << getUsageString(args.executableName) << '\n';
		rv = EXIT_SUCCESS;
		break;
	case Operation::IdentifyExtraction:
		try {
			std::cout <<
			    getExtractionInterfaceIdentificationString() <<
			    '\n';
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
	static const std::string usagePrompt{"Usage:"};
	const std::string prefix(usagePrompt.length() + 4, ' ');

	std::string s{usagePrompt + ' ' + name + " ...\n"};

	s += prefix + "# Identify\n" +
	    prefix + "-i l # library\n" +
	    prefix + "-i e # ExtractionInterface\n" +
	    prefix + "-i s # SearchInterface";

	return (s);
}

FRIF::Evaluations::Exemplar1N::Validation::Arguments
FRIF::Evaluations::Exemplar1N::Validation::parseArguments(
    const int argc,
    char * const argv[])
{
	static const char options[] {"d:i:z:"};
	Arguments args{};
	args.executableName = argv[0];

	int c{};
	while ((c = getopt(argc, argv, options)) != -1) {
		switch (c) {
		case 'd':	/* Enrollment database directory */
			args.dbDir = optarg;
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

		case 'z':	/* Config dir */
			args.configDir = optarg;
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

	return (args);
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
