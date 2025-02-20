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

#include "frifte_e1n_data.h"

FRIF::TemplateArchive
FRIF::Evaluations::Exemplar1N::Validation::Data::getTemplateArchive(
    const std::string &databaseName,
    const std::string &outputDir)
{
	const auto dir = outputDir / Data::getTemplateDir(
	    TemplateType::Reference) / databaseName;

	return {dir / Data::TemplateArchiveArchiveName, dir /
	    Data::TemplateArchiveManifestName};
}

const std::filesystem::path&
FRIF::Evaluations::Exemplar1N::Validation::Data::getTemplateDir(
    TemplateType type)
{
	switch (type) {
	case TemplateType::Probe:
		return (ProbeTemplateDir);
	case TemplateType::Reference:
		return (ReferenceTemplateDir);
	default:
		throw std::runtime_error{"Unknown TemplateType sent to "
		    "Data::getTemplateDir()"};
	}
}

FRIF::Evaluations::Exemplar1N::Validation::Data::Dataset
FRIF::Evaluations::Exemplar1N::Validation::Data::readCSV(
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
	std::unordered_map<std::string, std::vector<Input>> inputMap{};

	/* Skip header */
	if (!std::getline(csv, line))
		throw std::runtime_error{"Could not read header of " +
		    csvPath.string()};

	while (std::getline(csv, line)) {
		/* Allow comments */
		if ((line.length() > 1) && line[0] == '#')
			continue;
		/* Allow blank lines */
		if (line.empty())
			continue;

		static const uint16_t expectedCols{15};
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
		if (!cols[1].empty() && cols[1] != "NA") {
			auto im = Data::CSVImage{};
			im.filename = cols[1];
			im.width = std::stoull(cols[2]);
			im.height = std::stoull(cols[3]);

			if (std::stoul(cols[4]) > std::numeric_limits<decltype(
			    CSVImage::ppi)::value_type>::max())
				throw std::runtime_error{"PPI out of range"};
			else
				im.ppi = std::stoul(cols[4]);

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
		}

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

		/*
		 * Cores (don't load uncertainty)
		 */
		std::stringstream coordSS{}, multiValSS{cols[12]};
		std::string coord{}, multiVal{};
		std::vector<std::string> coords{}, multiVals{};
		static const uint8_t expectedCoreCoords{3};
		while (std::getline(multiValSS, multiVal, multiValueSep))
			multiVals.push_back(multiVal);
		if (!multiVals.empty() &&
		    !((multiVals.size() == 1) &&
		    (multiVals.front() == Util::NA))) {
			for (const auto &mv : multiVals) {
				coordSS.str(mv);
				while (std::getline(coordSS, coord, coordSep))
					coords.push_back(coord);
				if (coords.size() != expectedCoreCoords)
					throw std::runtime_error{"Expected " +
					    std::to_string(expectedCoreCoords) +
					    " columns for core, read " +
					    std::to_string(coords.size())};
			}

			if ((std::stoul(coords[0]) > std::numeric_limits<
			    decltype(Coordinate::x)>::max()) ||
			    (std::stoul(coords[1]) > std::numeric_limits<
			    decltype(Coordinate::y)>::max()) ||
			    (std::stoul(coords[2]) > std::numeric_limits<
			    uint16_t>::max()))
				throw std::runtime_error{"Core coord out of "
				    "range"};
			else {
				if (features.cores.has_value())
					(*features.cores).emplace_back(
					    Coordinate(static_cast<decltype(
					    Coordinate::x)>(std::stoul(
					    coords[0])),
					    static_cast<decltype(
					    Coordinate::y)>(std::stoul(
					    coords[1]))),
					    static_cast<uint16_t>(std::stoul(
					    coords[2])));
				else
					features.cores = {EFS::Core{Coordinate(
					    static_cast<decltype(
					    Coordinate::x)>(std::stoul(
					    coords[0])),
					    static_cast<decltype(
					    Coordinate::y)>(std::stoul(
					    coords[1]))),
					    static_cast<uint16_t>(std::stoul(
					    coords[2]))}};
			}
		}

		/*
		 * Deltas (don't load uncertainty)
		 */
		multiValSS.str(cols[13]);
		coords.clear();
		multiVals.clear();
		static const uint8_t expectedDeltaCoords{5};
		while (std::getline(multiValSS, multiVal, multiValueSep))
			multiVals.push_back(multiVal);
		if (!multiVals.empty() &&
		    !((multiVals.size() == 1) &&
		    (multiVals.front() == Util::NA))) {
			for (const auto &mv : multiVals) {
				coordSS.str(mv);
				while (std::getline(coordSS, coord, coordSep))
					coords.push_back(coord);
				if (coords.size() != expectedDeltaCoords)
					throw std::runtime_error{"Expected " +
					    std::to_string(
					    expectedDeltaCoords) + " "
					    "columns for delta, read " +
					    std::to_string(coords.size())};
			}

			if ((std::stoul(coords[0]) > std::numeric_limits<
			    decltype(Coordinate::x)>::max()) ||
			    (std::stoul(coords[1]) > std::numeric_limits<
			    decltype(Coordinate::y)>::max()) ||
			    (std::stoul(coords[2]) > std::numeric_limits<
			    uint16_t>::max()) ||
			    (std::stoul(coords[3]) > std::numeric_limits<
			    uint16_t>::max()) ||
			    (std::stoul(coords[4]) > std::numeric_limits<
			    uint16_t>::max()))
				throw std::runtime_error{"Delta coord out of "
				    "range"};
			else {
				if (features.deltas.has_value())
					(*features.deltas).emplace_back(
					    Coordinate{
					    static_cast<decltype(
					    Coordinate::x)>(std::stoul(
					    coords[0])),
					    static_cast<decltype(
					    Coordinate::y)>(std::stoul(
					    coords[1]))},
					    std::make_tuple(
					    static_cast<uint16_t>(std::stoul(
					    coords[2])),
					    static_cast<uint16_t>(std::stoul(
					    coords[3])),
					    static_cast<uint16_t>(std::stoul(
					    coords[4]))));
				else
					features.deltas = {EFS::Delta{
					    Coordinate{
					    static_cast<decltype(
					    Coordinate::x)>(std::stoul(
					    coords[0])),
					    static_cast<decltype(
					    Coordinate::y)>(std::stoul(
					    coords[1]))},
					    std::make_tuple(
					    static_cast<uint16_t>(std::stoul(
					    coords[2])),
					    static_cast<uint16_t>(std::stoul(
					    coords[3])),
					    static_cast<uint16_t>(std::stoul(
					    coords[4])))}};
			}
		}

		/*
		 * Minutia
		 */
		multiValSS.str(cols[14]);
		coords.clear();
		multiVals.clear();
		static const uint8_t expectedMinutiaCoords{4};
		while (std::getline(multiValSS, multiVal, multiValueSep))
			multiVals.push_back(multiVal);
		if (!multiVals.empty() &&
		    !((multiVals.size() == 1) &&
		    (multiVals.front() == Util::NA))) {
			for (const auto &mv : multiVals) {
				coordSS.str(mv);
				while (std::getline(coordSS, coord, coordSep))
					coords.push_back(coord);
				if (coords.size() != expectedMinutiaCoords)
					throw std::runtime_error{"Expected " +
					    std::to_string(
					    expectedMinutiaCoords) + " "
					    "columns for minutia, read " +
					    std::to_string(coords.size())};
			}

			if ((std::stoul(coords[0]) > std::numeric_limits<
			    decltype(Coordinate::x)>::max()) ||
			    (std::stoul(coords[1]) > std::numeric_limits<
			    decltype(Coordinate::y)>::max()) ||
			    (std::stoul(coords[2]) > 359))
				throw std::runtime_error{"Minutia value out "
				    "of range"};
			else {
				if (features.minutiae.has_value())
					(*features.minutiae).emplace_back(
					    Coordinate{
					    static_cast<decltype(
					    Coordinate::x)>(std::stoul(
					    coords[0])),
					    static_cast<decltype(
					    Coordinate::y)>(std::stoul(
					    coords[1]))},
					    static_cast<decltype(
					    EFS::Minutia::theta)>(std::stoul(
					    coords[2])),
					    EFS::toMinutiaType(std::stoi(
					    coords[3])));
				else
					features.minutiae = {EFS::Minutia{
					    Coordinate{
					    static_cast<decltype(
					    Coordinate::x)>(std::stoul(
					    coords[0])),
					    static_cast<decltype(
					    Coordinate::y)>(std::stoul(
					    coords[1]))},
					    static_cast<decltype(
					    EFS::Minutia::theta)>(std::stoul(
					    coords[2])),
					    EFS::toMinutiaType(std::stoi(
					    coords[3]))}};
			}
		}

		input.features = features;

		/* Collect samples that all belong to the same subject ID */
		if (inputMap.contains(cols[0]))
			inputMap[cols[0]].push_back(input);
		else
			inputMap[cols[0]] = {input};
	}

	Dataset dataset{};
	std::get<std::string>(dataset) = csvPath.stem().string();
	for (const auto &[id, in] : inputMap) {
		std::get<std::vector<ImageSet>>(dataset).emplace_back(id, in);
	}

	return (dataset);
}
