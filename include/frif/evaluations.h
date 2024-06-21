/*
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 */

#ifndef FRIF_EVALUATIONS_H_
#define FRIF_EVALUATIONS_H_

/** Functionality unique to each of the NIST %FRIF Technology Evaluations. */
namespace FRIF::Evaluations
{
	/**
	 * @brief
	 * Identification information about the submitted library.
	 *
	 * @details
	 * The values provided form the filename of the submitted library and
	 * serve as a unique identifier used to refer to this implementation in
	 * all subsequent publications.
	 */
	struct LibraryIdentifier
	{
		/**
		 * Version number of this submission.
		 *
		 * @note
		 * Required to be unique for each new submission.
		 * @note
		 * Must be identical to the last underscore-delimited token in
		 * in the submitted shared library's filename, in uppercase
		 * hexadecimal.
		 */
		uint16_t versionNumber{};
		/**
		 * @brief
		 * Non-infringing identifier of this submission.
		 *
		 * @note
		 * Should be the same for all submissions from an organization.
		 * @note
		 * Case sensitive.
		 * @note
		 * Must match the regular expression `[:alnum:]+`.
		 * @note
		 * Must be identical to the third underscore-delimited token in
		 * of the submitted shared library's filename.
		 */
		std::string identifier{};
	};
}

#endif /* FRIF_EVALUATIONS_H_ */
