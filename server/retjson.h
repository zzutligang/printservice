#ifndef RETJSON_H
#define RETJSON_H

#include <QJsonObject>

class RetJson : public QJsonObject
{
public:
    RetJson();
    static RetJson ok();
    static RetJson fail();
    RetJson setMsg(const QString msg);
    RetJson set(const QString key, const QJsonValue value);
    RetJson set(const QString key, const QString value);
};

#endif // RETJSON_H
