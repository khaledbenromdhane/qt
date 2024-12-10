#ifndef FINANCE_H
#define FINANCE_H
#include <QString>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QPieSeries>
#include <QSqlQuery>
class finance
{
    QString id,dt,tp,cat,mode;
    int mt;
    QSqlQuery query;

public:
    finance(){}
    finance(QString,QString,int,QString,QString,QString);

    bool ajouter();
    QSqlQueryModel * afficher();
    bool supprimer(QString);
    bool existe(const QString &ID_T) ;
    bool modifier(const QString &ID_T, const QString &attribut, const QString &nouvelleValeur) ;
    bool modifierAttribut(const QString &ID_T, const QString &attribut, const QString &nouvelleValeur) ;
    QPieSeries* generateFinanceStatistics();
    void genererPDF();
    QString chatbot(const QString &userMessage);




};

#endif // FINANCE_H

