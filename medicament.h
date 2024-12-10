#ifndef MEDICAMENT_H
#define MEDICAMENT_H
#include<QString>
#include<QSqlQuery>
#include<QSqlQueryModel>
#include <QVariant>
#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QChart>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts>

class medicament
{
  QString nom,famille,date_fabrication,date_expiration,seuil_min,image;
  int vignette,dosage,stock,prix;
public:

    medicament();
    medicament(QString,int,QString,QString,QString,int,QString,QString,int,int);

    QString getnom(){return nom ;}
    QString getfamille(){return famille ;}
    QString getdate_fabrication(){return date_fabrication ;}
    QString getdate_expiration(){return date_expiration ;}
    QString getseuil_min(){return seuil_min ;}
    int getvignette(){return vignette; }
    int getdosage(){return dosage; }
    int getprix(){return prix; }
    int getstock(){return stock; }
    void setImage(const QString &encodedImage) { image = encodedImage; }



    QString getImageById(int id);


    bool ajouter();
    bool modifier(int vignette, const QString &attribut, const QVariant &nouvelleValeur);

    bool supprimer(int vignette);
    bool existe(int vignette);        // Vérifier si un médicament existe en fonction de sa vignette

    QSqlQueryModel * afficher();
    bool modifierAttribut( int vignette, const QString &attribut, const QString &nouvelleValeur);

void exportDataToPDF();
QChartView* createPieChartPrix();
QChartView* createBarChartByFamily();
QSqlQueryModel* trier(const QString& sortColumn);
QSqlQueryModel* rechercher(const QString& searchType, const QString& value) ;


};

#endif // MEDICAMENT_H
