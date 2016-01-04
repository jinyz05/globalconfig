#include "edpcmdhandlehelper.h"
#include "cJSON.h"
EDPCMDHandleHelper::EDPCMDHandleHelper(QObject* parent)
    :QObject(parent)
{

}
int EDPCMDHandleHelper::execute(QByteArray cmdid,QByteArray req)
{
    int ret=0;
    cJSON* root=cJSON_Parse(req.constData());
    if(root==NULL)
    {
        return ret;
    }
    cJSON* cmdjson=cJSON_GetObjectItem(root,"cmd");
    if(cmdjson==NULL)
    {
        cJSON_Delete(root);
        return ret;
    }
    ret=1;
    cJSON* params=cJSON_GetObjectItem(root,"params");
    if(params)
    {
        emit Cmd(cmdjson->valuestring,cJSON_Print(params));
        cJSON_Delete(root);
    }
    else
    {
        emit Cmd(cmdjson->valuestring,"");
        cJSON_Delete(root);
    }
    return ret;
}
