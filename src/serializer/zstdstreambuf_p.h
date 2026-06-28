#ifndef OPENDSPX_SERIALIZER_ZSTDSTREAMBUF_P_H
#define OPENDSPX_SERIALIZER_ZSTDSTREAMBUF_P_H

#include <array>
#include <istream>
#include <ostream>
#include <streambuf>
#include <string>

#include <zstd.h>

namespace opendspx {

    class ZstdStreamBuf final : public std::streambuf {
    public:
        explicit ZstdStreamBuf(std::ostream &out, int compressionLevel = ZSTD_CLEVEL_DEFAULT);
        explicit ZstdStreamBuf(std::istream &in);
        ~ZstdStreamBuf() override;

        bool good() const;
        bool hasError() const;
        const std::string &errorMessage() const;

        bool finish();

    protected:
        int_type underflow() override;
        int_type overflow(int_type ch) override;
        std::streamsize xsputn(const char *s, std::streamsize count) override;
        int sync() override;

    private:
        enum class Mode {
            Compress,
            Decompress,
        };

        bool compressBuffered();
        bool finishCompression();
        bool setError(std::string message);

        static constexpr size_t BufferSize = 1 << 15;

        Mode m_mode;
        std::istream *m_in = nullptr;
        std::ostream *m_out = nullptr;

        ZSTD_CStream *m_cstream = nullptr;
        ZSTD_DStream *m_dstream = nullptr;

        std::array<char, BufferSize> m_inBuffer{};
        std::array<char, BufferSize> m_outBuffer{};

        ZSTD_inBuffer m_zstdIn{nullptr, 0, 0};
        size_t m_decompressRet = 0;
        bool m_finished = false;

        std::string m_errorMessage;
    };

}

#endif // OPENDSPX_SERIALIZER_ZSTDSTREAMBUF_P_H