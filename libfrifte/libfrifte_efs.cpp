/*
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 */

#include <frifte/efs.h>

FRIF::EFS::CorrespondenceRelationship::CorrespondenceRelationship(
    CorrespondenceType type_,
    const std::string &probeIdentifier_,
    const FrictionRidgeGeneralizedPosition probeFGP_,
    const uint8_t probeInputIdentifier_,
    const Minutia &probeMinutia_,
    const std::string &referenceIdentifier_,
    const FrictionRidgeGeneralizedPosition referenceFGP_,
    const uint8_t referenceInputIdentifier_,
    const Minutia &referenceMinutia_) :
    type{type_},
    probeIdentifier{probeIdentifier_},
    probeFGP{probeFGP_},
    probeInputIdentifier{probeInputIdentifier_},
    probeMinutia{probeMinutia_},
    referenceIdentifier{referenceIdentifier_},
    referenceFGP{referenceFGP_},
    referenceInputIdentifier{referenceInputIdentifier_},
    referenceMinutia{referenceMinutia_}
{

}

FRIF::EFS::Minutia::Minutia(
    const Coordinate &coordinate_,
    const uint16_t theta_,
    const MinutiaType type_,
    const std::optional<uint16_t> coordinateUncertainty_,
    const std::optional<uint8_t> thetaUncertainty_) :
    coordinate{coordinate_},
    coordinateUncertainty{coordinateUncertainty_},
    theta{theta_},
    thetaUncertainty{thetaUncertainty_},
    type{type_}
{

}

FRIF::EFS::Core::Core(
    const Coordinate &coordinate_,
    const std::optional<uint16_t> &direction_,
    const std::optional<uint16_t> coordinateUncertainty_,
    const std::optional<uint8_t> directionUncertainty_) :
    coordinate{coordinate_},
    coordinateUncertainty{coordinateUncertainty_},
    direction{direction_},
    directionUncertainty{directionUncertainty_}
{

}

FRIF::EFS::Delta::Delta(
    const Coordinate &coordinate_,
    const std::optional<std::tuple<std::optional<uint16_t>,
        std::optional<uint16_t>, std::optional<uint16_t>>> &direction_,
    const std::optional<uint16_t> coordinateUncertainty_,
    const std::optional<std::tuple<std::optional<uint8_t>,
	std::optional<uint8_t>, std::optional<uint8_t>>>
	&directionUncertainty_) :
    coordinate{coordinate_},
    coordinateUncertainty{coordinateUncertainty_},
    direction{direction_},
    directionUncertainty{directionUncertainty_}
{

}

FRIF::EFS::Crease::Crease(
    const FrictionRidgeGeneralizedPosition fgp_,
    const CreaseClassification cld_,
    const std::vector<Segment> &segments_) :
    fgp{fgp_},
    segments{segments_}
{
	this->setCreaseClassification(cld_);
}

FRIF::EFS::FrictionRidgeGeneralizedPosition
FRIF::EFS::Crease::getFrictionRidgeGeneralizedPosition()
    const
{
	return (this->fgp);
}

void
FRIF::EFS::Crease::setFrictionRidgeGeneralizedPosition(
    const FRIF::EFS::FrictionRidgeGeneralizedPosition fgp_)
{
	this->fgp = fgp_;
}

FRIF::EFS::CreaseClassification
FRIF::EFS::Crease::getCreaseClassification()
    const
{
	return (this->cld);
}

void
FRIF::EFS::Crease::setCreaseClassification(
    const FRIF::EFS::CreaseClassification cld_)
{
	if (std::get_if<PalmCrease>(&cld_) != nullptr) {
		switch (this->fgp) {
		case FrictionRidgeGeneralizedPosition::UnknownFinger:
			[[ fallthrough ]];
		case FrictionRidgeGeneralizedPosition::RightThumb:
			[[ fallthrough ]];
		case FrictionRidgeGeneralizedPosition::RightIndex:
			[[ fallthrough ]];
		case FrictionRidgeGeneralizedPosition::RightMiddle:
			[[ fallthrough ]];
		case FrictionRidgeGeneralizedPosition::RightRing:
			[[ fallthrough ]];
		case FrictionRidgeGeneralizedPosition::RightLittle:
			[[ fallthrough ]];
		case FrictionRidgeGeneralizedPosition::LeftThumb:
			[[ fallthrough ]];
		case FrictionRidgeGeneralizedPosition::LeftIndex:
			[[ fallthrough ]];
		case FrictionRidgeGeneralizedPosition::LeftMiddle:
			[[ fallthrough ]];
		case FrictionRidgeGeneralizedPosition::LeftRing:
			[[ fallthrough ]];
		case FrictionRidgeGeneralizedPosition::LeftLittle:
			[[ fallthrough ]];
		case FrictionRidgeGeneralizedPosition::RightExtraDigit:
			[[ fallthrough ]];
		case FrictionRidgeGeneralizedPosition::LeftExtraDigit:
			[[ fallthrough ]];

		case FrictionRidgeGeneralizedPosition::RightFour:
			[[ fallthrough ]];
		case FrictionRidgeGeneralizedPosition::LeftFour:
			[[ fallthrough ]];
		case FrictionRidgeGeneralizedPosition::RightAndLeftThumbs:
			[[ fallthrough ]];

		case FrictionRidgeGeneralizedPosition::EJIOrTip:
			[[ fallthrough ]];
		case FrictionRidgeGeneralizedPosition::UnknownFrictionRidge:
			throw std::logic_error{"CreaseClassification = "
			    "PalmCrease, but FrictionRidgeGeneralizedPosition "
			    "is not from a palm"};

		case FrictionRidgeGeneralizedPosition::UnknownPalm:
			[[ fallthrough ]];
		case FrictionRidgeGeneralizedPosition::RightFullPalm:
			[[ fallthrough ]];
		case FrictionRidgeGeneralizedPosition::RightWritersPalm:
			[[ fallthrough ]];
		case FrictionRidgeGeneralizedPosition::LeftFullPalm:
			[[ fallthrough ]];
		case FrictionRidgeGeneralizedPosition::LeftWritersPalm:
			[[ fallthrough ]];
		case FrictionRidgeGeneralizedPosition::RightLowerPalm:
			[[ fallthrough ]];
		case FrictionRidgeGeneralizedPosition::RightUpperPalm:
			[[ fallthrough ]];
		case FrictionRidgeGeneralizedPosition::LeftLowerPalm:
			[[ fallthrough ]];
		case FrictionRidgeGeneralizedPosition::LeftUpperPalm:
			[[ fallthrough ]];
		case FrictionRidgeGeneralizedPosition::RightPalmOther:
			[[ fallthrough ]];
		case FrictionRidgeGeneralizedPosition::LeftPalmOther:
			[[ fallthrough ]];
		case FrictionRidgeGeneralizedPosition::RightInterdigital:
			[[ fallthrough ]];
		case FrictionRidgeGeneralizedPosition::RightThenar:
			[[ fallthrough ]];
		case FrictionRidgeGeneralizedPosition::RightHypothenar:
			[[ fallthrough ]];
		case FrictionRidgeGeneralizedPosition::LeftInterdigital:
			[[ fallthrough ]];
		case FrictionRidgeGeneralizedPosition::LeftThenar:
			[[ fallthrough ]];
		case FrictionRidgeGeneralizedPosition::LeftHypothenar:
			[[ fallthrough ]];
		case FrictionRidgeGeneralizedPosition::RightGrasp:
			[[ fallthrough ]];
		case FrictionRidgeGeneralizedPosition::LeftGrasp:
			[[ fallthrough ]];
		case FrictionRidgeGeneralizedPosition::RightCarpalDeltaArea:
			[[ fallthrough ]];
		case FrictionRidgeGeneralizedPosition::LeftCarpalDeltaArea:
			[[ fallthrough ]];
		case FrictionRidgeGeneralizedPosition::
		    RightFullPalmAndWritersPalm:
			[[ fallthrough ]];
		case FrictionRidgeGeneralizedPosition::
		    LeftFullPalmAndWritersPalm:
			[[ fallthrough ]];
		case FrictionRidgeGeneralizedPosition::RightWristBracelet:
			[[ fallthrough ]];
		case FrictionRidgeGeneralizedPosition::LeftWristBracelet:
			break;
		}
	}

	this->cld = cld_;
}

std::vector<FRIF::Segment>
FRIF::EFS::Crease::getSegments()
    const
{
	return (this->segments);
}

void
FRIF::EFS::Crease::addSegment(
    const FRIF::Segment &segment)
{
	this->segments.push_back(segment);
}

void
FRIF::EFS::Crease::setSegments(
    const std::vector<FRIF::Segment> &segments_)
{
	this->segments = segments_;
}

FRIF::EFS::PatternClassification::PatternClassification(
    const PatternGeneralClassification gcf_,
    const std::optional<PatternSubclassification> sub_,
    const std::optional<WhorlDeltaRelationship> wdr_)
{
	this->setPatternGeneralClassification(gcf_);

	if (sub_.has_value())
		this->setPatternSubclassification(sub_.value());

	if (wdr_.has_value())
		this->setWhorlDeltaRelationship(wdr_.value());

}

void
FRIF::EFS::PatternClassification::setPatternGeneralClassification(
    const PatternGeneralClassification gcf_)
{
	this->gcf = gcf_;

	this->sub = std::nullopt;
	this->wdr = std::nullopt;
}

void
FRIF::EFS::PatternClassification::setPatternSubclassification(
    const PatternSubclassification sub_)
{
	switch (this->gcf) {
	case PatternGeneralClassification::Arch:
		if (std::get_if<ArchPatternSubclassification>(&sub_) == nullptr)
			throw std::logic_error{"PatternGeneralClassification = "
			    "Arch, but PatternSubclassification is not an "
			    "ArchPatternSubclassification"};

		break;
	case PatternGeneralClassification::Whorl:
		if (std::get_if<WhorlPatternSubclassification>(&sub_) ==
		    nullptr)
			throw std::logic_error{"PatternGeneralClassification = "
			    "Whorl, but PatternSubclassification is not a "
			    "WhorlPatternSubclassification"};
		break;
	default:
		throw std::logic_error{"PatternGeneralClassification is not "
		    "Arch or Whorl"};
	}

	this->sub = sub_;
}

void
FRIF::EFS::PatternClassification::setWhorlDeltaRelationship(
    const WhorlDeltaRelationship wdr_)
{
	if (this->gcf != PatternGeneralClassification::Whorl)
		throw std::logic_error{"PatternGeneralClassification is not "
		    "Whorl"};
	if (!this->sub.has_value())
		throw std::logic_error{"PatternSubClassification is not set"};
	if (std::get_if<WhorlPatternSubclassification>(&this->sub.value()) ==
	    nullptr)
		throw std::logic_error{"PatternGeneralClassification = Whorl, "
		    "but PatternSubclassification is not a "
		    "WhorlPatternSubclassification"};

	this->wdr = wdr_;
}

FRIF::EFS::PatternGeneralClassification
FRIF::EFS::PatternClassification::getPatternGeneralClassification()
    const
{
	return (this->gcf);
}

std::optional<FRIF::EFS::PatternSubclassification>
FRIF::EFS::PatternClassification::getPatternSubclassification()
    const
{
	return (this->sub);
}

std::optional<FRIF::EFS::WhorlDeltaRelationship>
FRIF::EFS::PatternClassification::getWhorlDeltaRelationship()
    const
{
	return (this->wdr);
}

FRIF::EFS::Impression
FRIF::EFS::toImpression(
    const std::underlying_type_t<Impression> i)
{
	switch (static_cast<Impression>(i)) {
	case EFS::Impression::PlainContact:
		[[fallthrough]];
	case EFS::Impression::RolledContact:
		[[fallthrough]];
	case EFS::Impression::Latent:
		[[fallthrough]];
	case EFS::Impression::LiveScanSwipe:
		[[fallthrough]];
	case EFS::Impression::PlainContactlessStationary:
		[[fallthrough]];
	case EFS::Impression::RolledContactlessStationary:
		[[fallthrough]];
	case EFS::Impression::Other:
		[[fallthrough]];
	case EFS::Impression::Unknown:
		[[fallthrough]];
	case EFS::Impression::Contactless:
		return (static_cast<Impression>(i));
	default:
		throw std::runtime_error{"Invalid Impression value: " +
		    std::to_string(i)};
	}
}

FRIF::EFS::FrictionRidgeCaptureTechnology
FRIF::EFS::toFrictionRidgeCaptureTechnology(
    const std::underlying_type_t<FrictionRidgeCaptureTechnology> i)
{
	switch (static_cast<FRIF::EFS::FrictionRidgeCaptureTechnology>(i)) {
	case EFS::FrictionRidgeCaptureTechnology::Unknown:
		[[fallthrough]];
	case EFS::FrictionRidgeCaptureTechnology::ScannedInkOnPaper:
		[[fallthrough]];
	case EFS::FrictionRidgeCaptureTechnology::OpticalTIRBright:
		[[fallthrough]];
	case EFS::FrictionRidgeCaptureTechnology::OpticalDirect:
		[[fallthrough]];
	case EFS::FrictionRidgeCaptureTechnology::Capacitive:
		[[fallthrough]];
	case EFS::FrictionRidgeCaptureTechnology::Electroluminescent:
		[[fallthrough]];
	case EFS::FrictionRidgeCaptureTechnology::LatentImpression:
		[[fallthrough]];
	case EFS::FrictionRidgeCaptureTechnology::LatentLift:
		return (static_cast<FrictionRidgeCaptureTechnology>(i));
	default:
		throw std::runtime_error{"Invalid FrictionRidgeCapture"
		    "Technology value: " + std::to_string(i)};
	}
}

FRIF::EFS::FrictionRidgeGeneralizedPosition
FRIF::EFS::toFrictionRidgeGeneralizedPosition(
    const std::underlying_type_t<FrictionRidgeGeneralizedPosition> i)
{
	switch (static_cast<FRIF::EFS::FrictionRidgeGeneralizedPosition>(i)) {
	case EFS::FrictionRidgeGeneralizedPosition::UnknownFinger:
		[[fallthrough]];
	case EFS::FrictionRidgeGeneralizedPosition::RightThumb:
		[[fallthrough]];
	case EFS::FrictionRidgeGeneralizedPosition::RightIndex:
		[[fallthrough]];
	case EFS::FrictionRidgeGeneralizedPosition::RightMiddle:
		[[fallthrough]];
	case EFS::FrictionRidgeGeneralizedPosition::RightRing:
		[[fallthrough]];
	case EFS::FrictionRidgeGeneralizedPosition::RightLittle:
		[[fallthrough]];
	case EFS::FrictionRidgeGeneralizedPosition::LeftThumb:
		[[fallthrough]];
	case EFS::FrictionRidgeGeneralizedPosition::LeftIndex:
		[[fallthrough]];
	case EFS::FrictionRidgeGeneralizedPosition::LeftMiddle:
		[[fallthrough]];
	case EFS::FrictionRidgeGeneralizedPosition::LeftRing:
		[[fallthrough]];
	case EFS::FrictionRidgeGeneralizedPosition::LeftLittle:
		[[fallthrough]];
	case EFS::FrictionRidgeGeneralizedPosition::RightExtraDigit:
		[[fallthrough]];
	case EFS::FrictionRidgeGeneralizedPosition::LeftExtraDigit:
		[[fallthrough]];
	case EFS::FrictionRidgeGeneralizedPosition::RightFour:
		[[fallthrough]];
	case EFS::FrictionRidgeGeneralizedPosition::LeftFour:
		[[fallthrough]];
	case EFS::FrictionRidgeGeneralizedPosition::RightAndLeftThumbs:
		[[fallthrough]];
	case EFS::FrictionRidgeGeneralizedPosition::UnknownPalm:
		[[fallthrough]];
	case EFS::FrictionRidgeGeneralizedPosition::RightFullPalm:
		[[fallthrough]];
	case EFS::FrictionRidgeGeneralizedPosition::RightWritersPalm:
		[[fallthrough]];
	case EFS::FrictionRidgeGeneralizedPosition::LeftFullPalm:
		[[fallthrough]];
	case EFS::FrictionRidgeGeneralizedPosition::LeftWritersPalm:
		[[fallthrough]];
	case EFS::FrictionRidgeGeneralizedPosition::RightLowerPalm:
		[[fallthrough]];
	case EFS::FrictionRidgeGeneralizedPosition::RightUpperPalm:
		[[fallthrough]];
	case EFS::FrictionRidgeGeneralizedPosition::LeftLowerPalm:
		[[fallthrough]];
	case EFS::FrictionRidgeGeneralizedPosition::LeftUpperPalm:
		[[fallthrough]];
	case EFS::FrictionRidgeGeneralizedPosition::RightPalmOther:
		[[fallthrough]];
	case EFS::FrictionRidgeGeneralizedPosition::LeftPalmOther:
		[[fallthrough]];
	case EFS::FrictionRidgeGeneralizedPosition::RightInterdigital:
		[[fallthrough]];
	case EFS::FrictionRidgeGeneralizedPosition::RightThenar:
		[[fallthrough]];
	case EFS::FrictionRidgeGeneralizedPosition::RightHypothenar:
		[[fallthrough]];
	case EFS::FrictionRidgeGeneralizedPosition::LeftInterdigital:
		[[fallthrough]];
	case EFS::FrictionRidgeGeneralizedPosition::LeftThenar:
		[[fallthrough]];
	case EFS::FrictionRidgeGeneralizedPosition::LeftHypothenar:
		[[fallthrough]];
	case EFS::FrictionRidgeGeneralizedPosition::RightGrasp:
		[[fallthrough]];
	case EFS::FrictionRidgeGeneralizedPosition::LeftGrasp:
		[[fallthrough]];
	case EFS::FrictionRidgeGeneralizedPosition::RightCarpalDeltaArea:
		[[fallthrough]];
	case EFS::FrictionRidgeGeneralizedPosition::LeftCarpalDeltaArea:
		[[fallthrough]];
	case EFS::FrictionRidgeGeneralizedPosition::RightFullPalmAndWritersPalm:
		[[fallthrough]];
	case EFS::FrictionRidgeGeneralizedPosition::LeftFullPalmAndWritersPalm:
		[[fallthrough]];
	case EFS::FrictionRidgeGeneralizedPosition::RightWristBracelet:
		[[fallthrough]];
	case EFS::FrictionRidgeGeneralizedPosition::LeftWristBracelet:
		[[fallthrough]];
	case EFS::FrictionRidgeGeneralizedPosition::UnknownFrictionRidge:
		[[fallthrough]];
	case EFS::FrictionRidgeGeneralizedPosition::EJIOrTip:
		return (static_cast<FrictionRidgeGeneralizedPosition>(i));
	default:
		throw std::runtime_error{"Invalid FrictionRidgeGeneralized"
		    "Position value: " + std::to_string(i)};
	}
}

FRIF::EFS::Orientation::Orientation(
    const int16_t direction_,
    const std::optional<uint8_t> &uncertainty_) :
    direction{direction_},
    uncertainty{uncertainty_}
{

}

FRIF::EFS::ProcessingMethod
FRIF::EFS::toProcessingMethod(
    const std::underlying_type_t<ProcessingMethod> i)
{
	switch (static_cast<ProcessingMethod>(i)) {
	case EFS::ProcessingMethod::Indanedione:
		[[fallthrough]];
	case EFS::ProcessingMethod::BlackPowder:
		[[fallthrough]];
	case EFS::ProcessingMethod::Other:
		[[fallthrough]];
	case EFS::ProcessingMethod::Cyanoacrylate:
		[[fallthrough]];
	case EFS::ProcessingMethod::Laser:
		[[fallthrough]];
	case EFS::ProcessingMethod::RUVIS:
		[[fallthrough]];
	case EFS::ProcessingMethod::StickysidePowder:
		[[fallthrough]];
	case EFS::ProcessingMethod::Visual:
		[[fallthrough]];
	case EFS::ProcessingMethod::WhitePowder:
		return (static_cast<ProcessingMethod>(i));
	default:
		throw std::runtime_error{"Invalid ProcessingMethod value: " +
		    std::to_string(i)};
	}
}

FRIF::EFS::PatternGeneralClassification
FRIF::EFS::toPatternGeneralClassification(
    const std::underlying_type_t<PatternGeneralClassification> i)
{
	switch (static_cast<PatternGeneralClassification>(i)) {
	case EFS::PatternGeneralClassification::Arch:
		[[fallthrough]];
	case EFS::PatternGeneralClassification::Whorl:
		[[fallthrough]];
	case EFS::PatternGeneralClassification::RightLoop:
		[[fallthrough]];
	case EFS::PatternGeneralClassification::LeftLoop:
		[[fallthrough]];
	case EFS::PatternGeneralClassification::Amputation:
		[[fallthrough]];
	case EFS::PatternGeneralClassification::UnableToPrint:
		[[fallthrough]];
	case EFS::PatternGeneralClassification::Unclassifiable:
		[[fallthrough]];
	case EFS::PatternGeneralClassification::Scar:
		[[fallthrough]];
	case EFS::PatternGeneralClassification::DissociatedRidges:
		return (static_cast<PatternGeneralClassification>(i));
	default:
		throw std::runtime_error{"Invalid PatternGeneralClassification "
		    "value: " + std::to_string(i)};
	}
}

FRIF::EFS::ArchPatternSubclassification
FRIF::EFS::toArchPatternSubclassification(
    const std::underlying_type_t<ArchPatternSubclassification> i)
{
	switch (static_cast<ArchPatternSubclassification>(i)) {
	case EFS::ArchPatternSubclassification::PlainArch:
		[[fallthrough]];
	case EFS::ArchPatternSubclassification::TentedArch:
		return (static_cast<ArchPatternSubclassification>(i));
	default:
		throw std::runtime_error{"Invalid ArchPatternSubclassification "
		    "value: " + std::to_string(i)};
	}
}

FRIF::EFS::WhorlPatternSubclassification
FRIF::EFS::toWhorlPatternSubclassification(
    const std::underlying_type_t<WhorlPatternSubclassification> i)
{
	switch (static_cast<WhorlPatternSubclassification>(i)) {
	case EFS::WhorlPatternSubclassification::PlainWhorl:
		[[fallthrough]];
	case EFS::WhorlPatternSubclassification::CentralPocketLoop:
		[[fallthrough]];
	case EFS::WhorlPatternSubclassification::DoubleLoop:
		[[fallthrough]];
	case EFS::WhorlPatternSubclassification::AccidentalWhorl:
		return (static_cast<WhorlPatternSubclassification>(i));
	default:
		throw std::runtime_error{"Invalid WhorlPattern"
		    "Subclassification value: " + std::to_string(i)};
	}
}

FRIF::EFS::WhorlDeltaRelationship
FRIF::EFS::toWhorlDeltaRelationship(
    const std::underlying_type_t<WhorlDeltaRelationship> i)
{
	switch (static_cast<WhorlDeltaRelationship>(i)) {
	case EFS::WhorlDeltaRelationship::Inner:
		[[fallthrough]];
	case EFS::WhorlDeltaRelationship::Outer:
		[[fallthrough]];
	case EFS::WhorlDeltaRelationship::Meeting:
		return (static_cast<WhorlDeltaRelationship>(i));
	default:
		throw std::runtime_error{"Invalid WhorlDeltaRelationship "
		    "value: " + std::to_string(i)};
	}
}

FRIF::EFS::ValueAssessment
FRIF::EFS::toValueAssessment(
    const std::underlying_type_t<ValueAssessment> i)
{
	switch (static_cast<ValueAssessment>(i)) {
	case EFS::ValueAssessment::Value:
		[[fallthrough]];
	case EFS::ValueAssessment::Limited:
		[[fallthrough]];
	case EFS::ValueAssessment::NoValue:
		[[fallthrough]];
	case EFS::ValueAssessment::NonPrint:
		return (static_cast<ValueAssessment>(i));
	default:
		throw std::runtime_error{"Invalid ValueAssessment value: " +
		    std::to_string(i)};
	}
}

FRIF::EFS::Substrate
FRIF::EFS::toSubstrate(
    const std::underlying_type_t<Substrate> i)
{
	switch (static_cast<Substrate>(i)) {
	case EFS::Substrate::Paper:
		[[fallthrough]];
	case EFS::Substrate::PorousOther:
		[[fallthrough]];
	case EFS::Substrate::Plastic:
		[[fallthrough]];
	case EFS::Substrate::Glass:
		[[fallthrough]];
	case EFS::Substrate::MetalPainted:
		[[fallthrough]];
	case EFS::Substrate::MetalUnpainted:
		[[fallthrough]];
	case EFS::Substrate::TapeAdhesiveSide:
		[[fallthrough]];
	case EFS::Substrate::NonporousOther:
		[[fallthrough]];
	case EFS::Substrate::PaperGlossy:
		[[fallthrough]];
	case EFS::Substrate::SemiporousOther:
		[[fallthrough]];
	case EFS::Substrate::Other:
		[[fallthrough]];
	case EFS::Substrate::Unknown:
		return (static_cast<Substrate>(i));
	default:
		throw std::runtime_error{"Invalid Substrate value: " +
		    std::to_string(i)};
	}
}

FRIF::EFS::MinutiaType
FRIF::EFS::toMinutiaType(
    const std::underlying_type_t<MinutiaType> i)
{
	switch (static_cast<EFS::MinutiaType>(i)) {
	case EFS::MinutiaType::RidgeEnding:
		[[fallthrough]];
	case EFS::MinutiaType::Bifurcation:
		[[fallthrough]];
	case EFS::MinutiaType::Other:
		[[fallthrough]];
	case EFS::MinutiaType::Unknown:
		return (static_cast<MinutiaType>(i));
	default:
		throw std::runtime_error{"Invalid MinutiaType value: " +
		    std::to_string(i)};
	}
}

FRIF::EFS::CorrespondenceType
FRIF::EFS::toCorrespondenceType(
    const std::underlying_type_t<CorrespondenceType> i)
{
	switch (static_cast<CorrespondenceType>(i)) {
	case EFS::CorrespondenceType::Definite:
		[[fallthrough]];
	case EFS::CorrespondenceType::Possible:
		[[fallthrough]];
	case EFS::CorrespondenceType::DoesNotExist:
		[[fallthrough]];
	case EFS::CorrespondenceType::OutOfRegion:
		[[fallthrough]];
	case EFS::CorrespondenceType::UnclearArea:
		return (static_cast<CorrespondenceType>(i));
	default:
		throw std::runtime_error{"Invalid CorrespondenceType value: " +
		    std::to_string(i)};
	}
}

FRIF::EFS::RidgeQuality
FRIF::EFS::toRidgeQuality(
    const std::underlying_type_t<RidgeQuality> i)
{
	switch (static_cast<EFS::RidgeQuality>(i)) {
	case EFS::RidgeQuality::Background:
		[[fallthrough]];
	case EFS::RidgeQuality::DebatableRidgeFlow:
		[[fallthrough]];
	case EFS::RidgeQuality::DebatableMinutiae:
		[[fallthrough]];
	case EFS::RidgeQuality::DefinitiveMinutiae:
		[[fallthrough]];
	case EFS::RidgeQuality::DefinitiveRidgeEdges:
		[[fallthrough]];
	case EFS::RidgeQuality::DefinitivePores:
		return (static_cast<RidgeQuality>(i));
	default:
		throw std::runtime_error{"Invalid RidgeQuality value: " +
		    std::to_string(i)};
	}
}

FRIF::EFS::FingerCrease
FRIF::EFS::toFingerCrease(
    const std::underlying_type_t<FingerCrease> i)
{
	switch (static_cast<EFS::FingerCrease>(i)) {
	case EFS::FingerCrease::DistalInterphalangeal:
		[[fallthrough]];
	case EFS::FingerCrease::ProximalInterphalangeal:
		[[fallthrough]];
	case EFS::FingerCrease::ProximalDigital:
		return (static_cast<FingerCrease>(i));
	default:
		throw std::runtime_error{"Invalid FingerCrease value: " +
		    std::to_string(i)};
	}
}

FRIF::EFS::PalmCrease
FRIF::EFS::toPalmCrease(
    const std::underlying_type_t<PalmCrease> i)
{
	switch (static_cast<EFS::PalmCrease>(i)) {
	case EFS::PalmCrease::DistalTransverse:
		[[fallthrough]];
	case EFS::PalmCrease::ProximalTransverse:
		[[fallthrough]];
	case EFS::PalmCrease::RadialLongitudinal:
		[[fallthrough]];
	case EFS::PalmCrease::Wrist:
		return (static_cast<PalmCrease>(i));
	default:
		throw std::runtime_error{"Invalid PalmCrease value: " +
		    std::to_string(i)};
	}
}

FRIF::EFS::LinearDiscontinuity
FRIF::EFS::toLinearDiscontinuity(
    const std::underlying_type_t<LinearDiscontinuity> i)
{
	switch (static_cast<EFS::LinearDiscontinuity>(i)) {
	case EFS::LinearDiscontinuity::MinorCrease:
		[[fallthrough]];
	case EFS::LinearDiscontinuity::Crack:
		[[fallthrough]];
	case EFS::LinearDiscontinuity::Cut:
		[[fallthrough]];
	case EFS::LinearDiscontinuity::NonPermanentScar:
		[[fallthrough]];
	case EFS::LinearDiscontinuity::Other:
		return (static_cast<LinearDiscontinuity>(i));
	default:
		throw std::runtime_error{"Invalid LinearDiscontinuity value: " +
		    std::to_string(i)};
	}
}

auto
FRIF::EFS::QualityMeasure::Description::operator<=>(
    const FRIF::EFS::QualityMeasure::Description&)
    const = default;

bool
FRIF::EFS::QualityMeasure::Description::operator==(
    const FRIF::EFS::QualityMeasure::Description&)
    const = default;

const FRIF::EFS::QualityMeasure::Description FRIF::EFS::QualityMeasure::NFIQ2v23
{
	ProductIdentifier{"NFIQ 2 v2.3",
	    ProductIdentifier::CBEFFIdentifier{0x101, 0x0057}},
	"2.3",
	std::nullopt,
	"b4a1e7586b3be906f9770e4b77768038"
};

std::size_t
FRIF::EFS::QualityMeasure::DescriptionHash::operator()(
    const FRIF::EFS::QualityMeasure::Description &d)
    const
{
	static const std::string fs = "\x1C";
	static const std::string us = "\x1F";
	static const std::string gs = "\x1D";

	std::string id{};
	if (d.identifier.has_value()) {
		id += d.identifier->marketing.value_or("") + us;

		if (d.identifier->cbeff.has_value()) {
			id += std::to_string(d.identifier->cbeff->owner) + us;

			if (d.identifier->cbeff->algorithm.has_value())
				id += std::to_string(
				    *d.identifier->cbeff->algorithm);
		} else
			id += us;
	} else
		id += us + us;

	return (std::hash<std::string>{}(
	    id + gs +
	    d.version.value_or("") + gs +
	    d.comment.value_or("") + gs +
	    d.modelSHA256.value_or("") + fs
	));
}

FRIF::EFS::QualityMeasure::QualityMeasure() = default;

FRIF::EFS::QualityMeasure::QualityMeasure(
    const double value_) :
    value{value_}
{

}

FRIF::EFS::QualityMeasure::operator bool()
    const
    noexcept
{
	return (this->hasValue());
}

bool
FRIF::EFS::QualityMeasure::hasValue()
    const
    noexcept
{
	return ((this->status == Status::Success) &&
	    this->value.has_value());
}

double
FRIF::EFS::QualityMeasure::operator*()
    const
{
	return (*this->value);
}

double
FRIF::EFS::QualityMeasure::getValue()
    const
{
	return (this->value.value());
}

FRIF::EFS::QualityMeasure::Status
FRIF::EFS::QualityMeasure::getStatus()
    const
{
	return (this->status);
}

std::optional<std::string>
FRIF::EFS::QualityMeasure::getMessage()
    const
{
	return (this->message);
}

void
FRIF::EFS::QualityMeasure::setMessage(
    const std::string &message_)
{
	this->message = message_;
}
