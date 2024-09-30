/*
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 */

#ifndef FRIF_IO_H_
#define FRIF_IO_H_

#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include <frif/common.h>
#include <frif/efs.h>

/** TE input/output types. */
namespace FRIF
{
	/**
	 * Information possibly stored in a template.
	 *
	 * @note
	 * If provided a multi-position image and applicable to the feature
	 * extraction algorithm, Features#roi should be populated with
	 * segmentation coordinates and Features#frgp should be set for each
	 * position.
	 */
	struct TemplateData
	{
		/** Identifier provided during template creation. */
		std::string identifier{};

		/** Link to Image#identifier and/or Features#identifier. */
		uint8_t inputIdentifier{};

		/** Extended feature set data */
		std::optional<EFS::Features> features{};

		/** Quality of the image, [0-100]. */
		std::optional<uint8_t> imageQuality{};
	};

	/** Output from extracting features into a template .*/
	struct CreateTemplateResult
	{
		/** Contents of the template. */
		std::vector<std::byte> data{};

		/**
		 * @brief
		 * Information contained within #data.
		 *
		 * @details
		 * Some participants may find they have already performed the
		 * calculations needed for
		 * ExtractionInterface::extractTemplateData within
		 * ExtractionInterface::createTemplateData. If that is the case,
		 * TemplateData may be returned here instead.
		 *
		 * @attention
		 * If this value is populated,
		 * ExtractionInterface::extractTemplateData will not be called,
		 * as the information returned is expected to be redundant.
		 *
		 * @note
		 * Reported and enforced template creation times will include
		 * the time it takes to populate this variable.
		 *
		 * @see
		 * ExtractionInterface::extractTemplateData.
		 */
		std::optional<std::vector<TemplateData>> extractedData{};
	};

	/** Information about a probe/reference relationship. */
	struct Correspondence
	{
		/**
		 * @brief
		 * Groups of relationships between features.
		 * @details
		 * Each CorrespondenceRelationship shows the relationship
		 * between two individual features. The vector of
		 * CorrespondenceRelationship groups multiple sets of
		 * relationships corresponding to the same images.
		 */
		std::vector<EFS::CorrespondenceRelationship> relationships{};

		/**
		 * @brief
		 * Whether or not the comparison was complex.
		 *
		 * @details
		 * Complexity should be determined as specified by the
		 * documentation for the "complex comparison flag (CCF)"
		 * of ANSI/NIST-ITL 1-2011 (2015) Field 9.362.
		 */
		std::optional<bool> complex{};
	};

	/**
	 * Candidate list entry identifying a specific region of a subject.
	 *
	 * @see SubjectCandidate
	 */
	struct SubjectPositionCandidate
	{
		/** Identifier of the sample in the reference database. */
		std::string identifier{};
		/** Most localized position in the identifier. */
		EFS::FrictionRidgeGeneralizedPosition fgp{};

		/**
		 * @brief
		 * SubjectPositionCandidate constructor.
		 *
		 * @param identifier
		 * Identifier of the sample in the reference database.
		 * @param fgp
		 * Most localized position in the identifier.
		 */
		SubjectPositionCandidate(
		    const std::string &identifier = {},
		    const EFS::FrictionRidgeGeneralizedPosition fgp = {});

		auto operator<=>(const SubjectPositionCandidate&) const;
		bool operator==(const SubjectPositionCandidate&) const;
	};

	/**
	 * Candidate list entry identifying a subject without respect for
	 * region.
	 *
	 * @see SubjectPositionCandidate
	 */
	using SubjectCandidate = std::string;

	/** Hash function for SubjectPositionCandidate. */
	struct SubjectPositionCandidateListKeyHash
	{
		std::size_t
		operator()(
		    const FRIF::SubjectPositionCandidate &c)
		    const
		    noexcept;
	};

	/**
	 * @brief
	 * Representation of a list of likely candidates returned from a search.
	 * @details
	 * Key is a unique subject identifier and finger position from that
	 * subject, representing a reference identity. Value is a quantification
	 * of a probe's similarity to the friction ridge data represented by
	 * the key.
	 *
	 * @note
	 * This structure is used to disallow duplicate finger positions from
	 * the same subject identifier.
	 */
	using SubjectPositionCandidateList = std::unordered_map<SubjectPositionCandidate, double,
	    SubjectPositionCandidateListKeyHash>;

	/**
	 * @brief
	 * Representation of a list of likely candidates returned from a search.
	 * @details
	 * Key is a unique subject identifier, representing a reference
	 * identity. Value is a quantification of a probe's similarity to the
	 * friction ridge data represented by the key.
	 *
	 * @note
	 * This structure is used to disallow duplicate subject identifiers.
	 */
	using SubjectCandidateList = std::unordered_map<std::string, double>;

	/**
	 * @brief
	 * Representation to output Correspondence for each SubjectPositionCandidate from
	 * a SubjectPositionCandidateList.
	 * @details
	 * Key is a unique subject identifier and finger position from that
	 * subject, representing a reference identity. Value is a set of
	 * Correspondence that align features from the probe to features from
	 * the reference (the key).
	 */
	using SubjectPositionCandidateListCorrespondence =
	    std::unordered_map<SubjectPositionCandidate, Correspondence,
	    SubjectPositionCandidateListKeyHash>;

	/**
	 * @brief
	 * Representation to output Correspondence for each SubjectCandidate from
	 * a SubjectCandidateList.
	 * @details
	 * Key is a unique subject identifier, representing a reference
	 * identity. Value is a set of Correspondence that align features from
	 * the probe to features from the reference (the key).
	 */
	using SubjectCandidateListCorrespondence =
	    std::unordered_map<std::string, Correspondence>;

	/** The results of comparing two templates. */
	struct ComparisonResult
	{
		/**
		 * Best guess on if probe and reference come from the same
		 * source.
		 */
		bool decision{};

		/** Quantification of probe's similarity to reference sample. */
		double similarity{};

		/**
		 * @brief
		 * Pairs of corresponding Minutia between TemplateType::Probe
		 * and TemplateType::Reference templates.
		 *
		 * @details
		 * Some participants may find they have already performed the
		 * calculations needed during comparison. If that is the case,
		 * correspondence may be returned here instead.
		 *
		 * @attention
		 * If this value is populated, no separate correspondence
		 * method will be called, as the information returned is
		 * expected to be redundant.
		 *
		 * @note
		 * Reported and enforced search times will include the time it
		 * takes to populate this variable.
		 */
		std::optional<Correspondence> correspondence{};
	};

	/**
	 * The results of a searching a database for subject finger positions.
	 */
	struct SearchSubjectPositionResult
	{
		/**
		 * Best guess on if #candidateList contains an identification.
		 */
		bool decision{};

		/**
		 * @brief
		 * List of SubjectPositionCandidate most similar to the probe.
		 */
		SubjectPositionCandidateList candidateList{};

		/**
		 * @brief
		 * Pairs of corresponding Minutia between TemplateType::Probe
		 * and TemplateType::Reference templates.
		 *
		 * @details
		 * Some participants may find they have already performed the
		 * calculations needed during search. If that is the case,
		 * correspondence may be returned here instead.
		 *
		 * @attention
		 * If this value is populated, no separate correspondence
		 * method will be called, as the information returned is
		 * expected to be redundant.
		 *
		 * @note
		 * Reported and enforced search times will include the time it
		 * takes to populate this variable.
		 */
		std::optional<SubjectPositionCandidateListCorrespondence>
		    correspondence{};
	};

	/**
	 * The results of a searching a database for a subject, without respect
	 * to finger positions.
	 */
	struct SearchSubjectResult
	{
		/**
		 * Best guess on if #candidateList contains an identification.
		 */
		bool decision{};

		/**
		 * @brief
		 * List of SubjectCandidate most similar to the probe.
		 */
		SubjectCandidateList candidateList{};

		/**
		 * @brief
		 * Pairs of corresponding Minutia between TemplateType::Probe
		 * and TemplateType::Reference templates.
		 *
		 * @details
		 * Some participants may find they have already performed the
		 * calculations needed during search. If that is the case,
		 * correspondence may be returned here instead.
		 *
		 * @attention
		 * If this value is populated, no separate correspondence
		 * method will be called, as the information returned is
		 * expected to be redundant.
		 *
		 * @note
		 * Reported and enforced search times will include the time it
		 * takes to populate this variable.
		 */
		std::optional<SubjectCandidateListCorrespondence>
		    correspondence{};
	};

	/** Collection of templates on disk. */
	struct TemplateArchive
	{
		/** File containing concatenated CreateTemplateResult#data. */
		std::filesystem::path archive{};
		/**
		 * @brief
		 * Manifest for parsing #archive.
		 *
		 * @details
		 * Each line of #manifest is in the form
		 * `identifier length offset`, where `identifier` matches
		 * `identifier` provided when creating a template,
		 * `length` is the result of calling `size()` on
		 * CreateTemplateResult#data, and `offset` is the number of
		 * bytes from the beginning of #archive to the first byte of
		 * CreateTemplateResult#data.
		 *
		 * @note
		 * Identifiers are guaranteed to never contain spaces. That is,
		 * each line of the manifest is guaranteed to have exactly two
		 * spaces, used to delimit the three fields in each line.
		 */
		std::filesystem::path manifest{};
	};

	/** Convenience definition for a friction ridge sample */
	using Sample = std::tuple<std::optional<Image>,
	    std::optional<EFS::Features>>;

	/** Use for data extracted from Sample. */
	enum class TemplateType
	{
		/**
		 * Item being compared to a reference or searched against a
		 * reference database.
		 */
		Probe,
		/**
		 * Use within a database for search, or the initial capture
		 * during a comparison.
		 */
		Reference
	};
}

#endif /* FRIF_IO_H_ */
