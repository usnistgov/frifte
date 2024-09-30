/*
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 */

#include <exception>

#include <frif/common.h>

FRIF::Coordinate::Coordinate(
    const uint32_t x_,
    const uint32_t y_) :
    x{x_},
    y{y_}
{

}

auto
FRIF::Coordinate::operator<=>(const FRIF::Coordinate&)
    const = default;

bool
FRIF::Coordinate::operator==(const FRIF::Coordinate&)
    const = default;

FRIF::ReturnStatus::operator bool()
    const
    noexcept
{
	return (this->result == Result::Success);
}

static std::string imagePropertiesString(const FRIF::Image &image)
{
	return {std::to_string(image.width) + 'x' +
	    std::to_string(image.height) + ", colorspace = " +
	    std::to_string(static_cast<std::underlying_type_t<
	    FRIF::Image::Colorspace>>(image.colorspace)) + ", bpc = " +
	    std::to_string(static_cast<std::underlying_type_t<
	    FRIF::Image::BitsPerChannel>>(image.bpc)) + ", bpp = " +
	    std::to_string(static_cast<std::underlying_type_t<
	    FRIF::Image::BitsPerPixel>>(image.bpp))};
}

FRIF::Image::Image() = default;
FRIF::Image::Image(
    const uint8_t identifier_,
    const uint16_t width_,
    const uint16_t height_,
    const uint16_t ppi_,
    const Colorspace colorspace_,
    const BitsPerChannel bpc_,
    const BitsPerPixel bpp_,
    const std::vector<std::byte> &pixels_) :
    identifier{identifier_},
    width{width_},
    height{height_},
    ppi{ppi_},
    colorspace{colorspace_},
    bpc{bpc_},
    bpp{bpp_},
    pixels{pixels_}
{

}

FRIF::Image::BitsPerPixel
FRIF::Image::toBitsPerPixel(
    const std::underlying_type_t<BitsPerPixel> i)
{
	switch (static_cast<BitsPerPixel>(i)) {
	case BitsPerPixel::Eight:
		[[fallthrough]];
	case BitsPerPixel::Sixteen:
		[[fallthrough]];
	case BitsPerPixel::TwentyFour:
		[[fallthrough]];
	case BitsPerPixel::FortyEight:
		return (static_cast<BitsPerPixel>(i));
	default:
		throw std::runtime_error{"Invalid BitsPerPixel value: " +
		    std::to_string(i)};
	}
}

FRIF::Image::BitsPerChannel
FRIF::Image::toBitsPerChannel(
    const std::underlying_type_t<BitsPerChannel> i)
{
	switch (static_cast<BitsPerChannel>(i)) {
	case BitsPerChannel::Eight:
		[[fallthrough]];
	case BitsPerChannel::Sixteen:
		return (static_cast<BitsPerChannel>(i));
	default:
		throw std::runtime_error{"Invalid BitsPerChannel value: " +
		    std::to_string(i)};
	}
}

void
FRIF::Image::sanityCheck()
    const
{
	if (this->width == 0 || this->height == 0)
		throw std::logic_error{"Unexpected dimensions (" +
		    imagePropertiesString(*this) + ')'};

	decltype(pixels.size()) expected{};
	switch (colorspace) {
	case Colorspace::Grayscale:
		switch (bpc) {
		case BitsPerChannel::Sixteen:
			switch (bpp) {
			case BitsPerPixel::Sixteen:
				break;

			case BitsPerPixel::Eight:
				[[fallthrough]];
			case BitsPerPixel::TwentyFour:
				[[fallthrough]];
			case BitsPerPixel::FortyEight:
				throw std::logic_error{"Unexpected "
				    "combination (" +
				    imagePropertiesString(*this) + ')'};
			}

			expected = width * height * 2ul;
			if (pixels.size() != expected)
				throw std::logic_error{"Unexpected pixel "
				    "length (expected = " +
				    std::to_string(expected) +", actual = " +
				    std::to_string(pixels.size()) + ')'};

			return;
		case BitsPerChannel::Eight:
			switch (bpp) {
			case BitsPerPixel::Eight:
				break;

			case BitsPerPixel::Sixteen:
				[[fallthrough]];
			case BitsPerPixel::TwentyFour:
				[[fallthrough]];
			case BitsPerPixel::FortyEight:
				throw std::logic_error{"Unexpected "
				    "combination (" +
				    imagePropertiesString(*this) + ')'};
			}

			expected = width * height;
			if (pixels.size() != expected)
				throw std::logic_error{"Unexpected pixel "
				    "length (expected = " +
				    std::to_string(expected) +", actual = " +
				    std::to_string(this->pixels.size()) + ')'};

			return;
		}
		break;

	case Colorspace::RGB:
		switch (bpc) {
		case BitsPerChannel::Sixteen:
			switch (bpp) {
			case BitsPerPixel::FortyEight:
				break;

			case BitsPerPixel::Eight:
				[[fallthrough]];
			case BitsPerPixel::Sixteen:
				[[fallthrough]];
			case BitsPerPixel::TwentyFour:
				throw std::logic_error{"Unexpected "
				    "combination (" +
				    imagePropertiesString(*this) + ')'};
			}

			expected = width * height * (3ul * 2ul);
			if (pixels.size() != expected)
				throw std::logic_error{"Unexpected pixel "
				    "length (expected = " +
				    std::to_string(expected) +", actual = " +
				    std::to_string(this->pixels.size()) + ')'};

			return;
		case BitsPerChannel::Eight:
			switch (bpp) {
			case BitsPerPixel::TwentyFour:
				break;

			case BitsPerPixel::Eight:
				[[fallthrough]];
			case BitsPerPixel::Sixteen:
				[[fallthrough]];
			case BitsPerPixel::FortyEight:
				throw std::logic_error{"Unexpected "
				    "combination (" +
				    imagePropertiesString(*this) + ')'};
			}

			expected = width * height * 3ul;
			if (pixels.size() != expected)
				throw std::logic_error{"Unexpected pixel "
				    "length (expected = " +
				    std::to_string(expected) +", actual = " +
				    std::to_string(this->pixels.size()) + ')'};

			return;
		}
		break;
	}

	throw std::logic_error{"Untested "
	    "combination (" + imagePropertiesString(*this) + ')'};
}
