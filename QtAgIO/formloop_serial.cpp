#include "formloop.h"
#include "agioproperty.h"
#include <QNetworkInterface>


void FormLoop::openSerialPort(QString comName, int baud){
    serialPort = new QSerialPort(this);
    serialPort->setPortName(comName);
    serialPort->setBaudRate(baud);
    //serialPort->setBaudRate(baud);
    serialPort->setFlowControl(QSerialPort::NoFlowControl);
    serialPort->setDataBits(QSerialPort::Data8);
    serialPort->setStopBits(QSerialPort::OneStop);
    serialPort->open(QIODevice::ReadWrite);
    isSendToSerial = true;
    connect(serialPort,SIGNAL(readyRead()),this,SLOT(readFromSerialPort()));
}

void FormLoop::closeSerialPort(){

    serialPort->close();

}

bool FormLoop::getConnectionStatus(){
    serialPort = new QSerialPort(this);
    return serialPort->isOpen();

}

void FormLoop::writeToSerialPort(QString message){

    const QByteArray &messageArray = message.toLocal8Bit();
    serialPort->write(messageArray);
}

void FormLoop::SendGPSPort(QByteArray byteData){
    serialPort->write(byteData);
}

void FormLoop::openSerialPortSlot(QString comName, int baud){

    this->openSerialPort(comName,baud);
}

void FormLoop::writeToSerialPortSlot(QString message){

    this->writeToSerialPort(message);
}

void FormLoop::closeSerialPortSlot(){

    this->closeSerialPort();
}

bool FormLoop::getConnectionStatusSlot(){

    return this->getConnectionStatus();
}

void FormLoop::readFromSerialPort(){

    if (serialPort->canReadLine()){
        QString data = QString::fromLatin1(serialPort->readAll());

        rawBuffer += data;
        ParseNMEA(rawBuffer);
        if(!haveWeSentToParser) {
            qDebug() << "sent to parser";
            haveWeSentToParser = true;
        }

        //QString data = QString::fromLatin1(serialPort->readAll());
        //emit getData(data);
    }

}
