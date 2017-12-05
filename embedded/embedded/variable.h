#ifndef VARIABLE_H
#define VARIABLE_H

#include <QString>
#include <QVariant>

enum class VarType {
    Bool,
    Int,
    Float,
    IntList,
    FloatList
};

struct Variable
{
public:
    QString name;
    VarType type;
    bool readOnly;
    QVariant value;
};

#endif // VARIABLE_H
