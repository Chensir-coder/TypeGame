#include "pixmapslicehelper.h"

#include <QtGlobal>

QVector<QPixmap> PixmapSliceHelper::slice(const QString& resourcePath,
                                          int cols,
                                          int rows)
{
    QPixmap pixmap(resourcePath);
    return doSlice(pixmap, cols, rows, QSize(), false);
}

QVector<QPixmap> PixmapSliceHelper::slice(const QString& resourcePath,
                                          int cols,
                                          int rows,
                                          const QSize& targetSize)
{
    QPixmap pixmap(resourcePath);
    return doSlice(pixmap, cols, rows, targetSize, true);
}

QVector<QPixmap> PixmapSliceHelper::slice(const QPixmap& sourcePixmap,
                                          int cols,
                                          int rows)
{
    return doSlice(sourcePixmap, cols, rows, QSize(), false);
}

QVector<QPixmap> PixmapSliceHelper::slice(const QPixmap& sourcePixmap,
                                          int cols,
                                          int rows,
                                          const QSize& targetSize)
{
    return doSlice(sourcePixmap, cols, rows, targetSize, true);
}

QVector<QPixmap> PixmapSliceHelper::doSlice(const QPixmap& sourcePixmap,
                                            int cols,
                                            int rows,
                                            const QSize& targetSize,
                                            bool useScale)
{
    QVector<QPixmap> result;

    if (sourcePixmap.isNull()) {
        return result;
    }

    if (cols <= 0 || rows <= 0) {
        return result;
    }

    const int totalWidth = sourcePixmap.width();
    const int totalHeight = sourcePixmap.height();

    if (totalWidth <= 0 || totalHeight <= 0) {
        return result;
    }

    const int baseCellW = totalWidth / cols;
    const int baseCellH = totalHeight / rows;

    if (baseCellW <= 0 || baseCellH <= 0) {
        return result;
    }

    result.reserve(cols * rows);

    // 按行优先顺序：从上到下、每行从左到右。
    // 最后一列 / 最后一行吃掉除法余数，避免 copy 越过图集边界。
    for (int row = 0; row < rows; ++row) {
        const int y = row * baseCellH;
        if (y >= totalHeight) {
            break;
        }

        const int h = (row == rows - 1) ? (totalHeight - y) : baseCellH;

        for (int col = 0; col < cols; ++col) {
            const int x = col * baseCellW;
            if (x >= totalWidth) {
                break;
            }

            const int w = (col == cols - 1) ? (totalWidth - x) : baseCellW;

            if (w <= 0 || h <= 0) {
                continue;
            }

            QPixmap piece = sourcePixmap.copy(x, y, w, h);

            if (useScale && targetSize.isValid() && !targetSize.isEmpty()) {
                piece = piece.scaled(targetSize,
                                     Qt::IgnoreAspectRatio,
                                     Qt::SmoothTransformation);
            }

            result.push_back(piece);
        }
    }

    return result;
}
