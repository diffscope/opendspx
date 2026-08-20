#include <array>
#include <string>

#include <serializer/helpers_p.h>

#include <boost/test/unit_test.hpp>

using namespace opendspx;
using namespace opendspx::test;

BOOST_AUTO_TEST_SUITE(test_serializer)

BOOST_AUTO_TEST_CASE(test_version_text) {
    BOOST_CHECK_EQUAL(Serializer::versionToText(Model::Version::V1), "1.0.0");

    bool ok = false;
    BOOST_CHECK(Serializer::versionFromText("1.0.0", &ok) == Model::Version::V1);
    BOOST_CHECK(ok);

    Serializer::versionFromText("1.0", &ok);
    BOOST_CHECK(!ok);
    Serializer::versionFromText("", &ok);
    BOOST_CHECK(!ok);

    // The out parameter is optional, and asking without it must not reach through a null.
    BOOST_CHECK_NO_THROW(Serializer::versionFromText("nonsense"));
}

// The document a model writes reads back as the same model. Checked over a model with something
// in every corner, because the mappings are generated per property and a missing one is silent.
BOOST_AUTO_TEST_CASE(test_round_trip) {
    SerializationErrorList writeErrors;
    const auto text = serialize(richModel(), writeErrors);
    BOOST_REQUIRE_EQUAL(writeErrors.size(), 0);

    SerializationErrorList readErrors;
    const auto model = deserialize(text, readErrors);
    BOOST_REQUIRE_EQUAL(readErrors.size(), 0);

    BOOST_CHECK(model.version == Model::Version::V1);
    BOOST_CHECK_EQUAL(model.content.global.name, "song");
    BOOST_CHECK_EQUAL(model.content.global.centShift, -3);
    BOOST_CHECK_EQUAL(model.content.master.control.gain, -1.5);
    BOOST_CHECK_EQUAL(model.content.master.control.mute, true);

    BOOST_REQUIRE_EQUAL(model.content.timeline.tempos.size(), 2);
    BOOST_CHECK_EQUAL(model.content.timeline.tempos[1].value, 87.5);
    BOOST_REQUIRE_EQUAL(model.content.timeline.timeSignatures.size(), 2);
    BOOST_CHECK_EQUAL(model.content.timeline.timeSignatures[1].denominator, 8);
    BOOST_REQUIRE_EQUAL(model.content.timeline.labels.size(), 2);
    BOOST_CHECK_EQUAL(model.content.timeline.labels[1].text, "verse");

    BOOST_REQUIRE_EQUAL(model.content.tracks.size(), 1);
    const auto &track = model.content.tracks.front();
    BOOST_CHECK_EQUAL(track.name, "track");
    BOOST_CHECK_EQUAL(track.control.solo, true);
    BOOST_REQUIRE_EQUAL(track.clips.size(), 2);

    // The clips came back as their own kinds, not as the base.
    const auto singing = derivedAs<SingingClip>(track.clips[0], Clip::Type::Singing);
    BOOST_REQUIRE(singing);
    BOOST_CHECK(singing->type == Clip::Type::Singing);
    BOOST_CHECK_EQUAL(singing->name, "vocal");
    BOOST_REQUIRE_EQUAL(singing->notes.size(), 1);

    const auto &note = singing->notes.front();
    BOOST_CHECK_EQUAL(note.keyNum, 60);
    BOOST_CHECK_EQUAL(note.centShift, 25);
    BOOST_CHECK_EQUAL(note.lyric, "la");
    BOOST_CHECK_EQUAL(note.pronunciation.edited, "l a");
    BOOST_REQUIRE_EQUAL(note.phonemes.original.size(), 2);
    BOOST_CHECK_EQUAL(note.phonemes.original[0].onset, true);
    BOOST_CHECK_EQUAL(note.phonemes.original[1].start, 100);
    BOOST_CHECK_EQUAL(note.vibrato.amp, 40);
    BOOST_CHECK_EQUAL(note.vibrato.freq, 5.5);
    BOOST_REQUIRE_EQUAL(note.vibrato.points.amp.size(), 2);
    BOOST_CHECK_EQUAL(note.vibrato.points.amp[1].y, 1.0);

    // Both parameter curve kinds, again dispatched by their type property.
    const auto pitch = singing->params.find("pitch");
    BOOST_REQUIRE(pitch != singing->params.end());
    BOOST_REQUIRE_EQUAL(pitch->second.original.size(), 1);
    const auto free = derivedAs<ParamCurveFree>(pitch->second.original.front(), ParamCurve::Free);
    BOOST_REQUIRE(free);
    BOOST_CHECK_EQUAL(free->step, 5);
    BOOST_CHECK_EQUAL(free->values.size(), 3);
    const auto anchors = derivedAs<ParamCurveAnchor>(pitch->second.edited.front(), ParamCurve::Anchor);
    BOOST_REQUIRE(anchors);
    BOOST_REQUIRE_EQUAL(anchors->nodes.size(), 2);
    BOOST_CHECK(anchors->nodes[1].interp == AnchorNode::Interpolation::Hermite);
    BOOST_CHECK_EQUAL(anchors->nodes[1].y, 6200);

    // Both singer kinds, the mixed one holding singers of its own.
    BOOST_REQUIRE(singing->sources.has_value());
    BOOST_CHECK_EQUAL(singing->sources->category, "voice");
    BOOST_REQUIRE_EQUAL(singing->sources->singers.size(), 2);
    const auto single = derivedAs<SingleSinger>(singing->sources->singers[0], Singer::Type::Single);
    BOOST_REQUIRE(single);
    BOOST_CHECK_EQUAL(single->id, "alice");
    const auto mixed = derivedAs<MixedSinger>(singing->sources->singers[1], Singer::Type::Mixed);
    BOOST_REQUIRE(mixed);
    BOOST_REQUIRE_EQUAL(mixed->singers.size(), 2);
    BOOST_REQUIRE_EQUAL(mixed->ratio.size(), 1);
    BOOST_CHECK_EQUAL(mixed->ratio[0], 0.4);

    const auto audio = derivedAs<AudioClip>(track.clips[1], Clip::Type::Audio);
    BOOST_REQUIRE(audio);
    BOOST_CHECK_EQUAL(audio->path, "a/b.wav");

    // Workspaces are carried through untouched at every level they appear.
    BOOST_CHECK_EQUAL(model.content.workspace.at("app").at("page").toInt(), 2);
    BOOST_CHECK_EQUAL(track.workspace.at("track").at("height").toInt(), 120);
    BOOST_CHECK_EQUAL(singing->workspace.at("clip").at("colour").toString(), "red");
    BOOST_CHECK_EQUAL(note.workspace.at("note").at("marked").toBool(), true);
}

// Writing what was read has to give the same bytes, or a save that changed nothing would still
// show up as a change to whatever is watching the file.
BOOST_AUTO_TEST_CASE(test_round_trip_is_stable) {
    SerializationErrorList errors;
    const auto first = serialize(richModel(), errors);
    const auto second = serialize(deserialize(first, errors), errors);
    BOOST_REQUIRE_EQUAL(errors.size(), 0);
    BOOST_CHECK_EQUAL(first, second);
}

BOOST_AUTO_TEST_CASE(test_output_is_compact) {
    SerializationErrorList errors;
    const auto text = serialize(richModel(), errors);
    BOOST_REQUIRE_EQUAL(errors.size(), 0);

    BOOST_CHECK(text.find('\n') == std::string::npos);
    BOOST_CHECK(text.find(": ") == std::string::npos);
    BOOST_CHECK(text.find(", ") == std::string::npos);
}

BOOST_AUTO_TEST_CASE(test_compressed_round_trip) {
    SerializationErrorList errors;
    const auto plain = serialize(richModel(), errors);
    const auto packed = serialize(richModel(), errors, defaultOptions, true);
    BOOST_REQUIRE_EQUAL(errors.size(), 0);

    // A zstd frame, and actually smaller than the text it came from.
    BOOST_REQUIRE_GE(packed.size(), 4);
    const std::array<unsigned char, 4> magic{0x28, 0xB5, 0x2F, 0xFD};
    for (std::size_t i = 0; i < magic.size(); ++i) {
        BOOST_CHECK_EQUAL(static_cast<unsigned char>(packed[i]), magic[i]);
    }
    BOOST_CHECK_LT(packed.size(), plain.size());

    // Reading does not have to be told which one it was given.
    SerializationErrorList readErrors;
    const auto model = deserialize(packed, readErrors);
    BOOST_REQUIRE_EQUAL(readErrors.size(), 0);
    BOOST_CHECK_EQUAL(serialize(model, readErrors), plain);
}

// A number keeps the form it was written in through a workspace, which is the one place the
// document carries values the model does not describe. An integer that came back as 1.0 would
// change the file every time it was opened and saved.
BOOST_AUTO_TEST_CASE(test_number_form_survives_a_round_trip) {
    Model model;
    model.content.workspace["k"] = stdc::json::Object{
        {"i", stdc::json::Value(1)},
        {"d", stdc::json::Value(1.0)},
        {"negative", stdc::json::Value(-7)},
        {"big", stdc::json::Value(std::int64_t(9007199254740993))},
    };

    SerializationErrorList errors;
    const auto text = serialize(model, errors);
    BOOST_REQUIRE_EQUAL(errors.size(), 0);
    BOOST_CHECK(text.find("\"i\":1,") != std::string::npos);
    BOOST_CHECK(text.find("\"d\":1.0") != std::string::npos);
    BOOST_CHECK(text.find("\"negative\":-7") != std::string::npos);
    BOOST_CHECK(text.find("9007199254740993") != std::string::npos);

    const auto back = deserialize(text, errors);
    BOOST_REQUIRE_EQUAL(errors.size(), 0);
    const auto &k = back.content.workspace.at("k");
    BOOST_CHECK(k.at("i").isInt());
    BOOST_CHECK(k.at("d").isDouble());
    BOOST_CHECK_EQUAL(k.at("big").toInt(), 9007199254740993);
}

// Writing must not fail on text that is not UTF-8, because the model can hold whatever a caller
// put in a std::string. The offending bytes are replaced and the rest of the document survives.
BOOST_AUTO_TEST_CASE(test_invalid_utf8_is_replaced_rather_than_fatal) {
    Model model;
    model.content.global.name = std::string("a\xFF\xFE"
                                            "b");
    model.content.global.author = "kept";

    SerializationErrorList errors;
    const auto text = serialize(model, errors);
    BOOST_REQUIRE_EQUAL(errors.size(), 0);

    SerializationErrorList readErrors;
    const auto back = deserialize(text, readErrors);
    BOOST_REQUIRE_EQUAL(readErrors.size(), 0);

    BOOST_CHECK_EQUAL(back.content.global.author, "kept");
    BOOST_CHECK_NE(back.content.global.name, model.content.global.name);
    BOOST_CHECK(back.content.global.name.find("\xEF\xBF\xBD") != std::string::npos);
    BOOST_CHECK_EQUAL(back.content.global.name.front(), 'a');
    BOOST_CHECK_EQUAL(back.content.global.name.back(), 'b');
}

// A rejected document says where it was rejected. This used to be a sentence the caller had to
// match on; an editor that wants to put the cursor on the offending character needs the numbers.
BOOST_AUTO_TEST_CASE(test_parse_failure_says_where) {
    const std::string text = "{\"version\": \"1.0.0\",\n  oops}";

    SerializationErrorList errors;
    deserialize(text, errors);

    BOOST_REQUIRE_EQUAL(errors.size(), 1);
    BOOST_REQUIRE_EQUAL(errors[0]->type(), SerializationError::JsonParseFailure);
    BOOST_CHECK(errors[0]->isFatal());

    const auto failure = errorAs<JsonParseFailureError>(errors[0]);
    BOOST_CHECK_EQUAL(static_cast<int>(failure->code()),
                      static_cast<int>(stdc::json::ParseError::UnexpectedToken));
    BOOST_CHECK_EQUAL(failure->line(), 2);
    BOOST_CHECK_EQUAL(failure->column(), 3);
    BOOST_CHECK_EQUAL(failure->offset(), 23);
    BOOST_CHECK_EQUAL(text[failure->offset()], 'o');
    BOOST_CHECK(!failure->message().empty());
    BOOST_CHECK(!failure->error().what.empty());
}

// Each of the parser's codes reaches the caller as itself rather than collapsing into one.
BOOST_AUTO_TEST_CASE(test_parse_failure_codes) {
    const std::pair<const char *, stdc::json::ParseError::Code> cases[] = {
        {"{\"version\":", stdc::json::ParseError::UnexpectedEnd},
        {"{\"version\":01}", stdc::json::ParseError::IllegalNumber},
        {"{\"version\":\"\\q\"}", stdc::json::ParseError::IllegalEscape},
        {"{\"version\":\"a\tb\"}", stdc::json::ParseError::IllegalString},
        {"{\"version\":\"1.0.0\"} trailing", stdc::json::ParseError::TrailingContent},
        // A comment is called out by name only where a value was expected. One after the whole
        // document is trailing content, comments enabled or not, so it stays that.
        {"{\"version\":/* c */\"1.0.0\"}", stdc::json::ParseError::CommentNotAllowed},
        {"{\"version\":\"1.0.0\"} /* c */", stdc::json::ParseError::TrailingContent},
    };

    for (const auto &[text, code] : cases) {
        SerializationErrorList errors;
        deserialize(text, errors);
        BOOST_REQUIRE_MESSAGE(errors.size() >= 1, "no error for " << text);
        BOOST_REQUIRE_EQUAL(errors[0]->type(), SerializationError::JsonParseFailure);
        BOOST_CHECK_EQUAL(static_cast<int>(errorAs<JsonParseFailureError>(errors[0])->code()),
                          static_cast<int>(code));
    }
}

BOOST_AUTO_TEST_CASE(test_root_must_be_an_object) {
    SerializationErrorList errors;
    deserialize("[1,2,3]", errors);

    BOOST_REQUIRE_GE(errors.size(), 1);
    BOOST_CHECK_EQUAL(errors[0]->type(), SerializationError::JsonRootIsNotObject);
    BOOST_CHECK(errors[0]->isFatal());
}

// The version decides which mapping reads the rest, so it is looked at before anything else and
// nothing is reported about a document whose version nobody knows.
BOOST_AUTO_TEST_CASE(test_version_must_be_recognised) {
    {
        SerializationErrorList errors;
        deserialize(R"({"content":{}})", errors);
        BOOST_REQUIRE_EQUAL(errors.size(), 1);
        BOOST_REQUIRE_EQUAL(errors[0]->type(), SerializationError::UnrecognizedVersion);
        BOOST_CHECK_EQUAL(errorAs<UnrecognizedVersionError>(errors[0])->actualVersion(), "");
    }
    {
        SerializationErrorList errors;
        deserialize(R"({"version":"9.9.9","content":{}})", errors);
        BOOST_REQUIRE_EQUAL(errors.size(), 1);
        BOOST_REQUIRE_EQUAL(errors[0]->type(), SerializationError::UnrecognizedVersion);
        BOOST_CHECK_EQUAL(errorAs<UnrecognizedVersionError>(errors[0])->actualVersion(), "9.9.9");
    }
    {
        // A version that is not text at all, which used to come out of the JSON library as an
        // exception rather than as an error in the list.
        SerializationErrorList errors;
        deserialize(R"({"version":42,"content":{}})", errors);
        BOOST_REQUIRE_EQUAL(errors.size(), 1);
        BOOST_CHECK_EQUAL(errors[0]->type(), SerializationError::UnrecognizedVersion);
    }
}

BOOST_AUTO_TEST_CASE(test_writing_an_unknown_version) {
    Model model;
    model.version = static_cast<Model::Version>(99);

    SerializationErrorList errors;
    const auto text = serialize(model, errors);
    BOOST_REQUIRE_EQUAL(errors.size(), 1);
    BOOST_CHECK_EQUAL(errors[0]->type(), SerializationError::UnrecognizedVersion);
    BOOST_CHECK_EQUAL(errorAs<UnrecognizedVersionError>(errors[0])->actualVersionFlag(), 99);
    BOOST_CHECK(text.empty());
}

// With CheckError off nothing is reported and whatever could be read is read. This is the mode
// for opening a file that is known to be imperfect.
BOOST_AUTO_TEST_CASE(test_check_error_off_reports_nothing) {
    SerializationErrorList errors;
    auto text = serialize(richModel(), errors);
    BOOST_REQUIRE_EQUAL(errors.size(), 0);
    text = replaceFirst(text, R"("keyNum":60)", R"("keyNum":9000)");
    text = replaceFirst(text, R"("centShift":-3)", R"("centShift":"nonsense")");

    {
        SerializationErrorList quiet;
        const auto model = deserialize(text, quiet, Serializer::Option{});
        BOOST_CHECK_EQUAL(quiet.size(), 0);
        // The good parts still arrived.
        BOOST_CHECK_EQUAL(model.content.global.name, "song");
        BOOST_REQUIRE_EQUAL(model.content.tracks.size(), 1);
    }
    {
        SerializationErrorList loud;
        deserialize(text, loud, Serializer::CheckError);
        BOOST_CHECK_GE(loud.size(), 2);
    }
}

// Fail-fast stops at the first thing that is wrong; without it the whole document is walked and
// everything wrong with it comes back at once.
BOOST_AUTO_TEST_CASE(test_fail_fast_stops_at_the_first_error) {
    SerializationErrorList errors;
    auto text = serialize(richModel(), errors);
    BOOST_REQUIRE_EQUAL(errors.size(), 0);
    text = replaceFirst(text, R"("centShift":-3)", R"("centShift":-999)");
    text = replaceFirst(text, R"("pan":0.25)", R"("pan":9.0)");

    SerializationErrorList fast;
    deserialize(text, fast, Serializer::FailFast | Serializer::CheckError);
    BOOST_CHECK_EQUAL(fast.size(), 1);

    SerializationErrorList all;
    deserialize(text, all, Serializer::CheckError);
    BOOST_CHECK_EQUAL(all.size(), 2);
    BOOST_CHECK_EQUAL(all[0]->type(), SerializationError::RangeConstraintViolation);
    BOOST_CHECK_EQUAL(errorAs<RangeConstraintViolationError>(all[0])->path(),
                      "$.content.global.centShift");
    BOOST_CHECK_EQUAL(errorAs<RangeConstraintViolationError>(all[1])->path(),
                      "$.content.master.control.pan");
}

// A property nobody knows is an error by default -- it is usually a typo or a newer file -- but
// a caller that wants to be lenient can say so.
BOOST_AUTO_TEST_CASE(test_redundant_property) {
    SerializationErrorList errors;
    const auto text =
        replaceFirst(serialize(richModel(), errors), R"({"content")", R"({"stowaway":1,"content")");
    BOOST_REQUIRE_EQUAL(errors.size(), 0);

    SerializationErrorList strict;
    deserialize(text, strict, Serializer::CheckError);
    BOOST_REQUIRE_EQUAL(strict.size(), 1);
    BOOST_REQUIRE_EQUAL(strict[0]->type(), SerializationError::RedundantProperty);
    const auto redundant = errorAs<RedundantPropertyError>(strict[0]);
    BOOST_CHECK_EQUAL(redundant->path(), "$");
    BOOST_REQUIRE_EQUAL(redundant->redundantProperties().size(), 1);
    BOOST_CHECK_EQUAL(redundant->redundantProperties().front(), "stowaway");
    BOOST_CHECK(strict[0]->isError());

    SerializationErrorList lenient;
    const auto model = deserialize(
        text, lenient, Serializer::CheckError | Serializer::TolerateRedundantProperty);
    BOOST_CHECK_EQUAL(lenient.size(), 0);
    BOOST_CHECK_EQUAL(model.content.global.name, "song");
}

BOOST_AUTO_TEST_CASE(test_missing_property) {
    SerializationErrorList errors;
    const auto text = replaceFirst(serialize(richModel(), errors), R"("author")", R"("Author")");
    BOOST_REQUIRE_EQUAL(errors.size(), 0);

    SerializationErrorList found;
    deserialize(text, found, Serializer::CheckError);

    // One for the property that is gone and one for the one nobody asked for.
    BOOST_REQUIRE_GE(found.size(), 1);
    BOOST_REQUIRE_EQUAL(found[0]->type(), SerializationError::MissingProperty);
    const auto missing = errorAs<MissingPropertyError>(found[0]);
    BOOST_CHECK_EQUAL(missing->path(), "$.content.global");
    BOOST_REQUIRE_EQUAL(missing->missingProperties().size(), 1);
    BOOST_CHECK_EQUAL(missing->missingProperties().front(), "author");
}

// An empty stream is not a document, and the reader has to say so rather than hand back a model
// that looks like a new project.
BOOST_AUTO_TEST_CASE(test_empty_input) {
    SerializationErrorList errors;
    deserialize("", errors);
    BOOST_REQUIRE_GE(errors.size(), 1);
    BOOST_CHECK(errors[0]->isFatal());
}

// Bytes that start like a zstd frame and then are not. The compression failure has to be
// reported as itself rather than as whatever the parser makes of the wreckage.
BOOST_AUTO_TEST_CASE(test_truncated_compressed_input) {
    SerializationErrorList errors;
    auto packed = serialize(richModel(), errors, defaultOptions, true);
    BOOST_REQUIRE_EQUAL(errors.size(), 0);
    packed.resize(packed.size() / 2);

    SerializationErrorList readErrors;
    deserialize(packed, readErrors);
    BOOST_REQUIRE_GE(readErrors.size(), 1);
    BOOST_CHECK(readErrors[0]->isFatal());
}

BOOST_AUTO_TEST_SUITE_END()
