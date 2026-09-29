/*
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 */

#include <fstream>
#include <string>
#include <unordered_map>

#include "frifte_quality_data.h"

FRIF::Evaluations::Quality::Validation::Data::Dataset
FRIF::Evaluations::Quality::Validation::Data::readCSV(
    const std::filesystem::path &csvPath,
    const char colSep,
    const char coordSep,
    const char multiValueSep)
{
	if ((colSep == coordSep) || (colSep == multiValueSep) ||
	    (coordSep == multiValueSep))
		throw std::runtime_error{"Separators cannot be identical"};


	std::ifstream csv{csvPath};
	if (!csv)
		throw std::runtime_error{"Could not open " + csvPath.string()};

	std::string line{};
	std::unordered_map<std::string, Input> inputMap{};

	/* Skip header */
	if (!std::getline(csv, line))
		throw std::runtime_error{"Could not read header of " +
		    csvPath.string()};

	while (std::getline(csv, line)) {
		/* Allow comments */
		if ((line.length() >= 1) && line[0] == '#')
			continue;
		/* Allow blank lines */
		if (line.empty())
			continue;

		static const uint16_t expectedCols{12};
		std::vector<std::string> cols{};
		cols.reserve(expectedCols);

		std::stringstream lineSS{line};
		std::string col{};
		while (std::getline(lineSS, col, colSep))
			cols.push_back(col);
		if (cols.size() != expectedCols)
			throw std::runtime_error{"Expected " +
			    std::to_string(expectedCols) + " columns, read " +
			    std::to_string(cols.size()) + " in " +
			    csvPath.string()};

		Input input{};
		auto im = Data::CSVImage{};
		im.filename = cols[1];

		unsigned long ulval = std::stoul(cols[2]);
		if (ulval > std::numeric_limits<decltype(CSVImage::width)>::
		    max())
			throw std::runtime_error{"Width out of range"};
		else
			im.width = static_cast<decltype(CSVImage::width)>(
			    ulval);

		ulval = std::stoul(cols[3]);
		if (ulval > std::numeric_limits<decltype(CSVImage::height)>::
		    max())
			throw std::runtime_error{"Height out of range"};
		else
			im.height = static_cast<decltype(CSVImage::height)>(
			    ulval);

		ulval = std::stoul(cols[4]);
		if (ulval > std::numeric_limits<decltype(CSVImage::ppi)>::max())
			throw std::runtime_error{"PPI out of range"};
		else
			im.ppi = static_cast<decltype(CSVImage::height)>(ulval);

		if (Util::lower(cols[5]) == "grayscale")
			im.colorspace = Image::Colorspace::Grayscale;
		else if (Util::lower(cols[5]) == "rgb")
			im.colorspace = Image::Colorspace::RGB;
		else
			throw std::runtime_error{"Invalid colorspace "
			    "value: " + cols[5]};

		im.bpc = Image::toBitsPerChannel(static_cast<uint8_t>(
		    std::stoul(cols[6])));
		im.bpp = Image::toBitsPerPixel(static_cast<uint8_t>(
		    std::stoul(cols[7])));

		input.image = im;


		EFS::Features features{};
		if (std::stoul(cols[8]) >
		    std::numeric_limits<decltype(EFS::Features::ppi)>::max())
			throw std::runtime_error{"PPI out of range"};
		else
			features.ppi = static_cast<decltype(
			    EFS::Features::ppi)>(std::stoul(cols[8]));

		features.imp = EFS::toImpression(std::stoi(cols[9]));
		features.frct = EFS::toFrictionRidgeCaptureTechnology(
		    std::stoi(cols[10]));
		features.frgp = EFS::toFrictionRidgeGeneralizedPosition(
		    std::stoi(cols[11]));

		input.features = features;

		/* use filename as identifier */
		inputMap[cols[1]] = input;
	}

	Dataset dataset{};
	std::get<std::string>(dataset) = csvPath.stem().string();
	for (const auto &[id, in] : inputMap) {
		std::get<std::vector<ImageSet>>(dataset).emplace_back(id, in);
	}

	return (dataset);
}
