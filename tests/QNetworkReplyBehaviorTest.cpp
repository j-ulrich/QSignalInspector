/*! \file
 * \date Created on: 22.11.2017
 * \author Jochen Ulrich <jochenulrich@t-online.de>
 */

#include "QSignalInspector.h"

#include <QtDebug>
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

QString escapeChar(QChar character)
{
	switch (character.unicode())
	{
	case '\0': return "\\0";
	case '\'': return "\\'";
	case '\"': return "\\\"";
	case '\\': return "\\\\";
	case '\a': return "\\a";
	case '\b': return "\\b";
	case '\f': return "\\f";
	case '\n': return "\\n";
	case '\r': return "\\r";
	case '\t': return "\\t";
	case '\v': return "\\v";
	default: break;
	}

	if (!character.isPrint())
	{
		return "\\x" + QString::number(character.unicode(), 16);
	}
	else
		return character;
}

QString escapeString(const QString& string)
{
	QString result;

	for (int i=0; i < string.size(); ++i)
	{
		QChar c = string[i];

		if (c.isHighSurrogate() || c.isLowSurrogate())
		{
			if (i < (string.size() - 1))
			{
				QString surrogatePair = QString(string[i]) + string[i+1];
				result += "\\u" + QString::number(surrogatePair.toUcs4().first(), 16).rightJustified(8, '0');
				i += 1;
			}
		}
		else
			result += escapeChar(c);
	}

	return result;
}



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

	qDebug() << "Reply from:" << reply->url() << endl;

	return inspector;
}

void printSignals(QTextStream& out, QSharedPointer<QSignalInspector> inspector)
{
	for (auto signalEmission : *inspector)
	{
		QStringList parameterValues;
		for (auto parameter : signalEmission.parameters)
		{
			QString valueStr;
			if (parameter.canConvert<QString>())
			{
				valueStr = parameter.toString();
				QMetaType::Type type = static_cast<QMetaType::Type>(parameter.type());
				if (   type == QMetaType::QString
				    || type == QMetaType::QByteArray)
				{
					valueStr = QStringLiteral("\"%1\"").arg(escapeString(valueStr));
				}
				else if (   type == QMetaType::QChar
				         || type == QMetaType::Char)
				{
					valueStr = QStringLiteral("'%1'").arg(escapeChar(valueStr.at(0)));
				}
			}
			else
				valueStr = QString::fromUtf8(parameter.typeName());
			parameterValues << valueStr;
		}
		out << "[" << signalEmission.timestamp.toString("yyyy-MM-ddTHH:mm:ss.zzz") << "] " << signalEmission.signal.name() << "(" << parameterValues.join(", ") << ")" << endl;
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
	req.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
	req.setMaximumRedirectsAllowed(4);
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
	executeAndPrint(QNetworkAccessManager::PostOperation, "http://eu.httpbin.org/status/500", out, qNam, "foo bar", "text/plain");
	executeAndPrint(QNetworkAccessManager::GetOperation, "http://eu.httpbin.org/get", out, qNam);
	executeAndPrint(QNetworkAccessManager::PostOperation, "http://eu.httpbin.org/post", out, qNam,
	                "{ \"random\": \"gLINDuERfaRBQesd6FuF2DJ0vMaVvHDlCPm9ib2DnIIrgSwVSYQdlb5RvUiK89EZIJMeCQOk1iRRDYSiCN8sdpvehD9JP1eKWxfTzZtLLUlAEVVKBqmz4oheyJYcLpWxNnxPlAUHmJ3I09ZYli99sF9wM1Amhjnj4vKwWuWSJRtsQZrJdMDdWteUjnxQQy4SyTzVoXlFxljLu96WAc7kOPREd9brDzyfVGxOJ79cnwO8M3wBLExAwVX4fdLf9qbPz0UsErefH9t1Sy2hVeGFKzeewRLN97djADR\" }",
	                "application/json");
	executeAndPrint(QNetworkAccessManager::GetOperation, "http://httpbin.org/redirect/3", out, qNam);
	executeAndPrint(QNetworkAccessManager::GetOperation, "http://httpbin.org/redirect-to?url=http%3A%2F%2Fhttpbin.org%2Fstatus%2F404", out, qNam);
	executeAndPrint(QNetworkAccessManager::GetOperation, "http://httpbin.org/delay/3", out, qNam);
	executeAndPrint(QNetworkAccessManager::GetOperation, "http://httpbin.org/drip?duration=5&code=500&numbytes=5", out, qNam);
	executeAndPrint(QNetworkAccessManager::GetOperation, "http://httpbin.org/stream/20", out, qNam);
	executeAndPrint(QNetworkAccessManager::GetOperation, "http://httpbin.org/redirect/5", out, qNam);

	return 0;
}
