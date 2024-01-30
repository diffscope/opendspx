#include "converters/midi.h"

#include <QFile>

#include <QMidiFile.h>

namespace QDspx {

    ReturnCode MidiConverter::load(const QString &path, Model *out, const QVariantMap &args) {
        QMidiFile midi;
        if (!midi.load(path)) {
            return {ReturnCode::File, QFileDevice::ReadError};
        }

        // Convert

        return ReturnCode::Success;
    }

    ReturnCode MidiConverter::save(const QString &path, const Model &in, const QVariantMap &args) {
        QMidiFile midi;
        midi.setFileFormat(1);
        midi.setDivisionType(QMidiFile::DivisionType::PPQ);
        midi.setResolution(480);

        // Convert

        if (!midi.save(path)) {
            return {ReturnCode::File, QFileDevice::WriteError};
        }
        return ReturnCode::Success;
    }

}