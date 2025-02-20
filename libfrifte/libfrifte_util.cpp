/*
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 */

#include <algorithm>
#include <cctype>
#include <chrono>
#include <vector>
#include <random>
#include <numeric>
#include <string>

#include <frifte/efs.h>
#include <frifte/util.h>

std::string
FRIF::Util::duration(
    const std::chrono::steady_clock::time_point &start,
    const std::chrono::steady_clock::time_point &stop)
{
	return (ts(std::chrono::duration_cast<
	    std::chrono::microseconds>(stop - start).count()));
}

std::string&
FRIF::Util::lower(
    std::string &s)
{
	std::transform(s.begin(), s.end(), s.begin(),
	    [](unsigned char c) {
		return (std::tolower(c));
	    });
	return (s);
}

std::vector<uint64_t>
FRIF::Util::randomizeIndices(
    const uint64_t size,
    const uint64_t seed)
{
	std::vector<uint64_t> indices(size);
	std::iota(indices.begin(), indices.end(), 0);
	std::shuffle(indices.begin(), indices.end(), std::mt19937_64(seed));
	return (indices);
}

std::vector<std::byte>
FRIF::Util::readFile(
    const std::filesystem::path &pathName)
{
	return (readFile(pathName.string()));
}

std::vector<std::byte>
FRIF::Util::readFile(
    const std::string &pathName)
{
	std::ifstream file{pathName,
	    std::ifstream::ate | std::ifstream::binary};
	if (!file)
		throw std::runtime_error{"Could not open " + pathName};
	file.unsetf(std::ifstream::skipws);

	const auto size = file.tellg();
	if (size == -1)
		throw std::runtime_error{"Could not open " + pathName};

	std::vector<std::byte> buf{};
	buf.resize(static_cast<std::vector<std::byte>::size_type>(size));
	file.seekg(std::ifstream::beg);
	file.read(reinterpret_cast<char*>(buf.data()), size);

	return (buf);
}

std::string
FRIF::Util::sanitizeMessage(
    const std::string &message,
    const bool escapeQuotes,
    const bool wrapInQuotes)
{
	if (message.empty())
		return (wrapInQuotes ? "\"\"" : message);

	std::string sanitized{message};

	/* Replace characters not matching the documented regex with space */
	auto it = sanitized.begin();
	while ((it = std::find_if_not(sanitized.begin(), sanitized.end(),
	    [](const char &c) -> bool {
		return (std::isgraph(c) || c == ' ');
	    })) != sanitized.end()) {
		sanitized.replace(it, std::next(it), " ");
	}

	/* Replace " with \" (we log to quoted CSV columns) */
	if (escapeQuotes) {
		static const std::string from{"\""};
		static const std::string to{"\\\""};
		std::string::size_type position{0};
		while ((position = sanitized.find(from, position)) !=
		    std::string::npos) {
			sanitized.replace(position, from.length(), to);
			position += to.length();
		}
	}
	return (wrapInQuotes ? '"' + sanitized + '"' : sanitized);
}

std::string
FRIF::Util::splice(
    const std::vector<Coordinate> &v,
    const std::string &elementSep,
    const std::string &itemSep)
{
	if (elementSep == itemSep)
		throw std::runtime_error{"elementSep == itemSep"};

	std::string ret{};
	for (const auto &c : v)
		ret += ts(c.x) + elementSep + ts(c.y) + itemSep;
	ret.erase(ret.find_last_of(itemSep), itemSep.length());
	return (ret);
}

std::string
FRIF::Util::splice(
    const std::vector<EFS::Minutia> &v,
    const std::string &elementSep,
    const std::string &itemSep)
{
	if (elementSep == itemSep)
		throw std::runtime_error{"elementSep == itemSep"};

	std::string ret{};
	for (const auto &m : v)
		ret += ts(m.coordinate.x) + elementSep + ts(m.coordinate.y) +
		    elementSep +
		    (m.coordinateUncertainty ?
		    ts(*m.coordinateUncertainty) : NA) + elementSep +
		    ts(m.theta) + elementSep +
		    (m.thetaUncertainty ? ts(*m.thetaUncertainty) : NA) +
		    elementSep + e2i2s(m.type) + itemSep;
	ret.erase(ret.find_last_of(itemSep), itemSep.length());
	return (ret);
}

std::string
FRIF::Util::splice(
    const std::vector<EFS::Core> &v,
    const std::string &elementSep,
    const std::string &itemSep)
{
	if (elementSep == itemSep)
		throw std::runtime_error{"elementSep == itemSep"};

	std::string ret{};
	for (const auto &c : v)
		ret += ts(c.coordinate.x) + elementSep +
		    ts(c.coordinate.y) + elementSep +
		    (c.coordinateUncertainty ?
		    ts(*c.coordinateUncertainty) : NA) + elementSep +
		    (c.direction ? ts(*c.direction) : NA) + itemSep;
	ret.erase(ret.find_last_of(itemSep), itemSep.length());
	return (ret);
}

std::string
FRIF::Util::splice(
    const std::vector<EFS::Delta> &v,
    const std::string &elementSep,
    const std::string &itemSep)
{
	if (elementSep == itemSep)
		throw std::runtime_error{"elementSep == itemSep"};

	std::string ret{};
	for (const auto &d : v) {
		ret += ts(d.coordinate.x) + elementSep +
		    ts(d.coordinate.y) + elementSep +
		    (d.coordinateUncertainty ?
		    ts(*d.coordinateUncertainty) : NA) + elementSep;
		if (d.direction)
			ret +=
			    (std::get<0>(*d.direction) ?
				ts(*std::get<0>(*d.direction)) : NA) +
				elementSep +
			    (std::get<1>(*d.direction) ?
				ts(*std::get<1>(*d.direction)) : NA) +
				elementSep +
			    (std::get<2>(*d.direction) ?
				ts(*std::get<2>(*d.direction)) : NA) +
				elementSep;
		if (d.directionUncertainty)
			ret +=
			    (std::get<0>(*d.directionUncertainty) ?
				ts(*std::get<0>(*d.directionUncertainty)) :
				NA) + elementSep +
			    (std::get<1>(*d.directionUncertainty) ?
				ts(*std::get<1>(*d.directionUncertainty)) :
				NA) + elementSep +
			    (std::get<2>(*d.directionUncertainty) ?
				ts(*std::get<2>(*d.directionUncertainty)) :
				NA) + elementSep;
		ret += itemSep;
	}
	ret.erase(ret.find_last_of(itemSep), itemSep.length());
	return (ret);
}

std::string
FRIF::Util::splice(
    const std::vector<EFS::RidgeQualityRegion> &v,
    const std::string &elementSep,
    const std::string &itemSep,
    const std::string &coordinateElementSep,
    const std::string &coordinateItemSep)
{
	if ((elementSep == itemSep) || (elementSep == coordinateElementSep) ||
	    (elementSep == coordinateItemSep) ||
	    (itemSep == coordinateElementSep) ||
	    (itemSep == coordinateItemSep) ||
	    (coordinateElementSep == coordinateItemSep))
		throw std::runtime_error{"separator equality"};

	std::string ret{};
	for (const auto &r : v)
		ret += e2i2s(r.quality) + elementSep +
		    splice(r.region, coordinateElementSep, coordinateItemSep) +
		    itemSep;
	ret.erase(ret.find_last_of(itemSep), itemSep.length());
	return (ret);
}

std::string
FRIF::Util::splice(
    const std::vector<std::string> &v,
    const std::string &itemSep)
{
	std::string ret{};
	for (const auto &s : v)
		ret += s + itemSep;
	ret.erase(ret.find_last_of(itemSep), itemSep.length());
	return (ret);
}

std::string
FRIF::Util::splice(
    const EFS::QualityMeasure::Description &d,
    const std::string &elementSep)
{
	std::string ret{};
	if (d.identifier) {
		if (d.identifier->marketing)
			ret += sanitizeMessage(*d.identifier->marketing);
		else
			ret += NA;
		ret += elementSep;

		if (d.identifier->cbeff) {
			ret += std::to_string(d.identifier->cbeff->owner) +
			    elementSep;
			if (d.identifier->cbeff->algorithm)
				ret += std::to_string(
				    *d.identifier->cbeff->algorithm);
			else
				ret += NA;
			ret += elementSep;
		} else
			ret += NA + elementSep + NA + elementSep;
	} else
		ret += NA + elementSep + NA + elementSep + NA + elementSep;

	if (d.version)
		ret += sanitizeMessage(*d.version);
	else
		ret += NA;
	ret += elementSep;

	if (d.comment)
		ret += sanitizeMessage(*d.comment);
	else
		ret += NA;
	ret += elementSep;

	if (d.modelSHA256)
		ret += sanitizeMessage(*d.modelSHA256);
	else
		ret += NA;

	return (ret);
}

std::string
FRIF::Util::splice(
    const EFS::QualityMeasure &q,
    const std::string &elementSep)
{
	std::string ret = e2i2s(q.getStatus()) + elementSep;
	if (q)
		ret += std::to_string(q.getValue());
	else
		ret += NA;
	ret += elementSep;

	if (q.getMessage())
		ret += sanitizeMessage(*q.getMessage());
	else
		ret += NA;

	return (ret);
}

std::string
FRIF::Util::splice(
    const EFS::QualityMeasureMap &m,
    const std::string &elementSep,
    const std::string &itemSep)
{
	if (elementSep == itemSep)
		throw std::runtime_error{"elementSep == itemSep"};

	std::string ret{};
	for (const auto &[k, v] : m)
		ret += splice(k, elementSep) + elementSep +
		    splice(v, elementSep) + itemSep;
	ret.erase(ret.find_last_of(itemSep), itemSep.length());
	return (ret);
}

std::vector<std::vector<uint64_t>>
FRIF::Util::splitSet(
    const std::vector<uint64_t> &combinedSet,
    const uint8_t numSets)
{
	if (numSets == 0)
		return {};
	if (numSets == 1)
		return {combinedSet};

	const auto baseSize{combinedSet.size() / numSets};
	const auto remainder{combinedSet.size() % numSets};

	std::vector<std::vector<uint64_t>> sets{};
	sets.reserve(numSets);

	auto it = combinedSet.begin();
	for (uint8_t i{}; i < numSets; ++i) {
		const auto currentSize = baseSize + ((i < remainder) ? 1 : 0);
		const auto nextIt = std::next(it,
		    static_cast<std::vector<uint64_t>::difference_type>(
		    currentSize));
		sets.emplace_back(it, nextIt);
		it = nextIt;
	}

	return (sets);
}

void
FRIF::Util::writeFile(
    const std::vector<std::byte> &data,
    const std::string &pathName)
{
	std::ofstream file{pathName,
	    std::ifstream::binary | std::ifstream::trunc};
	if (!file)
		throw std::runtime_error{"Could not open " + pathName};

	if (!file.write(reinterpret_cast<const char*>(data.data()),
	    static_cast<std::streamsize>(data.size())))
		throw std::runtime_error("Could not write " +
		    ts(data.size()) + " bytes to " + pathName);
}
