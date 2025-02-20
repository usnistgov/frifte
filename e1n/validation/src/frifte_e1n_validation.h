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
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <variant>

#include <frifte/e1n.h>

namespace FRIF::Evaluations::Exemplar1N::Validation
{
	using InterfaceVariant = std::variant<
	    std::shared_ptr<ExtractionInterface>,
	    std::shared_ptr<SearchInterface>>;

	/** Operations that this executable can perform. */
	enum class Operation
	{
		/** Create database from reference templates. */
		CreateDatabases,
		/** Extract features and generate templates. */
		CreateTemplates,
		/** Print library identification information. */
		IdentifyLibrary,
		/** Print identification provided by ExtractionInterface. */
		IdentifyExtraction,
		/** Print identification provided by SearchInterface. */
		IdentifySearch,
		/** Search, returning subject identifier only. */
		SearchSubject,
		/** Search, returning subject identifier and position. */
		SearchSubjectPosition,
		/** Print usage. */
		Usage
	};

	/** Arguments passed on the command line */
	struct Arguments
	{
		/** Number used to seed the random number generator. */
		std::mt19937_64::result_type randomSeed{std::random_device()()};
		/** Operation to be performed. */
		std::optional<Operation> operation{};
		/** Name of the executable */
		std::string executableName{};
		/** Configuration directory. */
		std::filesystem::path configDir{};
		/** Enrollment database directory. */
		std::optional<std::filesystem::path> dbDir{};
		/** Directory where all output will be written. */
		std::filesystem::path outputDir{"output"};
		/** Directory containing images named in ImageSet. */
		std::filesystem::path imageDir{"images"};

		/** Number of processes to fork. */
		uint8_t numProcs{1};
		/** Maximum number of {candidates, DB size}. */
		uint64_t maximum{};

		/**
		 * Type of template to make (Operation::CreateTemplates only).
		 */
		std::optional<TemplateType> templateType{};
	};

	/**
	 * @brief
	 * Configure and run database creation.
	 *
	 * @param args
	 * Arguments parsed from command line.
	 *
	 * @throw
	 * Numerous propaged reasons, consult message.
	 *
	 * @note
	 * Must be called after templates exist.
	 * @note
	 * Does not check that all expected templates exist.
	 */
	void
	dispatchCreateDatabases(
	    const Arguments &args);

	/**
	 * @brief
	 * Configure and run template creation operation.
	 *
	 * @param args
	 * Arguments parsed from command line.
	 *
	 * @throw
	 * Numerous propaged reasons, consult message.
	 */
	void
	dispatchCreateTemplates(
	    const Arguments &args);

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
	 * Configure and run template searching.
	 *
	 * @param args
	 * Arguments parsed from command line.
	 *
	 * @throw
	 * Numerous propaged reasons, consult message.
	 */
	void
	dispatchSearch(
	    const Arguments &args);

	/**
	 * @brief
	 * Execute a single call to searchSubject.
	 *
	 * @param impl
	 * Initialized ExtractionInterface implementation.
	 * @param probe
	 * Contents of probe template.
	 * @param maxCandidates
	 * Maximum number of candidate list entries to return.
	 *
	 * @return
	 * Tuple with call start time, stop time and values returned from
	 * searchSubject.
	 *
	 * @throw
	 * Rethrown exception from searchSubject.
	 */
	std::tuple<std::chrono::steady_clock::time_point,
	std::chrono::steady_clock::time_point, ReturnStatus,
	std::optional<SearchSubjectResult>>
	executeSingleSearchSubject(
	    std::shared_ptr<SearchInterface> impl,
	    const std::vector<std::byte> &probe,
	    const uint16_t maxCandidates);

	/**
	 * @brief
	 * Execute a single call to searchSubjectPosition.
	 *
	 * @param impl
	 * Initialized ExtractionInterface implementation.
	 * @param probe
	 * Contents of probe template.
	 * @param maxCandidates
	 * Maximum number of candidate list entries to return.
	 *
	 * @return
	 * Tuple with call start time, stop time and values returned from
	 * searchSubjectPosition.
	 *
	 * @throw
	 * Rethrown exception from searchSubjectPosition.
	 */
	std::tuple<std::chrono::steady_clock::time_point,
	std::chrono::steady_clock::time_point, ReturnStatus,
	std::optional<SearchSubjectPositionResult>>
	executeSingleSearchSubjectPosition(
	    std::shared_ptr<SearchInterface> impl,
	    const std::vector<std::byte> &probe,
	    const uint16_t maxCandidates);

	/**
	 * @brief
	 * Spawn forks that perform a FRIF TE E1N task on a set of samples.
	 *
	 * @param args
	 * Arguments parsed from the command line.
	 * @param dataset
	 * Dataset of samples.
	 * @param indices
	 * Indices into `dataset` that should be exercised.
	 * @param fn
	 * Function to call that operations on the `indices` subset of
	 * `dataset`.
	 *
	 * @throw
	 * Numerous propaged reasons, consult message.
	 *
	 * @note
	 * When `args.numProcs` == 1, no new processes are forked. `fn` will be
	 * called in the current process.
	 */
	void
	forkOperation(
	    const Arguments &args,
	    const Data::Dataset &dataset,
	    const std::vector<uint64_t> &indices,
	    const std::function<void(InterfaceVariant, const Data::Dataset&,
		const std::vector<uint64_t>&, const Arguments&)> &fn);

	/**
	 * @brief
	 * Format ExtractionInterface compatibility information
	 *
	 * @return
	 * Multiple "key = value" lines of information about the linked
	 * implementation.
	 */
	std::string
	getExtractionInterfaceCompatibilityString();

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
	 * Format SearchInterface compatibility information
	 *
	 * @return
	 * Multiple "key = value" lines of information about the linked
	 * implementation.
	 */
	std::string
	getSearchInterfaceCompatibilityString();

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
	 * Instantiate instance of linked ExtractionInterface or SearchInterface
	 * implementation.
	 *
	 * @param args
	 * Arguments parsed from command line.
	 *
	 * @return
	 * Implementation of ExtractionInterface or SearchInterface (after
	 * calling SearchInterface::load()).
	 *
	 * @throw
	 * Required directories do not exist, or error propagated from
	 * ExtractionInterface::getImplementation or
	 * SearchInterface::getImplementation.
	 */
	InterfaceVariant
	instantiateImplementation(
	    const Arguments &args);

	/**
	 * @brief
	 * Create output directories for template creation.
	 *
	 * @param args
	 * Arguments parsed from command line.
	 * @param dataset
	 * Name of the dataset being processed.
	 *
	 * @throw
	 * Error creating directories.
	 */
	void
	makeCreateTemplatesDirs(
	    const Arguments &args,
	    const Data::Dataset &dataset);

	/**
	 * @brief
	 * Create log file.
	 *
	 * @param prefix
	 * File name prefix
	 * @param header
	 * Line to log immediately to the newly created file.
	 * @param outputDir
	 * Output directory passed from command line arguments.
	 *
	 * @return
	 * Path to log file to append to for this process.
	 *
	 * @throw
	 * Error creating log file.
	 */
	std::string
	makeLog(
	    const std::string &prefix,
	    const std::string &header,
	    const std::filesystem::path &outputDir);

	/**
	 * @brief
	 * Generate a log-able string for search result correspondence.
	 *
	 * @param identifier
	 * Identifier of the search probe.
	 * @param duration
	 * String representation of the elapsed time calling searchSubject,
	 * searchSubjectPosition, or extractCorrespondence.
	 * @param searchResult
	 * The result of calling searchSubject or searchSubjectPosition.
	 * @param args
	 * Arguments parsed from command line.
	 *
	 * @return
	 * String (likely multiple lines) that can be logged to the extract
	 * correspondence log.
	 *
	 * @throw
	 * Inconsistency with correspondence data.
	 */
	std::string
	makeExtractCorrespondenceLogLine(
	    const std::string &identifier,
	    const std::string &duration,
	    const std::variant<SearchSubjectResult,
	        SearchSubjectPositionResult> &searchResult,
	    const Arguments &args);

	/**
	 * @brief
	 * Generate single-file archive of templates with manifest.
	 *
	 * @param datasetName
	 * Name of dataset whose templates are included in the database.
	 * @param args
	 * Arguments parsed from command line.
	 *
	 * @throw std::runtime_exception
	 * Error reading or writing to disk.
	 */
	void
	makeReferenceTemplateArchive(
	    const std::string &datasetName,
	    const Arguments &args);

	/**
	 * @brief
	 * Generate log-able string for output of
	 * ExtractionInterface::extractTemplateData
	 *
	 * @param identifier
	 * Identifier provided during template generation.
	 * @param duration
	 * String representing time elapsed when calling
	 * ExtractionInterface::extractTemplateData (or
	 * ExtractionInterface::createTemplate),
	 * in microseconds.
	 * @param extractedData
	 * Data returned from ExtractionInterface::extractTemplateData (or
	 * ExtractionInterface::createTemplate).
	 * @param rs
	 * ReturnStatus returned from calling
	 * ExtractionInterface::extractTemplateData (or
	 * ExtractionInterface::createTemplate)
	 * @param args
	 * Arguments parsed from command line.
	 *
	 * @return
	 * String (potentially >1 line) log-able to the extracted template data
	 * log file.
	 *
	 * @throw
	 * Error parsing or inconsistency within `extractedData`.
	 */
	std::string
	makeExtractTemplateDataLogLine(
	    const std::string &identifier,
	    const std::string &duration,
	    const std::optional<std::vector<TemplateData>> &extractedData,
	    const std::optional<ReturnStatus> &rs,
	    const Arguments &args);

	/**
	 * @brief
	 * Make Sample from Data::Input.
	 *
	 * @param metadatas
	 * List of Data::Input.
	 * @param args
	 * Arguments parsed from command line.
	 *
	 * @return
	 * Sample versions of `metadatas`.
	 *
	 * @note
	 * Reads image from disk.
	 */
	std::vector<Sample>
	makeSamples(
	    const std::vector<Data::Input> &metadatas,
	    const Arguments &args);

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

	/**
	 * @brief
	 * Create templates for a subset of validation dataset images.
	 *
	 * @param iv
	 * Variant containing initialized ExtractionInterface implementation.
	 * @param dataset
	 * Dataset of samples.
	 * @param indices
	 * Indices into `dataset` that should be exercised.
	 * @param args
	 * Arguments parsed from command line.
	 *
	 * @throw
	 * Error writing to log, error with samples, or error propagated from
	 * `impl`.
	 */
	void
	runPartialCreateTemplates(
	    const InterfaceVariant &iv,
	    const Data::Dataset &dataset,
	    const std::vector<uint64_t> &indices,
	    const Arguments &args);

	/**
	 * @brief
	 * Search a subset of probe templates.
	 *
	 * @param iv
	 * Variant containing initialized SearchInterface implementation.
	 * @param probeDataset
	 * Set of probe templates.
	 * @param probeIndices
	 * Indices into `probeDataset` that should be exercised.
	 * @param args
	 * Arguments parsed from command line.
	 *
	 * @throw
	 * Error writing to log, error with samples, or error propagated from
	 * `impl`.
	 */
	void
	runPartialSearch(
	    const InterfaceVariant &iv,
	    const Data::Dataset &probeDataset,
	    const std::vector<uint64_t> &probeIndices,
	    const Arguments &args);

	/**
	 * @brief
	 * Create a database for one dataset worth of templates.
	 *
	 * @param impl
	 * Initialized ExtractionInterface implementation.
	 * @param datasetName
	 * Name of dataset of samples (from template creation).
	 * @param args
	 * Arguments parsed from command line.
	 *
	 * @throw
	 * Exception thrown from implementation, failure to write to log, or
	 * returning failure from database creation operation.
	 */
	void
	singleCreateDatabase(
	    std::shared_ptr<ExtractionInterface> impl,
	    const std::string &datasetName,
	    const Arguments &args);

	/**
	 * @brief
	 * Create template for one sample.
	 *
	 * @param impl
	 * Initialized ExtractionInterface implementation.
	 * @param dataset
	 * Dataset of samples.
	 * @param datasetIndex
	 * Index into `dataset` corresponding to the single sample.
	 * @param args
	 * Arguments parsed from command line.
	 *
	 * @return
	 * Pair of strings. First is log string suitable for writing to template
	 * creation log. Second is log string suitable for writing to the
	 * template extract data log.
	 *
	 * @throw
	 * Data inconsistency observed, error writing data, or exception thrown
	 * from implementation.
	 */
	std::pair<std::string, std::optional<std::string>>
	singleCreateTemplate(
	    std::shared_ptr<ExtractionInterface> impl,
	    const Data::Dataset &dataset,
	    const uint64_t datasetIndex,
	    const Arguments &args);

	/**
	 * @brief
	 * Extract correspondence from a single search result.
	 *
	 * @param impl
	 * Initialized ExtractionInterface implementation.
	 * @param dataset
	 * Dataset of samples.
	 * @param datasetIndex
	 * Index into `dataset` corresponding to the single sample.
	 * @param result
	 * Result returned from SearchInterface::extractCorrespondence.
	 * @param args
	 * Arguments parsed from command line.
	 *
	 * @return
	 * String that can be logged to correspondence log.
	 *
	 * @throw
	 * Error calling implementation, or inconsistencies in returned data.
	 */
	std::string
	singleExtractCorrespondence(
	    std::shared_ptr<SearchInterface> impl,
	    const Data::Dataset &dataset,
	    const uint64_t datasetIndex,
	    const std::variant<SearchSubjectResult, SearchSubjectPositionResult>
		&result,
	    const Arguments &args);

	/**
	 * @brief
	 * Extract TemplateData for a single template creation process.
	 *
	 * @param impl
	 * Initialized ExtractionInterface implementation.
	 * @param dataset
	 * Dataset of samples.
	 * @param datasetIndex
	 * Index into `dataset` corresponding to the single sample.
	 * @param templateResult
	 * Result returned from ExtractionInterface::createTemplate.
	 * @param args
	 * Arguments parsed from command line.
	 *
	 * @return
	 * Log string suitable for writing to template creation log. String
	 * is likely to be multiple lines.
	 *
	 * @throw
	 * Data inconsistency observed, error writing data, or exception thrown
	 * from implementation.
	 */
	std::string
	singleExtractTemplateData(
	    std::shared_ptr<ExtractionInterface> impl,
	    const Data::Dataset &dataset,
	    const uint64_t datasetIndex,
	    const CreateTemplateResult &templateResult,
	    const Arguments &args);

	/**
	 * @brief
	 * Run a search of a single probe template.
	 *
	 * @param impl
	 * Initialized ExtractionInterface implementation.
	 * @param dataset
	 * Dataset of samples.
	 * @param datasetIndex
	 * Index into `dataset` corresponding to the single sample.
	 * @param args
	 * Arguments parsed from command line.
	 *
	 * @return
	 * Pair of strings. First is log string suitable for writing to search
	 * log. Second is log string suitable for writing to the correspondence
	 * log.
	 *
	 * @throw
	 * Error writing log or exception thrown from implementation.
	 */
	std::pair<std::string, std::optional<std::string>>
	singleSearch(
	    std::shared_ptr<SearchInterface> impl,
	    const Data::Dataset &dataset,
	    const uint64_t datasetIndex,
	    const Arguments &args);

	/**
	 * @brief
	 * Wait for forked children to exit.
	 *
	 * @param numChildren
	 * The number of children required to exit.
	 */
	void
	waitForExit(
	    const uint8_t numChildren);
}

#endif /* FRIF_E1N_VALIDATION_H_ */
