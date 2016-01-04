#ifndef RECORDJSON_H
#define RECORDJSON_H
#include "cJSON.h"
#include <QList>
#include <QMap>
#include <QString>
class RecordJSON
{
public:    
    static cJSON* GetRecordJSON(QMap<QString,QString>& map,QString index);
    static cJSON* GetPhotoJSON(QMap<QString,QString>& map,QByteArray&);
};

#endif // RECORDJSON_H
