#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "medicament.h"
#include <QMainWindow>
#include <QPushButton>
#include <QSqlQueryModel>
#include <QString>
#include <QMessageBox>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void on_pushButton_ajouter_clicked();
   void on_pushButton_supprimer_clicked();
 //  void on_pushButton_modifier_clicked();  // Ajouter cette ligne


private slots:
   void on_pdf_clicked();

   void on_stats_clicked();

   void on_comboBox_currentTextChanged(const QString &arg1);

   void on_comboBox_currentIndexChanged(const QString &arg1);

   void on_le_recherche_textChanged(const QString &arg1);

   void on_histo_clicked();

   void on_map_2_clicked();

   void on_modifier_f_clicked();

   void on_pushButton_3_clicked();

   void on_upload_image_2_clicked();

   void on_afficher_image_clicked();

   private:
    Ui::MainWindow *ui;
    medicament mt;

    QString encodeImageToBase64(const QString &filePath); // Fonction pour encoder une image en Base64


};

#endif // MAINWINDOW_H
