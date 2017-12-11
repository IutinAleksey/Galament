#include "tcpsocketcontext.h"

//-----------------------------------------------------------------------------
TcpSocketContext::TcpSocketContext( QObject * parent ):
	QObject( parent )
{
}
//-----------------------------------------------------------------------------
TcpSocketContext::~TcpSocketContext(  )
{
	this->data.clear(  );
}
