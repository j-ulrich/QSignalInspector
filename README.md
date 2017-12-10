# QSignalInspector #

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
		QStringList parameters;
		QList<QVariant>::ConstIterator paramIter;

		for (paramIter = iter->second.begin(); paramIter != iter.second.end(); ++paramIter)
			parameters << paramIter->toString();

		out << iter->first.name() << "(" << parameters.join(", ") << ")" << endl;
	}

	return 0;
}


```


## Usage ##

```c++
QSignalInspector(const QObject* object, bool includeParentClassSignals = true)
```

Creates a QSignalInspector recording the signal emissions of _object_.

** Parameters **<br/>
**object** - The object whose signals should be recorded.<br/>
**includeParentClassSignals** - If `true`, the signals of all parent classes of
_object_ are recorded as well. If `false`, only those signals are recorded that
are declared by the last class in the inheritance hierarchy of _object_.


## Requirements ##

* Qt 4.8 or later


## License ##

Copyright (c) 2017 Jochen Ulrich

Licensed under [MIT license](LICENSE).



[QSignalSpy]: http://doc.qt.io/qt-5/qsignalspy.html