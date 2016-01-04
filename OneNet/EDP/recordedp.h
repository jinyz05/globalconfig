#ifndef RECORDEDP_H
#define RECORDEDP_H
#include "recordjson.h"
#include "edp.h"
class RecordEDP : public RecordJSON
{
public:
    RecordEDP(QList<quint32> list,QString tokenid);
    void UpLoad();
};

#endif // RECORDEDP_H
