/*
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 */

#ifndef FRIF_UTIL_H_
#define FRIF_UTIL_H_

#include <chrono>
#include <fstream>
#include <string>
#include <type_traits>

namespace FRIF::Util
{
	/** Constant used in logs when value is undefined. */
	inline static const std::string NA{"NA"};

	/**
	 * @brief
	 * Make a log-able string of the difference of two times.
	 *
	 * @param start
	 * Start time.
	 * @param stop
	 * Stop time.
	 *
	 * @return
	 * String version of end - start, in microseconds.
	 */
	std::string
	duration(
	    const std::chrono::steady_clock::time_point &start,
	    const std::chrono::steady_clock::time_point &stop);

	/**
	 * @brief
	 * Cast an enumeration to its underlying integral value.
	 *
	 * @param t
	 * Item of enumeration type.
	 *
	 * @return
	 * Integral representation of `t`.
	 */
	template<typename T,
	    typename = typename std::enable_if_t<std::is_enum_v<T>>>
	typename std::underlying_type_t<T>
	e2i(
	    const T t)
	{
		return (static_cast<typename std::underlying_type_t<T>>(t));
	}

	/**
	 * @brief
	 * Obtain string value of an enumeration's underlying integral
	 * value.
	 *
	 * @param t
	 * Item of enumeration type.
	 *
	 * @return
	 * String representation of integral value of t.
	 */
	template<typename T,
	    typename = typename std::enable_if_t<std::is_enum_v<T>>>
	typename std::string
	e2i2s(
	    const T t)
	{
		return (std::to_string(e2i(t)));
	}

	/**
	 * @brief
	 * Make a string lowercase in place.
	 *
	 * @param s
	 * The string to lowercase.
	 *
	 * @return
	 * Reference to `s` that has been modified to be lowercase.
	 */
	std::string&
	lower(
	    std::string &s);

	/**
	 * @brief
	 * Obtain a list of random non-repeating positive integers.
	 *
	 * @param size
	 * Size of list to generate.
	 * @param seed
	 * Value used to initialize internal state of randomization engine.
	 *
	 * @return
	 * Vector of size `size` containing non-repeating positive integers,
	 * from 0 to `size`.
	 */
	std::vector<uint64_t>
	randomizeIndices(
	    const uint64_t size,
	    const uint64_t seed);

	/**
	 * @brief
	 * Read a file from disk.
	 *
	 * @param pathName
	 * Path to file to read.
	 *
	 * @return
	 * Contents of pathName as a vector of bytes.
	 *
	 * @throw runtime_error
	 * Error reading from file.
	 */
	std::vector<std::byte>
	readFile(
	    const std::filesystem::path &pathName);

	/**
	 * @brief
	 * Read a file from disk.
	 *
	 * @param pathName
	 * Path to file to read.
	 *
	 * @return
	 * Contents of pathName as a vector of bytes.
	 *
	 * @throw runtime_error
	 * Error reading from file.
	 */
	std::vector<std::byte>
	readFile(
	    const std::string &pathName);

	/**
	 * @brief
	 * Sanitize a message for printing in a log file.
	 *
	 * @param message
	 * Message to sanitize.
	 * @param escapeQuotes
	 * Whether or not to escape the " character.
	 * @param wrapInQuotes
	 * Whether or not to wrap result in ".
	 *
	 * @return
	 * Sanitized version of message that can be placed into a
	 * log file.
	 */
	std::string
	sanitizeMessage(
	    const std::string &message,
	    const bool escapeQuotes = true,
	    const bool wrapInQuotes = true);

	/**
	 * @brief
	 * Make a log-able string out of a vector of Coordinate.
	 *
	 * @param v
	 * Vector of Coordinate to output.
	 * @param elementSep
	 * Token to separate elements of Coordinate.
	 * @param itemSep
	 * Token to separate multiple Coordinate.
	 *
	 * @return
	 * Log-able string version of `v`.
	 *
	 * @note
	 * All separators must be distinct.
	 */
	std::string
	splice(
	    const std::vector<Coordinate> &v,
	    const std::string &elementSep = ";",
	    const std::string &itemSep = "|");

	/**
	 * @brief
	 * Make a log-able string out of a vector of Minutia.
	 *
	 * @param v
	 * Vector of Minutia to output.
	 * @param elementSep
	 * Token to separate elements of Minutia.
	 * @param itemSep
	 * Token to separate multiple Minutia.
	 *
	 * @return
	 * Log-able string version of `v`.
	 *
	 * @note
	 * All separators must be distinct.
	 */
	std::string
	splice(
	    const std::vector<EFS::Minutia> &v,
	    const std::string &elementSep = ";",
	    const std::string &itemSep = "|");

	/**
	 * @brief
	 * Make a log-able string out of a vector of Core.
	 *
	 * @param v
	 * Vector of Core to output.
	 * @param elementSep
	 * Token to separate elements of Core.
	 * @param itemSep
	 * Token to separate multiple Core.
	 *
	 * @return
	 * Log-able string version of `v`.
	 *
	 * @note
	 * All separators must be distinct.
	 */
	std::string
	splice(
	    const std::vector<EFS::Core> &v,
	    const std::string &elementSep = ";",
	    const std::string &itemSep = "|");

	/**
	 * @brief
	 * Make a log-able string out of a vector of Delta.
	 *
	 * @param v
	 * Vector of Delta to output.
	 * @param elementSep
	 * Token to separate elements of Delta.
	 * @param itemSep
	 * Token to separate multiple Delta.
	 *
	 * @return
	 * Log-able string version of `v`.
	 *
	 * @note
	 * All separators must be distinct.
	 */
	std::string
	splice(
	    const std::vector<EFS::Delta> &v,
	    const std::string &elementSep = ";",
	    const std::string &itemSep = "|");

	/**
	 * @brief
	 * Make a log-able string out of a vector of RidgeQualityRegion.
	 *
	 * @param v
	 * Vector of RidgeQualityRegion to output.
	 * @param elementSep
	 * Token to separate elements of RidgeQualityRegion.
	 * @param itemSep
	 * Token to separate multiple RidgeQualityRegion.
	 * @param coordinateElementSep
	 * Token to separate elements of Coordinate.
	 * @param coordinateItemSep
	 * Token to separate multiple Coordinate.
	 *
	 * @return
	 * Log-able string version of `v`.
	 *
	 * @note
	 * All separators must be distinct.
	 */
	std::string
	splice(
	    const std::vector<EFS::RidgeQualityRegion> &v,
	    const std::string &elementSep = ":",
	    const std::string &itemSep = "^",
	    const std::string &coordinateElementSep = ";",
	    const std::string &coordinateItemSep = "|");

	/**
	 * @brief
	 * Make a log-able string out of a vector of string.
	 *
	 * @param v
	 * Vector of string to output.
	 * @param itemSep
	 * Token to separate multiple strings.
	 *
	 * @return
	 * Log-able string version of `v`.
	 */
	std::string
	splice(
	    const std::vector<std::string> &v,
	    const std::string &itemSep);

	/**
	 * @brief
	 * Make a log-able string out of a vector of Segment.
	 *
	 * @param v
	 * Vector of Segment to output.
	 * @param itemSep
	 * Token to separate multiple Segment.
	 * @param coordinateElementSep
	 * Token to separate elements of Coordinate (within Segment).
	 * @param coordinateItemSep
	 * Token to separate multiple Coordinate (within Segment).
	 *
	 * @return
	 * Log-able string version of `d`.
	 */
	std::string
	splice(
	    const std::vector<Segment> &v,
	    const std::string &itemSep = "^",
	    const std::string &coordinateElementSep = ";",
	    const std::string &coordinateItemSep = "|");

	/**
	 * @brief
	 * Make a log-able string out of a vector of Crease.
	 *
	 * @param v
	 * Vector of Crease to output.
	 * @param itemSep
	 * Token to separate multiple Crease.
	 * @param segmentItemSep
	 * Token to separate multiple Segment (within Crease).
	 * @param coordinateElementSep
	 * Token to separate elements of Coordinate (within Segment).
	 * @param coordinateItemSep
	 * Token to separate multiple Coordinate (within Segment).
	 *
	 * @return
	 * Log-able string version of `d`.
	 */
	std::string
	splice(
	    const std::vector<EFS::Crease> &v,
	    const std::string &elementSep = ":",
	    const std::string &itemSep = "^",
	    const std::string &segmentItemSep = "~",
	    const std::string &coordinateElementSep = ";",
	    const std::string &coordinateItemSep = "|");

	/**
	 * @brief
	 * Make a log-able string out of a QualityMeasure::Description.
	 *
	 * @param d
	 * QualityMeasure::Description to output.
	 * @param elementSep
	 * Token to separate elements of QualityMeasure::Description.
	 *
	 * @return
	 * Log-able string version of `d`.
	 */
	std::string
	splice(
	    const EFS::QualityMeasure::Description &d,
	    const std::string &elementSep = ";");

	/**
	 * @brief
	 * Make a log-able string out of a QualityMeasure.
	 *
	 * @param q
	 * QualityMeasure to output.
	 * @param elementSep
	 * Token to separate elements of QualityMeasure.
	 *
	 * @return
	 * Log-able string version of `q`.
	 */
	std::string
	splice(
	    const EFS::QualityMeasure &q,
	    const std::string &elementSep = ";");

	/**
	 * @brief
	 * Make a log-able string out of a QualityMeasureMap.
	 *
	 * @param m
	 * QualityMeasure::Description to output.
	 * @param elementSep
	 * Token to separate elements of QualityMeasure and
	 * QualityMeasure::Description.
	 * @param itemSep
	 * Token to separate multiple entries.
	 *
	 * @return
	 * Log-able string version of `m`.
	 *
	 * @note
	 * All separators must be distinct.
	 */
	std::string
	splice(
	    const EFS::QualityMeasureMap &m,
	    const std::string &elementSep = ";",
	    const std::string &itemSep = "|");

	/**
	 * @brief
	 * Make a log-able string out of a vector of enumerations.
	 *
	 * @param v
	 * Vector of enumerations to output.
	 * @param itemSep
	 * Token to separate multiple enumerations
	 *
	 * @return
	 * Log-able string version of `v`.
	 */
	template<typename T,
	    typename = typename std::enable_if_t<std::is_enum_v<T>>>
	std::string
	splice(
	    const std::vector<T> &v,
	    const std::string &itemSep = "|")
	{
		std::string ret{};
		for (const auto &e : v)
			ret += e2i2s(e) + itemSep;
		ret.erase(ret.find_last_of(itemSep), itemSep.length());
		return (ret);
	}

	/**
	 * @brief
	 * Create multiple smaller sets from a large set.
	 *
	 * @param combinedSet
	 * Set of numbers to split.
	 * @param numSets
	 * Number of sets to create.
	 *
	 * @return
	 * Collection of numSets collections, created sequentially
	 * from combinedSet.
	 *
	 * @throw
	 * numSets > combinedSet.size()
	 */
	std::vector<std::vector<uint64_t>>
	splitSet(
	    const std::vector<uint64_t> &combinedSet,
	    const uint8_t numSets);

	/**
	 * @brief
	 * Abbreviation for std::to_string for integral/floating point types.
	 *
	 * @param t
	 * Item of integral or floating point type.
	 *
	 * @return
	 * String representation of `t`.
	 */
	template<typename T,
	    typename = typename std::enable_if_t<
	    std::is_integral_v<T> || std::is_floating_point_v<T>>>
	typename std::string
	ts(
	    const T t)
	{
		return (std::to_string(t));
	}

	/**
	 * @brief
	 * Stringification of CreaseClassification.
	 *
	 * @param c
	 * CreaseClassification to convert.
	 *
	 * @return
	 * String representation of `c`.
	 */
	std::string
	ts(
	    const EFS::CreaseClassification &c);

	/**
	 * @brief
	 * Write data to a file on disk.
	 *
	 * @param data
	 * Data to write.
	 * @param pathName
	 * Path at which to write data.
	 *
	 * @throw runtime_error
	 * Error writing to path.
	 */
	void
	writeFile(
	    const std::vector<std::byte> &data,
	    const std::string &pathName);
}

#endif /* FRIF_UTIL_H_ */
