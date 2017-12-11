#include "widget.h"

#include <QBoxLayout>
#include <QMessageBox>
//-------------------------------------------------------------------------
Widget::Widget(QWidget *parent) :
	QWidget(parent)
{
	QBoxLayout * mainLayout = new QBoxLayout( QBoxLayout::TopToBottom, this );
	QBoxLayout * layout;

	this->setWindowTitle( "Test task" );
	//-------------------------------------------------------------------------
	layout = new QBoxLayout( QBoxLayout::LeftToRight );

	this->addressLineEdit = new QLineEdit( "127.0.0.1", this );
	layout->addWidget( this->addressLineEdit );

	this->portLineEdit = new QLineEdit( "8000", this );
	layout->addWidget( this->portLineEdit );

	this->connectDisconnectPushButton = new QPushButton( "Connect", this );
	connect( this->connectDisconnectPushButton, SIGNAL( clicked( bool ) ), this, SLOT( connectDisconnectPushButtonClick(  ) ) );
	layout->addWidget( this->connectDisconnectPushButton );
	//-------------------------------------------------------------------------
	mainLayout->addLayout( layout );
	//-------------------------------------------------------------------------
	this->originalTextEdit = new QPlainTextEdit( this );
	mainLayout->addWidget( this->originalTextEdit );
	//-------------------------------------------------------------------------
	layout = new QBoxLayout( QBoxLayout::LeftToRight );

	this->transformComboBox = new QComboBox( this );
	this->transformComboBox->addItem( "Sort symbol in descending order" );
	this->transformComboBox->addItem( "Reversal text" );
	this->transformComboBox->addItem( "Sort line in ascending order" );
	this->transformComboBox->addItem( "Symbol statistic" );
	layout->addWidget( this->transformComboBox );

	this->sendPushButton = new QPushButton( "Send", this );
	connect( this->sendPushButton, SIGNAL( clicked( bool ) ), this, SLOT( sendPushButtonClick(  ) ) );
	layout->addWidget( this->sendPushButton );
	//-------------------------------------------------------------------------
	mainLayout->addLayout( layout );
	//-------------------------------------------------------------------------
	this->transformTextEdit = new QPlainTextEdit( this );
	mainLayout->addWidget( this->transformTextEdit );
	//-------------------------------------------------------------------------
	this->tcpSocket = new QTcpSocket( this );

	connect( this->tcpSocket, SIGNAL( readyRead(  ) ), this, SLOT( tcpSocketReadyRead(  ) ) );
	connect( this->tcpSocket, SIGNAL( error( QAbstractSocket::SocketError ) ), this, SLOT( tcpSocketError( QAbstractSocket::SocketError ) ) );
}
//-------------------------------------------------------------------------
Widget::~Widget()
{
	//
}
//-------------------------------------------------------------------------
void Widget::connectDisconnectPushButtonClick(  )
{
	if( this->connectDisconnectPushButton->text(  ) == "Connect" )
	{
		this->tcpSocket->abort(  );
		this->tcpSocket->connectToHost( this->addressLineEdit->text(  ), this->portLineEdit->text(  ).toInt(  ) );
		this->connectDisconnectPushButton->setText( "Disconnect" );
	}
	else
	if( this->connectDisconnectPushButton->text(  ) == "Disconnect" )
	{
		this->tcpSocket->abort(  );
		this->connectDisconnectPushButton->setText( "Connect" );
	}
}
//-------------------------------------------------------------------------
void Widget::sendPushButtonClick(  )
{
	if( tcpSocket->isValid(  ) )
	{
		QJsonObject textJsonObject;
		textJsonObject[ "transform" ] = this->transformComboBox->currentText(  );
		textJsonObject[ "text" ] = this->originalTextEdit->toPlainText(  );

		QJsonDocument textJsonDoc( textJsonObject );
		// Так как формат данных текстовый то можно использовать любой не печатный символ в качестве
		// признака конца пакета, необходимо в случае передачи длинного текста.
		tcpSocket->write( textJsonDoc.toJson(  ).append( ( char )0 ) );
	}
}
//-------------------------------------------------------------------------
void Widget::tcpSocketReadyRead(  )
{
	QByteArray data = tcpSocket->readAll(  );

	this->data.append( data );
	// Если принята последняя порция передаваемого пакета данных, то пытаемся
	// выделить необходимую информацию.
	if( data.endsWith( ( char )0 ) && !this->data.isEmpty(  ) )
	{
		QJsonDocument jsonDocument( QJsonDocument::fromJson( this->data ) );
		QJsonObject jsonObject = jsonDocument.object(  );

		this->transformTextEdit->clear(  );
		this->transformTextEdit->appendPlainText( jsonObject[ "transformText" ].toString(  ) );

		this->data.clear(  );
	}
}
//-------------------------------------------------------------------------
void Widget::tcpSocketError( QAbstractSocket::SocketError )
{
	QMessageBox::information(this, tr( "Test task" ), tr("The following error occurred: %1.").arg( this->tcpSocket->errorString(  ) ) );
	this->connectDisconnectPushButton->setText( "Connect" );
}
