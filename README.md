# QSignalInspector #

[![Build Status](https://travis-ci.org/j-ulrich/QSignalInspector.svg?branch=master)](https://travis-ci.org/j-ulrich/QSignalInspector)

Implements spying on **all** signals of a QObject instance.

QSignalInspector is very similar to [QSignalSpy][] but it records the emission of all signals of a class. Internally, QSignalInspector uses one QSignalSpy for each of the signals.


## Example ##

```c++
#include "MyObject.h"
#include "QSignalInspector.h"
#include <QTextStream>
#include <QCoreApplication>
#include <cstdio>

int main(int argc, char **argv)
{
	QCoreApplication app(argc, argv);

	// Setup object to be inspected
	MyQObject object;
	QSignalInspector inspector(&object);

	QObject::connect(&object, SIGNAL(finished()), &app, SLOT(quit()));
	object.start();
	app.exec();

	// Print the recorded signals
	QTextStream out(stdout);
	QSignalInspector::ConstIterator iter;
	for (iter = inspector.begin(); iter != inspector.end(); ++iter)
	{
		QSignalEmissionEvent emission = *iter;
		QStringList parameters;
		QList<QVariant>::ConstIterator paramIter;

		for (paramIter = emission.parameters.begin(); paramIter != emission.parameters.end(); ++paramIter)
			parameters << paramIter->toString();

		out << emission.signal.name() << "(" << parameters.join(", ") << ")" << endl;
	}

	return 0;
}


```


## Usage ##

```c++
QSignalInspector(const QObject* object, bool includeParentClassSignals = true)
```

Creates a QSignalInspector recording the signal emissions of _object_.

**Parameters**<br/>
**object** - The object whose signals should be recorded.<br/>
**includeParentClassSignals** - If `true`, the signals of all parent classes of
_object_ are recorded as well. If `false`, only those signals are recorded that
are declared by the last class in the inheritance hierarchy of _object_.

Then use the `QSignalInspector` as a `QList<QSignalEmissionEvent>` to retrieve
the information about the emissions:

```c++
QSignalInspector mySignalInspector(&objectToBeWatched);

// Trigger emissions of signals ...

QSignalInspector::ConstIterator iter;
for (iter = mySignalInspector.constBegin(); iter != mySignalInspector.constEnd(); ++iter)
{
	const QSignalEmissionEvent event = *iter;
	// Do something with event ...
}
```

Where `QSignalEmissionEvent` is a simple `struct`:

```c++
/*! Struct representing one emission of a signal.
 */
struct QSignalEmissionEvent
{
	QMetaMethod signal;         //!< The QMetaMethod of the signal that was emitted.
	QDateTime timestamp;        //!< The time when the signal was emitted.
	QList<QVariant> parameters; //!< The parameter values of the emission.
};
```


## Requirements ##

* Qt 4.8 or later


## License ##

Copyright (c) 2018 Jochen Ulrich

Licensed under [MIT license](LICENSE).



[QSignalSpy]: http://doc.qt.io/qt-5/qsignalspy.html
