#ifndef __TYPEGAME_PIXMAPSLICEHELPER_H__
#define __TYPEGAME_PIXMAPSLICEHELPER_H__

#include <QPixmap>
#include <QVector>
#include <QString>
#include <QSize>

/* ------------------------------------------------------------------
 // 文件名     : pixmapslicehelper.h
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-26
 // 功能描述   : pixmapslicehelper 头文件声明
------------------------------------------------------------------ */

class PixmapSliceHelper
{
public:
    // 从资源路径加载并按 rows x cols 切图
    static QVector<QPixmap> slice(const QString& resourcePath,
                                  int cols,
                                  int rows);

    // 从资源路径加载并按 rows x cols 切图，可限制输出大小
    static QVector<QPixmap> slice(const QString& resourcePath,
                                  int cols,
                                  int rows,
                                  const QSize& targetSize);

    // 直接对已加载的 QPixmap 切图
    static QVector<QPixmap> slice(const QPixmap& sourcePixmap,
                                  int cols,
                                  int rows);

    // 直接对已加载的 QPixmap 切图，并缩放到指定大小
    static QVector<QPixmap> slice(const QPixmap& sourcePixmap,
                                  int cols,
                                  int rows,
                                  const QSize& targetSize);

private:
    static QVector<QPixmap> doSlice(const QPixmap& sourcePixmap,
                                    int cols,
                                    int rows,
                                    const QSize& targetSize,
                                    bool useScale);
};

#endif // __TYPEGAME_PIXMAPSLICEHELPER_H__
