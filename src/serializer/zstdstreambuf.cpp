#include "zstdstreambuf_p.h"

#include <algorithm>

namespace opendspx {

    ZstdStreamBuf::ZstdStreamBuf(std::ostream &out, int compressionLevel)
        : m_mode(Mode::Compress), m_out(&out) {
        m_cstream = ZSTD_createCStream();
        if (!m_cstream) {
            setError("Failed to create zstd compression stream");
            return;
        }
        const auto initRet = ZSTD_initCStream(m_cstream, compressionLevel);
        if (ZSTD_isError(initRet)) {
            setError(ZSTD_getErrorName(initRet));
            return;
        }
        setp(m_inBuffer.data(), m_inBuffer.data() + m_inBuffer.size());
    }

    ZstdStreamBuf::ZstdStreamBuf(std::istream &in)
        : m_mode(Mode::Decompress), m_in(&in), m_decompressRet(1) {
        m_dstream = ZSTD_createDStream();
        if (!m_dstream) {
            setError("Failed to create zstd decompression stream");
            return;
        }
        const auto initRet = ZSTD_initDStream(m_dstream);
        if (ZSTD_isError(initRet)) {
            setError(ZSTD_getErrorName(initRet));
            return;
        }
        setg(m_outBuffer.data(), m_outBuffer.data(), m_outBuffer.data());
    }

    ZstdStreamBuf::~ZstdStreamBuf() {
        if (m_mode == Mode::Compress && m_cstream && !m_finished && !hasError()) {
            finish();
        }
        if (m_cstream) {
            ZSTD_freeCStream(m_cstream);
        }
        if (m_dstream) {
            ZSTD_freeDStream(m_dstream);
        }
    }

    bool ZstdStreamBuf::good() const {
        return !hasError();
    }

    bool ZstdStreamBuf::hasError() const {
        return !m_errorMessage.empty();
    }

    const std::string &ZstdStreamBuf::errorMessage() const {
        return m_errorMessage;
    }

    bool ZstdStreamBuf::finish() {
        if (m_mode == Mode::Decompress) {
            m_finished = true;
            return !hasError();
        }
        if (m_finished) {
            return !hasError();
        }
        if (!compressBuffered()) {
            return false;
        }
        if (!finishCompression()) {
            return false;
        }
        m_finished = true;
        return !hasError();
    }

    ZstdStreamBuf::int_type ZstdStreamBuf::underflow() {
        if (m_mode != Mode::Decompress || hasError() || !m_dstream) {
            return traits_type::eof();
        }
        if (gptr() < egptr()) {
            return traits_type::to_int_type(*gptr());
        }

        while (true) {
            if (m_zstdIn.pos >= m_zstdIn.size) {
                m_in->read(m_inBuffer.data(), static_cast<std::streamsize>(m_inBuffer.size()));
                const auto n = m_in->gcount();
                if (n <= 0) {
                    if (m_decompressRet != 0) {
                        setError("Unexpected end of zstd stream");
                    }
                    return traits_type::eof();
                }
                m_zstdIn.src = m_inBuffer.data();
                m_zstdIn.size = static_cast<size_t>(n);
                m_zstdIn.pos = 0;
            }

            ZSTD_outBuffer zstdOut{m_outBuffer.data(), m_outBuffer.size(), 0};
            const auto ret = ZSTD_decompressStream(m_dstream, &zstdOut, &m_zstdIn);
            if (ZSTD_isError(ret)) {
                setError(ZSTD_getErrorName(ret));
                return traits_type::eof();
            }
            m_decompressRet = ret;

            if (zstdOut.pos > 0) {
                setg(m_outBuffer.data(), m_outBuffer.data(), m_outBuffer.data() + static_cast<std::ptrdiff_t>(zstdOut.pos));
                return traits_type::to_int_type(*gptr());
            }

            if (m_decompressRet == 0 && m_zstdIn.pos >= m_zstdIn.size && m_in->peek() == traits_type::eof()) {
                return traits_type::eof();
            }
        }
    }

    ZstdStreamBuf::int_type ZstdStreamBuf::overflow(int_type ch) {
        if (m_mode != Mode::Compress || hasError() || !m_cstream) {
            return traits_type::eof();
        }
        if (traits_type::eq_int_type(ch, traits_type::eof())) {
            return sync() == 0 ? traits_type::not_eof(ch) : traits_type::eof();
        }
        if (pptr() == epptr() && !compressBuffered()) {
            return traits_type::eof();
        }
        *pptr() = traits_type::to_char_type(ch);
        pbump(1);
        return ch;
    }

    std::streamsize ZstdStreamBuf::xsputn(const char *s, std::streamsize count) {
        if (m_mode != Mode::Compress || hasError() || !m_cstream || count <= 0) {
            return 0;
        }

        std::streamsize totalWritten = 0;
        while (totalWritten < count) {
            if (pptr() == epptr() && !compressBuffered()) {
                break;
            }

            const auto writable = static_cast<std::streamsize>(epptr() - pptr());
            const auto chunk = std::min(writable, count - totalWritten);
            std::copy_n(s + totalWritten, chunk, pptr());
            pbump(static_cast<int>(chunk));
            totalWritten += chunk;
        }
        return totalWritten;
    }

    int ZstdStreamBuf::sync() {
        if (m_mode != Mode::Compress || hasError() || !m_cstream) {
            return hasError() ? -1 : 0;
        }
        if (!compressBuffered()) {
            return -1;
        }
        m_out->flush();
        return m_out->good() ? 0 : -1;
    }

    bool ZstdStreamBuf::compressBuffered() {
        if (m_mode != Mode::Compress || hasError() || !m_cstream) {
            return false;
        }

        ZSTD_inBuffer zstdIn{pbase(), static_cast<size_t>(pptr() - pbase()), 0};
        while (zstdIn.pos < zstdIn.size) {
            ZSTD_outBuffer zstdOut{m_outBuffer.data(), m_outBuffer.size(), 0};
            const auto ret = ZSTD_compressStream2(m_cstream, &zstdOut, &zstdIn, ZSTD_e_continue);
            if (ZSTD_isError(ret)) {
                return setError(ZSTD_getErrorName(ret));
            }
            if (zstdOut.pos > 0) {
                m_out->write(m_outBuffer.data(), static_cast<std::streamsize>(zstdOut.pos));
                if (!m_out->good()) {
                    return setError("Failed to write compressed output");
                }
            }
        }

        setp(m_inBuffer.data(), m_inBuffer.data() + m_inBuffer.size());
        return true;
    }

    bool ZstdStreamBuf::finishCompression() {
        while (true) {
            ZSTD_inBuffer zstdIn{nullptr, 0, 0};
            ZSTD_outBuffer zstdOut{m_outBuffer.data(), m_outBuffer.size(), 0};
            const auto ret = ZSTD_compressStream2(m_cstream, &zstdOut, &zstdIn, ZSTD_e_end);
            if (ZSTD_isError(ret)) {
                return setError(ZSTD_getErrorName(ret));
            }
            if (zstdOut.pos > 0) {
                m_out->write(m_outBuffer.data(), static_cast<std::streamsize>(zstdOut.pos));
                if (!m_out->good()) {
                    return setError("Failed to write compressed output");
                }
            }
            if (ret == 0) {
                break;
            }
        }
        m_out->flush();
        if (!m_out->good()) {
            return setError("Failed to flush compressed output");
        }
        return true;
    }

    bool ZstdStreamBuf::setError(std::string message) {
        if (m_errorMessage.empty()) {
            m_errorMessage = std::move(message);
        }
        return false;
    }

}