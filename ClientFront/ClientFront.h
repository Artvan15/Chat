#pragma once

#include <QtWidgets/QWidget>
#include <memory>
#include "ui_ClientFront.h"
#include "ClientBack.h"
#include "IClient.h"


class ClientFront : public QWidget
{
    Q_OBJECT

public:
    ClientFront(QWidget *parent = Q_NULLPTR);

private slots:
    void ConnectButton();

private:
    Ui::ClientFrontClass ui;
    std::unique_ptr<IClient> client_;
};
