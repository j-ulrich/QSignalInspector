/*! \file
 *
 * QSignalInspector
 *
 * https://github.com/j-ulrich/QSignalInspector
 *
 * \author Jochen Ulrich <jochenulrich@t-online.de>
 * \copyright
 * \parblock
 * © 2017 Jochen Ulrich
 *
 * MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * \endparblock
 */

#include <QObject>
#include <QList>
#include <QSignalSpy>
#include <QMetaMethod>
#include <QSharedPointer>

class QSignalInspector : public QObject, public QList<QPair<QMetaMethod, QList<QVariant> > >
{
	Q_OBJECT

public:
	explicit QSignalInspector(const QObject* obj, bool includeBaseClassSignals = true)
		: QObject()
	{
		const QMetaObject* const metaObject = obj->metaObject();

		QMetaMethod signalEmittedSlot = staticMetaObject.method(staticMetaObject.indexOfSlot("signalEmitted()"));

		int methodIndex = includeBaseClassSignals? 0 : metaObject->methodOffset();
		for (; methodIndex < metaObject->methodCount(); ++methodIndex)
		{
			QMetaMethod metaMethod = metaObject->method(methodIndex);
			if (metaMethod.methodType() == QMetaMethod::Signal)
			{
				m_signalSpies.insert(methodIndex, QSharedPointer<QSignalSpy>(new QSignalSpy(obj, ("2"+metaMethod.methodSignature()).constData())));
				QObject::connect(obj, metaMethod, this, signalEmittedSlot);
			}
		}
	}

private Q_SLOTS:
	void signalEmitted()
	{
		QObject* sender = this->sender();
		const QMetaObject* metaObject = sender->metaObject();

		int signalIndex = this->senderSignalIndex();
		QSharedPointer<QSignalSpy> signalSpy;

		/* For overloaded signals, senderSignalIndex() does not necessarily
		 * return the correct index. Instead, it always returns the index of the
		 * signal with all parameters which seems to have lowest index of the overloads.
		 * So we check the signal spies of the following signals until we find the
		 * one which caught the signal emission.
		 */
		for (; signalIndex < metaObject->methodCount(); ++signalIndex)
		{
			signalSpy = m_signalSpies.value(signalIndex);
			if (!signalSpy)
			{
				qWarning("QSignalInspector: Unexpected signal emitted");
				return;
			}
			if (signalSpy->count() > 0)
				break;
		}

		QList<QVariant> signalParameters = signalSpy->at(0);
		this->append(qMakePair(metaObject->method(signalIndex), signalParameters));
		signalSpy->clear();
	}

private:
	QMap<int, QSharedPointer<QSignalSpy> > m_signalSpies;
};
