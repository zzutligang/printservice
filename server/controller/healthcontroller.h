/**
  @file
  @author
*/

#ifndef HEALTHCONTROLLER_H
#define HEALTHCONTROLLER_H

#include "httprequest.h"
#include "httpresponse.h"
#include "httprequesthandler.h"

using namespace stefanfrings;

/**
  This controller demonstrates how to use HTTP basic login.
*/

class HealthController : public HttpRequestHandler {
    Q_OBJECT
    Q_DISABLE_COPY(HealthController)
public:

    /** Constructor */
    HealthController();

    /** Generates the response */
    void service(HttpRequest& request, HttpResponse& response);

};

#endif // HEALTHCONTROLLER_H
