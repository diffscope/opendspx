#include <any>
#include <string>

#include <serializer/helpers_p.h>

#include <opendspx/serializer/jsonconverterv1.h>

#include <boost/test/unit_test.hpp>

using namespace opendspx;
using namespace opendspx::test;

namespace {

    // The mapping layer is where the error paths are built, so most cases here read a fragment
    // and look at what came back rather than at the model.
    template <typename T>
    T read(std::string_view text, SerializationErrorList &errors,
           Serializer::Option options = Serializer::CheckError) {
        return JsonConverterV1::fromJson<T>(parse(text), errors, options);
    }

    template <typename T>
    stdc::json::Value write(const T &entity, SerializationErrorList &errors,
                          Serializer::Option options = Serializer::CheckError) {
        return JsonConverterV1::toJson(entity, errors, options);
    }


}

BOOST_AUTO_TEST_SUITE(test_jsonconverterv1)

BOOST_AUTO_TEST_CASE(test_entity_round_trip) {
    Global global;
    global.author = "author";
    global.name = "name";
    global.centShift = 12;
    global.editorId = "id";
    global.editorName = "editor";

    SerializationErrorList errors;
    const auto json = write(global, errors);
    BOOST_REQUIRE_EQUAL(errors.size(), 0);
    BOOST_CHECK(json.isObject());

    const auto back = JsonConverterV1::fromJson<Global>(json, errors);
    BOOST_REQUIRE_EQUAL(errors.size(), 0);
    BOOST_CHECK_EQUAL(back.author, "author");
    BOOST_CHECK_EQUAL(back.centShift, 12);
}

// The root of a path is the caller's to name, so an error from a fragment can be reported
// against wherever that fragment sits in a larger document.
BOOST_AUTO_TEST_CASE(test_path_root_is_the_callers) {
    SerializationErrorList errors;
    JsonConverterV1::fromJson<Global>(parse(R"({"centShift":900})"), errors,
                                      Serializer::CheckError, "somewhere");

    BOOST_REQUIRE_GE(errors.size(), 1);
    bool sawIt = false;
    for (const auto &error : errors) {
        if (error->type() == SerializationError::RangeConstraintViolation) {
            BOOST_CHECK_EQUAL(errorAs<RangeConstraintViolationError>(error)->path(),
                              "somewhere.centShift");
            sawIt = true;
        }
    }
    BOOST_CHECK(sawIt);
}

// An index for an array, a dot for a property, all the way down. This is what an editor puts in
// front of the user, so it is checked at more than one level of nesting.
BOOST_AUTO_TEST_CASE(test_error_paths) {
    SerializationErrorList errors;
    read<Timeline>(
        R"({"labels":[],"timeSignatures":[],"tempos":[{"pos":0,"value":120},{"pos":0,"value":5}]})",
        errors);

    BOOST_REQUIRE_EQUAL(errors.size(), 1);
    BOOST_REQUIRE_EQUAL(errors[0]->type(), SerializationError::RangeConstraintViolation);
    BOOST_CHECK_EQUAL(errorAs<RangeConstraintViolationError>(errors[0])->path(),
                      "$.tempos[1].value");
}

BOOST_AUTO_TEST_CASE(test_deeply_nested_error_path) {
    SerializationErrorList errors;
    read<ParamCurveAnchor>(
        R"({"type":"anchor","start":0,"nodes":[{"x":0,"y":0,"interp":"linear"},)"
        R"({"x":-5,"y":0,"interp":"linear"}]})",
        errors);

    BOOST_REQUIRE_EQUAL(errors.size(), 1);
    BOOST_CHECK_EQUAL(errorAs<RangeConstraintViolationError>(errors[0])->path(),
                      "$.nodes[1].x");
}

// Every scalar kind reports what it was given as well as what it wanted, so the message can say
// both. The reported kind is the JSON one, not the C++ one.
BOOST_AUTO_TEST_CASE(test_data_type_errors) {
    const struct {
        const char *json;
        InvalidDataTypeError::DataType actual;
    } cases[] = {
        {R"({"author":1,"name":"n","centShift":0,"editorId":"e","editorName":"E"})",
         InvalidDataTypeError::Integer},
        {R"({"author":1.5,"name":"n","centShift":0,"editorId":"e","editorName":"E"})",
         InvalidDataTypeError::Double},
        {R"({"author":true,"name":"n","centShift":0,"editorId":"e","editorName":"E"})",
         InvalidDataTypeError::Bool},
        {R"({"author":null,"name":"n","centShift":0,"editorId":"e","editorName":"E"})",
         InvalidDataTypeError::Null},
        {R"({"author":[],"name":"n","centShift":0,"editorId":"e","editorName":"E"})",
         InvalidDataTypeError::Array},
        {R"({"author":{},"name":"n","centShift":0,"editorId":"e","editorName":"E"})",
         InvalidDataTypeError::Object},
    };

    for (const auto &[json, actual] : cases) {
        SerializationErrorList errors;
        read<Global>(json, errors);
        BOOST_REQUIRE_MESSAGE(errors.size() == 1, "for " << json);
        BOOST_REQUIRE_EQUAL(errors[0]->type(), SerializationError::InvalidDataType);
        const auto error = errorAs<InvalidDataTypeError>(errors[0]);
        BOOST_CHECK_EQUAL(error->path(), "$.author");
        BOOST_CHECK_EQUAL(static_cast<int>(error->actualType()), static_cast<int>(actual));
        BOOST_REQUIRE_EQUAL(error->expectedTypes().size(), 1);
        BOOST_CHECK_EQUAL(static_cast<int>(error->expectedTypes().front()),
                          static_cast<int>(InvalidDataTypeError::String));
    }
}

// A number is an integer when its value is one, not when it was written without a point. 1.0
// where an integer belongs is the same number, and a document that has been through a library
// that writes every number as a double still reads.
BOOST_AUTO_TEST_CASE(test_a_whole_double_counts_as_an_integer) {
    {
        SerializationErrorList errors;
        const auto global = read<Global>(
            R"({"author":"a","name":"n","centShift":12.0,"editorId":"e","editorName":"E"})",
            errors);
        BOOST_CHECK_EQUAL(errors.size(), 0);
        BOOST_CHECK_EQUAL(global.centShift, 12);
    }
    {
        SerializationErrorList errors;
        read<Global>(R"({"author":"a","name":"n","centShift":12.5,"editorId":"e","editorName":"E"})",
                     errors);
        BOOST_REQUIRE_EQUAL(errors.size(), 1);
        BOOST_REQUIRE_EQUAL(errors[0]->type(), SerializationError::InvalidDataType);
        BOOST_CHECK_EQUAL(static_cast<int>(errorAs<InvalidDataTypeError>(errors[0])->actualType()),
                          static_cast<int>(InvalidDataTypeError::Double));
    }
    // The other way round is fine: a double property takes a number written without a point.
    {
        SerializationErrorList errors;
        const auto tempo = read<Tempo>(R"({"pos":0,"value":120})", errors);
        BOOST_CHECK_EQUAL(errors.size(), 0);
        BOOST_CHECK_EQUAL(tempo.value, 120.0);
    }
}

// A range error carries the value and the bounds, not only a sentence, so a caller can offer to
// clamp. A one-sided bound leaves the other empty rather than making one up.
BOOST_AUTO_TEST_CASE(test_range_constraint_carries_its_bounds) {
    {
        SerializationErrorList errors;
        read<Note>(R"({"pos":0,"length":0,"keyNum":200,"centShift":0,"language":"","lyric":"",)"
                   R"("pronunciation":{"original":"","edited":""},)"
                   R"("phonemes":{"original":[],"edited":[]},)"
                   R"("vibrato":{"start":0,"end":1,"amp":0,"freq":0,"phase":0,"offset":0,)"
                   R"("points":{"amp":[],"freq":[]}},"workspace":{}})",
                   errors);
        BOOST_REQUIRE_EQUAL(errors.size(), 1);
        const auto error = errorAs<RangeConstraintViolationError>(errors[0]);
        BOOST_CHECK_EQUAL(error->path(), "$.keyNum");
        BOOST_CHECK_EQUAL(std::any_cast<int>(error->actualValue()), 200);
        BOOST_CHECK_EQUAL(std::any_cast<int>(error->expectedMinimum()), 0);
        BOOST_CHECK_EQUAL(std::any_cast<int>(error->expectedMaximum()), 127);
    }
    {
        // pos has a floor and no ceiling.
        SerializationErrorList errors;
        read<Tempo>(R"({"pos":-1,"value":120})", errors);
        BOOST_REQUIRE_EQUAL(errors.size(), 1);
        const auto error = errorAs<RangeConstraintViolationError>(errors[0]);
        BOOST_CHECK_EQUAL(std::any_cast<int>(error->expectedMinimum()), 0);
        BOOST_CHECK(!error->expectedMaximum().has_value());
    }
    // The bounds are inclusive.
    {
        SerializationErrorList errors;
        read<Tempo>(R"({"pos":0,"value":10})", errors);
        read<Tempo>(R"({"pos":0,"value":1000})", errors);
        BOOST_CHECK_EQUAL(errors.size(), 0);
    }
}

// A property with a single legal value is still a range, which is how "step must be 5" is said.
BOOST_AUTO_TEST_CASE(test_a_range_of_one) {
    SerializationErrorList errors;
    read<ParamCurveFree>(R"({"type":"free","start":0,"step":10,"values":[]})", errors);
    BOOST_REQUIRE_EQUAL(errors.size(), 1);
    const auto error = errorAs<RangeConstraintViolationError>(errors[0]);
    BOOST_CHECK_EQUAL(error->path(), "$.step");
    BOOST_CHECK_EQUAL(std::any_cast<int>(error->expectedMinimum()), 5);
    BOOST_CHECK_EQUAL(std::any_cast<int>(error->expectedMaximum()), 5);
}

BOOST_AUTO_TEST_CASE(test_enum_constraint) {
    {
        SerializationErrorList errors;
        read<AnchorNode>(R"({"x":0,"y":0,"interp":"wobble"})", errors);
        BOOST_REQUIRE_EQUAL(errors.size(), 1);
        BOOST_REQUIRE_EQUAL(errors[0]->type(), SerializationError::EnumConstraintViolation);
        const auto error = errorAs<EnumConstraintViolationError>(errors[0]);
        BOOST_CHECK_EQUAL(error->path(), "$.interp");
        // The three the format allows, offered back to the caller.
        BOOST_CHECK_EQUAL(error->expectedEnumValues().size(), 3);
    }
    {
        // A name where a name belongs, but the wrong kind of value, is a type error rather than
        // an enum one -- there is nothing to compare against the list.
        SerializationErrorList errors;
        read<AnchorNode>(R"({"x":0,"y":0,"interp":3})", errors);
        BOOST_REQUIRE_EQUAL(errors.size(), 1);
        BOOST_CHECK_EQUAL(errors[0]->type(), SerializationError::InvalidDataType);
    }
    {
        // The time signature denominator is an enum of numbers rather than of names.
        SerializationErrorList errors;
        read<TimeSignature>(R"({"index":0,"numerator":4,"denominator":5})", errors);
        BOOST_REQUIRE_EQUAL(errors.size(), 1);
        BOOST_REQUIRE_EQUAL(errors[0]->type(), SerializationError::EnumConstraintViolation);
        BOOST_CHECK_EQUAL(errorAs<EnumConstraintViolationError>(errors[0])->expectedEnumValues().size(), 8);
    }
    {
        SerializationErrorList errors;
        const auto signature = read<TimeSignature>(R"({"index":0,"numerator":4,"denominator":128})", errors);
        BOOST_CHECK_EQUAL(errors.size(), 0);
        BOOST_CHECK_EQUAL(signature.denominator, 128);
    }
}

// Which kind of clip a fragment is comes from its type property, and an unknown one is answered
// with the ones there are.
BOOST_AUTO_TEST_CASE(test_polymorphic_dispatch) {
    {
        SerializationErrorList errors;
        const auto clip = read<ClipRef>(
            R"({"type":"audio","name":"n","path":"p","control":{"gain":0,"pan":0,"mute":false},)"
            R"("time":{"pos":0,"length":0,"clipStart":0,"clipLen":0},"workspace":{}})",
            errors);
        BOOST_REQUIRE_EQUAL(errors.size(), 0);
        BOOST_REQUIRE(clip);
        BOOST_CHECK(clip->type == Clip::Type::Audio);
        BOOST_CHECK_EQUAL(derivedAs<AudioClip>(clip, Clip::Type::Audio)->path, "p");
    }
    {
        SerializationErrorList errors;
        read<ClipRef>(R"({"type":"video"})", errors);
        BOOST_REQUIRE_EQUAL(errors.size(), 1);
        BOOST_REQUIRE_EQUAL(errors[0]->type(), SerializationError::InvalidObjectType);
        const auto error = errorAs<InvalidObjectTypeError>(errors[0]);
        BOOST_CHECK_EQUAL(error->actualType(), "video");
        BOOST_REQUIRE_EQUAL(error->expectedTypes().size(), 2);
        BOOST_CHECK_EQUAL(error->expectedTypes()[0], "audio");
        BOOST_CHECK_EQUAL(error->expectedTypes()[1], "singing");
    }
    {
        // No type property at all, which is a missing property rather than an unknown type.
        SerializationErrorList errors;
        read<ClipRef>(R"({"name":"n"})", errors);
        BOOST_REQUIRE_EQUAL(errors.size(), 1);
        BOOST_REQUIRE_EQUAL(errors[0]->type(), SerializationError::MissingProperty);
        BOOST_CHECK_EQUAL(errorAs<MissingPropertyError>(errors[0])->missingProperties().front(),
                          "type");
    }
    {
        // Not an object at all.
        SerializationErrorList errors;
        read<ClipRef>("42", errors);
        BOOST_REQUIRE_EQUAL(errors.size(), 1);
        BOOST_CHECK_EQUAL(errors[0]->type(), SerializationError::InvalidDataType);
    }
    {
        // A type property that is not a name. It used to reach the JSON library as a bad cast.
        SerializationErrorList errors;
        read<ClipRef>(R"({"type":7})", errors);
        BOOST_REQUIRE_EQUAL(errors.size(), 1);
        BOOST_CHECK_EQUAL(errors[0]->type(), SerializationError::InvalidObjectType);
    }
}

// An optional property is null when it is absent from the model, and null reads back as absent.
// A missing one is a different thing, and is reported.
BOOST_AUTO_TEST_CASE(test_optional_property) {
    SingingClip clip;
    SerializationErrorList errors;
    const auto json = write(clip, errors);
    BOOST_REQUIRE_EQUAL(errors.size(), 0);
    BOOST_CHECK(json["sources"].isNull());

    const auto back = JsonConverterV1::fromJson<SingingClip>(json, errors);
    BOOST_REQUIRE_EQUAL(errors.size(), 0);
    BOOST_CHECK(!back.sources.has_value());
}

// A workspace value is an object, and the key it was found under is part of the path -- without
// it the report says only which workspace, not which entry of it.
BOOST_AUTO_TEST_CASE(test_workspace_entries) {
    {
        SerializationErrorList errors;
        const auto workspace = read<Workspace>(R"({"a":{"x":1},"b":{}})", errors);
        BOOST_REQUIRE_EQUAL(errors.size(), 0);
        BOOST_CHECK_EQUAL(workspace.size(), 2);
        BOOST_CHECK_EQUAL(workspace.at("a").at("x").toInt(), 1);
    }
    {
        SerializationErrorList errors;
        read<Workspace>(R"({"a":{},"bad":42})", errors);
        BOOST_REQUIRE_EQUAL(errors.size(), 1);
        BOOST_REQUIRE_EQUAL(errors[0]->type(), SerializationError::InvalidDataType);
        BOOST_CHECK_EQUAL(errorAs<InvalidDataTypeError>(errors[0])->path(), "$.bad");
    }
    {
        // A workspace that is not an object at all.
        SerializationErrorList errors;
        read<Workspace>("[]", errors);
        BOOST_REQUIRE_EQUAL(errors.size(), 1);
        BOOST_CHECK_EQUAL(errors[0]->type(), SerializationError::InvalidDataType);
    }
}

// A mixing ratio holds one number fewer than there are sources and the numbers have to be a
// share of a whole, so both the individual bounds and the total are checked.
BOOST_AUTO_TEST_CASE(test_mixing_ratio) {
    {
        SerializationErrorList errors;
        const auto ratio = read<SourceMixingRatio>("[0.25,0.5]", errors);
        BOOST_REQUIRE_EQUAL(errors.size(), 0);
        BOOST_CHECK_EQUAL(ratio.size(), 2);
    }
    {
        SerializationErrorList errors;
        read<SourceMixingRatio>("[0.25,1.5]", errors);
        BOOST_REQUIRE_EQUAL(errors.size(), 1);
        BOOST_REQUIRE_EQUAL(errors[0]->type(), SerializationError::RangeConstraintViolation);
        BOOST_CHECK_EQUAL(errorAs<RangeConstraintViolationError>(errors[0])->path(), "$[1]");
    }
    {
        // Each share is legal on its own, but together they are more than there is.
        SerializationErrorList errors;
        read<DynamicMixingAnchor>(R"({"pos":0,"ratio":[0.7,0.6]})", errors);
        BOOST_REQUIRE_EQUAL(errors.size(), 1);
        BOOST_REQUIRE_EQUAL(errors[0]->type(), SerializationError::InvalidRatioPartition);
        const auto error = errorAs<InvalidRatioPartitionError>(errors[0]);
        BOOST_CHECK_EQUAL(error->path(), "$.ratio");
        BOOST_REQUIRE_EQUAL(error->ratio().size(), 2);
        BOOST_CHECK_EQUAL(error->ratio()[0], 0.7);
    }
}

// The two source constraints that are not about one property: a mix has to name somebody, and
// each anchor has to have a share for each of them.
BOOST_AUTO_TEST_CASE(test_mixed_singer_constraints) {
    const auto singer = [](const char *singers, const char *ratio) {
        return std::string(R"({"type":"mixed","extra":{},"workspace":{},"singers":)") + singers +
               R"(,"ratio":)" + ratio + "}";
    };
    constexpr const char *one = R"([{"type":"single","id":"a","extra":{},"workspace":{}}])";
    constexpr const char *two =
        R"([{"type":"single","id":"a","extra":{},"workspace":{}},)"
        R"({"type":"single","id":"b","extra":{},"workspace":{}}])";

    {
        SerializationErrorList errors;
        read<SingerRef>(singer(two, "[0.4]"), errors);
        BOOST_CHECK_EQUAL(errors.size(), 0);
    }
    {
        SerializationErrorList errors;
        read<SingerRef>(singer("[]", "[]"), errors);
        BOOST_REQUIRE_GE(errors.size(), 1);
        BOOST_REQUIRE_EQUAL(errors[0]->type(), SerializationError::EmptySingerMixing);
        BOOST_CHECK_EQUAL(errorAs<EmptySingerMixingError>(errors[0])->path(), "$.singers");
    }
    {
        // Two singers but a ratio for three.
        SerializationErrorList errors;
        read<SingerRef>(singer(two, "[0.3,0.3]"), errors);
        BOOST_REQUIRE_EQUAL(errors.size(), 1);
        BOOST_REQUIRE_EQUAL(errors[0]->type(), SerializationError::PartCountNotMatch);
        const auto error = errorAs<PartCountNotMatchError>(errors[0]);
        BOOST_CHECK_EQUAL(error->path(), "$.ratio");
        BOOST_CHECK_EQUAL(error->expectedPartCount(), 2);
        BOOST_CHECK_EQUAL(error->actualPartCount(), 3);
    }
    {
        SerializationErrorList errors;
        read<SingerRef>(singer(one, "[]"), errors);
        BOOST_CHECK_EQUAL(errors.size(), 0);
    }
}

// Writing checks what reading checks. A model held in memory can be out of range -- nothing stops
// a caller assigning it -- and the error has to come before the file does, not after.
BOOST_AUTO_TEST_CASE(test_writing_checks_constraints_too) {
    {
        Note note;
        note.keyNum = 300;
        SerializationErrorList errors;
        write(note, errors);
        BOOST_REQUIRE_GE(errors.size(), 1);
        BOOST_REQUIRE_EQUAL(errors[0]->type(), SerializationError::RangeConstraintViolation);
        BOOST_CHECK_EQUAL(errorAs<RangeConstraintViolationError>(errors[0])->path(), "$.keyNum");
    }
    {
        MixedSinger mixed;
        SerializationErrorList errors;
        write(mixed, errors);
        BOOST_REQUIRE_GE(errors.size(), 1);
        BOOST_CHECK_EQUAL(errors[0]->type(), SerializationError::EmptySingerMixing);
    }
    {
        DynamicMixingAnchor anchor;
        anchor.ratio = SourceMixingRatio{0.7, 0.6};
        SerializationErrorList errors;
        write(anchor, errors);
        BOOST_REQUIRE_GE(errors.size(), 1);
        BOOST_CHECK_EQUAL(errors[0]->type(), SerializationError::InvalidRatioPartition);
    }
    // With checking off the same model writes without complaint.
    {
        Note note;
        note.keyNum = 300;
        SerializationErrorList errors;
        const auto json = write(note, errors, Serializer::Option{});
        BOOST_CHECK_EQUAL(errors.size(), 0);
        BOOST_CHECK_EQUAL(json["keyNum"].toInt(), 300);
    }
}

// An enum value the format has no name for cannot be written, and saying so is better than
// writing a document that will not read back.
BOOST_AUTO_TEST_CASE(test_writing_an_unnamed_enum_value) {
    AnchorNode node;
    node.interp = static_cast<AnchorNode::Interpolation>(42);

    SerializationErrorList errors;
    const auto json = write(node, errors);
    BOOST_REQUIRE_GE(errors.size(), 1);
    BOOST_CHECK_EQUAL(errors[0]->type(), SerializationError::EnumConstraintViolation);
    BOOST_CHECK(json["interp"].isNull());
}

// The error list keeps a running answer to "is any of this fatal", so a caller can decide
// whether to go on without walking the list.
BOOST_AUTO_TEST_CASE(test_error_list_summarises_itself) {
    SerializationErrorList errors;
    BOOST_CHECK(!errors.containsFatal());
    BOOST_CHECK(!errors.containsError());
    BOOST_CHECK(!errors.containsWarning());

    read<Global>(R"({"author":1,"name":"n","centShift":0,"editorId":"e","editorName":"E"})",
                 errors);
    BOOST_CHECK(errors.containsError());
    BOOST_CHECK(!errors.containsFatal());

    errors.addError<JsonRootIsNotObjectError>();
    BOOST_CHECK(errors.containsFatal());

    errors.addError<ZeroLengthRangeError>("$");
    BOOST_CHECK(errors.containsWarning());
}

BOOST_AUTO_TEST_SUITE_END()
