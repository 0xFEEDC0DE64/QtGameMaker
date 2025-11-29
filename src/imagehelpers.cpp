#include "imagehelpers.h"

#include <QWidget>
#include <QImage>
#include <QImageWriter>
#include <QList>
#include <QByteArray>
#include <QString>
#include <QHash>
#include <QFileDialog>
#include <QMessageBox>
#include <QCoreApplication>

void saveImage(QWidget *parent, const QImage &image)
{
    QList<QByteArray> formats = QImageWriter::supportedImageFormats();
    if (formats.contains("png"))
        formats.removeAll("png");
    formats.prepend("png");

    QString filterString;
    QString allFormats;
    QHash<QString, QByteArray> lookup;
    for (const QByteArray &format : std::as_const(formats))
    {
        auto text = QCoreApplication::translate("imagehelpers", "%1 format (*.%2)").arg(format.toUpper(), format);
        lookup.insert(text, format);

        if (!filterString.isEmpty())
            filterString += ";;";
        filterString.append(text);

        if (!allFormats.isEmpty())
            allFormats += " ";
        allFormats += "*.";
        allFormats += format;
    }

    allFormats = QCoreApplication::translate("imagehelpers", "All Supported Formats (%0)").arg(allFormats);
    if (!filterString.isEmpty())
        filterString += ";;";
    filterString.append(allFormats);

    QString selectedFilter;
    QString fileName = QFileDialog::getSaveFileName(parent, "Save Image", QString(), filterString, &selectedFilter);
    if (fileName.isEmpty())
        return;

    QByteArray selectedFormat;
    if (auto iter = lookup.constFind(selectedFilter); iter != std::cend(lookup))
    {
        selectedFormat = *iter;
        QString suffix = '.' + *iter;
        if (!fileName.endsWith(suffix, Qt::CaseInsensitive))
        {
            if (QMessageBox::warning(parent,
                                     QCoreApplication::translate("imagehelpers", "Append file extension?"),
                                     QCoreApplication::translate("imagehelpers", "It seems like you didnt append the expected file extension %0 at the end.\n\nDo you want to automatically append it before saving?").arg(suffix),
                                     QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
            {
                fileName += suffix;
                if (QFileInfo::exists(fileName))
                {
                    if (QMessageBox::warning(parent,
                                             QCoreApplication::translate("imagehelpers", "Overwrite file?"),
                                             QCoreApplication::translate("imagehelpers", "The file\n\n%0\n\nalready exists. Should it be overwritten?").arg(fileName),
                                             QMessageBox::Ok | QMessageBox::Cancel) != QMessageBox::Ok)
                        return;
                }
            }
        }
    }
    else if (auto iter = std::find_if(std::cbegin(formats), std::cend(formats),
                                      [&fileName](const QByteArray &format){
                                          return fileName.endsWith("." + format, Qt::CaseInsensitive);
                                      }); iter != std::cend(formats))
    {
        selectedFormat = *iter;
    }
    else
    {
        QMessageBox::warning(parent, QCoreApplication::translate("imagehelpers", "Unknown file format!"), QCoreApplication::translate("imagehelpers", "Unknown file format!"));
        return;
    }

    QImageWriter writer{fileName, selectedFormat};
    if (!writer.write(image))
        QMessageBox::warning(parent, QCoreApplication::translate("imagehelpers", "Error saving image!"), QCoreApplication::translate("imagehelpers", "Error saving image:\n\n%0").arg(writer.errorString()));
}
