/**
  @file
  @author
*/

#ifndef PRINTPZCONTROLLER_H
#define PRINTPZCONTROLLER_H

#include "httprequest.h"
#include "httpresponse.h"
#include "httprequesthandler.h"

using namespace stefanfrings;

/**
  This controller demonstrates how to use HTTP basic login.
*/

class PrintPZController : public HttpRequestHandler {
    Q_OBJECT
    Q_DISABLE_COPY(PrintPZController)
public:

    /** Constructor */
    PrintPZController();

    /** Generates the response */
    void service(HttpRequest& request, HttpResponse& response);

    /** 请求网络获取待打印凭证pdf文件流 */
    QByteArray requestPZData(QByteArray url, QByteArray authorization);

};

#endif // PRINTPZCONTROLLER_H
