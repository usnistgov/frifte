/*
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 */

#include <frifte/quality.h>

FRIF::Evaluations::Quality::Interface::Interface() = default;
FRIF::Evaluations::Quality::Interface::~Interface() = default;

void
FRIF::Evaluations::Quality::VerboseQuality::setUnifiedQualityScore(
    const int uqs_)
{
	this->setUnifiedQualityScore(
	    EFS::QualityMeasure{static_cast<double>(uqs_)});
}

void
FRIF::Evaluations::Quality::VerboseQuality::setUnifiedQualityScore(
    const EFS::QualityMeasure &uqs_)
{
	if (!uqs_.hasValue()) {
		this->uqs = uqs_;
		return;
	}

	/* Enforce whole number */
	if (static_cast<double>(static_cast<int>(uqs_.getValue())) !=
	    uqs_.getValue())
		throw std::out_of_range{std::to_string(*uqs_) + " "
		    "shall be a whole number [0,100]"};

	/* Enforce [0,100] */
	if ((*uqs_ < 0) || (*uqs_ > 100))
		throw std::out_of_range{std::to_string(*uqs_) + " "
		    "shall be a whole number [0,100]"};

	this->uqs = uqs_;
}

FRIF::EFS::QualityMeasure
FRIF::Evaluations::Quality::VerboseQuality::getUnifiedQualityScore()
    const
{
	return (this->uqs);
}

void
FRIF::Evaluations::Quality::VerboseQuality::setROI(
    const std::vector<Coordinate> &roi_)
{
	this->roi = roi_;
}

std::optional<std::vector<FRIF::Coordinate>>
FRIF::Evaluations::Quality::VerboseQuality::getROI()
    const
{
	return (this->roi);
}

void
FRIF::Evaluations::Quality::VerboseQuality::setRidgeQualityMap(
    const std::vector<FRIF::EFS::RidgeQualityRegion> &rqm_)
{
	this->rqm = rqm_;
}

std::optional<std::vector<FRIF::EFS::RidgeQualityRegion>>
FRIF::Evaluations::Quality::VerboseQuality::getRidgeQualityMap()
    const
{
	return (this->rqm);
}

void
FRIF::Evaluations::Quality::VerboseQuality::addComponent(
    const Component component,
    const EFS::QualityMeasure &value)
{
	if (!this->components)
		this->components.emplace();

	if (value.hasValue()) {
		switch (component) {
		case Component::Amputated:
			[[fallthrough]];
		case Component::AnatomicalCluesPresent:
			[[fallthrough]];
		case Component::Bandaged:
			[[fallthrough]];
		case Component::ClearCoreDeltaRidgeCount:
			[[fallthrough]];
		case Component::ComplexMatrix:
			[[fallthrough]];
		case Component::ComplexSubstrate:
			[[fallthrough]];
		case Component::CompressedRidges:
			[[fallthrough]];
		case Component::Condensation:
			[[fallthrough]];
		case Component::CorePresent:
			[[fallthrough]];
		case Component::DeltaPresent:
			[[fallthrough]];
		case Component::DevelopmentArtifacts:
			[[fallthrough]];
		case Component::DistinctiveClusterPresent:
			[[fallthrough]];
		case Component::EvenPressure:
			[[fallthrough]];
		case Component::EvidenceOfFraud:
			[[fallthrough]];
		case Component::ExcessiveDistortion:
			[[fallthrough]];
		case Component::ExtremeTip:
			[[fallthrough]];
		case Component::FocalPointsPresent:
			[[fallthrough]];
		case Component::Ghosting:
			[[fallthrough]];
		case Component::IsolatedRidgeClusters:
			[[fallthrough]];
		case Component::Level3Present:
			[[fallthrough]];
		case Component::MinutiaePresent:
			[[fallthrough]];
		case Component::Movement:
			[[fallthrough]];
		case Component::MultipleNonOverlappingImpressions:
			[[fallthrough]];
		case Component::MultipleOverlappingImpressions:
			[[fallthrough]];
		case Component::OpenField:
			[[fallthrough]];
		case Component::OrientationCertaintyLevel:
			[[fallthrough]];
		case Component::OrientationClue:
			[[fallthrough]];
		case Component::PatternClassification:
			[[fallthrough]];
		case Component::PatternForceMinutiae:
			[[fallthrough]];
		case Component::RidgeColorUncertain:
			[[fallthrough]];
		case Component::RidgeFlowContinuity:
			[[fallthrough]];
		case Component::RidgeValleyClarity:
			[[fallthrough]];
		case Component::SpottyRidges:
			[[fallthrough]];
		case Component::TonalReversal:
			[[fallthrough]];
		case Component::WetRidges:
			if ((*value < 0) || (*value > 100) ||
			    (static_cast<double>(
			    static_cast<int>(*value)) != *value))
				throw std::out_of_range{std::to_string(*value) +
				    " shall be an integer on the range "
				    "[0, 100]"};
			break;

		case Component::NumHighConfidenceMinutiae:
			[[fallthrough]];
		case Component::NumMinutiae:
			if ((*value < 0) || (static_cast<double>(
			    static_cast<int>(*value)) != *value))
				throw std::out_of_range{std::to_string(*value) +
				    " shall be 0 or a positive integer"};
			break;

		case Component::SpatialSamplingRatePPI:
			if ((*value <= 0) || (static_cast<double>(
			    static_cast<int>(*value)) != *value))
				throw std::out_of_range{std::to_string(*value) +
				    " shall be a positive integer"};
			break;

		case Component::RidgeValleyUniformity:
			if (*value < 0)
				throw std::out_of_range{std::to_string(*value) +
				    " shall be a positive integer"};
			break;
		}
	}

	this->components->insert_or_assign(component, value);
}

void
FRIF::Evaluations::Quality::VerboseQuality::removeComponent(
    const Component component)
{
	if (!this->components)
		return;

	this->components->erase(component);
}

std::optional<std::unordered_map<
    FRIF::Evaluations::Quality::Component, FRIF::EFS::QualityMeasure>>
FRIF::Evaluations::Quality::VerboseQuality::getComponents()
    const
{
	return (this->components);
}

std::optional<FRIF::EFS::QualityMeasure>
FRIF::Evaluations::Quality::VerboseQuality::getComponent(
    const FRIF::Evaluations::Quality::Component component)
    const
{
	if (!this->components)
		return (std::nullopt);

	const auto it = this->components->find(component);
	if (it == this->components->cend())
		return (std::nullopt);
	return (it->second);
}

bool
FRIF::Evaluations::Quality::VerboseQuality::hasComponent(
    const FRIF::Evaluations::Quality::Component component)
    const
{
	if (!this->components)
		return (false);

	return (this->components->find(component) != this->components->cend());
}
