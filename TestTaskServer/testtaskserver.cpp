#include "testtaskserver.h"

#include <iostream>

#include "tcpsocketcontext.h"
//-----------------------------------------------------------------------------
QString sortSymbolInDescendingOrder( const QString str );
QString reversalText( const QString str );
QString sortLineInAscendingOrder( const QString str );
QString symbolStatistic( const QString str );
//-----------------------------------------------------------------------------
TestTaskServer::TestTaskServer(QObject *parent) : QObject(parent)
{
	this->transformation.insert( "Sort symbol in descending order", sortSymbolInDescendingOrder );
	this->transformation.insert( "Reversal text", reversalText );
	this->transformation.insert( "Sort line in ascending order", sortLineInAscendingOrder );
	this->transformation.insert( "Symbol statistic", symbolStatistic );

	this->tcpServer = new QTcpServer( this );

	if( tcpServer->listen(  ) )
	{
		QString ipAddress;
		QList< QHostAddress > ipAddressesList = QNetworkInterface::allAddresses(  );

		for( int i = 0; i < ipAddressesList.size(  ) && ipAddress.isEmpty(  ); ++i )
		{
			if( ipAddressesList.at( i ) != QHostAddress::LocalHost && ipAddressesList.at( i ).toIPv4Address(  ) )
			{
				ipAddress = ipAddressesList.at( i ).toString(  );
			}
		}
		if( ipAddress.isEmpty(  ) )
		{
			ipAddress = QHostAddress( QHostAddress::LocalHost ).toString(  );
		}
		std::cout << "Start the server: " << ipAddress.toLocal8Bit(  ).data(  ) << ":" << tcpServer->serverPort(  ) << std::endl;
		connect( this->tcpServer, SIGNAL( newConnection(  ) ), this, SLOT( tcpServerNewConnection(  ) ) );
	}
	else
	{
		std::cout << "Unable to start the server" << std::endl;
	}
}
//-----------------------------------------------------------------------------
void TestTaskServer::tcpServerNewConnection(  )
{
	QTcpSocket * clientConnection = this->tcpServer->nextPendingConnection(  );
	// В случае разрыва соединения просто удаляем объект.
	connect( clientConnection, SIGNAL( disconnected(  ) ), clientConnection, SLOT( deleteLater(  ) ) );
	connect( clientConnection, SIGNAL( readyRead(  ) ), this, SLOT( tcpSubServerReadyRead(  ) ) );
	std::cout << "New connection" << std::endl;
	// В качестве "родителя" создаваемого объекта указываем полученный сокет соединения
	// с тем, чтобы уже он заботился об освобождении памяти занимаемой его "детьми".
	TcpSocketContext * tcpSocketContext = new TcpSocketContext( clientConnection );
	tcpSocketContext->setObjectName( "TcpSocketContext" );
}
//-----------------------------------------------------------------------------
void TestTaskServer::tcpSubServerReadyRead(  )
{
	QTcpSocket * tcpSocket = static_cast< QTcpSocket * >( this->sender(  ) );

	if( tcpSocket != NULL )
	{
		QByteArray data = tcpSocket->readAll(  );
		TcpSocketContext * tcpSocketContext = tcpSocket->findChild<TcpSocketContext *>( "TcpSocketContext" );
		if( tcpSocketContext != NULL )
		{
			tcpSocketContext->data.append( data );
			// Если принята последняя порция передаваемого пакета данных, то пытаемся
			// выделить необходимую информацию.
			if( data.endsWith( ( char )0 ) && !tcpSocketContext->data.isEmpty(  ) )
			{
				QString text;
				QString transform;
				QString transformText = "!!! Unsupported transformation !!!";
				{
					QJsonDocument jsonDocument( QJsonDocument::fromJson( tcpSocketContext->data ) );
					QJsonObject jsonObject = jsonDocument.object(  );

					text = jsonObject[ "text" ].toString(  );
					transform = jsonObject[ "transform" ].toString(  );

					std::cout << text.toLocal8Bit(  ).data(  ) << std::endl;
				}
				//-----------------------------------------------------------------------------
				if( this->transformation.contains( transform ) )
				{
					transformText = this->transformation.value( transform )( text );
				}
				//-----------------------------------------------------------------------------
				{
					QJsonObject jsonObject;

					jsonObject[ "transformText" ] = transformText;

					QJsonDocument jsonDoc( jsonObject );
					// Так как формат данных текстовый то можно использовать любой не печатный символ в качестве
					// признака конца пакета, необходимо в случае передачи длинного текста.
					tcpSocket->write( jsonDoc.toJson(  ).append( ( char )0 ) );
				}
				//-----------------------------------------------------------------------------
				tcpSocketContext->data.clear(  );
			}
		}
	}
}
//-----------------------------------------------------------------------------
// "Sort symbol in descending order"
QString sortSymbolInDescendingOrder( const QString str )
{
	QStringList strList;

	strList = str.split( QRegExp(  ) );
	strList.removeFirst(  );
	strList.removeLast(  );
	strList.sort(  );

	return reversalText( strList.join( "" ) );
}
//-----------------------------------------------------------------------------
// "Reversal text"
QString reversalText( const QString str )
{
	QString res;
	QString::const_iterator i;

	for( i = str.begin(  ); i != str.end(  ); i++ )
	{
		res.prepend( *i );
	}

	return res;
}
//-----------------------------------------------------------------------------
// "Sort line in ascending order"
QString sortLineInAscendingOrder( const QString str )
{
	QStringList strList;

	strList = str.split( "\n" );
	strList.sort(  );

	return strList.join( "\n" );
}
//-----------------------------------------------------------------------------
// "Symbol statistic"
QString symbolStatistic( const QString str )
{
	QString res;
	QMap< QChar, int > statistic;
	QMap< QChar, int >::iterator j;
	QString::const_iterator i;

	for( i = str.begin(  ); i != str.end(  ); i++ )
	{
		if( statistic.contains( *i ) )
		{
			statistic[ *i ] = statistic.value( *i ) + 1;
		}
		else
		{
			statistic.insert( *i, 1 );
		}
	}

	for( j = statistic.begin(  ); j != statistic.end(  ); j++ )
	{
		res += QString( "\'%1\': %2\n" ).arg( j.key(  ) ).arg( j.value(  ) );
	}

	return res;
}
