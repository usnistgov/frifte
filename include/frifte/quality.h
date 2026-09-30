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
 * @page quality Quality
 * @brief
 * %FRIF Technology Evaluation---Quality---Image quality computation
 * Application Programming Interface.
 *
 * @details
 * # Overview
 * This is the Application Programming Interface (API) that must be implemented
 * to participate in the National Institute of Standards and Technology (NIST)'s
 * [Friction Ridge Image and Features (%FRIF) Technology Evaluation (TE)
 * Quality](https://fingerprint.nist.gov/frifte/quality). All Quality-specific
 * functionality is defined within FRIF::Evaluations::Quality, sharing common
 * base code from FRIF.
 *
 * # Implementation
 * A pure-virtual (abstract) class called FRIF::Evaluations::Quality::Interface
 * has been defined. Participants must implement all methods of this class in a
 * subclass and build as a shared library. The name of the library shall follow
 * the requirements outlined in the test plan and be identical to the required
 * information returned from
 * FRIF::Evaluations::Quality::Interface::getProductIdentifier. NIST's testing
 * apparatus will link against the submitted library and instantiate instances
 * of the implementations with
 * FRIF::Evaluations::Quality::Interface::getImplementation.
 *
 * ## Required Static Methods
 * The following methods are defined static in the API and must be implemented
 * by participants:
 *  - FRIF::Evaluations::Quality::Interface::getImplementation
 *  - FRIF::Evaluations::Quality::Interface::getCompatibility
 *     - Pay special attention to this function, as it determines what will be
 *       evaluated by NIST's testing apparatus.
 *  - FRIF::Evaluations::Quality::Interface::getProductIdentifier
 */

#ifndef FRIFTE_QUALITY_H_
#define FRIFTE_QUALITY_H_

#include <frifte/common.h>
#include <frifte/efs.h>
#include <frifte/evaluations.h>
#include <frifte/io.h>

/** %FRIF TE %Quality functionality. */
namespace FRIF::Evaluations::Quality
{
	/** @return Identification information about the submitted library. */
	LibraryIdentifier
	getLibraryIdentifier();

	/**
	 * @brief
	 * Quality components.
	 * @note
	 * All images sourced from NIST Special Databases 300, 301, and 302.
	 * @note
	 * Some components and component descriptions are based on work by
	 * Heidi Eldridge and Christophe Champod for Irregular Warfare Technical
	 * Support Directorate Task Number FE-TE-4766.
	 */
	enum class Component
	{
		/**
		 * @brief
		 * Evidence of finger amputation.
		 * @details
		 * Value shall be an integer [0, 100] as encoded in an
		 * ISO/IEC 29794-1:2024 quality block.
		 *
		 * @note
		 * Algorithms should detect information such as an obstructed
		 * pattern or only flat ridges when curved ridges are expected.
		 * Algorithms should not necessarily recognize the letters "AMP"
		 * that are often typed or handwritten on a fingerprint card to
		 * indicate amputation.
		 */
		Amputated,

		/**
		 * @brief
		 * Presence of a feature that indicates the anatomical source.
		 * @details
		 * An "egg" shape might indicate a thumb, or a series of deltas
		 * might indicate an interdigital palm area.
		 * @details
		 * Value shall be an integer [0, 100] as encoded in an
		 * ISO/IEC 29794-1:2024 quality block.
		 */
		AnatomicalCluesPresent,

		/**
		 * @brief
		 * Evidence of full or partial bandage.
		 * @details
		 * Value shall be an integer [0, 100] as encoded in an
		 * ISO/IEC 29794-1:2024 quality block.
		 *
		 * @image html ROLL1000_799e4951_3.png "Bandage on the right of the fingerprint"
		 */
		Bandaged,

		/**
		 * @brief
		 * Confidence that the computed core-delta ridge count is
		 * within two ridges of the true value.
		 * @details
		 * This implies clear and unobstructed ridge flow between the
		 * core and delta.
		 * @details
		 * Value shall be an integer [0, 100] as encoded in an
		 * ISO/IEC 29794-1:2024 quality block.
		 *
		 * @see CorePresent
		 * @see DeltaPresent
		 * @see PatternClassification
		 *
		 * @image html 00002319_1F_L_L01_BP_S23_1200PPI_8BPC_1CH_LP02_1.png "Clarity of ridges between core and delta."
		 */
		ClearCoreDeltaRidgeCount,

		/**
		 * @brief
		 * The matrix makes the impression difficult to interpret.
		 * @details
		 * Value shall be an integer [0, 100] as encoded in an
		 * ISO/IEC 29794-1:2024 quality block.
		 *
		 * @see ComplexSubstrate
		 */
		ComplexMatrix,

		/**
		 * @brief
		 * The substrate (e.g., pattern, texture) makes the impression
		 * difficult to interpret.
		 * @details
		 * Value shall be an integer [0, 100] as encoded in an
		 * ISO/IEC 29794-1:2024 quality block.
		 *
		 * @image html 00002329_3_X_0470_IN_D800_1042PPI_16BPC_1CH_LP01_1.png "Substrate pattern makes ridges hard to see."
		 *
		 * @see ComplexMatrix
		 */
		ComplexSubstrate,

		/**
		 * @brief
		 * Ridges appear wider than normal and potentially overlap,
		 * limiting the area of the valleys between.
		 * @details
		 * Value shall be an integer [0, 100] as encoded in an
		 * ISO/IEC 29794-1:2024 quality block.
		 *
		 * @see EvenPressure
		 * @see RidgeValleyClarity
		 *
		 * @image html 00002315_6E_X_147_BT_D800_1103PPI_16BPC_1CH_LP10_1.png "Large area of compressed ridges."
		 */
		CompressedRidges,

		/**
		 * @brief
		 * Presence of condensation.
		 * @details
		 * Primarily present in exemplar impression images. Often
		 * referred to as a "halo," as in a halo of condensation around
		 * a finger.
		 * @details
		 * Value shall be an integer [0, 100] as encoded in an
		 * ISO/IEC 29794-1:2024 quality block.
		 */
		Condensation,

		/**
		 * @brief
		 * One or more cores are present or implied.
		 * @details
		 * Value shall be an integer [0, 100] as encoded in an
		 * ISO/IEC 29794-1:2024 quality block.
		 *
		 * @see PatternClassification
		 *
		 * @image html 00002303_1F_R_L01_BP_S03_1200PPI_8BPC_1CH_LP02_1.png "Image with core present."
		 * @image html 00002325_4A_X_609_IN_D800_1102PPI_16BPC_1CH_LP03_1.png "Image with core implied."
		 */
		CorePresent,

		/**
		 * @brief
		 * One or more deltas are present or implied.
		 * @details
		 * Value shall be an integer [0, 100] as encoded in an
		 * ISO/IEC 29794-1:2024 quality block.
		 *
		 * @image html 00002313_1E_L_L01_BP_S10_1200PPI_8BPC_1CH_LP01_1.png "Image with delta present."
		 */
		DeltaPresent,

		/**
		 * @brief
		 * Artifacts introduced in development of the impression have
		 * caused damage.
		 * @details
		 * Examples include brush strokes, tape, labels, specular
		 * reflection, and image compression.
		 * @details
		 * Value shall be an integer [0, 100] as encoded in an
		 * ISO/IEC 29794-1:2024 quality block.
		 *
		 * @image html 00002329_1D_L_L01_BP_S25_1200PPI_8BPC_1CH_LP03_1.png "Tape bubble artifacts affect interpretation of the mark."
		 * @image html 00002314_5A_X_058_IN_D800_1109PPI_16BPC_1CH_LP10_1.png "Photograph is out of focus."
		 */
		DevelopmentArtifacts,

		/**
		 * @brief
		 * Presence of areas with unique clusters of minutiae.
		 * @details
		 * Value shall be an integer [0, 100] as encoded in an
		 * ISO/IEC 29794-1:2024 quality block.
		 *
		 * @image html 00002315_6E_X_147_BT_D800_1103PPI_16BPC_1CH_LP14_1.png "A distinctive cluster of minutiae northwest of the core."
		 */
		DistinctiveClusterPresent,

		/**
		 * @brief
		 * Friction ridges are deposited with an even pressure.
		 * @details
		 * This implies a consistent grey level of ridges across the
		 * image.
		 * @details
		 * Value shall be an integer [0, 100] as encoded in an
		 * ISO/IEC 29794-1:2024 quality block.
		 *
		 * @see CompressedRidges
		 * @see RidgeValleyClarity
		 *
		 * @image html 00002319_1G_R_L01_BP_S23_1200PPI_8BPC_1CH_LP02_1.png "Image depicting even pressure."
		 * @image html 00002315_6E_X_147_BT_D800_1103PPI_16BPC_1CH_LP14_1.png "Image depicting uneven pressure in the fingerprint core."
		 */
		EvenPressure,

		/**
		 * @brief
		 * Evidence that impression has been placed by artificial means.
		 * @details
		 * Value shall be an integer [0, 100] as encoded in an
		 * ISO/IEC 29794-1:2024 quality block.
		 *
		 * @note
		 * This is not intended to be a full presentation attack
		 * detection system (though it could be), more that the
		 * impression was not left by a natural touch on the substrate.
		 */
		EvidenceOfFraud,

		/**
		 * @brief
		 * Evidence of excessive distortion.
		 * @details
		 * All impressions will have some distortion. This quality
		 * component should be selected if the print contains
		 * so much distortion that it impacts the overall interpretation
		 * of the impression. This is typically due to movement of some
		 * kind that affects the shape of the impression.
		 * @details
		 * Value shall be an integer [0, 100] as encoded in an
		 * ISO/IEC 29794-1:2024 quality block.
		 *
		 * @image html 00002317_1E_R_L01_BP_S22_1200PPI_8BPC_1CH_LP04_1.png "Major movement causing ridge compression and directional inconsistency."
		 */
		ExcessiveDistortion,

		/**
		 * @brief
		 * Extreme tips or other regions not typically imaged in an
		 * exemplar impression.
		 * @details
		 * Value shall be an integer [0, 100] as encoded in an
		 * ISO/IEC 29794-1:2024 quality block.
		 *
		 * @see AnatomicalCluesPresent
		 *
		 * @image html 00002337_1H_L_L01_BP_S03_1200PPI_8BPC_1CH_LP04_1.png "An image of an extreme tip that is likely not present in an exemplar."
		 */
		ExtremeTip,

		/**
		 * @brief
		 * Distinctive features other than cores or deltas are present.
		 * @details
		 * Features may include scars, warts, or creases, amongst
		 * others.
		 * @details
		 * Value shall be an integer [0, 100] as encoded in an
		 * ISO/IEC 29794-1:2024 quality block.
		 *
		 * @image html 00001166_plain_500_02.png "Scarring and wart provide a distinct focal point."
		 */
		FocalPointsPresent,

		/**
		 * @brief
		 * Evidence of non-primary impression in image.
		 * @details
		 * Primarily present for exemplar images due to residue
		 * remaining on platen.
		 * @details
		 * Value shall be an integer [0, 100] as encoded in an
		 * ISO/IEC 29794-1:2024 quality block.
		 *
		 * @image html 00002229_01_dryrun-E_500_plain_03.png "Image with \"ghost\" of previous impression."
		 */
		Ghosting,

		/**
		 * @brief
		 * Significant portion of mark is not connected to other
		 * portions.
		 * @details
		 * Value shall be an integer [0, 100] as encoded in an
		 * ISO/IEC 29794-1:2024 quality block.
		 *
		 * @image html 00002317_1H_L_L01_BP_S22_1200PPI_8BPC_1CH_LP04_1.png "Two segmented areas of the same fingerprint."
		 */
		IsolatedRidgeClusters,

		/**
		 * @brief
		 * Pores, incipient ridges, and/or edge shapes are clear.
		 * @details
		 * Value shall be an integer [0, 100] as encoded in an
		 * ISO/IEC 29794-1:2024 quality block.
		 *
		 * @see MinutiaePresent
		 *
		 * @image html 00002302_R_500_slap_04.png "Image with pores present."
		 */
		Level3Present,

		/**
		 * @brief
		 * Ridge endings and bifurcations are present or implied.
		 * @details
		 * Value shall be an integer [0, 100] as encoded in an
		 * ISO/IEC 29794-1:2024 quality block.
		 *
		 * @see Level3Present
		 *
		 * @image html 00002319_1G_R_L01_BP_S23_1200PPI_8BPC_1CH_LP02_1.png "Image with minutiae present."
		 */
		MinutiaePresent,

		/**
		 * @brief
		 * Twists, pulls, slips, and other movement is present.
		 * @details
		 * Value shall be an integer [0, 100] as encoded in an
		 * ISO/IEC 29794-1:2024 quality block.
		 *
		 * @image html 00002313_1H_R_L01_BP_S10_1200PPI_8BPC_1CH_LP03_1.png "Ridges are smeared, depicting motion."
		 */
		Movement,

		/**
		 * @brief
		 * Multiple impressions in ROI that do not overlap.
		 * @details
		 * Typically fixed by better segmentation.
		 * @details
		 * Value shall be an integer [0, 100] as encoded in an
		 * ISO/IEC 29794-1:2024 quality block.
		 *
		 * @see MultipleOverlappingImpressions
		 *
		 * @image html 00002626_1A_X_L01_BP_S21_1200PPI_8BPC_1CH_LP04_1.png "Two impressions that do not overlap."
		 */
		MultipleNonOverlappingImpressions,

		/**
		 * @brief
		 * Multiple impressions in ROI that overlap.
		 * @details
		 * The impressions may be from the same source.
		 * @details
		 * Value shall be an integer [0, 100] as encoded in an
		 * ISO/IEC 29794-1:2024 quality block.
		 *
		 * @see MultipleNonOverlappingImpressions
		 * @see Ghosting
		 *
		 * @image html 00002304_5A_X_648_IN_D800_1109PPI_16BPC_1CH_LP06_1.png "Overlapping impressions."
		 */
		MultipleOverlappingImpressions,

		/**
		 * @brief
		 * Number of highly-certain minutiae (i.e., minutiae in high
		 * clarity areas).
		 * @details
		 * Value shall be 0 or a positive integer, and less than or
		 * equal to NumMinutiae.
		 *
		 * @see NumMinutiae
		 */
		NumHighConfidenceMinutiae,

		/**
		 * @brief
		 * Total number of minutiae discovered.
		 * @details
		 * Value shall be 0 or a positive integer.
		 *
		 * @see NumHighConfidenceMinutiae
		 */
		NumMinutiae,

		/**
		 * @brief
		 * Large area without minutiae.
		 * @details
		 * Value shall be an integer [0, 100] as encoded in an
		 * ISO/IEC 29794-1:2024 quality block.
		 */
		OpenField,

		/**
		 * @brief
		 * Consistency of orientations of ridges within subregions.
		 * @details
		 * Value shall be an integer [0, 100] as encoded in an
		 * ISO/IEC 29794-1:2024 quality block.
		 *
		 * @see ISO/IEC 29794-4:2024, Clause 6.2.2
		 */
		OrientationCertaintyLevel,

		/**
		 * @brief
		 * Information present to help orient the impression.
		 * @details
		 * One or more features (e.g., pattern classification, ridge
		 * flow, interphalangeal joint) that help determine orientation.
		 * @details
		 * Value shall be an integer [0, 100] as encoded in an
		 * ISO/IEC 29794-1:2024 quality block.
		 *
		 * @image html 00002325_6E_X_074_BT_D800_1103PPI_16BPC_1CH_LP19_1.png "Ridge flow helps determine the proper orientation."
		 * @image html 00002327_1G_L_L01_BP_S03_1200PPI_8BPC_1CH_LP05_1.png "Pattern classification helps determine the proper orientation."
		 */
		OrientationClue,

		/**
		 * @brief
		 * Pattern classification can be determined.
		 * @details
		 * Value shall be an integer [0, 100] as encoded in an
		 * ISO/IEC 29794-1:2024 quality block.
		 *
		 * @image html 00002319_1G_R_L01_BP_S23_1200PPI_8BPC_1CH_LP02_1.png "Pattern classification can be readily determined."
		 */
		PatternClassification,

		/**
		 * @brief
		 * Impression contains mostly pattern force region.
		 * @details
		 * Pattern force is a region of friction ridge skin in which
		 * minutiae were forced to form due to pattern type and existing
		 * ridge fields during friction ridge formation (source: ASB
		 * Technical Report 12).
		 * @details
		 * Value shall be an integer [0, 100] as encoded in an
		 * ISO/IEC 29794-1:2024 quality block.
		 *
		 * @image html 00002330_1G_R_L01_BP_S06_1200PPI_8BPC_1CH_LP02_1.png "Small area around delta imaged, showing pattern-forced minutiae only."
		 */
		PatternForceMinutiae,

		/**
		 * @brief
		 * Inability to determine the ridge color in a large portion
		 * of the image.
		 * @details
		 * Value shall be an integer [0, 100] as encoded in an
		 * ISO/IEC 29794-1:2024 quality block.
		 *
		 * @see TonalReversal
		 *
		 * @image html 00002644_1A_R_L03_BP_S21_1200PPI_8BPC_1CH_LP02_1.png "Area of image where ridge color is uncertain or completely inverted."
		 */
		RidgeColorUncertain,

		/**
		 * @brief
		 * Orientation differences between a region and its neighbors.
		 * @details
		 * Value shall be an integer [0, 100] as encoded in an
		 * ISO/IEC 29794-1:2024 quality block.
		 *
		 * @see ISO/IEC 29794-4:2024, Clause 6.2.6
		 */
		RidgeFlowContinuity,

		/**
		 * @brief
		 * Percentage of usable area with good ridge-valley contrast.
		 * @details
		 * Value shall be an integer [0, 100] as encoded in an
		 * ISO/IEC 29794-1:2024 quality block.
		 *
		 * @see ISO/IEC 29794-4:2024, Clause 6.2.3
		 */
		RidgeValleyClarity,

		/**
		 * @brief
		 * Consistency between ridge and valley widths.
		 * @details
		 * Value shall be a positive floating point value indicating the
		 * ratio of widths of ridges against valleys.
		 *
		 * @see ISO/IEC 29794-4:2024, Clause 6.2.5
		 */
		RidgeValleyUniformity,

		/**
		 * @brief
		 * Estimate of pixels per inch resolution of image.
		 * @details
		 * Value shall be a positive integer.
		 *
		 * @note
		 * **Do not** use EFS::Features::ppi in estimate. Instead,
		 * make use of internally-derived calculations (e.g., ridge
		 * width, known object size measurement).
		 */
		SpatialSamplingRatePPI,

		/**
		 * @brief
		 * Ridges are spotty, regardless of reason.
		 * @details
		 * Value shall be an integer [0, 100] as encoded in an
		 * ISO/IEC 29794-1:2024 quality block.
		 *
		 * @see DevelopmentArtifacts
		 *
		 * @image html 00002304_5A_X_647_IN_D800_1109PPI_16BPC_1CH_LP05_1.png "Spotty ridges."
		 */
		SpottyRidges,

		/**
		 * @brief
		 * Valleys appear darker than ridges.
		 * @details
		 * Value shall be an integer [0, 100] as encoded in an
		 * ISO/IEC 29794-1:2024 quality block.
		 *
		 * @image html 00002626_1A_X_L01_BP_S21_1200PPI_8BPC_1CH_LP04_1.png "Areas of ridge tonal reversal."
		 */
		TonalReversal,

		/**
		 * @brief
		 * Ridges that are wet or deposited in a liquid matrix,
		 * including blood.
		 * @details
		 * Value shall be an integer [0, 100] as encoded in an
		 * ISO/IEC 29794-1:2024 quality block.
		 */
		WetRidges
	};

	/** Detailed quality information for a sample. */
	class VerboseQuality
	{
	public:
		/**
		 * @brief
		 * Set the unified quality score or communicate an error about
		 * its computation.
		 *
		 * @param uqs
		 * EFS::QualityMeasure conforming to the specifications of
		 * ISO/IEC 29794-1:2024.
		 */
		void
		setUnifiedQualityScore(
		    const EFS::QualityMeasure &uqs);

		/**
		 * @brief
		 * Convenience method to set the unified quality score.
		 *
		 * @param uqs
		 * Value [0,100] conforming to specifications of
		 * ISO/IEC 29794-1:2024.
		 */
		void
		setUnifiedQualityScore(
		    const int uqs);

		EFS::QualityMeasure
		getUnifiedQualityScore()
		    const;

		/**
		 * @brief
		 * Define a region of interest of a sample.
		 * @details
		 * The region of interest is a closed convex polygon that
		 * bounds pixels of a friction ridge impression.
		 *
		 * @warning
		 * Input will replace any existing values.
		 *
		 * @param roi
		 * Consecutive coordinates of the closed convex polygon.
		 */
		void
		setROI(
		    const std::vector<Coordinate> &roi);

		std::optional<std::vector<Coordinate>>
		getROI()
		    const;

		/**
		 * @brief
		 * Define a series of ridge quality regions.
		 * @warning
		 * Input will replace any existing values.
		 *
		 * @param rqm
		 * Rectangles encompassing pixels of a particular ridge quality.
		 */
		void
		setRidgeQualityMap(
		    const std::vector<EFS::RidgeQualityRegion> &rqm);

		std::optional<std::vector<EFS::RidgeQualityRegion>>
		getRidgeQualityMap()
		    const;

		/**
		 * @brief
		 * Append a quality component with associated measurement.
		 *
		 * @param component
		 * Component measured.
		 * @param value
		 * Value of `component`.
		 *
		 * @note
		 * If failing to compute `component`, `value.getStatus()` must
		 * indicate EFS::QualityMeasure::Status::Error.
		 */
		void
		addComponent(
		    const Component component,
		    const EFS::QualityMeasure &value);

		/**
		 * @brief
		 * Remove a quality component.
		 *
		 * @param component
		 * Component to remove.
		 */
		void
		removeComponent(
		    const Component component);

		std::optional<EFS::QualityMeasure>
		getComponent(
		    const Component component)
		    const;

		std::optional<std::unordered_map<Component,
		    EFS::QualityMeasure>>
		getComponents()
		    const;

		bool
		hasComponent(
		    const Component component)
		    const;

	private:
		EFS::QualityMeasure uqs{};
		std::optional<std::unordered_map<Component,
		    EFS::QualityMeasure>> components{};
		std::optional<std::vector<Coordinate>> roi{};
		std::optional<std::vector<EFS::RidgeQualityRegion>> rqm{};
	};

	/** %Interface for quality calculation implemented by participant. */
	class Interface
	{
	public:
		/**
		 * Information used by the NIST testing apparatus to help
		 * efficiently test this implementation.
		 */
		struct Compatibility
		{
			/** Will implementation compute a region of interest? */
			bool regionOfInterest{false};

			/** Will implementation compute a ridge quality map? */
			bool ridgeQualityMap{false};

			/** Will implementation compute any Component? */
			bool qualityComponents{false};

			/**
			 * Will implementation process latent impressions from
			 * palm regions?
			 */
			bool palmLatent{false};
			/**
			 * Will implementation process exemplar impressions from
			 * palm regions?
			 */
			bool palmExemplar{false};

			/**
			 * Will implementation process latent impressions from
			 * distal phalanx regions?
			 */
			bool distalLatent{false};
			/**
			 * Will implementation process exemplar impressions from
			 * distal phalanx regions?
			 */
			bool distalExemplar{false};

			/**
			 * Will implementation process latent impressions from
			 * non-distal and non-palm regions (e.g., medial and/or
			 * proximal phalanges, entire joint images)?
			 */
			bool nonDistalLatent{false};
			/**
			 * Will implementation process exemplar impressions from
			 * non-distal and non-palm regions (e.g., medial and/or
			 * proximal phalanges, entire joint images)?
			 */
			bool nonDistalExemplar{false};

			/**
			 * Will implementation process latent impressions
			 * where the source region (e.g., finger, palm) cannot
			 * be determined?
			 */
			bool unknownLatent{false};
		};

		/**
		 * @brief
		 * Compute detailed quality information about a sample.
		 *
		 * @param sample
		 * A single sample on which quality shall be computed.
		 *
		 * @return
		 * Verbose quality information, populated with features listed
		 * as supported in getCompatibility().
		 *
		 * @note
		 * `sample` will **always** contain both an Image and
		 * EFS::Features.
		 *
		 * @note
		 * `sample`'s EFS::Features will **not** contain any of
		 * optional members, with the exception of EFS::Features::roi
		 * on occasion.
		 *
		 * @note
		 * This method shall not spawn threads.
		 *
		 * @note
		 * This method shall return in <= 500 milliseconds.
		 *
		 * @important
		 * Check `sample` for EFS::Features::roi to aid in computation.
		 */
		virtual
		VerboseQuality
		computeVerboseQuality(
		    const Sample &sample)
		    const = 0;

		/**
		 * @brief
		 * Compute an ISO/IEC 29794-1:2024 unified quality score.
		 *
		 * @param sample
		 * A single sample on which quality shall be computed.
		 *
		 * @return
		 * ISO/IEC 29794-1:2024 unified quality score.
		 *
		 * @note
		 * `sample` will **always** contain both an Image and
		 * EFS::Features.
		 *
		 * @note
		 * `sample`'s EFS::Features will **not** contain any of
		 * optional members, with the exception of EFS::Features::roi
		 * on occasion.
		 *
		 * @note
		 * This method shall not spawn threads.
		 *
		 * @note
		 * This method shall return in <= 500 milliseconds.
		 *
		 * @important
		 * Check `sample` for EFS::Features::roi to aid in computation.
		 */
		virtual
		EFS::QualityMeasure
		computeUnifiedQualityScore(
		    const Sample &sample)
		    const = 0;

		/**************************************************************/

		/**
		 * @brief
		 * Obtain a managed pointer to an object implementing Interface.
		 *
		 * @param configurationDirectory
		 * Read-only directory populated with configuration files
		 * provided by participant.
		 *
		 * @return
		 * Shared pointer to an instance of Interface containing the
		 * participant's code to perform quality assessment operations.
		 *
		 * @note
		 * A possible implementation might be:
		 * @code{.cpp}
		 * return (std::make_shared<Implementation>(
		 *     configurationDirectory));
		 * @endcode
		 *
		 * @note
		 * This method shall return in <= 5 seconds.
		 *
		 * @note
		 * This method shall not spawn threads.
		 */
		static
		std::shared_ptr<Interface>
		getImplementation(
		    const std::filesystem::path &configurationDirectory);

		/**
		 * @brief
		 * Obtain identification and version information.
		 *
		 * @return
		 * Information used to identify the quality assessment
		 * algorithms in reports, using fields as defined by
		 * ANSI/NIST-ITL 1-2025, or `std::nullopt` if no product
		 * information is available.
		 *
		 * @note
		 * This method shall return instantly.
		 *
		 * @note
		 * This method shall not spawn threads.
		 */
		static
		std::optional<EFS::QualityMeasure::Description>
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
		Interface(const Interface&) = delete;
		/** Suppress copying polymorphic class (C.63). */
		Interface& operator=(const Interface&) = delete;
		/** @endcond */

		Interface();
		virtual ~Interface();
	};

	/*
	 * API versioning.
	 *
	 * NIST code will extern the version number symbols. Participant code
	 * shall compile them into their core library.
	 */
	#ifdef NIST_EXTERN_FRIFTE_QUALITY_API_VERSION
	/** API major version number. */
	extern uint16_t API_MAJOR_VERSION;
	/** API minor version number. */
	extern uint16_t API_MINOR_VERSION;
	/** API patch version number. */
	extern uint16_t API_PATCH_VERSION;
	#else /* NIST_EXTERN_FRIFTE_QUALITY_API_VERSION */
	/** API major version number. */
	inline uint16_t API_MAJOR_VERSION{0};
	/** API minor version number. */
	inline uint16_t API_MINOR_VERSION{0};
	/** API patch version number. */
	inline uint16_t API_PATCH_VERSION{1};
	#endif /* NIST_EXTERN_FRIFTE_QUALITY_API_VERSION */
}

#endif /* FRIFTE_QUALITY_H_ */
