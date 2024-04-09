/**
  @file
  @author
*/

#include <QDateTime>
//#include "../global.h"
#include "printpzcontroller.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>

#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

#include <QPrinter>
#include <QPdfDocument>
#include <QMatrix2x2>
#include <QPainter>
#include <QBuffer>
#include <QPrintDialog>
#include <QPrinterInfo>

#include "../retjson.h"
#include "../iniutil.h"

PrintPZController::PrintPZController()
{}

void PrintPZController::service(HttpRequest& request, HttpResponse& response)
{
    if(request.getMethod()=="OPTIONS"){
        response.setStatus(200,"OK");
        response.setHeader("Access-Control-Allow-Origin", "*");
        response.setHeader("Access-Control-Allow-Headers", "DNT,X-Mx-ReqToken,Keep-Alive,User-Agent,X-Requested-With,If-Modified-Since,Cache-Control,Content-Type,Authorization");
        response.setHeader("Access-Control-Allow-Methods", "*");
        return;
    }

    QByteArray auth = request.getHeader("Authorization");
    if (auth.isNull())
    {
        qCritical("User is not logged in");
        response.setStatus(401,"Unauthorized");
        //response.setHeader("Access-Control-Allow-Origin", "*");
        //response.setHeader("Access-Control-Allow-Headers", "DNT,X-Mx-ReqToken,Keep-Alive,User-Agent,X-Requested-With,If-Modified-Since,Cache-Control,Content-Type,Authorization");
        //response.setHeader("Content-Type", "application/json; charset=UTF-8");
        //response.setHeader("WWW-Authenticate","Basic realm=Please login with any name and password");
    }
    else
    {
        qDebug("Authorization: %s",qPrintable(auth));
        RetJson ret = RetJson::ok();
        QByteArray url = request.getParameter("URL");
        QByteArray data = requestPZData(url, auth);
        if(!data.isNull() && !data.isEmpty()){ //如果加载到数据，就解析pdf
            qDebug() << "数据长度：" << data.length();
            QPdfDocument pdf;
            QBuffer buffer(&data);
            buffer.open(QIODevice::ReadOnly);
            pdf.load(&buffer);
            buffer.close();
            qDebug() << "加载进QPdfDocument完毕";
            int pageCount = pdf.pageCount();
            qDebug() << "pdf文件总页数：" << pageCount;
            //准备打印机信息
            //取用户指定的打印机
            QString defaultPrinter = QPrinterInfo::defaultPrinterName(); //系统默认打印机
            QString configFileName = IniUtil::searchConfigFile(); //注意：这里可能会因为没找到配置文件而返回nullptr
            if(configFileName!=nullptr){
                QSettings* printerSettings=new QSettings(configFileName,QSettings::IniFormat, nullptr);
                printerSettings->beginGroup("printer");
                defaultPrinter = printerSettings->value("default", defaultPrinter).toString();
                delete printerSettings;
            }
            QPrinterInfo printerInfo = QPrinterInfo::printerInfo(defaultPrinter);
            //QPrinterInfo printerInfo = QPrinterInfo::defaultPrinter();
            QPrinter printer(printerInfo, QPrinter::HighResolution);
            qDebug() << "打印机名称：" << printer.printerName();
            //设置纸张A4
            printer.setPageSize(QPageSize::A4);
            QPainter painter;
            if(painter.begin(&printer)){
                qDebug() << "打开打印机成功";
                const QRect printerPageRect = printer.pageRect(QPrinter::Unit::DevicePixel).toRect();
                qDebug() << "打印机printerPageRect=" << printerPageRect;
                const QSize printerPageSize = printerPageRect.size();
                qDebug() << "打印机printerPageSize=" << printerPageSize;

                for (int i = 0; i < pdf.pageCount(); ++i) {
                    QPdfDocumentRenderOptions op;
                    const QSize pdfPageSize = pdf.pagePointSize(i).toSize();
                    qDebug() << "pdf第" << i << "页，宽：" << pdfPageSize.width() << "，高：" << pdfPageSize.height();
                    if(pdfPageSize.width() > pdfPageSize.height()){ //表示pdf文件是横向
                        if(printerPageSize.width() < printerPageSize.height()){ //表示打印机纵向
                            op.setRotation(QPdfDocumentRenderOptions::Rotation::Clockwise90); //就把pdf旋转90度
                        }
                    }else if(pdfPageSize.width() < pdfPageSize.height()){ //表示pdf是纵向
                        if(printerPageSize.width() > printerPageSize.height()){ //表示打印机横向
                            op.setRotation(QPdfDocumentRenderOptions::Rotation::Clockwise270); //就把pdf旋转270度
                        }
                    }
                    //渲染pdf页面
                    const QImage &page = pdf.render(i, printerPageSize, op);
                    if (i > 0)
                        printer.newPage();
                    painter.drawImage(printerPageRect, page);
                }
                painter.end();
            }else{
                qDebug() << "无法打开打印机:" << defaultPrinter;
                ret = RetJson::fail().setMsg("无法打开打印机");
            }
        }else{
            qDebug() << "没有加载到凭证pdf文件";
            ret = RetJson::fail().setMsg("没有加载到凭证pdf文件");
        }
        //写响应数据
        response.setHeader("Access-Control-Allow-Origin", "*");
        response.setHeader("Access-Control-Allow-Headers", "DNT,X-Mx-ReqToken,Keep-Alive,User-Agent,X-Requested-With,If-Modified-Since,Cache-Control,Content-Type,Authorization");
        response.setHeader("Content-Type", "application/json; charset=UTF-8");
        QJsonDocument jsonDoc(ret);
        QByteArray body = jsonDoc.toJson();
        response.write(body,true);
    } //if (auth.isNull())
}

QByteArray PrintPZController::requestPZData(QByteArray url, QByteArray authorization)
{
    QNetworkAccessManager manager;
    // 设置请求的URL
    QUrl URL(url);
    // 创建QNetworkRequest实例
    QNetworkRequest request(URL);
    // 添加自定义头信息
    request.setRawHeader("Authorization", authorization);
    // 发送GET请求
    QNetworkReply *reply = manager.get(request);
    // 等待网络请求完成
    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();
    QByteArray data;
    if (reply->error() == QNetworkReply::NoError) {
        qDebug("Network request ok.");
        data = reply->readAll();
        //最后删除网络请求过来的数据，释放内存
        reply->deleteLater();
    }else{
        qWarning("Network error: %s", qPrintable(reply->errorString()));
    }
    return data;
}
