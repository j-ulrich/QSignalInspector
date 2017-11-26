/*! \file
 * \date Created on: 22.11.2017
 * \author Jochen Ulrich <jochenulrich@t-online.de>
 */

#include "QSignalInspector.h"

#include <QCoreApplication>
#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSharedPointer>
#include <QTextStream>
#include <QUrl>

#include <cstdio>
#include <iostream>

QSharedPointer<QSignalInspector> executeRequest(QNetworkAccessManager::Operation op,
                                                const QNetworkRequest& req,
                                                QNetworkAccessManager& qNam,
                                                const QByteArray& data = QByteArray())
{
	QSharedPointer<QNetworkReply> reply;
	switch (op)
	{
	case QNetworkAccessManager::GetOperation: reply.reset(qNam.get(req)); break;
	case QNetworkAccessManager::PostOperation: reply.reset(qNam.post(req, data)); break;
	}
	auto inspector = QSharedPointer<QSignalInspector>::create(reply.data());

	QEventLoop eventLoop;
	QObject::connect(reply.data(), &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
	eventLoop.exec();

	return inspector;
}

void printSignals(QTextStream& out, QSharedPointer<QSignalInspector> inspector)
{
	for (auto signalEmission : *inspector)
	{
		QStringList parameterValues;
		for (auto parameter : signalEmission.second)
		{
			if (parameter.canConvert<QString>())
				parameterValues << parameter.toString();
			else
				parameterValues << QString::fromUtf8(parameter.typeName());
		}
		out << signalEmission.first.name() << "(" << parameterValues.join(", ") << ")" << endl;
	}
}

void executeAndPrint(QNetworkAccessManager::Operation op,
                  const QString& url,
                  QTextStream& out,
                  QNetworkAccessManager& qNam,
                  const QByteArray& body = QByteArray(),
                  const QByteArray& contentType = QByteArray())
{
	QString verb;
	switch (op)
	{
	case QNetworkAccessManager::GetOperation: verb = "GET"; break;
	case QNetworkAccessManager::PostOperation: verb = "POST"; break;
	}

	out << verb << " " << url << endl;
	out << QString{verb.size() + 1 + url.size(), '-'} << endl;

	QNetworkRequest req{QUrl{url}};
	if (!contentType.isEmpty())
		req.setHeader(QNetworkRequest::ContentTypeHeader, contentType);
	printSignals(out, executeRequest(op, req, qNam, body));

	out << endl;
}


int main(int argc, char **argv)
{
	QCoreApplication app{argc, argv};
	QTextStream out{stdout};
	QNetworkAccessManager qNam;

	executeAndPrint(QNetworkAccessManager::GetOperation, "http://eu.httpbin.org/status/200", out, qNam);
	executeAndPrint(QNetworkAccessManager::GetOperation, "http://eu.httpbin.org/status/500", out, qNam);
	executeAndPrint(QNetworkAccessManager::GetOperation, "http://eu.httpbin.org/get", out, qNam);
	executeAndPrint(QNetworkAccessManager::PostOperation, "http://eu.httpbin.org/post", out, qNam,
	                "{ \"random\": \"gLINDuERfaRBQesd6FuF2DJ0vMaVvHDlCPm9ib2DnIIrgSwVSYQdlb5RvUiK89EZIJMeCQOk1iRRDYSiCN8sdpvehD9JP1eKWxfTzZtLLUlAEVVKBqmz4oheyJYcLpWxNnxPlAUHmJ3I09ZYli99sF9wM1Amhjnj4vKwWuWSJRtsQZrJdMDdWteUjnxQQy4SyTzVoXlFxljLu96WAc7kOPREd9brDzyfVGxOJ79cnwO8M3wBLExAwVX4fdLf9qbPz0UsErefH9t1Sy2hVeGFKzeewRLN97djADR\" }",
	                "application/json");

	return 0;
}
