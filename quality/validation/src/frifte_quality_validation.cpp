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
#include <fstream>
#include <iostream>
#include <random>
#include <numeric>
#include <string_view>
#include <sstream>
#include <stdexcept>
#include <thread>
#include <tuple>

#include <getopt.h>
#include <sys/wait.h>
#include <unistd.h>

#include <frifte/quality.h>
#include <frifte/util.h>

#include "frifte_quality_validation.h"

static std::shared_ptr<FRIF::Evaluations::Quality::Interface>
instantiateImplementation(
    const FRIF::Evaluations::Quality::Validation::Arguments &args)
{
	if (!std::filesystem::exists(args.configDir))
		throw std::runtime_error{"Config dir does not exist: " +
		    args.configDir.string()};

	return (FRIF::Evaluations::Quality::Interface::getImplementation(
	    args.configDir));
}

static const
std::map<FRIF::Evaluations::Quality::Component, std::string> ComponentLabels{
    {FRIF::Evaluations::Quality::Component::Amputated, "Amputated"},
    {FRIF::Evaluations::Quality::Component::AnatomicalCluesPresent,
        "AnatomicalCluesPresent"},
    {FRIF::Evaluations::Quality::Component::Bandaged, "Bandaged"},
    {FRIF::Evaluations::Quality::Component::ClearCoreDeltaRidgeCount,
       "ClearCoreDeltaRidgeCount"},
    {FRIF::Evaluations::Quality::Component::ComplexMatrix, "ComplexMatrix"},
    {FRIF::Evaluations::Quality::Component::ComplexSubstrate,
       "ComplexSubstrate"},
    {FRIF::Evaluations::Quality::Component::CompressedRidges,
       "CompressedRidges"},
    {FRIF::Evaluations::Quality::Component::Condensation, "Condensation"},
    {FRIF::Evaluations::Quality::Component::CorePresent, "CorePresent"},
    {FRIF::Evaluations::Quality::Component::DeltaPresent, "DeltaPresent"},
    {FRIF::Evaluations::Quality::Component::DevelopmentArtifacts,
       "DevelopmentArtifacts"},
    {FRIF::Evaluations::Quality::Component::DistinctiveClusterPresent,
       "DistinctiveClusterPresent"},
    {FRIF::Evaluations::Quality::Component::EvenPressure, "EvenPressure"},
    {FRIF::Evaluations::Quality::Component::EvidenceOfFraud, "EvidenceOfFraud"},
    {FRIF::Evaluations::Quality::Component::ExcessiveDistortion,
       "ExcessiveDistortion"},
    {FRIF::Evaluations::Quality::Component::ExtremeTip, "ExtremeTip"},
    {FRIF::Evaluations::Quality::Component::FocalPointsPresent,
       "FocalPointsPresent"},
    {FRIF::Evaluations::Quality::Component::Ghosting, "Ghosting"},
    {FRIF::Evaluations::Quality::Component::IsolatedRidgeClusters,
       "IsolatedRidgeClusters"},
    {FRIF::Evaluations::Quality::Component::Level3Present, "Level3Present"},
    {FRIF::Evaluations::Quality::Component::MinutiaePresent, "MinutiaePresent"},
    {FRIF::Evaluations::Quality::Component::Movement, "Movement"},
    {FRIF::Evaluations::Quality::Component::MultipleNonOverlappingImpressions,
       "MultipleNonOverlappingImpressions"},
    {FRIF::Evaluations::Quality::Component::MultipleOverlappingImpressions,
       "MultipleOverlappingImpressions"},
    {FRIF::Evaluations::Quality::Component::NumHighConfidenceMinutiae,
       "NumHighConfidenceMinutiae"},
    {FRIF::Evaluations::Quality::Component::NumMinutiae, "NumMinutiae"},
    {FRIF::Evaluations::Quality::Component::OpenField, "OpenField"},
    {FRIF::Evaluations::Quality::Component::OrientationCertaintyLevel,
       "OrientationCertaintyLevel"},
    {FRIF::Evaluations::Quality::Component::OrientationClue, "OrientationClue"},
    {FRIF::Evaluations::Quality::Component::PatternClassification,
       "PatternClassification"},
    {FRIF::Evaluations::Quality::Component::PatternForceMinutiae,
       "PatternForceMinutiae"},
    {FRIF::Evaluations::Quality::Component::RidgeColorUncertain,
       "RidgeColorUncertain"},
    {FRIF::Evaluations::Quality::Component::RidgeFlowContinuity,
       "RidgeFlowContinuity"},
    {FRIF::Evaluations::Quality::Component::RidgeValleyClarity,
       "RidgeValleyClarity"},
    {FRIF::Evaluations::Quality::Component::RidgeValleyUniformity,
       "RidgeValleyUniformity"},
    {FRIF::Evaluations::Quality::Component::SpatialSamplingRatePPI,
       "SpatialSamplingRatePPI"},
    {FRIF::Evaluations::Quality::Component::SpottyRidges, "SpottyRidges"},
    {FRIF::Evaluations::Quality::Component::TonalReversal, "TonalReversal"},
    {FRIF::Evaluations::Quality::Component::WetRidges, "WetRidges"}
};

static std::string
verboseQualityCSV(
    const FRIF::Evaluations::Quality::VerboseQuality &vq,
    const FRIF::Evaluations::Quality::Interface::Compatibility &compatibility)
{
	std::stringstream ss{};
	ss << FRIF::Util::splice(vq.getUnifiedQualityScore(), ",") << ',';

	if (compatibility.regionOfInterest) {
		static const std::vector<FRIF::Coordinate> notComputed{};
		const auto roi = vq.getROI().value_or(notComputed);
		ss << (roi.empty() ? FRIF::Util::NA : FRIF::Util::splice(roi));
	} else
		ss << FRIF::Util::NA;
	ss << ',';

	if (compatibility.ridgeQualityMap) {
		static const std::vector<FRIF::EFS::RidgeQualityRegion>
		    notComputed{};
		const auto rqm = vq.getRidgeQualityMap().value_or(notComputed);
		ss << (rqm.empty() ? FRIF::Util::NA : FRIF::Util::splice(rqm));
	} else
		ss << FRIF::Util::NA;
	ss << ',';

	if (compatibility.qualityComponents) {
		for (const auto &[component, label] : ComponentLabels) {
		    	static const FRIF::EFS::QualityMeasure notComputed{};
			const auto qm = vq.getComponent(component).value_or(
			    notComputed);
			ss << FRIF::Util::splice(qm, ",") << ',';
		}
	} else {
		for (decltype(ComponentLabels)::size_type i{};
		    i < ComponentLabels.size(); ++i)
			ss << FRIF::Util::NA << ',';
	}

	std::string ret{ss.str()};
	ret.pop_back();

	return (ret);
}

static void
checkUnifiedQualityScoreProperties(
    const FRIF::EFS::QualityMeasure &q)
{
	if (!q)
		return;

	/* Enforce whole number */
	if (static_cast<double>(static_cast<int>(q.getValue())) !=
	    q.getValue())
		throw std::out_of_range{
		    std::to_string(*q) + " shall be a whole number [0,100]"};

	/* Enforce [0,100] */
	if ((*q < 0) || (*q > 100))
		throw std::out_of_range{
		    std::to_string(*q) + " shall be a whole number [0,100]"};
}

static std::string
resultCSV(
    const std::variant<FRIF::EFS::QualityMeasure,
        FRIF::Evaluations::Quality::VerboseQuality> &result,
    const FRIF::Evaluations::Quality::Interface::Compatibility &compatibility)
{
	return (std::visit([&compatibility](auto&& r)
	{
		using T = std::decay_t<decltype(r)>;
		if constexpr (std::is_same_v<T, FRIF::EFS::QualityMeasure>) {
			/* Note: FRIF::Util::splice() will log as double. */
			std::string ret = FRIF::Util::e2i2s(r.getStatus()) +
			    ',';
			if (r) {
				/* We already confirmed this is [0,100] */
				ret += std::to_string(static_cast<uint8_t>(
				    r.getValue()));
			} else
				ret += FRIF::Util::NA;
			ret += ',';

			if (r.getMessage())
				ret += FRIF::Util::sanitizeMessage(
				    *r.getMessage());
			else
				ret += FRIF::Util::NA;;

			return (ret);
		} else if constexpr (std::is_same_v<T,
		    FRIF::Evaluations::Quality::VerboseQuality>)
			return (verboseQualityCSV(r, compatibility));
	}, result));
}


FRIF::Sample
FRIF::Evaluations::Quality::Validation::makeSample(
    const Data::Input &md,
    const Arguments &args)
{
	static const uint8_t identifier{0};

	/* Optional features are never specified, except ROI */
	EFS::Features features{};
	features.identifier = identifier;
	if (md.features) {
		features.ppi = md.features->ppi;
		features.imp = md.features->imp;
		features.frct = md.features->frct;
		features.frgp = md.features->frgp;
		features.roi = md.features->roi;
	}

	const Sample sample{Image(identifier, md.image.width, md.image.height,
	    md.image.ppi, md.image.colorspace, md.image.bpc, md.image.bpp,
	    Util::readFile(args.imageDir / md.image.filename)), features};

	std::get<std::optional<Image>>(sample)->sanityCheck();

	return (sample);
}

std::string
FRIF::Evaluations::Quality::Validation::singleComputeQuality(
    std::shared_ptr<Interface> impl,
    const Interface::Compatibility &compatibility,
    const Data::Dataset &dataset,
    const uint64_t imageSetIndex,
    const Arguments &args)
{
	const std::string &datasetName{std::get<std::string>(dataset)};
	const auto &[identifier, metadata] =
	    std::get<std::vector<Data::ImageSet>>(dataset).at(imageSetIndex);

	Sample sample{};
	try {
		sample = makeSample(metadata, args);
	} catch (const std::exception &e) {
		throw std::runtime_error{"Exception while creating samples "
		    "from ID = " + identifier + ", dataset = " + datasetName +
		    ", index = " + Util::ts(imageSetIndex) + " (" + e.what() +
		    ")"};
	}

	std::variant<EFS::QualityMeasure, VerboseQuality> ret{};
	std::chrono::steady_clock::time_point start{}, stop{};
	try {
		if (*args.operation == Operation::UnifiedQuality) {
			start = std::chrono::steady_clock::now();
			ret = impl->computeUnifiedQualityScore(sample);
			stop = std::chrono::steady_clock::now();

			checkUnifiedQualityScoreProperties(
			    std::get<EFS::QualityMeasure>(ret));
		} else {
			start = std::chrono::steady_clock::now();
			ret = impl->computeVerboseQuality(sample);
			stop = std::chrono::steady_clock::now();

			checkUnifiedQualityScoreProperties(
			    std::get<VerboseQuality>(ret).
			    getUnifiedQualityScore());
		}
	} catch (const std::exception &e) {
		stop = std::chrono::steady_clock::now();
		throw std::runtime_error{"Exception while computing quality "
		    "from ID = " + identifier + ",  dataset = " + datasetName +
		    ", index = " + Util::ts(imageSetIndex) + " after " +
		    Util::duration(start, stop) + " microseconds (" + e.what() +
		    ")"};
	} catch (...) {
		stop = std::chrono::steady_clock::now();
		throw std::runtime_error{"Unknown exception while computing "
		    "quality from ID = " + identifier + ",  dataset = " +
		    datasetName + ", index = " + Util::ts(imageSetIndex) + " "
		    "after " + Util::duration(start, stop) + " microseconds"};
	}

	return {'"' + identifier + "\"," + Util::duration(start, stop) + ',' +
	    resultCSV(ret, compatibility)};
}

void
FRIF::Evaluations::Quality::Validation::runPartialQualityComputation(
    std::shared_ptr<Quality::Interface> impl,
    const Data::Dataset &dataset,
    const std::vector<uint64_t> &indices,
    const Arguments &args)
{
	if (!impl)
		throw std::runtime_error{"Implementation is null"};
	const auto compatibility = impl->getCompatibility();

	static const std::string unifiedHeader{"\"identifier\",microseconds,"
	    "unified_status,unified,\"unified_msg\""};

	std::string verboseHeader{unifiedHeader + ",roi,rqm"};
	for (const auto &[component, label] : ComponentLabels) {
		verboseHeader += ","  + label + "_status," + label +
		    "_value,\"" + label + "_msg\"";
	}

	const std::string typeStr = (*args.operation ==
	    Operation::UnifiedQuality ? "unified" : "verbose");
	const std::string header = (*args.operation ==
	    Operation::UnifiedQuality ? unifiedHeader : verboseHeader);

	std::string prefix{typeStr + '-' + std::get<std::string>(dataset)};
	const auto logPath = makeLog(prefix, header, args.outputDir);

	std::ofstream logFile{logPath,
	    std::ios_base::out | std::ios_base::app};
	if (!logFile)
		throw std::runtime_error{Util::ts(getpid()) + ": Error "
		    "opening log file: " + logPath};

	for (const auto &n : indices) {
		const auto logLine = singleComputeQuality(impl, compatibility,
		    dataset, n, args);
		logFile << logLine << '\n';
		if (!logFile)
			throw std::runtime_error{Util::ts(getpid()) + ": Error "
			    "writing to log"};
	}
}

void
FRIF::Evaluations::Quality::Validation::forkOperation(
    const Arguments &args,
    const Data::Dataset &dataset,
    const std::vector<uint64_t> &imageSetIndicies,
    const std::function<void(std::shared_ptr<Quality::Interface>,
        const Data::Dataset&, const std::vector<uint64_t>&,
        const Arguments&)> &fn)
{
	auto impl = instantiateImplementation(args);

	if (args.numProcs == 1) {
		fn(impl, dataset, imageSetIndicies, args);
		return;
	}

	const auto splits = Util::splitSet(imageSetIndicies, args.numProcs);
	for (const auto &split : splits) {
		const auto pid = fork();
		switch (pid) {
		case 0:		/* Child */
			try {
				fn(impl, dataset, split, args);
			} catch (const std::exception &e) {
				std::cerr << e.what() << '\n';
				std::exit(EXIT_FAILURE);
			} catch (...) {
				std::cerr << "Caught unknown exception\n";
				std::exit(EXIT_FAILURE);
			}

			std::exit(EXIT_SUCCESS);
		case -1:	/* Error */
			throw std::runtime_error{"Error during fork()"};
		default:	/* Parent */
			break;
		}
	}

	/* Parent only */
	waitForExit(args.numProcs);
}

void
FRIF::Evaluations::Quality::Validation::dispatchQualityComputation(
    const Arguments &args)
{
	if (args.operation &&
	    !((*args.operation == Operation::UnifiedQuality) ||
	      (*args.operation == Operation::VerboseQuality)))
		throw std::runtime_error{"Unsupported operation was sent to "
		    "dispatchQualityComputation()"};
	if (!args.imageContent)
		throw std::runtime_error{"Image content not set"};

	const std::string csvName{Data::DatasetNames.at(*args.imageContent) +
	    ".csv"};
	Data::Dataset dataset = Data::readCSV(args.imageDir / csvName);
	const auto indices = Util::randomizeIndices(
	    std::get<std::vector<Data::ImageSet>>(dataset).size(),
	    args.randomSeed);

	forkOperation(args, dataset, indices, &runPartialQualityComputation);
}

std::string
FRIF::Evaluations::Quality::Validation::getUsageString(
    const std::string &name)
{
	static const std::string usagePrompt{"Usage: "};
	const std::string prefix(usagePrompt.length(), ' ');

	std::string s{usagePrompt + name + " ...\n"};

	s += prefix + "# Identify\n" +
	    prefix + "-i l # library\n" +
	    prefix + "-i q # product (quality algorithm)\n\n";

	s += prefix + "# Compatibility\n" + prefix + "-c\n\n";

	s += prefix + "# Compute unified quality scores\n" +
	    prefix + "-q u -t <content> -z <config_dir> [-r <random_seed> "
	    "-o <output_dir>\n" + prefix + "-I <image_dir> -j "
	    "<num_processes>]\n";
	s += prefix + "# Compute unified quality scores and verbose quality "
	    "components\n" + prefix + "-q v -t <content> -z <config_dir> "
	    "[-r <random_seed> -o <output_dir>\n" + prefix + "-I <image_dir> "
	    "-j <num_processes>]\n";

	s += prefix + "Where <content> is one of:\n";
	s += prefix + "  * D: distal exemplar\n";
	s += prefix + "  * d: distal latent\n";
	s += prefix + "  * N: non-distal exemplar\n";
	s += prefix + "  * n: non-distal latent\n";
	s += prefix + "  * P: palm exemplar\n";
	s += prefix + "  * p: palm latent\n";
	s += prefix + "  * u: unknown latent";

	return (s);
}

FRIF::Evaluations::Quality::Validation::Arguments
FRIF::Evaluations::Quality::Validation::parseArguments(
    const int argc,
    char * const argv[])
{
	static const char options[] {"ci:j:o:q:r:t:z:I:"};
	Arguments args{};
	args.executableName = argv[0];

	int c{};
	while ((c = getopt(argc, argv, options)) != -1) {
		switch (c) {
		case 'c':	/* Print compatibility */
			if (args.operation)
				throw std::logic_error{"Multiple operations "
				    "specified"};
			args.operation = Operation::Compatibility;
			break;

		case 'i':	/* Identification */
			if (std::string(optarg).length() != 1)
				throw std::logic_error{"Invalid -i argument"};
			if (args.operation)
				throw std::logic_error{"Multiple operations "
				    "specified"};

			switch (optarg[0]) {
			case 'l':
				args.operation = Operation::IdentifyLibrary;
				break;
			case 'q':
				args.operation = Operation::IdentifyQuality;
				break;
			default:
				throw std::logic_error{"Invalid -i argument"};
			}
			break;

		case 'j': {	/* Number of processes */
			try {
				args.numProcs = static_cast<uint8_t>(
				    std::stoul(optarg));
			} catch (const std::exception&) {
				throw std::invalid_argument{"Number of "
				    "processes (-j): an error occurred when "
				    "parsing \"" + std::string(optarg) + "\""};
			}

			const auto hwConcurrency = std::thread::
			    hardware_concurrency();

			/* Need to test multiple procs, even if only 1 core */
			if (hwConcurrency == 0) {
				if (args.numProcs > 2)
					throw std::invalid_argument{"Number of "
					    "processes (-f): Asked to spawn " +
					    Util::ts(args.numProcs) + " "
					    "processes, but refusing because "
					    "number of cores cannot be "
					    "determined"};
			} else if (args.numProcs > hwConcurrency) {
				const uint8_t newVal = static_cast<uint8_t>(
				    std::min(hwConcurrency,
				    static_cast<unsigned int>(
				    std::numeric_limits<uint8_t>::max())));
				std::cerr << "[NOTE] Number of processes seems "
				    "too large. Reducing to number of "
				    "concurrent threads detected (" +
				    std::to_string(newVal) + ").\n";
				args.numProcs = newVal;
			}
			break;
		}

		case 'o':	/* Output directory */
			args.outputDir = optarg;
			break;

		case 'q':	/* Compute quality */
			if (std::string(optarg).length() != 1)
				throw std::logic_error{"Invalid -q argument"};
			if (args.operation)
				throw std::logic_error{"Multiple operations "
				    "specified"};

			switch (optarg[0]) {
			case 'u':
				args.operation = Operation::UnifiedQuality;
				break;
			case 'v':
				args.operation = Operation::VerboseQuality;
				break;
			default:
				throw std::logic_error{"Invalid -q argument"};
			}
			break;

		case 'r':	/* Random seed */
			try {
				args.randomSeed = std::stoull(optarg);
			} catch (const std::exception&) {
				throw std::invalid_argument{"Random seed (-r): "
				    "an error occurred when parsing \"" +
				    std::string(optarg) + "\""};
			}
			break;

		case 't':	/* Image content */
			if (std::string(optarg).length() != 1)
				throw std::logic_error{"Invalid -t argument"};

			switch (optarg[0]) {
			case 'd':
				args.imageContent =
				    Data::ImageContent::DistalLatent;
				break;
			case 'D':
				args.imageContent =
				    Data::ImageContent::DistalExemplar;
				break;
			case 'n':
				args.imageContent =
				    Data::ImageContent::NonDistalLatent;
				break;
			case 'N':
				args.imageContent =
				    Data::ImageContent::NonDistalExemplar;
				break;
			case 'p':
				args.imageContent =
				    Data::ImageContent::PalmLatent;
				break;
			case 'P':
				args.imageContent =
				    Data::ImageContent::PalmExemplar;
				break;
			case 'u':
				args.imageContent =
				    Data::ImageContent::UnknownLatent;
				break;
			default:
				throw std::logic_error{"Invalid -t argument"};
			}
			break;

		case 'z':	/* Config dir */
			args.configDir = optarg;
			break;

		case 'I':
			args.imageDir = optarg;
			break;
		}
	}

	if (!args.operation)
		args.operation = Operation::Usage;

	if (args.configDir.empty() && !(
	    (args.operation == Operation::Usage) ||
	    (args.operation == Operation::IdentifyLibrary) ||
	    (args.operation == Operation::IdentifyQuality) ||
	    (args.operation == Operation::Compatibility)))
		throw std::invalid_argument{"Must provide path to "
		     "configuration directory"};

	if (((args.operation == Operation::UnifiedQuality) ||
	    (args.operation == Operation::VerboseQuality)) &&
	    !args.imageContent)
		throw std::invalid_argument{"Must provide image content if "
		    "computing quality"};

	return (args);
}



void
FRIF::Evaluations::Quality::Validation::waitForExit(
    const uint8_t numChildren)
{
	pid_t pid{-1};
	bool stop{false};
	uint8_t exitedChildren{0};
	int status{};
	while (exitedChildren != numChildren) {
		stop = false;
		while (!stop) {
			pid = ::wait(&status);
			switch (pid) {
			case 0:		/* Status not available */
				break;
			case -1:	/* Delivery of signal */
				switch (errno) {
				case ECHILD:	/* No child processes remain */
					stop = true;
					break;
				case EINTR:	/* Interruption, try again */
					break;
				default:
					throw std::runtime_error{"Error while "
					    "reaping: " + std::system_error(
					        errno, std::system_category()).
					        code().message()};
				}
				break;
			default:	/* Child exited */
				++exitedChildren;
				break;
			}
		}
	}
}

static
bool
validAPIVersion()
{
	/*
	 * Check FRIF TE API version.
	 */
	static const uint16_t expectedFRIFMajor{1};
	static const uint16_t expectedFRIFMinor{3};
	static const uint16_t expectedFRIFPatch{0};
	if (!((FRIF::API_MAJOR_VERSION == expectedFRIFMajor) &&
	    (FRIF::API_MINOR_VERSION == expectedFRIFMinor) &&
	    (FRIF::API_PATCH_VERSION == expectedFRIFPatch))) {
		std::cerr << "Incompatible API version encountered.\n "
		    "- Validation: " << expectedFRIFMajor << '.' <<
		    expectedFRIFMinor << '.' << expectedFRIFPatch << "\n - "
		    "Participant: " << FRIF::API_MAJOR_VERSION << '.' <<
		    FRIF::API_MINOR_VERSION << '.' <<
		    FRIF::API_PATCH_VERSION << '\n';
		std::cerr << "Rebuild your core library with the latest FRIF "
		    "TE header files\n";
		return (false);
	}

	/*
	 * Check evaluation API version.
	 */
	static const uint16_t expectedQualityMajor{0};
	static const uint16_t expectedQualityMinor{0};
	static const uint16_t expectedQualityPatch{1};
	if (!((FRIF::Evaluations::Quality::API_MAJOR_VERSION ==
	    expectedQualityMajor) &&
	    (FRIF::Evaluations::Quality::API_MINOR_VERSION ==
	    expectedQualityMinor) &&
	    (FRIF::Evaluations::Quality::API_PATCH_VERSION ==
	    expectedQualityPatch))) {
		std::cerr << "Incompatible API version encountered.\n "
		    "- Validation: " << expectedQualityMajor << '.' <<
		    expectedQualityMinor << '.' << expectedQualityPatch <<
		    "\n - Participant: " <<
		    FRIF::Evaluations::Quality::API_MAJOR_VERSION << '.' <<
		    FRIF::Evaluations::Quality::API_MINOR_VERSION << '.' <<
		    FRIF::Evaluations::Quality::API_PATCH_VERSION << '\n';
		std::cerr << "Rebuild your core library with the latest FRIF "
		    "TE Quality header files\n";
		return (false);
	}

	return (true);
}

int
FRIF::Evaluations::Quality::Validation::dispatchOperation(
    const Arguments &args)
{
	int rv{EXIT_FAILURE};

	switch (args.operation.value_or(Operation::Usage)) {
	case Operation::VerboseQuality:
		[[ fallthrough ]];
	case Operation::UnifiedQuality:
		try {
			dispatchQualityComputation(args);
			rv = EXIT_SUCCESS;
		} catch (const std::exception &e) {
			std::cerr << "dispatchQualityComputation(): " <<
			    e.what() << '\n';
		} catch (...) {
			std::cerr << "dispatchQualityComputation(): "
			    "Non-standard exception\n";
		}
		break;
	case Operation::Usage:
		std::cout << getUsageString(args.executableName) << '\n';
		rv = EXIT_SUCCESS;
		break;
	case Operation::Compatibility:
		try {
			std::cout << getCompatibilityString() << '\n';
			rv = EXIT_SUCCESS;
		} catch (const std::exception &e) {
			std::cerr << "Evaluations::getProductIdentifier(): " <<
			    e.what() << '\n';
		} catch (...) {
			std::cerr << "Evaluations::getProductIdentifier(): "
			    "Non-standard exception\n";
		}
		break;
	case Operation::IdentifyQuality:
		try {
			std::cout << getProductIdentifierString() << '\n';
			rv = EXIT_SUCCESS;
		} catch (const std::exception &e) {
			std::cerr << "Interface::getProductIdentifier(): " <<
			    e.what() << '\n';
		} catch (...) {
			std::cerr << "Interface::getProductIdentifier(): "
			    "Non-standard exception\n";
		}
		break;

	case Operation::IdentifyLibrary:
		try {
			std::cout << getLibraryIdentifierString() << '\n';
			rv = EXIT_SUCCESS;
		} catch (const std::exception &e) {
			std::cerr << "Quality::getLibraryIdentifier(): " <<
			    e.what() << '\n';
		} catch (...) {
			std::cerr << "Quality::getLibraryIdentifier(): "
			    "Non-standard exception\n";
		}
		break;
	}

	return (rv);
}

std::string
FRIF::Evaluations::Quality::Validation::makeLog(
    const std::string &prefix,
    const std::string &header,
    const std::filesystem::path &outputDir)
{
	const std::string logName = std::filesystem::path{outputDir /
	    std::string{prefix + '-' + Util::ts(getpid()) + ".log"}}.string();
	std::ofstream file{logName, std::ios_base::out | std::ios_base::trunc};
	if (!file)
		throw std::runtime_error{Util::ts(getpid()) + ": Error "
		    "creating " + prefix + " log file (" + logName + ")"};

	file << header << '\n';
	if (!file)
		throw std::runtime_error{Util::ts(getpid()) + ": Error writing "
		    "to " + prefix + " log"};

	return (logName);
}

std::string
FRIF::Evaluations::Quality::Validation::getLibraryIdentifierString()
{
	const auto id = Quality::getLibraryIdentifier();

	std::stringstream ss{};
	ss << "Identifier = " << id.identifier << '\n' <<
	    "Version = 0x" << std::setw(4) << std::hex << std::setfill('0') <<
	    std::uppercase << id.versionNumber;

	return (ss.str());
}

std::string
FRIF::Evaluations::Quality::Validation::getCompatibilityString()
{
	const auto compat = Quality::Interface::getCompatibility();

	std::stringstream ss{};
	ss << "Compute Region of Interest? = " << std::boolalpha <<
	    compat.regionOfInterest << '\n';
	ss << "Compute Ridge Quality Map? = " << std::boolalpha <<
	    compat.ridgeQualityMap << '\n';
	ss << "Compute Quality Components? = " << std::boolalpha <<
	    compat.qualityComponents << '\n';

	ss << "Supports Palm Latents? = " << std::boolalpha <<
	    compat.palmLatent << '\n';
	ss << "Supports Palm Exemplars? = " << std::boolalpha <<
	    compat.palmExemplar << '\n';

	ss << "Supports Distal Phalanx Latents? = " << std::boolalpha <<
	    compat.distalLatent << '\n';
	ss << "Supports Distal Phalanx Exemplars? = " << std::boolalpha <<
	    compat.distalExemplar << '\n';

	ss << "Supports Non-distal Phalanx Latents? = " << std::boolalpha <<
	    compat.nonDistalLatent << '\n';
	ss << "Supports Non-distal Phalanx Exemplars? = " << std::boolalpha <<
	    compat.nonDistalExemplar << '\n';

	ss << "Supports Unknown Latents? = " << std::boolalpha <<
	    compat.unknownLatent;

	return (ss.str());
}

std::string
FRIF::Evaluations::Quality::Validation::getProductIdentifierString()
{
	const auto id = FRIF::Evaluations::Quality::Interface::
	    getProductIdentifier();

	std::stringstream ss{};
	ss << "CBEFF Product Owner = ";
	if (id && id->identifier && id->identifier->cbeff)
		ss << "0x" << std::setw(4) << std::hex << std::setfill('0') <<
		    std::uppercase << id->identifier->cbeff->owner;
	ss << '\n';

	ss << "CBEFF Algorithm Identifier = ";
	if (id && id->identifier && id->identifier->cbeff &&
	    id->identifier->cbeff->algorithm)
		ss << "0x" << std::setw(4) << std::hex << std::setfill('0') <<
		    std::uppercase << *(id->identifier->cbeff->algorithm);
	ss << '\n';

	ss << "Marketing Name = ";
	if (id && id->identifier && id->identifier->marketing)
		ss << *(id->identifier->marketing);
	ss << '\n';

	ss << "Version = ";
	if (id && id->version)
		ss << *(id->version);
	ss << '\n';

	ss << "Comment = ";
	if (id && id->comment)
		ss << *(id->comment);
	ss << '\n';

	ss <<"Model SHA-256 = ";
	if (id && id->modelSHA256)
		ss << *(id->modelSHA256);

	return (ss.str());
}

int
main(
    int argc,
    char *argv[])
{
	if (!validAPIVersion()) {
		return (EXIT_FAILURE);
	}

	FRIF::Evaluations::Quality::Validation::Arguments args{};
	try {
		args = FRIF::Evaluations::Quality::Validation::
		    parseArguments(argc, argv);
	} catch (const std::exception &e) {
		std::cerr << "[ERROR] " << e.what() << '\n';
		std::cerr << FRIF::Evaluations::Quality::Validation::
		    getUsageString(argv[0]) << '\n';
		return (EXIT_FAILURE);
	}

	try {
		return (FRIF::Evaluations::Quality::Validation::
		    dispatchOperation(args));
	} catch (const std::exception &e) {
		std::cerr << "[ERROR] " << e.what() << '\n';
		return (EXIT_FAILURE);
	} catch (...) {
		std::cerr << "[ERROR] Caught non-standard exception\n";
		return (EXIT_FAILURE);
	}
}
