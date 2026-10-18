#pragma once

#include <QString>

struct User
{
    int id = 0;
    QString username;
    QString email;

    bool isValid() const { return id > 0 && !username.isEmpty(); }
};
