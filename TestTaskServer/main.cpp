#include <QCoreApplication>
#include "testtaskserver.h"

#include <iostream>

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);
	TestTaskServer testTaskServer;

	return a.exec();
}
