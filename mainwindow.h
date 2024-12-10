#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "finance.h"
#include <QMainWindow>
#include <QTableView>
#include <QNetworkAccessManager>
#include <QNetworkReply>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
        finance f;


private slots:
        void on_send_clicked();
        void onNetworkReply(QNetworkReply *reply);
    // Slots for each button click
    void on_projet_clicked();
    void on_employer_clicked();
    void on_materielle_clicked();
    void on_fournisseur_clicked();
    void on_facture_clicked();
  void  on_pushButton_ajouter_clicked();
  void  on_pushButton_supprimer_clicked();
  void  on_pushButton_modifierer_clicked();
  void on_pushButton_modifier_f_clicked();
  void on_ACTUALISER_clicked();
  void on_comboBox_trii_changed(int index);
  void on_lineEdit_15_cursorPositionChanged(const QString& text);
  void on_pushButton_9_clicked();
  void on_generatePieChartButton_clicked();
  void on_pdf_clicked();
  void on_question_returnPressed();

  // Slot for handling network responses




private:

    Ui::MainWindow *ui;
    QNetworkAccessManager *networkManager;

    // Helper function to send an SMS
    void sendSMS(QString num, QString text);


    // Helper function to send an SMS


};
#endif // MAINWINDOW_H
