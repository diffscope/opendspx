# OpenDSPX

OpenDSPX is a C++20 library for working with the [DSPX format](https://dspx.diffscope.org/).
It provides four layers:

- `model`: the core DSPX data model and headers
- `interpolator`: DSPX parameter curve interpolation utilities for evaluating parameter values
- `serializer`: serialization and deserialization for DSPX files
- `converter`: DSPX format conversion utilities built on top of the model layer; MIDI is the first supported backend today

## Requirements

Build-time dependencies:

- CMake 3.17 or newer
- A C++20-capable compiler
- `stdcorelib`
- `zstd` for the serializer module
- `wolf-midi` for the converter module
- `boost-test` for the test suite

The project is currently configured for Windows and other CMake-supported platforms.

## Build

Configure and build with CMake:

```bash
cmake -S . -B build
cmake --build build
```

Useful options:

- `OPENDSPX_BUILD_STATIC`: build static libraries instead of shared libraries
- `OPENDSPX_BUILD_TESTS`: build the test suite
- `OPENDSPX_BUILD_INTERPOLATOR`: build the interpolator interface library
- `OPENDSPX_BUILD_SERIALIZER`: build the serializer library
- `OPENDSPX_BUILD_CONVERTER`: build the converter library
- `OPENDSPX_INSTALL`: generate install rules and CMake package files

## Using OpenDSPX

### Core model

The core data type is `opendspx::Model`. It currently exposes version `V1` and the associated DSPX content tree.

## Interpolator

The interpolator module is a header-only utility for building and evaluating cubic curves.
It is useful for DSPX parameter evaluation.

The public API provides helpers for:

- creating a curve from two key points plus reference points
- creating a curve from only one side of reference data
- creating a linear interpolator
- evaluating the resulting curve at any x position

Example:

```cpp
#include <opendspx/interpolator/interpolator.h>

using opendspx::Interpolator;

int main() {
	auto curve = Interpolator<double>::createLinear(0.0, 0.0, 1.0, 1.0);
	double value = curve.evaluate(0.5);
	(void)value;
}
```

### Serialization

Use `opendspx::Serializer` to convert between `Model` and DSPX data streams.

```cpp
#include <fstream>
#include <opendspx/model.h>
#include <opendspx/serializer/serializer.h>

using namespace opendspx;

int main() {
	std::ifstream input("song.dspx", std::ios::binary);
	SerializationErrorList errors;

	Model model = Serializer::deserialize(input, errors);

	std::ofstream output("song-out.dspx", std::ios::binary);
	Serializer::serialize(output, model, errors);
}
```

`Serializer::serialize` can also write compressed output when the `compress` flag is set to `true`.

### DSPX format conversion

Use `opendspx::Converter` to convert between DSPX data and other formats.

```cpp
#include <fstream>
#include <opendspx/converter/midi/midiconverter.h>

using namespace opendspx;

int main() {
	Model model;

	auto intermediate = MidiConverter::convertDspxToIntermediate(model);
	std::ofstream midiOutput("output.mid", std::ios::binary);
	MidiConverter::convertIntermediateToMidi(midiOutput, intermediate);

	MidiConverter::Error error;
	std::ifstream midiInput("input.mid", std::ios::binary);
	auto imported = MidiConverter::convertMidiToIntermediate(midiInput, error);
	if (error != MidiConverter::Error::NoError) {
		return 1;
	}

	bool ok = false;
	Model roundTrip = MidiConverter::convertIntermediateToDspx(imported, &ok);
	if (!ok) {
		return 1;
	}

	(void)roundTrip;
}
```

## Project Structure

- `include/opendspx/`: public model headers
- `include/opendspx/interpolator/`: interpolator API
- `include/opendspx/serializer/`: serializer API
- `include/opendspx/converter/`: converter API
- `src/serializer/`: serializer implementation
- `src/converter/`: converter implementation, currently focused on MIDI
- `tests/auto/`: the regression suite, one directory per module

## Tests

Configure with `-DOPENDSPX_BUILD_TESTS=ON` and point CMake at Boost.Test, then:

```bash
ctest --test-dir build --output-on-failure --no-tests=error
```

The whole suite is one binary, so a single case can be run on its own with
`test_auto --run_test=<suite>/<case>`.

## License

This project is distributed under the Apache License, Version 2.0, as described in [LICENSE](LICENSE).

