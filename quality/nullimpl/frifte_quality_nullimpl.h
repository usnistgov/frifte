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

#include <frifte/quality.h>

namespace FRIF::Evaluations::Quality
{
	namespace NullImplementationConstants
	{
		constexpr uint16_t CBEFFProductOwner{0x000F};

		constexpr uint16_t libraryVersionNumber{0x0001};
		constexpr char libraryIdentifier[]{"nullimpl"};

		constexpr uint16_t algorithmID{0xF1A7};
		constexpr char productName[]{
		    "NullImplementation Quality 1.0"};

		constexpr char modelSHA256[]{
		    "fd5aae40d6946b8f859d6d1b6162939764e2009dc34760e928570c8646"
		    "60fbe8"};
	}

	class NullImplementation : public Interface
	{
	public:
		VerboseQuality
		computeVerboseQuality(
		    const Sample &sample)
		    const
		    override;

		virtual
		EFS::QualityMeasure
		computeUnifiedQualityScore(
		    const Sample &sample)
		    const
		    override;

		NullImplementation(
		    const std::filesystem::path &configurationDirectory_);

	private:
		const std::filesystem::path configurationDirectory{};
	};
}

#endif /* FRIF_E1N_NULLIMPL_H_ */
