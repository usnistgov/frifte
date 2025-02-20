/*
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 */

#ifndef FRIF_COMMON_H_
#define FRIF_COMMON_H_

#include <compare>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <tuple>
#include <type_traits>
#include <vector>

namespace FRIF
{
	/** Information about the result of calling a %FRIF TE API function. */
	struct ReturnStatus
	{
		/** Possible outcomes when performing operations. */
		enum class Result
		{
			/** Successfully performed operation. */
			Success = 0,
			/** Failed to perform operation. */
			Failure
		};

		/** The result of the operation. */
		Result result{};
		/**
		 * Information about the result. Must match the regular
		 * expression `[[:graph:] ]*`.
		 */
		std::optional<std::string> message{};

		/**
		 * @return
		 * `true` if #result is Result::Success, `false` otherwise.
		 */
		explicit operator bool()
		    const
		    noexcept;
	};

	/** Data and metadata for an image. */
	struct Image
	{
		/** Number of bits comprising a single image pixel. */
		enum class BitsPerPixel : uint8_t
		{
			Eight = 8,
			Sixteen = 16,
			TwentyFour = 24,
			FortyEight = 48
		};
		using BPP = BitsPerPixel;

		/**
		 * @brief
		 * Convert integer to enumerated type.
		 *
		 * @param i
		 * Integer version of enumerated type.
		 *
		 * @return
		 * Enumerated value of i.
		 *
		 * @throw
		 * Cannot map i to valid enumerated value.
		 */
		static BitsPerPixel
		toBitsPerPixel(
		    const std::underlying_type_t<BitsPerPixel> i);

		/**
		 * Number of bits comprising a single color channel of a single
		 * pixel.
		 */
		enum class BitsPerChannel : uint8_t
		{
			Eight = 8,
			Sixteen = 16
		};
		using BPC = BitsPerChannel;

		/**
		 * @brief
		 * Convert integer to enumerated type.
		 *
		 * @param i
		 * Integer version of enumerated type.
		 *
		 * @return
		 * Enumerated value of i.
		 *
		 * @throw
		 * Cannot map i to valid enumerated value.
		 */
		static BitsPerChannel
		toBitsPerChannel(
		    const std::underlying_type_t<BitsPerChannel> i);

		enum class Colorspace
		{
			/** Single channel (shades of gray) */
			Grayscale,
			/**
			 * Three channels, with equal bit widths representing
			 * red, green, and blue, in order.
			 */
			RGB
		};

		Image();

		/**
		 * @brief
		 * Image constructor.
		 *
		 * @param identifier
		 * An identifier for this image. Used to link Image to
		 * TemplateData and Correspondence.
		 * @param width
		 * Width of the image in pixels.
		 * @param height
		 * Height of the image in pixels.
		 * @param ppi
		 * Resolution of the image in pixels per inch.
		 * @param colorspace
		 * Representation of color in each byte within `pixels`.
		 * @param bpc
		 * Number of bits used by each color component.
		 * @param bpp
		 * Number of bits comprising a single pixel.
		 * @param pixels
		 * `width` * `height` * (`bpp` / `bpc`) bytes of image data,
		 * with `pixels.front()` representing the first byte of the
		 * top-left pixel, and `pixels.back()` representing the last
		 * byte of the bottom-right pixel. It is decompressed big endian
		 * image data, canonically coded as defined in
		 * ISO/IEC 19794-4:2005, section 6.2. For example, the value
		 * `0xFF00` when #bpc is BPC::Sixteen, #bpp is
		 * BPP::Sixteen and #colorspace is Colorspace::Grayscale is
		 * significantly closer to white than it is to black.
		 *
		 * @note
		 * You may call sanityCheck on this object to have some
		 * confidence that the image properties make sense.
		 */
		Image(
		    const uint8_t identifier,
		    const uint16_t width,
		    const uint16_t height,
		    const uint16_t ppi,
		    const Colorspace colorspace,
		    const BitsPerChannel bpc,
		    const BitsPerPixel bpp,
		    const std::vector<std::byte> &pixels);

		/**
		 * @brief
		 * Validate that the properties of this Image appear to
		 * correspond to valid image data.
		 *
		 * @throw std::logic_error
		 * Some combination of properties appears invalid.
		 */
		void
		sanityCheck()
		    const;

		/**
		 * An identifier for this image. Used to link Image to EFS,
		 * TemplateData, and Correspondence.
		 */
		uint8_t identifier{};
		/** Width of the image. */
		uint16_t width{};
		/** Height of the image. */
		uint16_t height{};
		/** Resolution of the image in pixels per inch. */
		uint16_t ppi{};
		/** Representation of color in each byte within `pixels`. */
		Colorspace colorspace{Colorspace::Grayscale};
		/** Number of bits used by each color component. */
		BitsPerChannel bpc{BitsPerChannel::Eight};
		/**  Number of bits comprising a single pixel. */
		BitsPerPixel bpp{BitsPerPixel::Eight};
		/**
		 * @brief
		 * Raw pixel data of image.
		 *
		 * @details
		 * #width * #height * (#bpp / #bpc) bytes of image data,
		 * with #pixels `.front()` representing the first byte of the
		 * top-left pixel, and #pixels `.back()` representing the last
		 * byte of the bottom-right pixel. It is decompressed big endian
		 * image data, canonically coded as defined in
		 * ISO/IEC 19794-4:2005, section 6.2. For example, the value
		 * `0xFF00` when #bpc is BPC::Sixteen, #bpp is
		 * BPP::Sixteen and #colorspace is Colorspace::Grayscale is
		 * significantly closer to white than it is to black.
		 *
		 * @note
		 * To pass pixels to a C-style array, invoke pixel's `data()`
		 * method (`pixels.data()`). You may safely `reinterpret_cast`
		 * this value to `std::vector<uint8_t>` if needed (e.g., to
		 * obtain a `uint8_t*` from `data()`).
		 */
		std::vector<std::byte> pixels{};
	};

	/** Pixel location in an image. */
	struct Coordinate
	{
		/** X coordinate in pixels. */
		uint32_t x{};
		/** Y coordinate in pixels. */
		uint32_t y{};

		/**
		 * @brief
		 * Coordinate constructor.
		 *
		 * @param x
		 * X coordinate in pixels.
		 * @param y
		 * Y coordinate in pixels.
		 */
		Coordinate(
		    const uint32_t x = {},
		    const uint32_t y = {});

		auto operator<=>(const Coordinate&) const;
		bool operator==(const Coordinate&) const;
	};

	/** Line segment. */
	using Segment = std::tuple<Coordinate, Coordinate>;

	/** Identifying details about algorithm components for documentation. */
	struct ProductIdentifier
	{
		/** CBEFF information registered with and assigned by IBIA. */
		struct CBEFFIdentifier
		{
			/** CBEFF Product Owner of the product. */
			uint16_t owner{};
			/** CBEFF Algorithm Identifier of the product. */
			std::optional<uint16_t> algorithm{};

			auto operator<=>(const CBEFFIdentifier&) const;
			bool operator==(const CBEFFIdentifier&) const;
		};

		/**
		 * Non-infringing marketing name of the product. Case sensitive.
		 * Must match the regular expression `[[:graph:] ]*`.
		 */
		std::optional<std::string> marketing{};
		/**
		 * CBEFF information about the product.
		 *
		 * @attention
		 * Do not set CBEFF information that is not registered with
		 * [IBIA](https://www.ibia.org/cbeff/iso).
		 */
		std::optional<CBEFFIdentifier> cbeff{};

		auto operator<=>(const ProductIdentifier&) const;
		bool operator==(const ProductIdentifier&) const;
	};

	/*
	 * API versioning.
	 *
	 * NIST code will extern the version number symbols. Participant code
	 * shall compile them into their core library.
	 */
	#ifdef NIST_EXTERN_FRIF_API_VERSION
	/** API major version number. */
	extern uint16_t API_MAJOR_VERSION;
	/** API minor version number. */
	extern uint16_t API_MINOR_VERSION;
	/** API patch version number. */
	extern uint16_t API_PATCH_VERSION;
	#else /* NIST_EXTERN_API_VERSION */
	/** API major version number. */
	uint16_t API_MAJOR_VERSION{1};
	/** API minor version number. */
	uint16_t API_MINOR_VERSION{0};
	/** API patch version number. */
	uint16_t API_PATCH_VERSION{0};
	#endif /* NIST_FRIF_EXTERN_API_VERSION */

	/*
	 * Ensure that std::byte is exactly 8 bits, such that reinterpret_casts
	 * may be safely used.
	 */
	static_assert(std::is_same_v<std::underlying_type_t<std::byte>,
	    uint8_t>, "std::byte not represented as unsigned 8 bit type");
}

#endif /* FRIF_COMMON_H_ */
