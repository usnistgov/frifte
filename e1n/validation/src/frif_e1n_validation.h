/*
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 */

#ifndef FRIF_E1N_VALIDATION_H_
#define FRIF_E1N_VALIDATION_H_

#include <filesystem>
#include <optional>
#include <string>

namespace FRIF::Evaluations::Exemplar1N::Validation
{
	/** Operations that this executable can perform. */
	enum class Operation
	{
		/** Print library identification information. */
		IdentifyLibrary,
		/** Print identification provided by ExtractionInterface. */
		IdentifyExtraction,
		/** Print identification provided by SearchInterface. */
		IdentifySearch,
		/** Print usage. */
		Usage
	};

	/** Arguments passed on the command line */
	struct Arguments
	{
		/** Operation to be performed. */
		std::optional<Operation> operation{};
		/** Name of the executable */
		std::string executableName{};
		/** Configuration directory. */
		std::filesystem::path configDir{};
		/** Enrollment database directory. */
		std::filesystem::path dbDir{};
	};

	/**
	 * @brief
	 * Call the appropriate starting method based on the operation argument
	 * passed on the command-line.
	 *
	 * @param args
	 * Arguments parsed from command line.
	 *
	 * @return
	 * Exit status from the operation.
	 */
	int
	dispatchOperation(
	    const Arguments &args);

	/**
	 * @brief
	 * Format identification information about a FRIF TE E1N
	 * implementation's ExtractionInterface.
	 *
	 * @return
	 * Multiple "key = value" lines of information about the linked
	 * implementation.
	 */
	std::string
	getExtractionInterfaceIdentificationString();

	/**
	 * @brief
	 * Format identification information about a FRIF TE E1N
	 * implementation's LibraryIdentifier.
	 *
	 * @return
	 * Multiple "key = value" lines of information about the linked
	 * implementation.
	 */
	std::string
	getLibraryIdentifierString();

	/**
	 * @brief
	 * Format identification information about a FRIF TE E1N
	 * implementation's SearchInterface.
	 *
	 * @return
	 * Multiple "key = value" lines of information about the linked
	 * implementation.
	 */
	std::string
	getSearchInterfaceIdentificationString();

	/**
	 * @brief
	 * Obtain the validation driver's usage string.
	 *
	 * @param name
	 * Name of the executable.
	 *
	 * @return
	 * Usage string.
	 */
	std::string
	getUsageString(
	    const std::string &name);

	/**
	 * @brief
	 * Parse command line arguments.
	 *
	 * @param argc
	 * argc from main().
	 * @param argv
	 * argv from main()
	 *
	 * @return
	 * Command line arguments parsed into an Argument.
	 *
	 * @throw std::exception
	 * Errors or inconsistencies when parsing arguments.
	 */
	Arguments
	parseArguments(
	    const int argc,
	    char * const argv[]);
}

#endif /* FRIF_E1N_VALIDATION_H_ */
