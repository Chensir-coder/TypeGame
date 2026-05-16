#ifndef __TYPEGAME_AI_HTTP_CLIENT_H__
#define __TYPEGAME_AI_HTTP_CLIENT_H__

#include "ai_http_request_config.h"

#include <QByteArray>
#include <QString>

class AiHttpClient
{
public:
    /**
     * 同步 POST：按 config 发送 JSON body，在调用线程阻塞直到完成或超时。
     * @return 是否成功（HTTP 2xx 且网络无错）；响应体写入 responseBodyOut。
     */
    static bool postJson(const AiHttpRequestConfig& config,
                         QByteArray* responseBodyOut,
                         QString* errorOut);
};

#endif // __TYPEGAME_AI_HTTP_CLIENT_H__
