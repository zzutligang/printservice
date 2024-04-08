/**
  @file
  @author
*/

#include <QDateTime>
//#include "../global.h"
#include "healthcontroller.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>

#include "../retjson.h"

HealthController::HealthController()
{}

void HealthController::service(HttpRequest& request, HttpResponse& response)
{
    RetJson ret = RetJson::ok();
    ret.set("version", request.getVersion());
    //写响应数据
    response.setHeader("Access-Control-Allow-Origin", "*");
    response.setHeader("Access-Control-Allow-Headers", "DNT,X-Mx-ReqToken,Keep-Alive,User-Agent,X-Requested-With,If-Modified-Since,Cache-Control,Content-Type,Authorization");
    response.setHeader("Content-Type", "application/json; charset=UTF-8");
    QJsonDocument jsonDoc(ret);
    QByteArray body = jsonDoc.toJson();
    response.write(body,true);

}
