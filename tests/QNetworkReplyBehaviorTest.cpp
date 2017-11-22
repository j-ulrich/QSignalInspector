/*! \file
 * \date Created on: 22.11.2017
 * \author Jochen Ulrich <jochenulrich@t-online.de>
 */

#include "QSignalInspector.h"

#include <QTextStream>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QCoreApplication>
#include <QSharedPointer>

#include <cstdio>

QSharedPointer<QSignalInspector> executeRequest(const QNetworkRequest& req, QNetworkAccessManager& qNam)
{
	QSharedPointer<QNetworkReply> reply{qNam.get(req)};
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
			parameterValues << parameter.toString();
		out << signalEmission.first.name() << "(" << parameterValues.join(", ") << ")" << endl;
	}
}

void executeAndPrint(const QString& url, QTextStream& out, QNetworkAccessManager& qNam)
{
	out << url << endl;
	out << QString{url.size(), '-'} << endl;

	QNetworkRequest req{QUrl{url}};
	printSignals(out, executeRequest(req, qNam));

	out << endl;
}

int main(int argc, char **argv)
{
	QCoreApplication app{argc, argv};
	QTextStream out{stdout};
	QNetworkAccessManager qNam;

	executeAndPrint("http://eu.httpbin.org/status/200", out, qNam);
	executeAndPrint("http://eu.httpbin.org/status/500", out, qNam);
	executeAndPrint("http://eu.httpbin.org/get", out, qNam);
}
