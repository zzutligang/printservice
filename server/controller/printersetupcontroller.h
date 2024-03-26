/**
  @file
  @author
*/

#ifndef PRINTERSETUPCONTROLLER_H
#define PRINTERSETUPCONTROLLER_H

#include "httprequest.h"
#include "httpresponse.h"
#include "httprequesthandler.h"

using namespace stefanfrings;

/**
  This controller demonstrates how to use HTTP basic login.
*/

class PrinterSetupController : public HttpRequestHandler {
    Q_OBJECT
    Q_DISABLE_COPY(PrinterSetupController)
public:

    /** Constructor */
    PrinterSetupController();

    /** Generates the response */
    void service(HttpRequest& request, HttpResponse& response);

    const QString GET="get";
    const QString SET="set";
};

#endif // PRINTERSETUPCONTROLLER_H
