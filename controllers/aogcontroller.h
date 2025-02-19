#ifndef AOGCONTROLLER_H
#define AOGCONTROLLER_H

#include <QObject>

class AOGController : public QObject
{
    Q_OBJECT
public:
    explicit AOGController(QObject *parent = nullptr);

signals:
};

#endif // AOGCONTROLLER_H
