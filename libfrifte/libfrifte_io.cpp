/*
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 */

#include <frifte/io.h>

FRIF::SubjectPositionCandidate::SubjectPositionCandidate(
    const std::string &identifier_,
    const EFS::FrictionRidgeGeneralizedPosition fgp_) :
    identifier{identifier_},
    fgp{fgp_}
{

}

auto
FRIF::SubjectPositionCandidate::operator<=>(const FRIF::SubjectPositionCandidate&)
    const = default;

bool
FRIF::SubjectPositionCandidate::operator==(const FRIF::SubjectPositionCandidate&)
    const = default;

std::size_t
FRIF::SubjectPositionCandidateListKeyHash::operator()(
    const FRIF::SubjectPositionCandidate &c)
    const
    noexcept
{
	/*
	 * Keys are guaranteed to not contain spaces (per
	 * FRIF::TemplateArchive), so combining a unique identifier
	 * with a finger position delimited by a space results in a unique
	 * string for that identifier + position combination.
	 */
	return (std::hash<std::string>{}(c.identifier + ' ' +
	    std::to_string(std::underlying_type_t<
	    EFS::FrictionRidgeGeneralizedPosition>(c.fgp))));
}
