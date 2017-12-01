#ifndef VARIABLE_H
#define VARIABLE_H

#include <QString>
#include <QVariant>

struct Variable
{
public:
    QString name;
    QString type;
    bool readOnly;
    QVariant value;
};

#endif // VARIABLE_H
