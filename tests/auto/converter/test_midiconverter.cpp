#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include <opendspx/model.h>
#include <opendspx/converter/midi/midiconverter.h>
#include <opendspx/converter/midi/midiintermediatedata.h>

#include <boost/test/unit_test.hpp>

using namespace opendspx;

namespace {

    using Intermediate = MidiIntermediateData;

    // DSPX counts 480 ticks to a quarter note whatever the MIDI file says, so a case that wants
    // to talk about beats says so here rather than spelling out the arithmetic each time.
    constexpr int dspxQuarter = 480;

    Model modelWithOneTrack() {
        Model model;
        model.content.global.name = "song";
        model.content.timeline.tempos = {Tempo{0, 100.0}, Tempo{4 * dspxQuarter, 150.0}};
        model.content.timeline.timeSignatures = {TimeSignature{0, 3, 4}};
        model.content.timeline.labels = {Label{0, "intro"}, Label{4 * dspxQuarter, "verse"}};

        auto clip = std::make_shared<SingingClip>();
        clip->name = "vocal";
        clip->time = ClipTime{0, 8 * dspxQuarter, 0, 8 * dspxQuarter};

        Note first;
        first.pos = 0;
        first.length = dspxQuarter;
        first.keyNum = 60;
        first.lyric = "do";

        Note second;
        second.pos = 2 * dspxQuarter;
        second.length = dspxQuarter / 2;
        second.keyNum = 67;
        second.lyric = "so";

        clip->notes = {first, second};

        Track track;
        track.name = "lead";
        track.clips = {clip};
        model.content.tracks = {track};
        return model;
    }

    std::string toMidiBytes(const Intermediate &data) {
        std::ostringstream out(std::ios::binary);
        MidiConverter::convertIntermediateToMidi(out, data);
        return out.str();
    }

    Intermediate fromMidiBytes(const std::string &bytes, MidiConverter::Error &error) {
        std::istringstream in(bytes, std::ios::binary | std::ios::in);
        return MidiConverter::convertMidiToIntermediate(in, error);
    }

}

BOOST_AUTO_TEST_SUITE(test_midiconverter)

// isValid is what stands between a model and a file nobody can open, so each of its rules is
// checked on its own rather than through one example that satisfies all of them.
BOOST_AUTO_TEST_CASE(test_intermediate_validity) {
    BOOST_CHECK(Intermediate{}.isValid());
    BOOST_CHECK(Intermediate(480, {}, {}, {}, {}).isValid());

    // Tempo has to be a tempo somebody could play.
    BOOST_CHECK(Intermediate(480, {{0, 10.0}}, {}, {}, {}).isValid());
    BOOST_CHECK(Intermediate(480, {{0, 1000.0}}, {}, {}, {}).isValid());
    BOOST_CHECK(!Intermediate(480, {{0, 9.9}}, {}, {}, {}).isValid());
    BOOST_CHECK(!Intermediate(480, {{0, 1000.1}}, {}, {}, {}).isValid());
    BOOST_CHECK(!Intermediate(480, {{-1, 120.0}}, {}, {}, {}).isValid());

    // A denominator is a power of two up to 128, because that is what a MIDI time signature can
    // say; a numerator only has to be positive.
    for (int denominator : {1, 2, 4, 8, 16, 32, 64, 128}) {
        BOOST_CHECK(Intermediate(480, {}, {{0, 4, denominator}}, {}, {}).isValid());
    }
    for (int denominator : {0, 3, 5, 6, 12, 256}) {
        BOOST_CHECK(!Intermediate(480, {}, {{0, 4, denominator}}, {}, {}).isValid());
    }
    BOOST_CHECK(!Intermediate(480, {}, {{0, 0, 4}}, {}, {}).isValid());
    BOOST_CHECK(!Intermediate(480, {}, {{-1, 4, 4}}, {}, {}).isValid());

    BOOST_CHECK(!Intermediate(480, {}, {}, {{-1, "x"}}, {}).isValid());

    // A key is a MIDI key.
    const auto withNote = [](int key, int tick, int length) {
        return Intermediate(480, {}, {}, {}, {{"t", {{tick, length, key, ""}}, 0, 0}});
    };
    BOOST_CHECK(withNote(0, 0, 1).isValid());
    BOOST_CHECK(withNote(127, 0, 1).isValid());
    BOOST_CHECK(!withNote(128, 0, 1).isValid());
    BOOST_CHECK(!withNote(-1, 0, 1).isValid());
    BOOST_CHECK(!withNote(60, -1, 1).isValid());
    BOOST_CHECK(!withNote(60, 0, -1).isValid());
    BOOST_CHECK(!Intermediate(-1, {}, {}, {}, {}).isValid());
}

// Nothing is written for data that would not read back, rather than a file that fails later.
BOOST_AUTO_TEST_CASE(test_invalid_data_writes_nothing) {
    BOOST_CHECK(toMidiBytes(Intermediate(480, {{0, 5000.0}}, {}, {}, {})).empty());
    BOOST_CHECK(toMidiBytes(Intermediate(0, {}, {}, {}, {})).empty());
}

BOOST_AUTO_TEST_CASE(test_dspx_to_intermediate) {
    const auto data = MidiConverter::convertDspxToIntermediate(modelWithOneTrack());
    BOOST_CHECK_EQUAL(data.resolution(), 480);

    BOOST_REQUIRE_EQUAL(data.tempos().size(), 2);
    BOOST_CHECK_EQUAL(data.tempos()[0].tick, 0);
    BOOST_CHECK_EQUAL(data.tempos()[0].tempo, 100.0);
    BOOST_CHECK_EQUAL(data.tempos()[1].tempo, 150.0);

    BOOST_REQUIRE_EQUAL(data.timeSignatures().size(), 1);
    BOOST_CHECK_EQUAL(data.timeSignatures()[0].numerator, 3);
    BOOST_CHECK_EQUAL(data.timeSignatures()[0].denominator, 4);

    BOOST_REQUIRE_EQUAL(data.markers().size(), 2);
    BOOST_CHECK_EQUAL(data.markers()[0].text, "intro");
    BOOST_CHECK_EQUAL(data.markers()[1].text, "verse");

    BOOST_REQUIRE_EQUAL(data.tracks().size(), 1);
    BOOST_CHECK_EQUAL(data.tracks()[0].title, "lead");
    BOOST_REQUIRE_EQUAL(data.tracks()[0].notes.size(), 2);
    BOOST_CHECK_EQUAL(data.tracks()[0].notes[0].key, 60);
    BOOST_CHECK_EQUAL(data.tracks()[0].notes[0].lyric, "do");
    BOOST_CHECK_EQUAL(data.tracks()[0].notes[1].key, 67);

    BOOST_CHECK(data.isValid());
}

// A tick is a fraction of a quarter note, so asking for a different resolution rescales rather
// than renumbers. A resolution of nothing is not a resolution and falls back.
BOOST_AUTO_TEST_CASE(test_resolution_scales_the_ticks) {
    const auto model = modelWithOneTrack();

    const auto half = MidiConverter::convertDspxToIntermediate(model, {240, false});
    BOOST_CHECK_EQUAL(half.resolution(), 240);
    BOOST_REQUIRE_EQUAL(half.tempos().size(), 2);
    BOOST_CHECK_EQUAL(half.tempos()[1].tick, 4 * 240);
    BOOST_REQUIRE_EQUAL(half.tracks().size(), 1);
    BOOST_CHECK_EQUAL(half.tracks()[0].notes[1].noteOnTick, 2 * 240);
    BOOST_CHECK_EQUAL(half.tracks()[0].notes[1].length, 240 / 2);

    // A resolution of nothing is not a resolution. Nothing is converted rather than a number
    // being made up, and the empty result does not pass isValid's resolution rule either.
    for (int bad : {0, -1}) {
        const auto refused = MidiConverter::convertDspxToIntermediate(model, {bad, false});
        BOOST_CHECK_EQUAL(refused.resolution(), 0);
        BOOST_CHECK(refused.tracks().empty());
        BOOST_CHECK(refused.tempos().empty());
        BOOST_CHECK(toMidiBytes(refused).empty());
    }
}

// A text hook is applied on the way out and on the way back, which is how a file in a legacy
// encoding is read at all.
BOOST_AUTO_TEST_CASE(test_text_hooks) {
    const auto shout = [](const std::string &text) {
        return text + "!";
    };

    const auto data = MidiConverter::convertDspxToIntermediate(modelWithOneTrack(), shout);
    BOOST_REQUIRE_EQUAL(data.markers().size(), 2);
    BOOST_CHECK_EQUAL(data.markers()[0].text, "intro!");
    BOOST_REQUIRE_EQUAL(data.tracks().size(), 1);
    BOOST_CHECK_EQUAL(data.tracks()[0].notes[0].lyric, "do!");

    bool ok = false;
    const auto model = MidiConverter::convertIntermediateToDspx(data, shout, &ok);
    BOOST_CHECK(ok);
    BOOST_REQUIRE_EQUAL(model.content.timeline.labels.size(), 2);
    BOOST_CHECK_EQUAL(model.content.timeline.labels[0].text, "intro!!");
}

// The whole way out and back: a model, a MIDI file, and a model again. The parts a MIDI file can
// carry have to survive; the rest is not this converter's to keep.
BOOST_AUTO_TEST_CASE(test_round_trip_through_a_midi_file) {
    const auto original = modelWithOneTrack();
    const auto written = MidiConverter::convertDspxToIntermediate(original);
    const auto bytes = toMidiBytes(written);
    BOOST_REQUIRE(!bytes.empty());
    BOOST_CHECK_EQUAL(bytes.compare(0, 4, "MThd"), 0);

    auto error = MidiConverter::Error::InvalidMidiData;
    const auto read = fromMidiBytes(bytes, error);
    BOOST_REQUIRE(error == MidiConverter::Error::NoError);

    BOOST_CHECK_EQUAL(read.resolution(), written.resolution());

    BOOST_REQUIRE_EQUAL(read.tempos().size(), written.tempos().size());
    for (std::size_t i = 0; i < read.tempos().size(); ++i) {
        BOOST_CHECK_EQUAL(read.tempos()[i].tick, written.tempos()[i].tick);
        // A MIDI tempo is microseconds per quarter note, so it comes back rounded.
        BOOST_CHECK_CLOSE(read.tempos()[i].tempo, written.tempos()[i].tempo, 0.01);
    }

    BOOST_REQUIRE_EQUAL(read.timeSignatures().size(), written.timeSignatures().size());
    BOOST_CHECK_EQUAL(read.timeSignatures()[0].numerator, 3);
    BOOST_CHECK_EQUAL(read.timeSignatures()[0].denominator, 4);

    BOOST_REQUIRE_EQUAL(read.markers().size(), written.markers().size());
    BOOST_CHECK_EQUAL(read.markers()[0].text, "intro");
    BOOST_CHECK_EQUAL(read.markers()[1].text, "verse");

    // Held by value: the accessors hand back a copy, so a reference into what they return would
    // be pointing at a vector that is already gone.
    const auto readTracks = read.tracks();
    BOOST_REQUIRE_EQUAL(readTracks.size(), 1);
    BOOST_CHECK_EQUAL(readTracks[0].title, "lead");
    const auto &notes = readTracks[0].notes;
    BOOST_REQUIRE_EQUAL(notes.size(), 2);
    BOOST_CHECK_EQUAL(notes[0].noteOnTick, 0);
    BOOST_CHECK_EQUAL(notes[0].length, dspxQuarter);
    BOOST_CHECK_EQUAL(notes[0].key, 60);
    BOOST_CHECK_EQUAL(notes[0].lyric, "do");
    BOOST_CHECK_EQUAL(notes[1].noteOnTick, 2 * dspxQuarter);
    BOOST_CHECK_EQUAL(notes[1].key, 67);
    BOOST_CHECK_EQUAL(notes[1].lyric, "so");

    bool ok = false;
    const auto back = MidiConverter::convertIntermediateToDspx(read, &ok);
    BOOST_CHECK(ok);
    BOOST_REQUIRE_EQUAL(back.content.timeline.tempos.size(), 2);
    BOOST_CHECK_CLOSE(back.content.timeline.tempos[1].value, 150.0, 0.01);
    BOOST_REQUIRE_EQUAL(back.content.timeline.labels.size(), 2);
    BOOST_CHECK_EQUAL(back.content.timeline.labels[1].pos, 4 * dspxQuarter);
    BOOST_REQUIRE_EQUAL(back.content.tracks.size(), 1);
    BOOST_REQUIRE_EQUAL(back.content.tracks[0].clips.size(), 1);
}

// Bytes that are not a MIDI file are turned down rather than read as an empty song, which the
// caller would show as a project that lost everything.
BOOST_AUTO_TEST_CASE(test_rejects_what_is_not_midi) {
    for (const auto *bytes : {"", "not a midi file at all", "MThd"}) {
        auto error = MidiConverter::Error::NoError;
        fromMidiBytes(bytes, error);
        BOOST_CHECK_MESSAGE(error != MidiConverter::Error::NoError, "accepted " << bytes);
    }
}

// A model with nothing in it is still a MIDI file, and reading it back is still a model.
BOOST_AUTO_TEST_CASE(test_empty_model) {
    const auto data = MidiConverter::convertDspxToIntermediate(Model{});
    BOOST_CHECK(data.isValid());

    const auto bytes = toMidiBytes(data);
    BOOST_REQUIRE(!bytes.empty());

    auto error = MidiConverter::Error::InvalidMidiData;
    const auto read = fromMidiBytes(bytes, error);
    BOOST_CHECK(error == MidiConverter::Error::NoError);

    bool ok = false;
    MidiConverter::convertIntermediateToDspx(read, &ok);
    BOOST_CHECK(ok);
}

// The ok flag is optional on both overloads, and asking without it must not reach through a null.
BOOST_AUTO_TEST_CASE(test_ok_flag_is_optional) {
    const auto data = MidiConverter::convertDspxToIntermediate(modelWithOneTrack());
    BOOST_CHECK_NO_THROW(MidiConverter::convertIntermediateToDspx(data));
    BOOST_CHECK_NO_THROW(MidiConverter::convertIntermediateToDspx(
        data, [](const std::string &text) { return text; }));
}

BOOST_AUTO_TEST_SUITE_END()
