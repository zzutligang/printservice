// Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
// SPDX-License-Identifier: BSD-3-Clause

#include <QApplication>
#include <QTextStream>
#include <QDateTime>
#include <QStringList>
#include <QDir>
#include <QSettings>
#include "global.h"
#include "httplistener.h"
#include "requestmapper.h"
#include "qtservice.h"

// HttpDaemon is the the class that implements the simple HTTP server.
class HttpDaemon : public QObject
{
    Q_OBJECT
public:
    HttpDaemon(quint16 port, QApplication* parent = 0)
    {
        disabled = false;
        defaultPort = port;
        app = parent;
    }

    void pause()
    {
        disabled = true;
    }

    void resume()
    {
        disabled = false;
    }

    bool start()
    {
        // Find the configuration file
        QString configFileName=searchConfigFile();

        // Configure logging into a file
        QSettings* logSettings=new QSettings(configFileName,QSettings::IniFormat,app);
        logSettings->beginGroup("logging");
        FileLogger* logger=new FileLogger(logSettings,10000,app);
        logger->installMsgHandler();

        // Configure template loader and cache
        QSettings* templateSettings=new QSettings(configFileName,QSettings::IniFormat,app);
        templateSettings->beginGroup("templates");
        templateCache=new TemplateCache(templateSettings,app);

        // Configure session store
        QSettings* sessionSettings=new QSettings(configFileName,QSettings::IniFormat,app);
        sessionSettings->beginGroup("sessions");
        sessionStore=new HttpSessionStore(sessionSettings,app);

        // Configure static file controller
        QSettings* fileSettings=new QSettings(configFileName,QSettings::IniFormat,app);
        fileSettings->beginGroup("docroot");
        staticFileController=new StaticFileController(fileSettings,app);

        // Configure and start the TCP listener
        QSettings* listenerSettings=new QSettings(configFileName,QSettings::IniFormat,app);
        listenerSettings->beginGroup("listener");
        //listenerSettings->setValue()
        new HttpListener(listenerSettings,new RequestMapper(app),app);
        return true;
    }

    /** Search the configuration file */
    QString searchConfigFile()
    {
        QString binDir=QApplication::applicationDirPath();
        QString appName=QApplication::applicationName();
        QString fileName("http.ini");

        QStringList searchList;
        searchList.append(binDir);
        searchList.append(binDir+"/etc");
        searchList.append(binDir+"/../etc");
        searchList.append(binDir+"/../"+appName+"/etc");     // for development with shadow build (Linux)
        searchList.append(binDir+"/../../"+appName+"/etc");  // for development with shadow build (Windows)
        searchList.append(QDir::rootPath()+"etc/opt");
        searchList.append(QDir::rootPath()+"etc");

        foreach (QString dir, searchList)
        {
            QFile file(dir+"/"+fileName);
            if (file.exists())
            {
                fileName=QDir(file.fileName()).canonicalPath();
                qDebug("Using config file %s",qPrintable(fileName));
                return fileName;
            }
        }

        // not found
        foreach (QString dir, searchList)
        {
            qWarning("%s/%s not found",qPrintable(dir),qPrintable(fileName));
        }
        qFatal("Cannot find config file %s",qPrintable(fileName));
        return nullptr;
    }

private slots:


private:
    bool disabled;
    quint16 defaultPort = 0;
    QApplication *app;
};

class HttpService : public QtService<QApplication>
{
public:
    HttpService(int argc, char **argv)
    : QtService<QApplication>(argc, argv, "Print Daemon")
    {
        setServiceDescription("皖域智慧财务凭证打印服务(Created by QT6.6.2)");
        setServiceFlags(QtServiceBase::CanBeSuspended);
    }

protected:
    void start()
    {
        QApplication *app = application();

#if QT_VERSION < 0x040100
        quint16 port = (app->argc() > 1) ?
                QString::fromLocal8Bit(app->argv()[1]).toUShort() : 8080;
#else
        const QStringList arguments = QApplication::arguments();
        quint16 port = (arguments.size() > 1) ? arguments.at(1).toUShort() : 8080;
#endif
        //这里写启动http的代码
        daemon = new HttpDaemon(port, app);
        if(!daemon->start()){
            logMessage(QString("Failed to start service."), QtServiceBase::Error);
            app->quit();
        }
    }

    void pause()
    {
        daemon->pause();
    }

    void resume()
    {
        daemon->resume();
    }

private:
    HttpDaemon *daemon;
};

#include "main.moc"

int main(int argc, char **argv)
{
#if !defined(Q_OS_WIN)
    // QtService stores service settings in SystemScope, which normally require root privileges.
    // To allow testing this example as non-root, we change the directory of the SystemScope settings file.
    QSettings::setPath(QSettings::NativeFormat, QSettings::SystemScope, QDir::tempPath());
    qWarning("(Example uses dummy settings file: %s/QtSoftware.conf)", QDir::tempPath().toLatin1().constData());
#endif
    HttpService service(argc, argv);
    return service.exec();
}
