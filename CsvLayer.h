#ifndef CSVLAYER_H
#define CSVLAYER_H

#include <QObject>

class CsvLayer : public QObject
{
    Q_OBJECT
public:
    explicit CsvLayer(QObject *parent = nullptr);

signals:

};

#endif // CSVLAYER_H
