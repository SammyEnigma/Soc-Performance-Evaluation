#ifndef MODULEINTERFACE_H
#define MODULEINTERFACE_H

#include <QString>

class ModuleInterface
{
public:
    virtual QString getModuleInfo() = 0;
    virtual void initData() = 0;
    virtual void clearData() = 0;

    virtual void passOnPackets() = 0; // 1、queue中packet延迟满后，传入到下一个queue
    virtual void delayOneClock() = 0; // 2、传输/处理/读取延迟到下一个clock
    virtual void updatePacketPos() = 0;
};

#endif // MODULEINTERFACE_H
