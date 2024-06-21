/*
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 */

#include <frif/efs.h>

FRIF::EFS::CorrespondenceRelationship::CorrespondenceRelationship(
    CorrespondenceType type_,
    const std::string &probeIdentifier_,
    const uint8_t probeInputIdentifier_,
    const Minutia &probeMinutia_,
    const std::string &referenceIdentifier_,
    const uint8_t referenceInputIdentifier_,
    const Minutia &referenceMinutia_) :
    type{type_},
    probeIdentifier{probeIdentifier_},
    probeInputIdentifier{probeInputIdentifier_},
    probeMinutia{probeMinutia_},
    referenceIdentifier{referenceIdentifier_},
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
