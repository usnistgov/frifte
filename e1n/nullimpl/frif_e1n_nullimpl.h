/*
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 */

#ifndef FRIF_E1N_NULLIMPL_H_
#define FRIF_E1N_NULLIMPL_H_

#include <frif/e1n.h>

namespace FRIF::Evaluations::Exemplar1N
{
	namespace NullImplementationConstants
	{
		constexpr uint16_t CBEFFProductOwner{0x000F};

		constexpr uint16_t libraryVersionNumber{0x0001};
		constexpr char libraryIdentifier[]{"nullimpl"};

		constexpr uint16_t featureExtractionProductVersion{0xF1A7};
		constexpr char featureExtractionProductName[]{
		    "NullImplementation Exemplar Extractor 1.0"};

		constexpr uint16_t searchProductVersion{0x0101};
		constexpr char searchProductName[]{
		    "NullImplementation Comparator (N 1:1 mode) 1.0"};
	}

	class NullExtractionImplementation : public ExtractionInterface
	{
	public:
		std::tuple<ReturnStatus, std::optional<CreateTemplateResult>>
		createTemplate(
		    const TemplateType templateType,
		    const std::string &identifier,
		    const std::vector<Sample> &samples)
		    const
		    override;

		std::optional<std::tuple<ReturnStatus,
		    std::vector<TemplateData>>>
		extractTemplateData(
		    const TemplateType templateType,
		    const CreateTemplateResult &templateResult)
		    const
		    override;

		ReturnStatus
		createReferenceDatabase(
		    const TemplateArchive &referenceTemplates,
		    const std::filesystem::path &databaseDirectory,
		    const uint64_t maxSize)
		    const
		    override;

		NullExtractionImplementation(
		    const std::filesystem::path &configurationDirectory = {});

	private:
		const std::filesystem::path configurationDirectory{};
	};

	class NullSearchImplementation : public SearchInterface
	{
	public:
		ReturnStatus
		load(
		    const uint64_t maxSize)
		    override;

		std::tuple<ReturnStatus, std::optional<SearchResult>>
		search(
		    const std::vector<std::byte> &probeTemplate,
		    const uint16_t maxCandidates)
		    const
		    override;

		std::optional<CandidateListCorrespondence>
		extractCorrespondence(
		    const std::vector<std::byte> &probeTemplate,
		    const SearchResult &searchResult)
		    const
		    override;

		NullSearchImplementation(
		    const std::filesystem::path &configurationDirectory,
		    const std::filesystem::path &databaseDirectory);

	private:
		const std::filesystem::path configurationDirectory{};
		const std::filesystem::path databaseDirectory{};
	};
}

#endif /* FRIF_E1N_NULLIMPL_H_ */
