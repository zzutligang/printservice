/**
  @file
  @author
*/

#include <QDateTime>
//#include "../global.h"
#include "printersetupcontroller.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>

#include <QPrinterInfo>

#include "../retjson.h"
#include "../iniutil.h"

PrinterSetupController::PrinterSetupController()
{}

void PrinterSetupController::service(HttpRequest& request, HttpResponse& response)
{
    RetJson ret = RetJson::ok();
    QByteArray action = request.getParameter("action");
    QByteArray printer = request.getParameter("printer"); //如果是set，printer参数必须有效的打印机名称，并且不能为null
    QByteArray printerIndex = request.getParameter("printerIndex"); //如果是set，可以指定有效打印机列表的序号，下标从0开始
    if(action.isNull() || action.isEmpty()){
        ret = RetJson::fail().setMsg("缺少action参数");
    }else{
        action = action.toLower();
        //
        if(PrinterSetupController::GET == QString(action)){
            QStringList printers = QPrinterInfo::availablePrinterNames();
            QString defaultPrinter = QPrinterInfo::defaultPrinterName();
            QJsonArray array = QJsonArray::fromStringList(printers); //QStringList转QJsonArray
            ret.set("availablePrinterNames", array);
            ret.set("systemDefaultPrinter", defaultPrinter);
            //读配置文件，获取用户自定义打印机
            QString configFileName=IniUtil::searchConfigFile();
            if(configFileName!=nullptr){
                QSettings* printerSettings=new QSettings(configFileName,QSettings::IniFormat, nullptr);
                printerSettings->beginGroup("printer");
                QString userPrinter = printerSettings->value("default", "").toString();
                delete printerSettings;
                ret.set("userPrinter", userPrinter);
            }else{
                ret.set("userPrinter", QString(""));
            }
        }else if(PrinterSetupController::SET == QString(action)){
            QStringList printers = QPrinterInfo::availablePrinterNames(); //获取所有打印机名称列表
            if(!printer.isNull() && !printer.isEmpty() && printers.contains(printer)){
                //读配置文件，获取用户自定义打印机
                QString configFileName=IniUtil::searchConfigFile();
                if(configFileName!=nullptr){
                    QSettings* printerSettings=new QSettings(configFileName,QSettings::IniFormat, nullptr);
                    printerSettings->beginGroup("printer");
                    printerSettings->setValue("default", QString(printer));
                    delete printerSettings;
                }else{
                    ret = RetJson::fail().setMsg("没有找到配置文件，无法写入");
                }
            }else if(!printerIndex.isNull() && !printerIndex.isEmpty() && printerIndex.toInt()>=0 && printerIndex.toInt()<printers.size()){
                //读配置文件，获取用户自定义打印机
                QString configFileName=IniUtil::searchConfigFile();
                if(configFileName!=nullptr){
                    QSettings* printerSettings=new QSettings(configFileName,QSettings::IniFormat, nullptr);
                    printerSettings->beginGroup("printer");
                    printerSettings->setValue("default", printers.value(printerIndex.toInt()));
                    delete printerSettings;
                }else{
                    ret = RetJson::fail().setMsg("没有找到配置文件，无法写入");
                }
            }else{
                ret = RetJson::fail().setMsg("printer或printerIndex参数无效，必须指定有效的打印机名称或则打印机列表索引");
            }
        }else{
            ret = RetJson::fail().setMsg("不支持的action参数");
        }
    }

    //写响应数据
    response.setHeader("Access-Control-Allow-Origin", "*");
    response.setHeader("Access-Control-Allow-Headers", "DNT,X-Mx-ReqToken,Keep-Alive,User-Agent,X-Requested-With,If-Modified-Since,Cache-Control,Content-Type,Authorization");
    response.setHeader("Content-Type", "application/json; charset=UTF-8");
    QJsonDocument jsonDoc(ret);
    QByteArray body = jsonDoc.toJson();
    response.write(body,true);

}
