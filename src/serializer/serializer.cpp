#include "serializer.h"

#include "zstdstreambuf_p.h"

#include <array>
#include <cassert>
#include <ios>
#include <memory>

#include <opendspx/model.h>

#include <opendspxserializer/jsonconverterv1.h>
#include <opendspxserializer/serializationerror.h>

namespace opendspx {

    namespace {

        bool isZstdHeader(std::istream &in) {
            using traits_type = std::streambuf::traits_type;

            auto *buf = in.rdbuf();
            std::array<unsigned char, 4> header{};

            const auto first = buf->sgetc();
            if (traits_type::eq_int_type(first, traits_type::eof())) {
                return false;
            }
            header[0] = static_cast<unsigned char>(traits_type::to_char_type(first));

            int count = 1;
            while (count < 4) {
                const auto ch = buf->snextc();
                if (traits_type::eq_int_type(ch, traits_type::eof())) {
                    break;
                }
                header[count] = static_cast<unsigned char>(traits_type::to_char_type(ch));
                ++count;
            }

            for (int i = 1; i < count; ++i) {
                if (buf->sungetc() == traits_type::eof()) {
                    return false;
                }
            }

            if (count < 4) {
                return false;
            }
            return header[0] == 0x28 && header[1] == 0xB5 && header[2] == 0x2F && header[3] == 0xFD;
        }

    }

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
                errors.addError<UnrecognizedVersionError>(static_cast<int>(model.version));
                return;
            }
        }

        std::ostream *jsonOutput = &out;
        std::unique_ptr<ZstdStreamBuf> zstdBuf;
        std::unique_ptr<std::ostream> zstdOut;

        if (compress) {
            zstdBuf = std::make_unique<ZstdStreamBuf>(out);
            if (!zstdBuf->good()) {
                errors.addError<CompressionFailureError>(zstdBuf->errorMessage());
                return;
            }

            zstdOut = std::make_unique<std::ostream>(zstdBuf.get());
            jsonOutput = zstdOut.get();
        }

        // Here we have to use private api to specify error_handler
        nlohmann::detail::serializer<nlohmann::json> s(nlohmann::detail::output_adapter<char>(*jsonOutput), ' ', nlohmann::detail::error_handler_t::replace);
        s.dump(doc, false, false, 0);

        if (compress) {
            zstdOut->flush();

            if (!zstdBuf->finish()) {
                errors.addError<CompressionFailureError>(zstdBuf->errorMessage());
            }
        }
    }

    Model Serializer::deserialize(std::istream &in, SerializationErrorList &errors, Option options) {
        nlohmann::json doc;

        std::istream *jsonInput = &in;
        std::unique_ptr<ZstdStreamBuf> zstdBuf;
        std::unique_ptr<std::istream> zstdIn;

        if (isZstdHeader(in)) {
            zstdBuf = std::make_unique<ZstdStreamBuf>(in);
            if (!zstdBuf->good()) {
                errors.addError<CompressionFailureError>(zstdBuf->errorMessage());
                return {};
            }
            zstdIn = std::make_unique<std::istream>(zstdBuf.get());
            jsonInput = zstdIn.get();
        }

        try {
            doc = nlohmann::json::parse(*jsonInput, nullptr);
        } catch (const nlohmann::json::parse_error &e) {
            if (zstdBuf && zstdBuf->hasError()) {
                errors.addError<CompressionFailureError>(zstdBuf->errorMessage());
                return {};
            }
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
        assert(false && "Unrecognized version");
        return {};
    }

}
