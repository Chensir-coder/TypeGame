#include "ai_word_service.h"

#include <QCoreApplication>
#include <QDebug>
#include <QTimer>

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    qDebug() << "Start AI word service demo...";

    AiWordService::instance().requestRandomWordAsync(
        "space war",
        4,
        8,
        [](const QString& word, bool fromLocal) {
            qDebug() << "Generated word:" << word;
            qDebug() << "From local fallback:" << fromLocal;

            QCoreApplication::quit();
        });

    QTimer::singleShot(10000, []() {
        qDebug() << "Timeout: no response in 10 seconds.";
        QCoreApplication::quit();
    });

    return app.exec();
}
