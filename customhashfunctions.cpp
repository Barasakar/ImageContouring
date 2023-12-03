#include<qpoint.h>
#include <QHashFunctions>
#include <qhash.h>

uint qHash(const QPointF& point, uint seed) {
    return qHash(point.x(), seed) ^ qHash(point.y(), seed);
}