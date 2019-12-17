#include "moduleport.h"

ModulePort::ModulePort(QWidget *parent) : PortBase(parent), bandwidth(1)
{

}

QString ModulePort::getClass()
{
    return "ModulePort";
}

void ModulePort::fromStringAddin(QString s)
{
    QString bandwidth = StringUtil::getXml(s, "BANDWIDTH");
    if (!s.isEmpty())
        this->bandwidth = bandwidth.toInt();
}

QString ModulePort::toStringAddin()
{
    QString full;
    QString indent = "\n\t\t";
    full += indent + StringUtil::makeXml(bandwidth, "BANDWIDTH");
    return full;
}

void ModulePort::slotDataList()
{
    /*PortDataDialog* pdd = new PortDataDialog(this);
    pdd->exec();
    pdd->deleteLater();*/
    emit signalDataList();
    qDebug() << "void ModulePort::slotDataList()";
}
