#pragma once

class QWidget;
class QPixmap;
class QImage;

QPixmap loadPixmap(QWidget *parent);
void saveImage(QWidget *parent, const QImage &image);
