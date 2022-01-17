#pragma once

class QBrush;
class QPen;
class QColor;

QBrush makeGridBrush(int snapX, int snapY, const QPen &gridPen, const QColor &backgroundColor);
QBrush makeGridBrush(int snapX, int snapY, const QColor &gridColor, const QColor &backgroundColor);
