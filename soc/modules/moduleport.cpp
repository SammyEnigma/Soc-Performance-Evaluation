#include "moduleport.h"

ModulePort::ModulePort(QWidget *parent) : PortBase(parent), bandwidth(1)
{

}

void ModulePort::slotDataList()
{
    /*PortDataDialog* pdd = new PortDataDialog(this);
    pdd->exec();
    pdd->deleteLater();*/
    emit signalDataList();
}
