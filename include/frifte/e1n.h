/*
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 */

/**
 * @page e1n Exemplar 1:N
 * @brief
 * %FRIF Technology Evaluation---Exemplar 1:N---Feature extraction and search
 * Application Programming Interface.
 *
 * @details
 * # Overview
 * This is the Application Programming Interface (API) that must be implemented
 * to participate in the National Institute of Standards and Technology (NIST)'s
 * [Friction Ridge Image and Features (%FRIF) Technology Evaluation (TE)
 * Exemplar 1:N (E1N)](https://fingerprint.nist.gov/frif/e1n). All E1N-specific
 * functionality is defined within FRIF::Evaluations::Exemplar1N, sharing common
 * base code from FRIF.
 *
 * # Implementation
 * Two pure-virtual (abstract) classes called
 * FRIF::Evaluations::Exemplar1N::ExtractionInterface and
 * FRIF::Evaluations::Exemplar1N::SearchInterface have been defined.
 * Participants must implement all methods of both classes in subclasses and
 * submit the implementations in a shared library. The name of the library must
 * follow the requirements outlined in the test plan and be identical to the
 * required information returned from ExtractionInterface::getIdentification.
 * NIST's testing apparatus will link against the submitted library and
 * instantiate instances of the implementations with the
 * FRIF::Evaluations::Exemplar1N::ExtractionInterface::getImplementation and
 * FRIF::Evaluations::Exemplar1N::SearchInterface::getImplementation functions.
 *
 * ## Required Static Methods
 * The following methods are defined static in the API and must be implemented
 * by E1N participants:
 *  - FRIF::Evaluations::Exemplar1N::ExtractionInterface::getImplementation
 *  - FRIF::Evaluations::Exemplar1N::ExtractionInterface::getCompatibility
 *  - FRIF::Evaluations::Exemplar1N::ExtractionInterface::getProductIdentifier
 *  - FRIF::Evaluations::Exemplar1N::SearchInterface::getImplementation
 *  - FRIF::Evaluations::Exemplar1N::SearchInterface::getProductIdentifier
 *  - FRIF::Evaluations::Exemplar1N::getLibraryIdentifier
 */

#ifndef FRIF_EVALUATIONS_E1N_H_
#define FRIF_EVALUATIONS_E1N_H_

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <tuple>
#include <vector>

#include <frifte/frifte.h>

/** %FRIF TE Exemplar 1:N functionality. */
namespace FRIF::Evaluations::Exemplar1N
{
	/** @return Identification information about the submitted library. */
	LibraryIdentifier
	getLibraryIdentifier();

	/** Interface for feature extraction implemented by participant. */
	class ExtractionInterface
	{
	public:
		/**
		 * Information used by the NIST testing apparatus to help
		 * efficiently test this implementation.
		 */
		struct Compatibility
		{
			/**
			 * Is useful information returned from
			 * extractTemplateData() for probes?
			 */
			bool supportsProbeTemplateIntrospection{};

			/**
			 * Is useful information returned from
			 * extractTemplateData() for references?
			 */
			bool supportsReferenceTemplateIntrospection{};

			/**
			 * LibraryIdentifier::versionNumber of
			 * previously-submitted software that generates probe
			 * templates that may be safely and accurately mixed
			 * with this version.
			 *
			 * @note
			 * The value of LibraryIdentifier::versionNumber
			 * returned from getLibraryIdentifier() does not need to
			 * be added to this set (i.e., the implementation is
			 * assumed to be compatible with itself).
			 */
			std::set<uint16_t> probeTemplateVersions{};

			/**
			 * LibraryIdentifier::versionNumber of
			 * previously-submitted software that generates
			 * reference templates that may be safely and accurately
			 * mixed with this version.
			 *
			 * @note
			 * The value of LibraryIdentifier::versionNumber
			 * returned from getLibraryIdentifier() does not need to
			 * be added to this set (i.e., the implementation is
			 * assumed to be compatible with itself).
			 */
			std::set<uint16_t> referenceTemplateVersions{};

			/**
			 * LibraryIdentifier::versionNumber of
			 * previously-submitted software that generates
			 * reference databases that may be safely and accurately
			 * mixed with templates from this version.
			 *
			 * @note
			 * The value of LibraryIdentifier::versionNumber
			 * returned from getLibraryIdentifier() does not need to
			 * be added to this set (i.e., the implementation is
			 * assumed to be compatible with itself).
			 */
			std::set<uint16_t> referenceDatabaseVersions{};
		};

		/**
		 * @brief
		 * Extract features from one or more images and encode them into
		 * a template.
		 *
		 * @param templateType
		 * Where this template will be used in the future.
		 * @param identifier
		 * Unique identifier used to identify the returned template
		 * in future *search* operations (e.g.,
		 * SubjectCandidate#identifier and
		 * SubjectPositionCandidate#identifier).
		 * @param samples
		 * One or more biometric samples to be considered and encoded
		 * into a template.
		 *
		 * @return
		 * A tuple containing a ReturnStatus on if the operation
		 * succeeded, and a CreateTemplateResult that contains a single
		 * template representative of samples when information was able
		 * to be extracted.
		 *
		 * @note
		 * If an implementation cannot extract information but did not
		 * encounter any errors, a ReturnStatus with Result::Success
		 * and no CreateTemplateResult would be expected.
		 *
		 * @note
		 * The value of the returned CreateTemplateResult#data will
		 * only be recorded if ReturnStatus#result is
		 * ReturnStatus::Result::Success. On
		 * ReturnStatus::Result::Failure or when the optional
		 * CreateTemplateResult tuple member is std::nullopt,
		 * subsequent searches will automatically increase false
		 * negative identification rate and a zero-length template will
		 * be provided to ExtractionInterface::createReferenceDatabase.
		 *
		 * @note
		 * This method must return in <= `N * M` milliseconds for each
		 * element of `samples`, on average, as measured on a fixed
		 * subset of data, where
		 *   * `N`
		 *     *  3000 for exemplar images
		 *     *  1500 for feature sets
		 *   * `M`
		 *     * 1 for single fingers
		 *     * 2 for two-finger simultaneous captures
		 *     * 4 for four-finger simultaneous captures
		 *     * 8 for half palm and joint regions
		 *     * 16 for full palm
		 *
		 * @note
		 * If `samples` contained `RightThumb`, and `LeftFour`, the
		 * time requirement would be <= 15s (i.e.,
		 * <= ((3000 * 1) + (3000 * 4)) milliseconds).
		 *
		 * @note
		 * This method shall not spawn threads.
		 */
		virtual
		std::tuple<ReturnStatus, std::optional<CreateTemplateResult>>
		createTemplate(
		    const TemplateType templateType,
		    const std::string &identifier,
		    const std::vector<Sample> &samples)
		    const = 0;

		/**
		 * @brief
		 * Extract information contained within a template.
		 *
		 * @param templateType
		 * templateType passed to createTemplate().
		 * @param templateResult
		 * Object returned from createTemplate().
		 *
		 * @return
		 * A optional with no value if not implemented (as noted in
		 * Compatibility::supportsTemplateIntrospection), or a
		 * ReturnStatus and one or more TemplateData describing the
		 * contents of CreateTemplateResult#data from `templateResult`
		 * otherwise. If CreateTemplateResult#data contains information
		 * separated by position (e.g., when provided a multi-position
		 * image) or multiple views of the same image (e.g., a compact
		 * and verbose template), there may be multiple TemplateData
		 * returned.
		 *
		 * @attention
		 * Be sure to note if this method is supported (i.e., if this
		 * method returns something other than `std::nullopt`) by
		 * setting Compatibility::supportsTemplateIntrospection to
		 * `true`.
		 *
		 * @note
		 * You must implement this method to compile, but providing the
		 * requested information is optional. If provided, information
		 * may help in debugging as well as inform future NIST research.
		 *
		 * @note
		 * NIST will attempt to filter out information provided in
		 * createTemplate(). If possible, do not return information that
		 * was provided in createTemplate(). For instance, if Minutia
		 * was provided, EFS#minutiae should be left `std::nullopt`.
		 * However, if you discovered _different_ Minutia, they should
		 * be returned.
		 *
		 * @note
		 * This method shall return in <= 500 milliseconds.
		 *
		 * @note
		 * This method shall not spawn threads.
		 */
		virtual
		std::optional<std::tuple<ReturnStatus,
		    std::vector<TemplateData>>>
		extractTemplateData(
		    const TemplateType templateType,
		    const CreateTemplateResult &templateResult)
		    const = 0;

		/**
		 * @brief
		 * Create a reference database on the file system.
		 *
		 * @param referenceTemplates
		 * One or more templates returned from createTemplate() with a
		 * `templateType` of TemplateType::Reference.
		 * @param databaseDirectory
		 * Entry to a read/write directory where the reference database
		 * shall be written.
		 * @param maxSize
		 * The maximum number of bytes of storage available to write.
		 *
		 * @return
		 * Information about the result of executing the method.
		 *
		 * @attention
		 * Implementations must, **at a minimum**, *copy* the files
		 * pointed to by `referenceTemplates` to use SearchInterface.
		 * The files pointed to by `referenceTemplates` **will not
		 * exist** when SearchInterface is instantiated.
		 *
		 * @attention
		 * Implementations may not write more than 1000 **total** files.
		 *
		 * @attention
		 * Writing many files is detrimental to the performance of the
		 * file system during this evaluation. It is strongly encouraged
		 * to write a small number of large files to store the reference
		 * database. *NIST reserves the right to reject the
		 * implementation based on the file system requirements of the
		 * reference database scheme employed.*
		 *
		 * @note
		 * This method **may** use more than one thread.
		 *
		 * @note
		 * `maxSize` is not necessarily the amount of RAM that will be
		 * available to SearchInterface to load the database. That value
		 * will be provided at search time as input to
		 * SearchInterface::load.
		 *
		 * @note
		 * This method must return in <= 5 milliseconds *
		 * the number of lines in TemplateArchive#manifest.
		 */
		virtual
		ReturnStatus
		createReferenceDatabase(
		    const TemplateArchive &referenceTemplates,
		    const std::filesystem::path &databaseDirectory,
		    const uint64_t maxSize)
		    const = 0;

		/**************************************************************/

		/**
		 * @brief
		 * Obtain a managed pointer to an object implementing
		 * ExtractionInterface.
		 *
		 * @param configurationDirectory
		 * Read-only directory populated with configuration files
		 * provided by participant.
		 *
		 * @return
		 * Shared pointer to an instance of ExtractionInterface
		 * containing the participant's code to perform extraction
		 * operations.
		 *
		 * @note
		 * A possible implementation might be:
		 * @code{.cpp}
		 * return (std::make_shared<ExtractionImplementation>(
		 *      configurationDirectory));
		 * @endcode
		 *
		 * @note
		 * This method shall return in <= 5 seconds.
		 *
		 * @note
		 * This method shall not spawn threads.
		 */
		static
		std::shared_ptr<ExtractionInterface>
		getImplementation(
		    const std::filesystem::path &configurationDirectory);

		/**
		 * @brief
		 * Obtain identification and version information for the
		 * extraction portion of this submission.
		 *
		 * @return
		 * ProductIdentifier populated with information used
		 * to identify the feature extraction algorithms in reports.
		 *
		 * @note
		 * This method shall return instantly.
		 *
		 * @note
		 * This method shall not spawn threads.
		 */
		static
		std::optional<ProductIdentifier>
		getProductIdentifier();

		/**
		 * @brief
		 * Obtain information about API feature and version
		 * compatibility of this implementation.
		 *
		 * @return
		 * Compatibility populated with information used to help
		 * efficiently run the evaluation of this implementation.
		 *
		 * @note
		 * This method shall return instantly.
		 *
		 * @note
		 * This method shall not spawn threads.
		 */
		static
		Compatibility
		getCompatibility();

		/** @cond SUPPRESS_FROM_DOXYGEN */
		/** Suppress copying polymorphic class (C.63). */
		ExtractionInterface(const ExtractionInterface&) = delete;
		/** Suppress copying polymorphic class (C.63). */
		ExtractionInterface& operator=(const ExtractionInterface&) =
		    delete;
		/** @endcond */

		ExtractionInterface();
		virtual ~ExtractionInterface();
	};

	/** Interface for database search implemented by participant. */
	class SearchInterface
	{
	public:
		/**
		 * Information used by the NIST testing apparatus to help
		 * efficiently test this implementation.
		 */
		struct Compatibility
		{
			/**
			 * Is useful information returned from
			 * extractCorrespondence()?
			 */
			bool supportsCorrespondence{};

			/**
			 * LibraryIdentifier::versionNumber of
			 * previously-submitted software that generates probe
			 * templates that may be safely and accurately searched
			 * with this version.
			 *
			 * @note
			 * The value of LibraryIdentifier::versionNumber
			 * returned from getLibraryIdentifier() does not need to
			 * be added to this set (i.e., the implementation is
			 * assumed to be compatible with itself).
			 */
			std::set<uint16_t> probeTemplateVersions{};

			/**
			 * LibraryIdentifier::versionNumber of
			 * previously-submitted software that generates
			 * reference databases that may be safely and accurately
			 * searched with this version.
			 *
			 * @note
			 * The value of LibraryIdentifier::versionNumber
			 * returned from getLibraryIdentifier() does not need to
			 * be added to this set (i.e., the implementation is
			 * assumed to be compatible with itself).
			 */
			std::set<uint16_t> referenceDatabaseVersions{};
		};

		/**
		 * @brief
		 * Load reference database into memory.
		 *
		 * @param maxSize
		 * Suggested maximum number of bytes of memory to consume in
		 * support of searching the reference database faster.
		 *
		 * @return
		 * Information about the result of executing the method.
		 *
		 * @attention
		 * You should confirm that your database is fully loaded and all
		 * files expected are present before returning.
		 *
		 * @warning
		 * This method will be called after construction and should
		 * **not** be called from an implementation's constructor. This
		 * allows calling getIdentification() without wasting resources.
		 *
		 * @warning
		 * **DO NOT MODIFY** the contents of your database in memory
		 * *after* this method returns! After calling this method, the
		 * test application may `fork()`, allowing calls to
		 * searchSubjectPosition() and searchSubject() to
		 * share the contents of memory using copy-on-write semantics.
		 *
		 * @warning
		 * **DO NOT MODIFY** the contents of the database on disk at any
		 * time! The `databaseDirectory` provided to
		 * getImplementation() may be stored on a read-only file system
		 * and may be destroyed and restored before calls to
		 * searchSubjectPosition() and searchSubject().
		 *
		 * @note
		 * `maxSize` will not be the full amount of memory available on
		 * the system, but it is the maximum amount of memory the
		 * reference database *should* consume. The test application may
		 * `fork()` after calls to this method, during which, this
		 * implementation and the test application are free to perform
		 * dynamic memory allocations. While there is no penalty
		 * for exceeding this memory limit with the reference database,
		 * it is a near certainty that implementations will run out of
		 * memory if they do.
		 *
		 * @note
		 * This method is guaranteed to be called at least once before
		 * calls to any SearchInterface method, except for calls to
		 * getIdentification().
		 *
		 * @note
		 * If the reference database is already loaded when this method
		 * is called, this method shall return immediately.
		 *
		 * @note
		 * This method **may** use more than one thread.
		 *
		 * @note
		 * This method shall return in <= 1 millisecond * the number of
		 * identifiers in the reference database.
		 */
		virtual
		ReturnStatus
		load(
		    const uint64_t maxSize) = 0;

		/**************************************************************/

		/**
		 * @brief
		 * Search the reference database for the samples represented in
		 * `probeTemplate` including the most localized friction ridge
		 * region.
		 *
		 * @param probeTemplate
		 * Object returned from createTemplate() with `templateType` of
		 * TemplateType::Probe.
		 * @param maxCandidates
		 * The maximum number of SubjectPositionCandidate to return.
		 *
		 * @return
		 * A tuple containing a ReturnStatus containing information on
		 * the result of completing the search task, and a
		 * SearchSubjectPositionResult object containing a list of one
		 * to less than or equal to `maxCandidates`
		 * SubjectPositionCandidate. If no SubjectPositionCandidate
		 * were found, the implementation shall return std::nullopt
		 * instead of a SearchSubjectPositionResult, but would still
		 * set Result::Success. If an error prevented the search from
		 * completing (e.g., invalid template, database error), the
		 * ReturnStatus should contain Result::Failure.
		 *
		 * @warning
		 * **DO NOT MODIFY** the database loaded with load() either in
		 * memory or on disk. This method will be called after calling
		 * load() and most likely from a `fork()`ed process, allowing
		 * the read-only database in memory to be shared using
		 * `fork()`'s copy-on-write semantics while not requiring this
		 * method to be threadsafe. Modifying the database will at best
		 * cause an out of memory error and at worst cause undefined
		 * behavior for the many running search processes.
		 *
		 * @note
		 * SearchSubjectPositionResult.candidateList will be sorted by
		 * descending SubjectPositionCandidate.similarity upon return
		 * from this method using `std::stable_sort()`.
		 *
		 * @note
		 * If provided a probe template that contains images from
		 * multiple regions of the same candidate,
		 * SubjectPositionCandidate.frgp will be ignored in analysis.
		 *
		 * @note
		 * SubjectPositionCandidate.frgp shall be the most localized
		 * region where the correspondence was noted to be considered
		 * as correct as possible. See the test plan for more
		 * information.
		 *
		 * @note
		 * This method must return in <= 40 * `number of database
		 * identifiers` microseconds, on average, as measured on a fixed
		 * subset of data.
		 *
		 * @note
		 * This method shall not spawn threads.
		 *
		 * @see searchSubject
		 */
		virtual
		std::tuple<ReturnStatus,
		    std::optional<SearchSubjectPositionResult>>
		searchSubjectPosition(
		    const std::vector<std::byte> &probeTemplate,
		    const uint16_t maxCandidates)
		    const = 0;

		/**
		 * @brief Search the reference database for the samples
		 * represented in `probeTemplate` including the most localized
		 * friction ridge region.
		 *
		 * @param probeTemplate Object returned from createTemplate()
		 * with `templateType` of TemplateType::Probe.  @param
		 * maxCandidates The maximum number of
		 * SubjectCandidate to return.
		 *
		 * @return A tuple containing a ReturnStatus containing
		 * information on the result of completing the search task, and
		 * a SearchSubjectResult object containing a list of one to
		 * less than or equal to `maxCandidates`
		 * SubjectPositionCandidate. If no SubjectCandidate were found,
		 * the implementation shall return std::nullopt instead of a
		 * SearchSubjectResult, but would still set Result::Success. If
		 * an error prevented the search from completing (e.g., invalid
		 * template, database error), the ReturnStatus should contain
		 * Result::Failure.
		 *
		 * @warning **DO NOT MODIFY** the database loaded with load()
		 * either in memory or on disk. This method will be called
		 * after calling load() and most likely from a `fork()`ed
		 * process, allowing the read-only database in memory to be
		 * shared using `fork()`'s copy-on-write semantics while not
		 * requiring this method to be threadsafe. Modifying the
		 * database will at best cause an out of memory error and at
		 * worst cause undefined behavior for the many running search
		 * processes.
		 *
		 * @note SearchSubjectResult.candidateList will be sorted by
		 * descending SubjectCandidate.similarity upon return from this
		 * method using `std::stable_sort()`.
		 *
		 * @note This method must return in <= 40 * `number of database
		 * identifiers` microseconds, on average, as measured on a
		 * fixed subset of data.
		 *
		 * @note This method shall not spawn threads.
		 *
		 * @see searchSubjectPosition
		 */
		virtual
		std::tuple<ReturnStatus, std::optional<SearchSubjectResult>>
		searchSubject(
		    const std::vector<std::byte> &probeTemplate,
		    const uint16_t maxCandidates)
		    const = 0;

		/**
		 * @brief
		 * Extract pairs of corresponding Minutia between
		 * TemplateType::Probe and TemplateType::Reference templates.
		 *
		 * @param probeTemplate
		 * Probe template sent to searchReferences().
		 * @param searchResult
		 * Object returned from searchReferences().
		 *
		 * @return
		 * An optional with no value if not implemented, or a collection
		 * of information containing corresponding features otherwise.
		 *
		 * @attention
		 * Be sure to note if this method is supported (i.e., if this
		 * method returns something other than `std::nullopt`) by
		 * setting Compatibility::supportsCorrespondence to `true`.
		 *
		 * @note
		 * Feature::Minutia must align with minutiae returned from
		 * ExtractionInterface::extractTemplateData() for the given
		 * identifier + position pair.
		 *
		 * @note
		 * You must implement this method to compile, but providing
		 * the requested information is optional. If provided,
		 * information may help in debugging, as well as informing
		 * future NIST research.
		 *
		 * @note
		 * `searchResult` is **not guaranteed** to be the identical
		 * object returned from searchSubjectPosition(). Specifically,
		 * ordering of SearchSubjectPositionResult.candidateList may
		 * have changed.
		 *
		 * @note
		 * The reference database will be stored on a read-only file
		 * system when this method is called. Do not attempt to modify
		 * the reference database here.
		 *
		 * @note
		 * This method shall return in <= 500 milliseconds.
		 *
		 * @note
		 * This method shall not spawn threads.
		 */
		virtual
		std::optional<SubjectPositionCandidateListCorrespondence>
		extractCorrespondenceSubjectPosition(
		    const std::vector<std::byte> &probeTemplate,
		    const SearchSubjectPositionResult &searchResult)
		    const = 0;

		/**
		 * @brief
		 * Extract pairs of corresponding Minutia between
		 * TemplateType::Probe and TemplateType::Reference templates.
		 *
		 * @param probeTemplate
		 * Probe template sent to searchReferences().
		 * @param searchResult
		 * Object returned from searchReferences().
		 *
		 * @return
		 * An optional with no value if not implemented, or a collection
		 * of information containing corresponding features otherwise.
		 *
		 * @attention
		 * Be sure to note if this method is supported (i.e., if this
		 * method returns something other than `std::nullopt`) by
		 * setting Compatibility::supportsCorrespondence to `true`.
		 *
		 * @note
		 * Feature::Minutia must align with minutiae returned from
		 * ExtractionInterface::extractTemplateData() for the given
		 * identifier + position pair.
		 *
		 * @note
		 * You must implement this method to compile, but providing
		 * the requested information is optional. If provided,
		 * information may help in debugging, as well as informing
		 * future NIST research.
		 *
		 * @note
		 * `searchResult` is **not guaranteed** to be the identical
		 * object returned from searchSubject(). Specifically, ordering
		 * of SearchSubjectResult.candidateList may have changed.
		 *
		 * @note
		 * The reference database will be stored on a read-only file
		 * system when this method is called. Do not attempt to modify
		 * the reference database here.
		 *
		 * @note
		 * This method shall return in <= 500 milliseconds.
		 *
		 * @note
		 * This method shall not spawn threads.
		 */
		virtual
		std::optional<SubjectCandidateListCorrespondence>
		extractCorrespondenceSubject(
		    const std::vector<std::byte> &probeTemplate,
		    const SearchSubjectResult &searchResult)
		    const = 0;

		/**************************************************************/

		/**
		 * @brief
		 * Obtain a managed pointer to an object implementing
		 * SearchInterface.
		 *
		 * @param configurationDirectory
		 * Read-only directory populated with configuration files
		 * provided by participant.
		 * @param databaseDirectory
		 * Read-only directory populated with files written in
		 * ExtractionInterface::createReferenceDatabase().
		 *
		 * @return
		 * Shared pointer to an instance of SearchInterface containing
		 * the participant's code to perform search operations.
		 *
		 * @warning
		 * Do **not** load your reference database into memory on
		 * construction. Instead, wait for a call to load().
		 * @warning
		 * It is possible that `databaseDirectory` does not exist when
		 * this function is called, but it will exist before load() is
		 * called. Do **not** emit errors based on the contents of
		 * `databaseDirectory` from this function. This is necessary to
		 * call getIdentification() prior to creating an enrollment
		 * database.
		 *
		 * @note
		 * A possible implementation might be:
		 * @code{.cpp}
		 * return (std::make_shared<SearchImplementation>(
		 *      configurationDirectory, databaseDirectory));
		 * @endcode
		 *
		 * @note
		 * This method shall return in <= 5 seconds.
		 *
		 * @note
		 * This method shall not spawn threads.
		 */
		static
		std::shared_ptr<SearchInterface>
		getImplementation(
		    const std::filesystem::path &configurationDirectory,
		    const std::filesystem::path &databaseDirectory);

		/**
		 * @brief
		 * Obtain identification and version information for the
		 * search portion of this submission.
		 *
		 * @return
		 * ProductIdentifier populated with information used to identify
		 * the search algorithm in reports.
		 *
		 * @note
		 * This method shall return instantly.
		 *
		 * @note
		 * This method shall not spawn threads.
		 */
		static
		std::optional<ProductIdentifier>
		getProductIdentifier();

		/**
		 * @brief
		 * Obtain information about API feature and version
		 * compatibility of this implementation.
		 *
		 * @return
		 * Compatibility populated with information used to help
		 * efficiently run the evaluation of this implementation.
		 *
		 * @note
		 * This method shall return instantly.
		 *
		 * @note
		 * This method shall not spawn threads.
		 */
		static
		Compatibility
		getCompatibility();

		/** @cond SUPPRESS_FROM_DOXYGEN */
		/** Suppress copying polymorphic class (C.63). */
		SearchInterface(const SearchInterface&) = delete;
		/** Suppress copying polymorphic class (C.63). */
		SearchInterface& operator=(const SearchInterface&) = delete;
		/** @endcond */

		SearchInterface();
		virtual ~SearchInterface();
	};

	/*
	 * API versioning.
	 *
	 * NIST code will extern the version number symbols. Participant code
	 * shall compile them into their core library.
	 */
	#ifdef NIST_EXTERN_FRIFTE_E1N_API_VERSION
	/** API major version number. */
	extern uint16_t API_MAJOR_VERSION;
	/** API minor version number. */
	extern uint16_t API_MINOR_VERSION;
	/** API patch version number. */
	extern uint16_t API_PATCH_VERSION;
	#else /* NIST_EXTERN_FRIFTE_E1N_API_VERSION */
	/** API major version number. */
	uint16_t API_MAJOR_VERSION{1};
	/** API minor version number. */
	uint16_t API_MINOR_VERSION{2};
	/** API patch version number. */
	uint16_t API_PATCH_VERSION{0};
	#endif /* NIST_EXTERN_FRIFTE_E1N_API_VERSION */
}

#endif /* FRIF_EVALUATIONS_E1N_H_ */
