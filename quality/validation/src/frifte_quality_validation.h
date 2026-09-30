/*
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 */

#ifndef FRIF_QUALITY_VALIDATION_H_
#define FRIF_QUALITY_VALIDATION_H_

#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <variant>

#include <frifte/quality.h>

#include "frifte_quality_data.h"

namespace FRIF::Evaluations::Quality::Validation
{
	/** Operations that this executable can perform. */
	enum class Operation
	{
		/** Print library identification information. */
		IdentifyLibrary,
		/** Print quality algorithm identification information. */
		IdentifyQuality,
		/** Print compatibility information. */
		Compatibility,
		/** Compute unified quality scores. */
		UnifiedQuality,
		/** Compute unified quality and all quality components. */
		VerboseQuality,
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
		/** Directory where all output will be written. */
		std::filesystem::path outputDir{"output"};
		/** Directory containing images named in ImageSet. */
		std::filesystem::path imageDir{"images"};
		/** The type of image to test (quality operation only) */
		std::optional<Data::ImageContent> imageContent{};

		/** Number of processes to fork. */
		uint8_t numProcs{1};
	};

	/**
	 * @brief
	 * Configure and run quality computation.
	 *
	 * @param args
	 * Arguments parsed from command line.
	 *
	 * @throw
	 * Numerous propaged reasons, consult message.
	 */
	void
	dispatchQualityComputation(
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
	 * Execute a single call to searchSubject.
	 *
	 * @param impl
	 * Initialized Interface implementation.
	 * @param sample
	 * Sample on which to compute quality.
	 *
	 * @return
	 * Tuple with call start time, stop time, and value returned from
	 * comupteUnifiedQualityScore.
	 *
	 * @throw
	 * Rethrown exception from comupteUnifiedQualityScore.
	 */
	std::tuple<std::chrono::steady_clock::time_point,
	std::chrono::steady_clock::time_point, EFS::QualityMeasure>
	executeSingleUnifiedQuality(
	    std::shared_ptr<Interface> impl,
	    const Sample &sample);

	/**
	 * @brief
	 * Spawn forks that perform a FRIF TE Quality task on a set of samples.
	 *
	 * @param args
	 * Arguments parsed from the command line.
	 * @param dataset
	 * Dataset of samples.
	 * @param imageSetIndicies
	 * Indices into `dataset`'s ImageSet vector that should be exercised.
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
	    const std::vector<uint64_t> &imageSetIndicies,
	    const std::function<void(std::shared_ptr<Interface>,
	        const Data::Dataset&, const std::vector<uint64_t>&,
	        const Arguments&)> &fn);

	/**
	 * @brief
	 * Format compatibility information.
	 *
	 * @return
	 * Multiple "key = value" lines of information about the linked
	 * implementation.
	 */
	std::string
	getCompatibilityString();

	/**
	 * @brief
	 * Format identification information about a FRIF TE Quality
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
	 * Format identification information about a FRIF TE Quality
	 * implementation's LibraryIdentifier.
	 *
	 * @return
	 * Multiple "key = value" lines of information about the linked
	 * implementation.
	 */
	std::string
	getProductIdentifierString();

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
	 * Perform quality computation on a subset of the identifiers
	 * from a dataset.
	 *
	 * @param impl
	 * Initialized Interface implementation.
	 * @param dataset
	 * Dataset of samples.
	 * @param indices
	 * Indices into `dataset`'s ImageSet vector that should be exercised.
	 * @param args
	 * Arguments parsed from the command line.
	 */
	void
	runPartialQualityComputation(
	    std::shared_ptr<Quality::Interface> impl,
	    const Data::Dataset &dataset,
	    const std::vector<uint64_t> &indices,
	    const Arguments &args);

	/**
	 * @brief
	 * Generate input for API quality computation methods.
	 *
	 * @param md
	 * Single Validation data input.
	 * @param args
	 * Arguments parsed from the command line.
	 *
	 * @return
	 * A value suitable for passing to Interface::computeUnifiedQualityScore
	 * or Interface::computeVerboseQuality.
	 */
	Sample
	makeSample(
	    const Data::Input &md,
	    const Arguments &args);

	/**
	 * @brief
	 * Compute quality for a single image/
	 *
	 * @param impl
	 * Initialized Interface implementation.
	 * @param compatibility
	 * Compatibility returned from `impl`
	 * @param dataset
	 * Dataset of samples.
	 * @param imageSetIndex
	 * Index into `dataset` whose quality is to be computed.
	 * @param args
	 * Arguments parsed from the command line.
	 *
	 * @return
	 * Log-able CSV indicating the results of the operation.
	 */
	std::string
	singleComputeQuality(
	    std::shared_ptr<Interface> impl,
	    const Interface::Compatibility &compatibility,
	    const Data::Dataset &dataset,
	    const uint64_t imageSetIndex,
	    const Arguments &args);

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
	 * Generate log-able string for output of
	 * Quality::Interface::computeUnifiedQualityScore
	 *
	 * @param identifier
	 * Identifier provided during quality computation
	 * @param duration
	 * String representing time elapsed when calling
	 * Quality::Interface::computeUnifiedQualityScore in microseconds.
	 * @param qm
	 * Data returned from Quality::Interface::computeUnifiedQualityScore
	 * @param args
	 * Arguments parsed from command line.
	 *
	 * @return
	 * Single line log-able string.
	 *
	 * @throw
	 * Error parsing or inconsistency within `qm`.
	 */
	std::string
	makeLogLine(
	    const std::string &identifier,
	    const std::string &duration,
	    const EFS::QualityMeasure &qm,
	    const Arguments &args);

	/**
	 * @brief
	 * Generate log-able string for output of
	 * Quality::Interface::computeVerboseQuality
	 *
	 * @param identifier
	 * Identifier provided during quality computation
	 * @param duration
	 * String representing time elapsed when calling
	 * Quality::Interface::computeVerboseQuality in microseconds.
	 * @param vq
	 * Data returned from Quality::Interface::computeVerboseQuality
	 * @param args
	 * Arguments parsed from command line.
	 *
	 * @return
	 * Single line log-able string.
	 *
	 * @throw
	 * Error parsing or inconsistency within `vq`.
	 */
	std::string
	makeLogLine(
	    const std::string &identifier,
	    const std::string &duration,
	    const VerboseQuality vq,
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
	 * @param impl
	 * Initialized Interface implementation.
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
	    std::shared_ptr<Interface> impl,
	    const Data::Dataset &dataset,
	    const std::vector<uint64_t> &indices,
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
	    std::shared_ptr<Interface> impl,
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

#endif /* FRIF_QUALITY_VALIDATION_H_ */
