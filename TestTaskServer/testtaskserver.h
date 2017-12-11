#ifndef TESTTASKSERVER_H
	#define TESTTASKSERVER_H

	#include <QObject>

	#include <QtNetwork>

	class TestTaskServer : public QObject
	{
		Q_OBJECT
	public:
		explicit TestTaskServer( QObject *parent = 0 );

	signals:

	public slots:
		void tcpServerNewConnection(  );
		void tcpSubServerReadyRead(  );

	private:
		QMap< QString, QString ( * )( const QString str ) > transformation;
		QTcpServer * tcpServer;
	};
#endif // TESTTASKSERVER_H
