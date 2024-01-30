#include "qdspxtrack.h"

namespace QDspx {

    QAS::JsonStream &operator>>(QAS::JsonStream &_stream, ClipRef &_var) {
        QJsonObject _obj;
        if (!QAS::JsonStreamUtils::parseAsObject(_stream, typeid(_var).name(), &_obj).good()) {
            return _stream;
        }

        Clip _tmpVar{};
        QAS::JsonStream _tmpStream;
        if (!(_tmpStream = QAS::JsonStreamUtils::parseObjectMember(
                  _obj, "type", typeid(_tmpVar).name(), &_tmpVar.type))
                 .good()) {
            _stream.setStatus(_tmpStream.status());
            return _stream;
        }

        switch (_tmpVar.type) {
            case Clip::Singing: {
                auto _realVar = SingingClipRef::create();
                _var = _realVar;
                return _stream >> *_realVar.data();
            }
            case Clip::Audio: {
                auto _realVar = AudioClipRef::create();
                _var = _realVar;
                return _stream >> *_realVar.data();
            }
        }

        return _stream;
    }

    QAS::JsonStream &operator<<(QAS::JsonStream &_stream, const ClipRef &_var) {
        if (_var.isNull()) {
            return _stream;
        }

        return _var->type == Clip::Singing ? (_stream << *_var.dynamicCast<SingingClip>())
                                           : (_stream << *_var.dynamicCast<AudioClip>());
    }

}