#ifndef TCPSOCKETCONTEXT_H
	#define TCPSOCKETCONTEXT_H

	#include <QObject>

	class TcpSocketContext : public QObject
	{
		Q_OBJECT
	public:
		explicit TcpSocketContext( QObject *parent = 0 );
		~TcpSocketContext(  );

		QByteArray data;
	};

#endif // TCPSOCKETCONTEXT_H
