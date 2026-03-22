#include "serializer.h"

#include <ios>

#include <opendspx/model.h>

#include <opendspxserializer/jsonconverterv1.h>
#include <opendspxserializer/serializationerror.h>

namespace opendspx {

    std::string Serializer::versionToText(Model::Version version) {
        switch (version) {
            case Model::Version::V1:
                return "1.0.0";
            default:
                return {};
        }
    }

    Model::Version Serializer::versionFromText(const std::string &text, bool *ok) {
        if (ok)
            *ok = true;
        if (text == "1.0.0") {
            return Model::Version::V1;
        }
        if (ok)
            *ok = false;
        return {};
    }

    void Serializer::serialize(std::ostream &out, const Model &model, SerializationErrorList &errors, Option options, bool compress) {
        nlohmann::json doc;
        switch (model.version) {
            case Model::Version::V1:
                doc = JsonConverterV1::toJson(model, errors, options);
                break;
            default: {
                errors.addError<UnrecognizedVersionError>(model.version);
                return;
            }
        }
        // Here we have to use private api to specify error_handler
        nlohmann::detail::serializer<nlohmann::json> s(nlohmann::detail::output_adapter<char>(out), ' ', nlohmann::detail::error_handler_t::replace);
        s.dump(doc, false, false, 0);
    }

    Model Serializer::deserialize(std::istream &in, SerializationErrorList &errors, Option options) {
        nlohmann::json doc;
        try {
            doc = nlohmann::json::parse(in, nullptr);
        } catch (const nlohmann::json::parse_error &e) {
            errors.addError<JsonParseFailureError>(e.id, e.byte, e.what());
            return {};
        }
        if (!doc.is_object()) {
            errors.addError<JsonRootIsNotObjectError>();
        }
        auto obj = std::move(doc);
        std::string versionText;
        try {
            versionText = obj.at("version").get<std::string>();
        } catch (const nlohmann::json::exception &e) {
            errors.addError<UnrecognizedVersionError>(std::string{});
            return {};
        }
        bool ok;
        auto version = versionFromText(versionText, &ok);
        if (!ok) {
            errors.addError<UnrecognizedVersionError>(versionText);
            return {};
        }
        switch (version) {
            case Model::Version::V1:
                return JsonConverterV1::fromJson<Model>(obj, errors, options);
        }
        Q_UNREACHABLE();
    }

}
