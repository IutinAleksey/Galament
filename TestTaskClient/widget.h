#ifndef WIDGET_H
	#define WIDGET_H

	#include <QWidget>

	#include <QComboBox>
	#include <QPushButton>
	#include <QPlainTextEdit>
	#include <QLineEdit>

	#include <QtNetwork>

	class Widget : public QWidget
	{
		Q_OBJECT

	public:
		Widget(QWidget *parent = 0);
		~Widget();

	private:
		QLineEdit * addressLineEdit;
		QLineEdit * portLineEdit;
		QPushButton * connectDisconnectPushButton;

		QComboBox * transformComboBox;
		QPushButton * sendPushButton;

		QPlainTextEdit * originalTextEdit;
		QPlainTextEdit * transformTextEdit;

		QTcpSocket * tcpSocket;

		QByteArray data;

	private slots:
		void connectDisconnectPushButtonClick();
		void sendPushButtonClick(  );

		void tcpSocketReadyRead(  );
		void tcpSocketError(QAbstractSocket::SocketError);
	};

#endif // WIDGET_H
