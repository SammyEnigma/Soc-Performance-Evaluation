#include "usettings.h"

USettings::USettings(QString filePath) : Settings(filePath)
{

}

void USettings::init()
{
    auto_save = getBool("us/auto_save", false);
}
