#include "qdspxparam.h"

namespace QDspx {

    QAS::JsonStream &operator>>(QAS::JsonStream &_stream, ParamCurveRef &_var) {
        QJsonObject _obj;
        if (!QAS::JsonStreamUtils::parseAsObject(_stream, typeid(_var).name(), &_obj).good()) {
            return _stream;
        }

        ParamCurve _tmpVar{};
        QAS::JsonStream _tmpStream;
        if (!(_tmpStream = QAS::JsonStreamUtils::parseObjectMember(
                  _obj, "type", typeid(_tmpVar).name(), &_tmpVar.type))
                 .good()) {
            _stream.setStatus(_tmpStream.status());
            return _stream;
        }

        switch (_tmpVar.type) {
            case ParamCurve::Anchor: {
                auto _realVar = ParamAnchorRef::create();
                _var = _realVar;
                return _stream >> *_realVar.data();
            }
            case ParamCurve::Free: {
                auto _realVar = ParamFreeRef::create();
                _var = _realVar;
                return _stream >> *_realVar.data();
            }
        }

        return _stream;
    }

    QAS::JsonStream &operator<<(QAS::JsonStream &_stream, const ParamCurveRef &_var) {
        if (_var.isNull()) {
            return _stream;
        }

        return _var->type == ParamCurve::Anchor ? (_stream << *_var.dynamicCast<ParamAnchor>())
                                                : (_stream << *_var.dynamicCast<ParamFree>());
    }

}