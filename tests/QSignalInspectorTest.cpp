/*! \file
 * \date Created on: 22.11.2017
 * \author Jochen Ulrich <jochenulrich@t-online.de>
 */

#include "QSignalInspector.h"

#include <QObject>
#include <QtTest>

/*! Contains the unit tests for the QSignalInspector library.
 */
namespace Tests
{

/*! Helper class for emitting signals in the QSignalInspectorTest.
 */
class TestSignalEmitter : public QObject
{
	Q_OBJECT

signals:
	void stringSignal(const QString& string);
	void complexSignal(int integer, const QString& string, const QJsonDocument& json);
};

/*! Implements unit tests for the QSignalInspector class.
 */
class QSignalInspectorTest : public QObject
{
	Q_OBJECT

private:
	void testSignalRecording();
	void testParentSignals();
};

/*! \test Tests the basic signal recording functionality.
 */
void QSignalInspectorTest::testSignalRecording()
{
	TestSignalEmitter emitter;

	QSignalInspector inspector(&emitter);

	QCOMPARE(inspector.size(), 0);

	QString testString("foo bar");
	QDateTime timeFrameBegin = QDateTime::currentDateTime();
	emit emitter.stringSignal(testString);
	QDateTime timeFrameEnd = QDateTime::currentDateTime();

	QTRY_COMPARE(inspector.size(), 1);
	QCOMPARE(inspector.at(0).signal, QMetaMethod::fromSignal(&TestSignalEmitter::stringSignal));
	QCOMPARE(inspector.at(0).parameters, QVariantList() << QVariant::fromValue(testString));
	QVERIFY(inspector.at(0).timestamp >= timeFrameBegin);
	QVERIFY(inspector.at(0).timestamp <= timeFrameEnd);

	int testInt = 17;
	QJsonDocument testJson = QJsonDocument::fromJson("{ \"foo\": \"bar\" }");
	timeFrameBegin = QDateTime::currentDateTime();
	emit emitter.complexSignal(testInt, testString, testJson);
	timeFrameEnd = QDateTime::currentDateTime();

	QTRY_COMPARE(inspector.size(), 2);
	QCOMPARE(inspector.at(1).signal, QMetaMethod::fromSignal(&TestSignalEmitter::complexSignal));
	QCOMPARE(inspector.at(1).parameters, QVariantList() << QVariant::fromValue(testInt) << QVariant::fromValue(testString) << QVariant::fromValue(testJson));
	QVERIFY(inspector.at(1).timestamp >= timeFrameBegin);
	QVERIFY(inspector.at(1).timestamp <= timeFrameEnd);

}

/*! \test Tests the behavior of the \p includeParentClassSignals parameter
 * of the QSignalInspector() constructor.
 */
void QSignalInspectorTest::testParentSignals()
{
	QScopedPointer<TestSignalEmitter> emitter(new TestSignalEmitter());

	QSignalInspector fullInspector(emitter.data(), true);
	QSignalInspector leafInspector(emitter.data(), false);

	QString testString("foo bar");
	emit emitter->stringSignal(testString);

	QTRY_COMPARE(fullInspector.size(), 1);
	QCOMPARE(leafInspector.size(), 1);

	emitter.reset();

	QTRY_COMPARE(fullInspector.size(), 2);
	QCOMPARE(fullInspector.at(1).signal, QMetaMethod::fromSignal(&QObject::destroyed));
	QCOMPARE(leafInspector.size(), 1);
}

} // namespace Tests

QTEST_MAIN(Tests::QSignalInspectorTest)
#include "QSignalInspectorTest.moc"
