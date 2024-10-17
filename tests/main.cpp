#include <iostream>

#include <QTextCodec>
#include <opendspx/qdspxbase.h>
#include <opendspx/converters/midi.h>


static bool trackSelector(const QList<QDspx::MidiConverter::TrackInfo>& trackInfoList,
                          const QList<QByteArray>& labelList, QList<int>* selectIDs, QTextCodec** codec) {
    *codec = QTextCodec::codecForName("UTF-8");

    selectIDs->clear();
    for (int i = 0; i < trackInfoList.size(); ++i)
        selectIDs->append(i);
    return true;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <midi_file_path>" << std::endl;
        return 1;
    }

    const char* midiFilePath = argv[1];
    std::cout << "MIDI File Path: " << midiFilePath << std::endl;

    const auto dspx = new QDspx::Model;
    std::function<bool(const QList<QDspx::MidiConverter::TrackInfo>&, const QList<QByteArray>&,
                       QList<int>*, QTextCodec**)>
        midiSelector = trackSelector;
    QVariantMap args = {};
    args.insert(QStringLiteral("selector"),
                QVariant::fromValue(reinterpret_cast<quintptr>(&midiSelector)));

    const auto midi = new QDspx::MidiConverter();
    const auto returnCode = midi->load(midiFilePath, dspx, args);

    std::cout << "returnCode: " << returnCode.code << " type: " << returnCode.type << std::endl;

    std::cout << "timeSignatures: " << dspx->content.timeline.timeSignatures[0].num << "/" << dspx->content.timeline.
        timeSignatures[0].den << std::endl;

    std::cout << "tempo: " << dspx->content.timeline.tempos[0].value << std::endl;

    return 0;
}
