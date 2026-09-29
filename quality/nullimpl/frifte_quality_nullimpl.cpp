/*
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 */

#include <random>

#include <frifte_quality_nullimpl.h>

FRIF::Evaluations::Quality::NullImplementation::NullImplementation(
    const std::filesystem::path &configurationDirectory_) :
    FRIF::Evaluations::Quality::Interface(),
    configurationDirectory{configurationDirectory_}
{

}

FRIF::Evaluations::Quality::VerboseQuality
FRIF::Evaluations::Quality::NullImplementation::computeVerboseQuality(
    const Sample &sample)
    const
{
	/*
	 * NOTE: This method won't be called by the NIST testing apparatus if
	 *       getCompatibility sets all of
	 *       Compatibility::qualityComponents,
	 *       Compatibility::regionOfInterest, and
	 *       Compatibility::ridgeQualityMap to false.
	 */

	if (!std::get<std::optional<Image>>(sample))
		return {};
	const Image &image = std::get<std::optional<Image>>(sample).value();
	const uint32_t w = image.width, h = image.height;

	VerboseQuality ret{};
	ret.setUnifiedQualityScore(this->computeUnifiedQualityScore(sample));
	if (!ret.getUnifiedQualityScore())
		return (ret);

	/*
	 * You probably know what your implementation computes already.
	 */

	const auto features = Interface::getCompatibility();
	if (features.regionOfInterest) {
		ret.setROI({{0, 0}, {0, w - 1}, {h - 1, w - 1},
		    {h - 1, 0}});
	}

	if (features.ridgeQualityMap) {
		EFS::RidgeQualityRegion background{};
		background.region = {{0, 0}, {0, w - 1}, {h - 1, w - 1},
		    {h - 1, 0}};
		background.quality = EFS::RidgeQuality::Background;

		ret.setRidgeQualityMap({background});
	}

	if (features.qualityComponents) {
		static std::mt19937 rng(std::random_device{}());
		auto distribution =
		    std::uniform_int_distribution<int>(0, 1);

		ret.addComponent(Component::Amputated, distribution(rng));
		ret.addComponent(Component::AnatomicalCluesPresent,
		    distribution(rng));

		/* ... others as supported ... */
	}

	return (ret);
}

FRIF::EFS::QualityMeasure
FRIF::Evaluations::Quality::NullImplementation::computeUnifiedQualityScore(
    const Sample &sample)
    const
{
	/*
	 * NOTE: This method might not be called by the NIST testing apparatus
	 *       if getCompatibility sets any of
	 *       Compatibility::qualityComponents,
	 *       Compatibility::regionOfInterest, or
	 *       Compatibility::ridgeQualityMap to true.
	 */

	static std::mt19937 rng(std::random_device{}());
	const int quality = std::uniform_int_distribution<int>(0, 101)(rng);

	/* Simulate failure */
	if (quality == 101)
		return {"Random failure"};

	return {static_cast<double>(quality)};
}

std::optional<FRIF::EFS::QualityMeasure::Description>
FRIF::Evaluations::Quality::Interface::getProductIdentifier()
{
	ProductIdentifier id{};
	id.cbeff = ProductIdentifier::CBEFFIdentifier{};
	id.cbeff->owner = NullImplementationConstants::CBEFFProductOwner;
	id.cbeff->algorithm = NullImplementationConstants::algorithmID;
	id.marketing = NullImplementationConstants::productName;

	EFS::QualityMeasure::Description desc{};
	desc.identifier = id;
	desc.version = std::to_string(
	    NullImplementationConstants::libraryVersionNumber);
	desc.modelSHA256 = NullImplementationConstants::modelSHA256;

	return (desc);
}

FRIF::Evaluations::LibraryIdentifier
FRIF::Evaluations::Quality::getLibraryIdentifier()
{
	LibraryIdentifier li{};
	li.versionNumber =
	    Quality::NullImplementationConstants::libraryVersionNumber;
	li.identifier =
	    Quality::NullImplementationConstants::libraryIdentifier;

	return (li);
}

FRIF::Evaluations::Quality::Interface::Compatibility
FRIF::Evaluations::Quality::Interface::getCompatibility()
{
	Compatibility compatibility{};

	compatibility.regionOfInterest = true;
	compatibility.ridgeQualityMap = true;
	compatibility.qualityComponents = true;

	compatibility.palmLatent = true;
	compatibility.palmExemplar = true;

	compatibility.distalLatent = true;
	compatibility.distalExemplar = true;

	compatibility.nonDistalLatent = true;
	compatibility.nonDistalExemplar = true;

	compatibility.unknownLatent = true;

	return (compatibility);
}

std::shared_ptr<FRIF::Evaluations::Quality::Interface>
FRIF::Evaluations::Quality::Interface::getImplementation(
    const std::filesystem::path &configurationDirectory)
{
	return (std::make_shared<NullImplementation>(configurationDirectory));
}
