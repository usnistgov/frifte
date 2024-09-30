/*
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 */

#include <frif_e1n_nullimpl.h>

FRIF::Evaluations::Exemplar1N::NullExtractionImplementation::
    NullExtractionImplementation(
    const std::filesystem::path &configurationDirectory_) :
    FRIF::Evaluations::Exemplar1N::ExtractionInterface(),
    configurationDirectory{configurationDirectory_}
{

}

std::tuple<FRIF::ReturnStatus, std::optional<FRIF::CreateTemplateResult>>
FRIF::Evaluations::Exemplar1N::NullExtractionImplementation::createTemplate(
    const TemplateType templateType,
    const std::string &identifier,
    const std::vector<Sample> &samples)
    const
{
	return {ReturnStatus{}, CreateTemplateResult{}};
}

std::optional<std::tuple<FRIF::ReturnStatus,
    std::vector<FRIF::TemplateData>>>
FRIF::Evaluations::Exemplar1N::NullExtractionImplementation::
    extractTemplateData(
    const TemplateType templateType,
    const CreateTemplateResult &templateResult)
    const
{
	/* Clarify support within getCompatibility() */
	return {};
}

FRIF::ReturnStatus
FRIF::Evaluations::Exemplar1N::NullExtractionImplementation::
    createReferenceDatabase(
    const TemplateArchive &referenceTemplates,
    const std::filesystem::path &databaseDirectory,
    const uint64_t maxSize)
    const
{
	try {
		std::filesystem::copy_file(referenceTemplates.archive,
		    databaseDirectory / "archive");
	} catch (const std::filesystem::filesystem_error &e) {
		return {ReturnStatus::Result::Failure, "Error when copying " +
		    referenceTemplates.archive.string() + ": " + e.what()};
	}

	try {
		std::filesystem::copy_file(referenceTemplates.manifest,
		    databaseDirectory / "manifest");
	} catch (const std::filesystem::filesystem_error &e) {
		return {ReturnStatus::Result::Failure, "Error when copying " +
		    referenceTemplates.manifest.string() + ": " + e.what()};
	}

	return {};
}

std::optional<FRIF::ProductIdentifier>
FRIF::Evaluations::Exemplar1N::ExtractionInterface::getProductIdentifier()
{
	/*
	 * Optional.
	 */
	ProductIdentifier exemplarID{};
	exemplarID.cbeff = ProductIdentifier::CBEFFIdentifier{};
	exemplarID.cbeff->owner =
	    NullImplementationConstants::CBEFFProductOwner;
	exemplarID.cbeff->algorithm =
	    NullImplementationConstants::featureExtractionProductVersion;
	exemplarID.marketing =
	    NullImplementationConstants::featureExtractionProductName;

	return (exemplarID);
}

FRIF::Evaluations::LibraryIdentifier
FRIF::Evaluations::Exemplar1N::getLibraryIdentifier()
{
	LibraryIdentifier li{};
	li.versionNumber =
	    Exemplar1N::NullImplementationConstants::libraryVersionNumber;
	li.identifier =
	    Exemplar1N::NullImplementationConstants::libraryIdentifier;

	return (li);
}

FRIF::Evaluations::Exemplar1N::ExtractionInterface::Compatibility
FRIF::Evaluations::Exemplar1N::ExtractionInterface::getCompatibility()
{
	Compatibility compatibility{};

	compatibility.supportsProbeTemplateIntrospection = false;
	compatibility.supportsReferenceTemplateIntrospection = false;

	compatibility.probeTemplateVersions = {
	    NullImplementationConstants::featureExtractionProductVersion,
	    NullImplementationConstants::featureExtractionProductVersion - 1};

	compatibility.referenceTemplateVersions = {
	    NullImplementationConstants::featureExtractionProductVersion,
	    NullImplementationConstants::featureExtractionProductVersion - 1,
	    NullImplementationConstants::featureExtractionProductVersion - 2};

	compatibility.referenceDatabaseVersions = {
	    NullImplementationConstants::featureExtractionProductVersion,
	    NullImplementationConstants::featureExtractionProductVersion - 1,
	    NullImplementationConstants::featureExtractionProductVersion - 2};

	return (compatibility);
}

std::shared_ptr<FRIF::Evaluations::Exemplar1N::ExtractionInterface>
FRIF::Evaluations::Exemplar1N::ExtractionInterface::getImplementation(
    const std::filesystem::path &configurationDirectory)
{
	return (std::make_shared<NullExtractionImplementation>(
	    configurationDirectory));
}

/*******************************************************************************
 *******************************************************************************
 ******************************************************************************/

FRIF::Evaluations::Exemplar1N::NullSearchImplementation::
    NullSearchImplementation(
    const std::filesystem::path &configurationDirectory_,
    const std::filesystem::path &databaseDirectory_) :
    FRIF::Evaluations::Exemplar1N::SearchInterface(),
    configurationDirectory{configurationDirectory_},
    databaseDirectory{databaseDirectory_}
{
	/*
	 * Do NOT load templates into RAM here. Use load() instead, and do NOT
	 * call load() from here.
	 */
}

FRIF::ReturnStatus
FRIF::Evaluations::Exemplar1N::NullSearchImplementation::load(
    const uint64_t maxSize)
{
	if (!std::filesystem::is_directory(this->databaseDirectory))
		throw std::runtime_error{this->databaseDirectory.string() + " "
		    "does not exist"};
	if (!std::filesystem::exists(this->databaseDirectory / "archive"))
		throw std::runtime_error{(this->databaseDirectory /
		    "archive").string() + " does not exist"};
	if (!std::filesystem::exists(this->databaseDirectory / "manifest"))
		throw std::runtime_error{(this->databaseDirectory /
		    "manifest").string() + " does not exist"};

	/* XXX: Load maxSize bytes of data into RAM. */

	return {};
}

std::tuple<FRIF::ReturnStatus, std::optional<FRIF::SearchSubjectPositionResult>>
FRIF::Evaluations::Exemplar1N::NullSearchImplementation::searchSubjectPosition(
    const std::vector<std::byte> &probeTemplate,
    const uint16_t maxCandidates)
    const
{
	FRIF::SearchSubjectPositionResult result{};
	result.decision = false;

	return {ReturnStatus{}, result};
}

std::tuple<FRIF::ReturnStatus, std::optional<FRIF::SearchSubjectResult>>
FRIF::Evaluations::Exemplar1N::NullSearchImplementation::searchSubject(
    const std::vector<std::byte> &probeTemplate,
    const uint16_t maxCandidates)
    const
{
	/*
	 * You might implement this in terms of searchSubjectPosition(), but
	 * suggest you ensure that it works as expected.
	 */

	const auto spResult = this->searchSubjectPosition(probeTemplate,
	    maxCandidates);

	if (!std::get<ReturnStatus>(spResult) ||
	    !std::get<std::optional<FRIF::SearchSubjectPositionResult>>(
	    spResult).has_value())
		return {std::get<ReturnStatus>(spResult), std::nullopt};

	FRIF::SearchSubjectResult result{};
	for (const auto &[candidate, score] : std::get<std::optional<
	    FRIF::SearchSubjectPositionResult>>(spResult)->candidateList) {
		/*
		 * XXX: If searchSubjectPosition would return duplicate finger
		 *      positions for the same subject, you want some logic here
		 *      to:
		 *        - adjust the similarity score to encompass all the
		 *          instances of similarity (this example loop would
		 *          assign the least similar finger position's
		 *          similarity score to the candidate)
		 *        - add additional candidates (up to maxCandidates)
		 */
		result.candidateList[candidate.identifier] = score;
	}

	const auto &spCorrespondence = std::get<std::optional<
	    FRIF::SearchSubjectPositionResult>>(spResult)->correspondence;
	if (spCorrespondence) {
		SubjectCandidateListCorrespondence corr{};
		/*
		 * XXX: See above note about duplicate finger positions.
		 */
		for (const auto &[candidate, list] : *spCorrespondence)
			corr[candidate.identifier] = list;
		result.correspondence = corr;
	}

	/* XXX: It's possible you can reuse this as-is, but confirm. */
	result.decision = std::get<std::optional<
	    FRIF::SearchSubjectPositionResult>>(spResult)->decision;

	return {ReturnStatus{}, result};
}

std::optional<FRIF::SubjectPositionCandidateListCorrespondence>
FRIF::Evaluations::Exemplar1N::NullSearchImplementation::
    extractCorrespondenceSubjectPosition(
    const std::vector<std::byte> &probeTemplate,
    const SearchSubjectPositionResult &searchResult)
    const
{
	/* Clarify support within getCompatibility() */
	return {};
}

std::optional<FRIF::SubjectCandidateListCorrespondence>
FRIF::Evaluations::Exemplar1N::NullSearchImplementation::
    extractCorrespondenceSubject(
    const std::vector<std::byte> &probeTemplate,
    const SearchSubjectResult &searchResult)
    const
{
	/* Clarify support within getCompatibility() */
	return {};
}

/******************************************************************************/

std::optional<FRIF::ProductIdentifier>
FRIF::Evaluations::Exemplar1N::SearchInterface::getProductIdentifier()
{
	/*
	 * Optional. `return {};` or `return (std::nullopt);` will suffice.
	 */

	ProductIdentifier::CBEFFIdentifier cbeff{};
	cbeff.owner = NullImplementationConstants::CBEFFProductOwner;
	cbeff.algorithm = NullImplementationConstants::searchProductVersion;

	ProductIdentifier id{};
	id.marketing = NullImplementationConstants::searchProductName;
	id.cbeff = cbeff;

	return (id);
}

FRIF::Evaluations::Exemplar1N::SearchInterface::Compatibility
FRIF::Evaluations::Exemplar1N::SearchInterface::getCompatibility()
{
	Compatibility compatibility{};

	compatibility.supportsCorrespondence = false;

	compatibility.probeTemplateVersions = {
	    NullImplementationConstants::featureExtractionProductVersion,
	    NullImplementationConstants::featureExtractionProductVersion - 1};

	compatibility.referenceDatabaseVersions = {
	    NullImplementationConstants::featureExtractionProductVersion,
	    NullImplementationConstants::featureExtractionProductVersion - 1,
	    NullImplementationConstants::featureExtractionProductVersion - 2};

	return (compatibility);
}

std::shared_ptr<FRIF::Evaluations::Exemplar1N::SearchInterface>
FRIF::Evaluations::Exemplar1N::SearchInterface::getImplementation(
    const std::filesystem::path &configurationDirectory,
    const std::filesystem::path &databaseDirectory)
{
	return (std::make_shared<NullSearchImplementation>(
	    configurationDirectory, databaseDirectory));
}
