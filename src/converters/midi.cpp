#include "converters/midi.h"

#include <functional>

#include <QFile>

#include <QMidiFile.h>

namespace QDspx {

    // Use `https://github.com/waddlesplash/QMidi` to read and write MIDI file.

    ReturnCode MidiConverter::load(const QString &path, Model *out, const QVariantMap &args) {
        QMidiFile midi;
        if (!midi.load(path)) {
            return {ReturnCode::File, QFileDevice::ReadError}; // Ignore concrete error type
        }

        // Convert
        // ...

        // Select
        {
            using Selector = std::function<bool(const QList<TrackInfo> &, const QList<QByteArray> &,
                                                QList<int> *, QTextCodec **)>;
            auto &selector = *reinterpret_cast<Selector *>(
                args.value(QStringLiteral("selector")).value<quintptr>());

            // Info
            QList<TrackInfo> trackInfoList;
            QList<QByteArray> labelList;

            // Result
            QList<int> selectIDs;
            QTextCodec *codec;

            if (!selector(trackInfoList, labelList, &selectIDs, &codec)) {
                return ReturnCode::Aborted;
            }
        }

        return ReturnCode::Success;
    }

    ReturnCode MidiConverter::save(const QString &path, const Model &in, const QVariantMap &args) {
        QMidiFile midi;
        midi.setFileFormat(1);
        midi.setDivisionType(QMidiFile::DivisionType::PPQ);
        midi.setResolution(480);

        // Convert
        // ...

        // Overlapped ?
        if (false) {
            using OverlapHandler = std::function<bool()>;

            auto &overlapHandler = *reinterpret_cast<OverlapHandler *>(
                args.value(QStringLiteral("overlapHandler")).value<quintptr>());
            if (!overlapHandler()) {
                return ReturnCode::Aborted;
            }
        }

        if (!midi.save(path)) {
            return {ReturnCode::File, QFileDevice::WriteError}; // Ignore concrete error type
        }

        return ReturnCode::Success;
    }

}