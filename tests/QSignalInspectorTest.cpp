/*! \file
 * \date Created on: 22.11.2017
 * \author Jochen Ulrich <jochenulrich@t-online.de>
 */

#include "QSignalInspector.h"

#include <QObject>
#include <QtTest>

class TestSignalEmitter : public QObject
{
	Q_OBJECT

signals:
	void stringSignal(const QString& string);
	void complexSignal(int integer, const QString& string, const QJsonDocument& json);
};

class QSignalInspectorTest : public QObject
{
	Q_OBJECT

private:
	void testSignalRecording();
};

void QSignalInspectorTest::testSignalRecording()
{
	TestSignalEmitter emitter;

	QSignalInspector inspector(&emitter);

	QCOMPARE(inspector.size(), 0);

	QString testString("foo bar");
	emit emitter.stringSignal(testString);

	QTRY_COMPARE(inspector.size(), 1);
	QCOMPARE(inspector.at(0).first, QMetaMethod::fromSignal(&TestSignalEmitter::stringSignal));
	QCOMPARE(inspector.at(0).second, QVariantList() << QVariant::fromValue(testString));

	int testInt = 17;
	QJsonDocument testJson = QJsonDocument::fromJson("{ \"foo\": \"bar\" }");
	emit emitter.complexSignal(testInt, testString, testJson);

	QTRY_COMPARE(inspector.size(), 2);
	QCOMPARE(inspector.at(1).first, QMetaMethod::fromSignal(&TestSignalEmitter::complexSignal));
	QCOMPARE(inspector.at(1).second, QVariantList() << QVariant::fromValue(testInt) << QVariant::fromValue(testString) << QVariant::fromValue(testJson));
}

QTEST_MAIN(QSignalInspectorTest)
#include "QSignalInspectorTest.moc"
