#include "ClientFront.h"
#include "MessageTypes.h"
#include "QMessageBox"
#include <QStringLiteral>

ClientFront::ClientFront(QWidget *parent)
    : QWidget(parent),
		client_(std::make_unique<ClientBack<MessageTypes>>())
{
   
    ui.setupUi(this);
    this->setFixedSize(QSize(540, 260));

    
    connect(ui.ConnectButton, &QPushButton::clicked, this, &ClientFront::ConnectButton);
}

void ClientFront::ConnectButton()
{
    ErrorMessage success = client_->Connect(ui.HostLineEdit->text().toStdString(), ui.PortLineEdit->text().toUInt());

    if(success.first)
    {
        QMessageBox msgBox;

        //recode to 8-bit string with default codec (cp1251)
        QString msg(QString::fromLocal8Bit(success.second.c_str()));
        msgBox.setText(msg);
        msgBox.exec();
    }
}
