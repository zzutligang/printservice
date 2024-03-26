#include "retjson.h"

RetJson::RetJson() {}

RetJson RetJson::ok()
{
    RetJson ret;
    ret.insert("rtnCode",0);
    ret.insert("rtnMsg","操作成功");
    return ret;
}

RetJson RetJson::fail()
{
    RetJson ret;
    ret.insert("rtnCode",-1);
    ret.insert("rtnMsg","操作失败");
    return ret;
}

RetJson RetJson::setMsg(const QString msg)
{
    this->insert("rtnMsg", msg);
    return *this;
}

RetJson RetJson::set(const QString key, const QJsonValue value)
{
    this->insert(key, value);
    return *this;
}

RetJson RetJson::set(const QString key, const QString value)
{
    this->insert(key, value);
    return *this;
}
