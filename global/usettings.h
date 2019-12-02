#ifndef USETTINGS_H
#define USETTINGS_H

#include "settings.h"

class USettings : public Settings
{
    Q_OBJECT
public:
    USettings(QString filePath);
    void init();

public:

};

#endif // USETTINGS_H
