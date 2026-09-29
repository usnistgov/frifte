/*
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 */

#ifndef FRIF_EVALUATIONS_QUALITY_VALIDATION_DATA_H_
#define FRIF_EVALUATIONS_QUALITY_VALIDATION_DATA_H_

#include <filesystem>
#include <functional>
#include <map>
#include <optional>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include <frifte/frifte.h>

namespace FRIF::Evaluations::Quality::Validation::Data
{
	enum class ImageContent
	{
		DistalExemplar,
		DistalLatent,
		NonDistalExemplar,
		NonDistalLatent,
		PalmExemplar,
		PalmLatent,
		UnknownLatent
	};

	const std::map<ImageContent, std::string> DatasetNames
	{
		{ImageContent::DistalExemplar, "DistalExemplar"},
		{ImageContent::DistalLatent, "DistalLatent"},
		{ImageContent::NonDistalExemplar, "NonDistalExemplar"},
		{ImageContent::NonDistalLatent, "NonDistalLatent"},
		{ImageContent::PalmExemplar, "PalmExemplar"},
		{ImageContent::PalmLatent, "PalmLatent"},
		{ImageContent::UnknownLatent, "UnknownLatent"}
	};

	/** Information about a validation image within the image CSV. */
	struct CSVImage
	{
		/** Name of the file within image directory. */
		std::string filename{};
		/** Width of the image. */
		uint16_t width{};
		/** Height of the image. */
		uint16_t height{};
		/** Resolution of the image in pixels per inch. */
		uint16_t ppi{};
		/** Image colorspace (always Grayscale) */
		Image::Colorspace colorspace{Image::Colorspace::Grayscale};
		/** Number of bits used by each color component (8 or 16). */
		Image::BitsPerChannel bpc{};
		/** Number of bits comprising a single pixel (8 or 16). */
		Image::BitsPerPixel bpp{};
	};

	/** Image and possibly Features */
	struct Input
	{
		/** Image data */
		CSVImage image{};
		/** EFS data */
		std::optional<EFS::Features> features{};
	};

	/** Hard-coded images (image identifier + input). */
	using ImageSet = std::pair<std::string, Input>;
	/** Dataset identifier + sets of hard-coded input pairs. */
	using Dataset = std::tuple<std::string, std::vector<ImageSet>>;

	/**
	 * @brief
	 * Instantiate Dataset from contents of CSV.
	 *
	 * @param csvPath
	 * Path to CSV file formatted as expected for validation.
	 * @param colSep
	 * Character that separates columns.
	 * @param coordSep
	 * Character that separates coordinates within a column
	 * @param multiValueSep
	 * Character that separates separate values within a column (e.g.,
	 * multiple coordinates.)
	 *
	 * @return
	 * Dataset representation of contents of CSV.
	 *
	 * @throw
	 * Illformed data in CSV, error opening/reading csvPath, or separators
	 * are identical.
	 */
	Dataset
	readCSV(
	    const std::filesystem::path &csvPath,
	    const char colSep = ',',
	    const char coordSep = ';',
	    const char multiValueSep = '|');
}

#endif /* FRIF_EVALUATIONS_QUALITY_VALIDATION_DATA_H_ */
