/*
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 */

#ifndef FRIF_EFS_H_
#define FRIF_EFS_H_

#include <variant>

#include <frif/common.h>

/** Types (mostly) mirroring the ANSI/NIST-ITL Type 9 Extended Feature Set */
namespace FRIF::EFS
{
	/** Friction ridge impression types from ANSI/NIST-ITL 1-202X. */
	enum class Impression
	{
		PlainContact = 0,
		RolledContact = 1,
		Latent = 4,
		LiveScanSwipe = 8,
		PlainContactlessStationary = 24,
		RolledContactlessStationary = 25,
		Other = 28,
		Unknown = 29,
		Contactless = 43
	};
	/** Convenience Impression type using ANSI/NIST-ITL mnemonic. */
	using IMP = Impression;

	/** Capture device codes from ANSI/NIST-ITL 1-2011 (2015). */
	enum class FrictionRidgeCaptureTechnology
	{
		Unknown = 0,
		ScannedInkOnPaper = 2,
		OpticalTIRBright = 3,
		OpticalDirect = 5,
		Capacitive = 9,
		Electroluminescent = 11,

		LatentImpression = 18,
		LatentLift = 22
	};
	/**
	 * Convenience FrictionRidgeCaptureTechnology type using ANSI/NIST-ITL
	 * mnemonic.
	 */
	using FCT = FrictionRidgeCaptureTechnology;

	/** Friction positions codes from ANSI/NIST-ITL 1-2011 (2015). */
	enum class FrictionRidgeGeneralizedPosition
	{
		UnknownFinger = 0,
		RightThumb = 1,
		RightIndex = 2,
		RightMiddle = 3,
		RightRing = 4,
		RightLittle = 5,
		LeftThumb = 6,
		LeftIndex = 7,
		LeftMiddle = 8,
		LeftRing = 9,
		LeftLittle = 10,
		RightExtraDigit = 16,
		LeftExtraDigit = 17,

		RightFour = 13,
		LeftFour = 14,
		RightAndLeftThumbs = 15,

		UnknownPalm = 20,
		RightFullPalm = 21,
		RightWritersPalm = 22,
		LeftFullPalm = 23,
		LeftWritersPalm = 24,
		RightLowerPalm = 25,
		RightUpperPalm = 26,
		LeftLowerPalm = 27,
		LeftUpperPalm = 28,
		RightPalmOther = 29,
		LeftPalmOther = 30,
		RightInterdigital = 31,
		RightThenar = 32,
		RightHypothenar = 33,
		LeftInterdigital = 34,
		LeftThenar = 35,
		LeftHypothenar = 36,
		RightGrasp = 37,
		LeftGrasp = 38,
		RightCarpalDeltaArea = 81,
		LeftCarpalDeltaArea = 82,
		RightFullPalmAndWritersPalm = 83,
		LeftFullPalmAndWritersPalm = 84,
		RightWristBracelet = 85,
		LeftWristBracelet = 86,

		UnknownFrictionRidge = 18,
		EJIOrTip = 19
	};
	/**
	 * Convenience FrictionRidgeGeneralizedPosition type using ANSI/NIST-ITL
	 * mnemonic.
	 */
	using FGP = FrictionRidgeGeneralizedPosition;

	/** Processing method codes from ANSI/NIST-ITL 1-2011 (2015). */
	enum class ProcessingMethod
	{
		Indanedione,
		BlackPowder,
		Other,
		Cyanoacrylate,
		Laser,
		RUVIS,
		StickysidePowder,
		Visual,
		WhitePowder
	};
	/**
	 * Convenience LatentProcessingMethod type using ANSI/NIST-ITL
	 * mnemonic.
	 */
	using LPM = ProcessingMethod;

	/**
	 * General classification of friction ridge structure.
	 *
	 * @note
	 * These enumerations map to ANSI/NIST-ITL 1-2011 Update:2015's PCT
	 * "General Class" codes from Table 44.
	 */
	enum class PatternGeneralClassification
	{
		Arch,
		Whorl,
		RightLoop,
		LeftLoop,
		Amputation,
		UnableToPrint,
		Unclassifiable,
		Scar,
		DissociatedRidges
	};
	/**
	 * Convenience PatternGeneralClassification type using ANSI/NIST-ITL
	 * mnemonic.
	 */
	using GCF = PatternGeneralClassification;

	/**
	 * Subclassification when PatternClassification is Arch.
	 *
	 * @note
	 * These enumerations map to ANSI/NIST-ITL 1-2011 Update:2015's PCT
	 * "General Class" codes from Table 44.
	 */
	enum class ArchPatternSubclassification
	{
		PlainArch,
		TentedArch
	};

	/**
	 * Subclassification when PatternClassification is Whorl.
	 *
	 * @note
	 * These enumerations map to ANSI/NIST-ITL 1-2011 Update:2015's PCT
	 * "General Class" codes from Table 44.
	 */
	enum class WhorlPatternSubclassification
	{
		PlainWhorl,
		CentralPocketLoop,
		DoubleLoop,
		AccidentalWhorl
	};

	/**
	 * Subclassification of PatternClassification.
	 *
	 * @note
	 * These enumerations map to ANSI/NIST-ITL 1-2011 Update:2015's PCT
	 * "General Class" codes from Table 44.
	 */
	using PatternSubclassification = std::variant<
	    ArchPatternSubclassification, WhorlPatternSubclassification>;

	/**
	 * Convenience PatternSubclassification type using ANSI/NIST-ITL
	 * mnemonic.
	 */
	using SUB = PatternSubclassification;

	/**
	 * Whorl-Delta relationship.
	 *
	 * @note
	 * These enumerations are described in ANSI/NIST-ITL 1-2011 Update:2015,
	 * Section 8.9.7.5.
	 */
	enum class WhorlDeltaRelationship
	{
		Inner,
		Outer,
		Meeting
	};

	/**
	 * Convenience WhorlDeltaRelationship type using ANSI/NIST-ITL
	 * mnemonic.
	 */
	using WDR = WhorlDeltaRelationship;

	/** Classification of friction ridge structure. */
	class PatternClassification
	{
	public:
		/**
		 * @throws std::logic_error
		 * Propagated from setPatternSubclassification() and
		 * setWhorlDeltaRelationship()
		 */
		PatternClassification(
		    const PatternGeneralClassification,
		    const std::optional<PatternSubclassification> = {},
		    const std::optional<WhorlDeltaRelationship> = {});

		/**
		 * @note
		 * Clears any values set for PatternSubclassification and
		 * WhorlDeltaRelationship.
		 */
		void
		setPatternGeneralClassification(
		    const PatternGeneralClassification);

		/**
		 * @throws std::logic_error
		 * PatternGeneralClassification is not
		 * PatternGeneralClassification::Arch or
		 * PatternGeneralClassification::Whorl, or when the
		 * PatternGeneralClassification and PatternSubclassification
		 * are not of the same type (e.g., Arch &
		 * WhorlPatternSubclassification, Whorl &
		 * ArchPatternSubclassification).
		 */
		void
		setPatternSubclassification(
		    const PatternSubclassification);

		/**
		 * @throws std::logic_error
		 * PatternGeneralClassification is not
		 * PatternGeneralClassification::Whorl or
		 * PatternSubclassification is not set.
		 */
		void
		setWhorlDeltaRelationship(
		    const WhorlDeltaRelationship);

		/**
		 * @return
		 * High-level pattern classification.
		 */
		PatternGeneralClassification
		getPatternGeneralClassification()
		    const;

		/**
		 * @return
		 * Pattern subclassification.
		 */
		std::optional<PatternSubclassification>
		getPatternSubclassification()
		    const;

		/**
		 * @return
		 * Whorl-delta relationship.
		 */
		std::optional<WhorlDeltaRelationship>
		getWhorlDeltaRelationship()
		    const;

	private:
		PatternGeneralClassification gcf{};
		std::optional<PatternSubclassification> sub{};
		std::optional<WhorlDeltaRelationship> wdr{};
	};

	/**
	 * Value assessment codes from ANSI/NIST-ITL 1-2011 (2015).
	 */
	enum class ValueAssessment
	{
		Value,
		Limited,
		NoValue,
		NonPrint
	};
	/**
	 * Convenience ExaminerValueAssessment type using ANSI/NIST-ITL
	 * mnemonic.
	 */
	using EAA = ValueAssessment;

	/** Substrate codes from ANSI/NIST-ITL 1-2011 (2015). */
	enum class Substrate
	{
		Paper,
		PorousOther,

		Plastic,
		Glass,
		MetalPainted,
		MetalUnpainted,
		TapeAdhesiveSide,
		NonporousOther,

		PaperGlossy,
		SemiporousOther,

		Other,
		Unknown
	};

	/** Types of minutiae. */
	enum class MinutiaType
	{
		RidgeEnding,
		Bifurcation,
		Other,
		Unknown
	};

	/** Friction ridge feature details. */
	struct Minutia
	{
		/** Location of the feature. */
		Coordinate coordinate{};
		/** Uncertainty of #coordinate, radius, in pixels. */
		std::optional<uint16_t> coordinateUncertainty{};

		/**
		 * Ridge direction of the feature, in degrees [0,359], following
		 * conventions from ANSI/NIST-ITL 1-2011 (2015) Field 9.331.
		 */
		uint16_t theta{};
		/** Uncertainty of #theta, in degrees [0, 180]. */
		std::optional<uint8_t> thetaUncertainty{};

		/** Type of feature. */
		MinutiaType type{MinutiaType::Unknown};

		/**
		 * @brief
		 * Minutia constructor.
		 *
		 * @param coordinate
		 * Location of the feature.
		 * @param theta
		 * Ridge direction of the feature, in degrees [0,359], following
		 * conventions from ANSI/NIST-ITL 1-2011 (2015) Field 9.331.
		 * @param type
		 * Type of feature.
		 * @param coordinateUncertainty
		 * Uncertainty of #coordinate, radius, in pixels.
		 * @param thetaUncertainty
		 * Uncertainty of #theta, in degrees [0, 180].
		 */
		Minutia(
		    const Coordinate &coordinate = {},
		    const uint16_t theta = {},
		    const MinutiaType type = MinutiaType::Unknown,
		    const std::optional<uint16_t> coordinateUncertainty = {},
		    const std::optional<uint8_t> thetaUncertainty = {});
	};

	/** Singular point of focus of innermost recurving ridge. */
	struct Core
	{
		/** Location of the feature. */
		Coordinate coordinate{};
		/** Uncertainty of #coordinate, radius, in pixels. */
		std::optional<uint16_t> coordinateUncertainty{};

		/**
		 * Direction pointing away from the center of the curve, in
		 * degrees [0,359] counterclockwise to the right, following
		 * conventions from ANSI/NIST-ITL 1-2011 (2015) Field 9.320.
		 */
		std::optional<uint16_t> direction{};
		/** Uncertainty of #direction, in degrees [0, 180]. */
		std::optional<uint8_t> directionUncertainty{};

		/**
		 * @brief
		 * Core constructor.
		 *
		 * @param coordinate
		 * Location of the feature.
		 * @param direction
		 * Direction pointing away from the center of the curve, in
		 * degrees [0,359] counterclockwise to the right, following
		 * conventions from ANSI/NIST-ITL 1-2011 (2015) Field 9.320.
		 * @param coordinateUncertainty
		 * Uncertainty of #coordinate, radius, in pixels.
		 * @param directionUncertainty
		 * Uncertainty of #direction, in degrees [0, 180].
		 */
		Core(
		    const Coordinate &coordinate = {},
		    const std::optional<uint16_t> &direction = {},
		    const std::optional<uint16_t> coordinateUncertainty = {},
		    const std::optional<uint8_t> directionUncertainty = {});
	};

	/** Singular point of ridge divergence. */
	struct Delta
	{
		/** Location of the feature. */
		Coordinate coordinate{};
		/** Uncertainty of #coordinate, radius, in pixels. */
		std::optional<uint16_t> coordinateUncertainty{};

		/**
		 * Ridge directions of the feature (typically up, left, and
		 * right), in degrees [0,359] counterclockwise to the right,
		 * following conventions from ANSI/NIST-ITL 1-2011 (2015) Field
		 * 9.321.
		 */
		std::optional<std::tuple<std::optional<uint16_t>,
		    std::optional<uint16_t>, std::optional<uint16_t>>>
		    direction{};
		/** Uncertainty of #direction, in degrees [0, 180]. */
		std::optional<std::tuple<std::optional<uint8_t>,
		    std::optional<uint8_t>, std::optional<uint8_t>>>
		    directionUncertainty{};

		/**
		 * Delta constructor.
		 *
		 * @param coordinate
		 * Location of the feature.
		 * @param direction
		 * Ridge directions of the feature (typically up, left, and
		 * right), in degrees [0,359] counterclockwise to the right,
		 * following conventions from ANSI/NIST-ITL 1-2011 (2015) Field
		 * 9.321.
		 * @param coordinateUncertainty
		 * Uncertainty of #coordinate, radius, in pixels.
		 * @param directionUncertainty
		 * Uncertainty of #direction, in degrees [0, 180].
		 */
		Delta(
		    const Coordinate &coordinate = {},
		    const std::optional<std::tuple<std::optional<uint16_t>,
		        std::optional<uint16_t>, std::optional<uint16_t>>>
		        &direction = {},
		    std::optional<uint16_t> coordinateUncertainty = {},
		    const std::optional<std::tuple<std::optional<uint8_t>,
		        std::optional<uint8_t>, std::optional<uint8_t>>>
		        &directionUncertainty = {});
	};

	/**
	 * Types of correspondence.
	 *
	 * @details
	 * Following ANSI/NIST-ITL 1-2011 (2015) Field 9.361, "types of
	 * correspondence (TOC)"
	 */
	enum class CorrespondenceType
	{
		/** Probe feature definitely corresponds. */
		Definite,
		/** Probe feature possibly/debatably corresponds. */
		Possible,
		/**
		 * Probe feature definitely does not exist.
		 *
		 * @note
		 * Correspondence#referenceMinutia will be ignored.
		 */
		DoesNotExist,
		/**
		 * Probe feature lies outside the reference.
		 *
		 * @note
		 * Correspondence#referenceMinutia will be ignored.
		 */
		OutOfRegion,
		/**
		 * Probe feature lies in an area experiencing quality issues in
		 * the reference.
		 *
		 * @note
		 * Correspondence#referenceMinutia will be ignored.
		 */
		UnclearArea
	};
	using TOC = CorrespondenceType;

	/** Relationship between probe and reference features. */
	struct CorrespondenceRelationship
	{
		/** Type of correspondence seen at these points. */
		CorrespondenceType type{};

		/**
		 * @brief
		 * Identifier from the probe template.
		 *
		 * @note
		 * This is `identifier` from
		 * ExtractionInterface::createReferenceTemplate.
		 */
		std::string probeIdentifier{};
		/**
		 * Link to Image#identifier and/or Features#identifier for
		 * probe.
		 */
		uint8_t probeInputIdentifier{};
		/** Location in the probe image of a reference image feature. */
		Minutia probeMinutia{};

		/**
		 * @brief
		 * Identifier from the reference template.
		 *
		 * @note
		 * This is `identifier` from
		 * ExtractionInterface::createReferenceTemplate.
		 */
		std::string referenceIdentifier{};
		/**
		 * Link to Image#identifier and/or Features#identifier for
		 * reference.
		 */
		uint8_t referenceInputIdentifier{};
		/**
		 * Location in the reference image of a probe image feature.
		 * @note
		 * This Minutia may be omitted if the #type indicates it.
		 */
		Minutia referenceMinutia{};

		/**
		 * @brief
		 * CorrespondenceRelationship constructor.
		 *
		 * @param type
		 * Type of correspondence seen at these points.
		 * @param probeIdentifier
		 * Identifier from the probe template.
		 * @param probeInputIdentifier
		 * Link to Image#identifier and/or Features#identifier for
		 * probe.
		 * @param probeMinutia
		 * Location in the probe image of a reference image feature.
		 * @param referenceIdentifier
		 * Identifier from the reference template.
		 * @param referenceInputIdentifier
		 * Link to Image#identifier and/or Features#identifier for
		 * reference.
		 * @param referenceMinutia
		 * Location in the reference image of a probe image feature.
		 * This Minutia may be omitted only if the #type indicates it.
		 */
		CorrespondenceRelationship(
		    const CorrespondenceType type = {},
		    const std::string &probeIdentifier = {},
		    const uint8_t probeInputIdentifier = {},
		    const Minutia &probeMinutia = {},
		    const std::string &referenceIdentifier = {},
		    const uint8_t referenceInputIdentifier = {},
		    const Minutia &referenceMinutia = {});
	};

	/** Local ridge quality codes from ANSI/NIST-ITL 1-2011 (2015). */
	enum class RidgeQuality
	{
		/** No ridge information. */
		Background = 0,
		/** Continuity of ridge flow is uncertain. */
		DebatableRidgeFlow = 1,
		/**
		 * Continuity of ridge flow is certain; minutiae are
		 * debatable.
		 */
		DebatableMinutiae = 2,
		/**
		 * Minutiae and ridge flow are obvious and unambiguous; ridge
		 * edges are debatable.
		 */
		DefinitiveMinutiae = 3,
		/**
		 * Ridge edges, minutiae, and ridge flow are obvious and
		 * unambiguous; pores are either debatable or not present.
		 */
		DefinitiveRidgeEdges = 4,
		/** Pores and ridge edges are obvious and unambiguous. */
		DefinitivePores = 5
	};

	/** Region defined in a map of ridge quality/confidence. */
	struct RidgeQualityRegion
	{
		/**
		 * Closed convex polygon whose contents is #quality.
		 *
		 * @details
		 * Coordinate are relative to the bounding rectangle created by
		 * Features::roi, if supplied. Otherwise, they are relative to
		 * the the source image. Add the minimum X and Y values from
		 * Features::roi to convert ROI-relative Coordinate to
		 * image-relative Coordinate.
		 */
		std::vector<Coordinate> region{};
		/** Clarity of ridge features enclosed within #region. */
		RidgeQuality quality{RidgeQuality::Background};
	};

	/** Permanent flexion creases on fingers. */
	enum class FingerCrease
	{
		/** Below distal phalanx. */
		DistalInterphalangeal,
		/** Below medial phalanx. */
		ProximalInterphalangeal,
		/** Between proximal phalanx and palm. */
		ProximalDigital
	};

	/** Permanent flexion creases on palms. */
	enum class PalmCrease
	{
		/** Bottom of the interdigital area. */
		DistalTransverse,
		/** Colloquial definition for DistalTransverse. */
		Top = DistalTransverse,

		/** Diagonal across palm. */
		ProximalTransverse,
		/** Colloquial definition for ProximalTransverse. */
		Middle = ProximalTransverse,

		/** Around the base of thenar. */
		RadialLongitudinal,
		/** Colloquial definition for RadialLongitudinal. */
		Bottom = RadialLongitudinal,

		/** Bottom of the palm at the wrist. */
		Wrist,
		/** Colloquial definition for Wrist. */
		WristBracelet = Wrist
	};

	/** Linear discontinuities that are not named permanent creases. */
	enum class LinearDiscontinuity
	{
		MinorCrease,
		Crack,
		Cut,
		NonPermanentScar,
		/*
		 * XXX: Considered a distinctive area in ANSI/NIST-ITL 1-2011
		 * Update:2015, requiring a polygon, not a segment.
		 */
//		PermanentScar,
		Other
	};

	/**
	 * Collection of crease types.
	 *
	 * @note
	 * These enumerations *mostly* map to ANSI/NIST-ITL 1-2011 Update:2015's
	 * TPD "Permanent Flexion Creases" codes from Table 58.
	 */
	using CreaseClassification = std::variant<FingerCrease, PalmCrease,
	    LinearDiscontinuity>;

	/** Friction ridge linear flexion creases. */
	class Crease
	{
	public:
		/**
		 * @brief
		 * Crease constructor.
		 *
		 * @param fgp
		 * Region where this crease occurs.
		 * @param cld
		 * The type of crease.
		 * @param segments
		 * Set of straight line segments overlaying the crease.
		 *
		 * @note
		 * The #segments need not form a single line, where each Segment
		 * begins where the last Segment ended. In cases where a crease
		 * is feathered or a series of crisscross creases, ANSI/NIST-ITL
		 * 1-2011 Update:2015 instructs users to mark each shorter
		 * section separately.
		 */
		Crease(
		    const FrictionRidgeGeneralizedPosition fgp,
		    const CreaseClassification cld,
		    const std::vector<Segment> &segments);

		/**
		 * @return
		 * Region where this crease occurs.
		 */
		FrictionRidgeGeneralizedPosition
		getFrictionRidgeGeneralizedPosition()
		    const;

		/**
		 * @param fgp
		 * Region where this crease occurs.
		 */
		void
		setFrictionRidgeGeneralizedPosition(
		    const FrictionRidgeGeneralizedPosition fgp);

		/**
		 * @return
		 * Type of crease.
		 */
		CreaseClassification
		getCreaseClassification()
		    const;

		/**
		 * @param cld
		 * Type of crease.
		 *
		 * @throw
		 * Attempt to set a CreaseClassificaiton of type PalmCrease
		 * when #fgp does not contain the palmar surface.
		 */
		void
		setCreaseClassification(
		    const CreaseClassification cld);

		/**
		 * @return
		 * Set of straight line segments overlaying the crease.
		 *
		 * @note
		 * The #segments need not form a single line, where each Segment
		 * begins where the last Segment ended. In cases where a crease
		 * is feathered or a series of crisscross creases, ANSI/NIST-ITL
		 * 1-2011 Update:2015 instructs users to mark each shorter
		 * section separately.
		 */
		std::vector<Segment>
		getSegments()
		    const;

		/**
		 * @param segment
		 * Segment to add to existing set of segments.
		 */
		void
		addSegment(
		    const Segment &segment);

		/**
		 * @param segments
		 * Set of segments to replace existing set of segments.
		 */
		void
		setSegments(
		    const std::vector<Segment> &segments);

	private:
		/** Region where this crease occurs. */
		FrictionRidgeGeneralizedPosition fgp{};
		/** Type of crease. */
		CreaseClassification cld{};
		/**
		 * Set of straight line segments overlaying the crease.
		 *
		 * @note
		 * The #segments need not form a single line, where each Segment
		 * begins where the last Segment ended. In cases where a crease
		 * is feathered or a series of crisscross creases, ANSI/NIST-ITL
		 * 1-2011 Update:2015 instructs users to mark each shorter
		 * section separately.
		 */
		std::vector<Segment> segments{};
	};

	/**
	 * Collection of ANSI/NIST-ITL 1-2011 (Update: 2015) Extended Feature
	 * Set fields understood by FRIF.
	 *
	 * @note
	 * All measurements and locations within the image SHALL be expressed in
	 * pixels, *not* units of 10 micrometers.
	 */
	struct Features
	{
		/**
		 * An identifier for this set of data. Used to link Features to
		 * Image, TemplateData, and Correspondence.
		 */
		uint8_t identifier{};

		/**
		 * Resolution of the image used to derive these features in
		 * pixels per inch.
		 */
		uint16_t ppi{};

		/** Impression type of the depicted region. */
		Impression imp{Impression::Unknown};
		/** Capture technology that created this image. */
		FrictionRidgeCaptureTechnology frct{
		    FrictionRidgeCaptureTechnology::Unknown};
		/** Description of the depicted region. */
		FrictionRidgeGeneralizedPosition frgp{
		    FrictionRidgeGeneralizedPosition::UnknownFrictionRidge};

		/** Degrees to rotate image upright. */
		std::optional<int16_t> orientation{};
		/**
		 * Uncertainty of #orientation, resulting in the orientation to
		 * be #orientation +/- #orientationUncertainty degrees.
		 */
		std::optional<uint8_t> orientationUncertainty{};
		/** Methods used process the print. */
		std::optional<std::vector<ProcessingMethod>> lpm{};
		/** Examiner/algorithmic value assessment for identification. */
		std::optional<ValueAssessment> valueAssessment{};
		/** Substrate from which the print was developed. */
		std::optional<Substrate> lsb{};
		/** Observed pattern classification. */
		std::optional<PatternClassification> pat{};

		/**
		 * Image is known to be or may possibly be laterally reversed.
		 */
		std::optional<bool> plr{};
		/**
		 * Part or all of image is known to be or may possibly be
		 * tonally reversed.
		 */
		std::optional<bool> trv{};

		/**
		 * Core locations.
		 *
		 * @details
		 * Coordinate are relative to the bounding rectangle created
		 * by #roi, if supplied. Otherwise, they are relative to the
		 * source image. Add the minimum X and Y values from #roi
		 * to convert ROI-relative Coordinate to image-relative
		 * Coordinate.
		 */
		std::optional<std::vector<Core>> cores{};
		/**
		 * Delta locations.
		 *
		 * @details
		 * Coordinate are relative to the bounding rectangle created
		 * by #roi, if supplied. Otherwise, they are relative to the
		 * source image. Add the minimum X and Y values from #roi
		 * to convert ROI-relative Coordinate to image-relative
		 * Coordinate.
		 */
		std::optional<std::vector<Delta>> deltas{};
		/**
		 * Locations of minutiae.
		 *
		 * @details
		 * Coordinate are relative to the bounding rectangle created
		 * by #roi, if supplied. Otherwise, they are relative to the
		 * source image. Add the minimum X and Y values from #roi
		 * to convert ROI-relative Coordinate to image-relative
		 * Coordinate.
		 *
		 * @note
		 * NIST **strongly** discourages more than one Minutia at
		 * equivalent Coordinate. This can result in ambiguous
		 * Correspondence.
		 */
		std::optional<std::vector<Minutia>> minutiae{};
		/** Creases and other linear discontinuities. */
		std::optional<std::vector<Crease>> creases{};
		/**
		 * Closed convex polygon forming region of interest.
		 *
		 * @details
		 * When specified, Coordinate in Features are relative to the
		 * bounding rectangle created here. Otherwise, they are relative
		 * to the source image. Add the minimum X and Y values here to
		 * convert ROI-relative Coordinate to image-relative
		 * Coordinate.
		 *
		 */
		std::optional<std::vector<Coordinate>> roi{};

		/**
		 * Assessment of ridge quality within local areas of an image.
		 *
		 * @details
		 * Coordinate are relative to the bounding rectangle created
		 * by #roi, if supplied. Otherwise, they are relative to the
		 * source image. Add the minimum X and Y values from #roi
		 * to convert ROI-relative Coordinate to image-relative
		 * Coordinate.
		 *
		 * @note
		 * If populated, regions not explicitly defined will default to
		 * RidgeQuality::Background.
		 */
		std::optional<std::vector<RidgeQualityRegion>> rqm{};

		/**
		 * @brief
		 * Whether or not feature extraction was complex.
		 *
		 * @details
		 * Complexity should be determined as specified by the
		 * documentation for the "analysis complexity flag (CXF)" of
		 * ANSI/NIST-ITL 1-2011 (2015) Field 9.353.
		 */
		std::optional<bool> complex{};
	};
}

#endif /* FRIF_EFS_H_ */
