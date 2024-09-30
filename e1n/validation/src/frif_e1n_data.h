/*
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 */

#ifndef FRIF_E1N_DATA_H_
#define FRIF_E1N_DATA_H_

#include <filesystem>
#include <functional>
#include <optional>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include <frif/frif.h>

namespace FRIF::Evaluations::Exemplar1N::Validation::Data
{
	/** Directory within output directory to contain databases */
	const std::filesystem::path DatabaseDir{"reference_databases"};
	/** Directory within output directory to contain templates */
	const std::filesystem::path TemplateDir{"templates"};
	/** Directory containing generated latent templates. */
	const std::filesystem::path ProbeTemplateDir{TemplateDir / "probe"};
	/** Directory containing generated reference templates. */
	const std::filesystem::path ReferenceTemplateDir{TemplateDir /
	    "reference"};
	/** Suffix added to template files. */
	const std::string TemplateSuffix{".tmpl"};
	/** Name of TemplateArchive archive. */
	const std::string TemplateArchiveArchiveName{"archive"};
	/** Name of TemplateArchive manifest. */
	const std::string TemplateArchiveManifestName{"manifest"};

	/** Directories containing images to templatize as probes. */
	const std::array<std::string, 6> ProbeNames {
	    "contactless-A",
	    "nonsense",
	    "palm-J",
	    "plain-M",
	    "roll-V",
	    "slap-R"
	};

	/** Directories containing images to templatize as references. */
	const std::array<std::string, 9> ReferenceNames {
	    "nonsense",
	    "palm-N",
	    "plain-M",
	    "roll-U",
	    "roll-U-palm-N",
	    "roll-U-roll-V-slap-R",
	    "segmented-slap-R",
	    "slap-R-contactless-A",
	    "slap-R-roll-U"
	};

	/** Probe + Reference pairs to search, returning identifiers and FGP. */
	const std::array<std::pair<std::string, std::string>, 4>
	SubjectPositionSearchSets {{
	    {"contactless-A", "palm-N"},
	    {"roll-V", "roll-U"},
	    {"roll-V", "slap-R-contactless-A"},
	    {"plain-M", "roll-U-roll-V-slap-R"}
	}};

	/** Probe + Reference pairs to search, returning only identifiers. */
	const std::array<std::pair<std::string, std::string>, 2>
	SubjectSearchSets {{
	    {"palm-J", "palm-N"},
	    {"slap-R", "roll-U"}
	}};

	/** Information about a validation image within the image CSV. */
	struct CSVImage
	{
		/** Name of the file within image directory. */
		std::optional<std::string> filename{};
		/** Width of the image. */
		std::optional<uint16_t> width{};
		/** Height of the image. */
		std::optional<uint16_t> height{};
		/** Resolution of the image in pixels per inch. */
		std::optional<uint16_t> ppi{};
		/** Image colorspace (always Grayscale for E1N) */
		std::optional<Image::Colorspace> colorspace{};
		/** Number of bits used by each color component (8 or 16). */
		std::optional<Image::BitsPerChannel> bpc{};
		/** Number of bits comprising a single pixel (8 or 16). */
		std::optional<Image::BitsPerPixel> bpp{};
	};

	/** Image and/or Features */
	struct Input
	{
		/** Image data */
		std::optional<CSVImage> image{};
		/** EFS data */
		std::optional<EFS::Features> features{};
	};

	/** Hard-coded images (image identifier + input). */
	using ImageSet = std::pair<std::string, std::vector<Input>>;
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

	/**
	 * @brief
	 * Obtain expected TemplateArchive for given dataset.
	 *
	 * @param databaseName
	 * Name of the database on disk.
	 * @param outputDir
	 * Location where output is stored.
	 *
	 * @return
	 * TemplateArchive with expected path to given dataset archive.
	 *
	 * @note
	 * This method does not check for existence.
	 */
	FRIF::TemplateArchive
	getTemplateArchive(
	    const std::string &databaseName,
	    const std::string &outputDir);

	/**
	 * @brief
	 * Obtain path where templates are stored.
	 *
	 * @param type
	 * Type of template.
	 *
	 * @return
	 * Path to directory where `type` templates are stored.
	 */
	const std::filesystem::path&
	getTemplateDir(
	    TemplateType type);

}

#endif /* FRIF_E1N_DATA_H_ */
