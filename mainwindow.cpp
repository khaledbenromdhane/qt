#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "medicament.h"
#include <QDebug>
#include <QPushButton>  // Ajouter cette ligne si ce n'est pas déjà inclus
#include <QDate>
#include <QRegularExpression>
#include <QSqlError>
#include <utility>
#include <QMessageBox>
#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QChart>
#include <QtCharts/QBarCategoryAxis>
#include <iostream>
#include <QFile>
#include <QSqlQuery>
#include <QFileDialog>


#include <QtCharts>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
   // ui->tableView->setModel(mt.afficher());
    connect(ui->pushButton_5, &QPushButton::clicked, this, &MainWindow::on_pushButton_ajouter_clicked);
     connect(ui->spprimer, &QPushButton::clicked, this, &MainWindow::on_pushButton_supprimer_clicked);
     // Connecter le bouton à la méthode pour modifier


}
MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_ajouter_clicked()
{
    // Récupération des informations saisies dans les champs
    QString nom = ui->lineEdit_nom->text();                // Nom du médicament
    QString famille = ui->lineEdit_famille->text();        // Famille du médicament
    QString date_fabrication = ui->lineEdit_date_fabrication->text();  // Date de fabrication
    QString date_expiration = ui->lineEdit_date_expiration->text();    // Date d'expiration
    QString seuil_min = ui->lineEdit_seuil_min->text();    // Seuil minimum
    QString vignetteStr = ui->lineEdit_vignette->text();   // Vignette du médicament
    QString dosageStr = ui->lineEdit_dosage->text();       // Dosage du médicament
    QString prixStr = ui->lineEdit_prix->text();           // Prix du médicament
    QString stockStr = ui->lineEdit_stock->text();         // Stock du médicament

    // Expression régulière pour vérifier que la vignette, dosage, prix et stock sont des entiers ou décimaux valides
    QRegularExpression nombreRegex("^\\d+$"); // Entiers positifs
    QRegularExpression prixRegex("^\\d+(\\.\\d{1,2})?$"); // Prix avec deux décimales max

    // Contrôle de saisie : Vérification que la vignette est un nombre entier
    if (!nombreRegex.match(vignetteStr).hasMatch()) {
        QMessageBox::warning(this, tr("Contrôle de saisie"),
                             tr("La vignette doit être un nombre entier positif."));
        return;
    }

    // Contrôle de saisie : Vérification que le dosage est un nombre entier
    if (!nombreRegex.match(dosageStr).hasMatch()) {
        QMessageBox::warning(this, tr("Contrôle de saisie"),
                             tr("Le dosage doit être un nombre entier positif."));
        return;
    }

    // Contrôle de saisie : Vérification que le prix est un nombre valide
    if (!prixRegex.match(prixStr).hasMatch()) {
        QMessageBox::warning(this, tr("Contrôle de saisie"),
                             tr("Le prix doit être un nombre valide (ex: 15.99)."));
        return;
    }

    // Contrôle de saisie : Vérification que le stock est un nombre entier
    if (!nombreRegex.match(stockStr).hasMatch()) {
        QMessageBox::warning(this, tr("Contrôle de saisie"),
                             tr("Le stock doit être un nombre entier positif."));
        return;
    }

    // Conversion des chaînes en entier après validation
    int vignette = vignetteStr.toInt();
    int dosage = dosageStr.toInt();
    double prix = prixStr.toDouble(); // Conversion en double pour le prix
    int stock = stockStr.toInt();     // Conversion en entier pour le stock

    // Contrôle de saisie : Vérifier que les champs obligatoires ne sont pas vides
    if (nom.isEmpty() || famille.isEmpty() || date_fabrication.isEmpty() ||
        date_expiration.isEmpty() || seuil_min.isEmpty() || vignette <= 0 || dosage <= 0 || prix <= 0 || stock < 0)
    {
        QMessageBox::warning(this, tr("Contrôle de saisie"),
                             tr("Veuillez remplir tous les champs correctement et ne laissez aucun champ vide."));
        return; // Arrêter la fonction si une erreur de saisie est détectée
    }

    // Nettoyer les dates pour éliminer les espaces supplémentaires
    date_fabrication = date_fabrication.trimmed();
    date_expiration = date_expiration.trimmed();



    // Contrôle de saisie : Vérification du format des dates (yyyy-MM-dd)


    // Contrôle de saisie : Vérification des formats de date
    QDate dateFab = QDate::fromString(date_fabrication, "yyyy-MM-dd");
    QDate dateExp = QDate::fromString(date_expiration, "yyyy-MM-dd");




    // Contrôle de saisie : Vérification de la date d'expiration
    if (dateExp < dateFab) {
        QMessageBox::warning(this, tr("Contrôle de saisie"),
                             tr("La date d'expiration doit être après la date de fabrication."));
        return;
    }

    // Instancier un objet Medicament avec les données saisies
    medicament med(nom, vignette, date_fabrication, date_expiration, seuil_min, dosage, famille,"", prix, stock);

    // Essayer d'ajouter l'objet Medicament à la base de données
    bool test = med.ajouter(); // Ajouter le médicament à la base de données

    if (test) { // Si ajout réussi
        // Mettre à jour l'affichage avec la nouvelle liste de médicaments
        ui->tableView->setModel(med.afficher());
        QMessageBox::information(nullptr, QObject::tr("Succès"),
                                 QObject::tr("Médicament ajouté avec succès.\nClick Cancel to exit."), QMessageBox::Cancel);
        QFile file("C:/Users/tasn8/Desktop/tasnim/ficher.txt");

                    if (!file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
                        qDebug() << "Impossible d'ouvrir le fichier.";
                        return;
                                  }
                    // Obtenir la date et l'heure actuelles
                               QDateTime currentDateTime = QDateTime::currentDateTime();
                               QString formattedDateTime = currentDateTime.toString("dd.MM.yyyy hh:mm:ss");

                               // Écrire la date et l'heure actuelles suivies du texte dans le fichier
                               QTextStream out(&file);
                               out<< "<font color=\"green\">Le medicament avec le nom : "<<nom <<"a ete ajoute a " << formattedDateTime<< "</font> \n" ; // endl ajoute un saut de ligne
                               file.close();
    } else { // Si l'ajout échoue
        QMessageBox::critical(nullptr, QObject::tr("Erreur"),
                              QObject::tr("Échec de l'ajout du médicament.\nClick Cancel to exit."), QMessageBox::Cancel);
    }
}



void MainWindow::on_pushButton_supprimer_clicked()
{
    // Récupérer l'ID du médicament à supprimer
    QString idStr = ui->supp->text().trimmed();  // Récupère l'ID et enlève les espaces
    bool ok;
    int ID_T = idStr.toInt(&ok);  // Convertir la chaîne en entier

    // Contrôle de saisie : Vérifier si l'ID est valide et supérieur à 0
    if (idStr.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "L'ID ne peut pas être vide.");

        return;
    }

    if (!ok || ID_T <= 0) {
        QMessageBox::warning(this, "Erreur", "Veuillez entrer un ID valide et supérieur à 0.");
        return;
    }

    // Vérifier si le médicament avec cet ID existe dans la base de données
    medicament m;  // Créer un objet de type medicament
    if (!m.existe(ID_T)) {  // Suppose que la méthode existe(ID_T) retourne true si l'ID existe
        QMessageBox::warning(this, "Erreur", "Aucun médicament trouvé avec cet ID.");
        return;
    }

    // Si l'ID est valide, procéder à la suppression
    bool test = m.supprimer(ID_T);

    // Si la suppression est réussie
    if (test) {
        // Mettre à jour l'affichage des médicaments dans la table
        ui->tableView->setModel(m.afficher());  // Rafraîchir la vue du tableau

        // Message de succès
        QMessageBox::information(this, "Succès", "Médicament supprimé avec succès!");
        QFile file("C:/Users/tasn8/Desktop/tasnim/ficher.txt");
                    if (!file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
                        qDebug() << "Impossible d'ouvrir le fichier.";
                        return;
                                  }
                    // Obtenir la date et l'heure actuelles
                               QDateTime currentDateTime = QDateTime::currentDateTime();
                               QString formattedDateTime = currentDateTime.toString("dd.MM.yyyy hh:mm:ss");

                               // Écrire la date et l'heure actuelles suivies du texte dans le fichier
                               QTextStream out(&file);
                               out << "<font color=\"red\">Le medicament avec le id : " << idStr << " a ete supprime le " << formattedDateTime << "</font> \n"; // endl ajoute un saut de ligne
                              file.close();
    } else {
        // Message d'erreur si la suppression échoue
        QMessageBox::critical(this, "Erreur", "Échec de la suppression du médicament.");
    }
}




/*

// Méthode pour modifier un médicament
void MainWindow::on_pushButton_modifier_clicked()
{
    // Récupérer les données saisies par l'utilisateur
    int vignette = ui->lineEdit_2->text().toInt();  // L'ID du médicament (vignette)
    QString attribut = ui->comboBox_MOD->currentText();  // L'attribut à modifier (nom, dosage, etc.)
    QString nouvelleValeur = ui->lineEdit_3->text();  // La nouvelle valeur


    // Vérifier que l'ID (vignette) n'est pas vide
    if (vignette == 0) {
        QMessageBox::warning(this, "Erreur", "Veuillez entrer un ID valide pour le médicament.");
        return;
    }

    // Vérifier que la nouvelle valeur n'est pas vide
    if (nouvelleValeur.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez entrer la nouvelle valeur pour l'attribut.");
        return;
    }

    // Vérifier si le médicament existe déjà dans la base de données
    medicament m;
    if (!m.existe(vignette)) {
        QMessageBox::warning(this, "Erreur", "Aucun médicament trouvé avec cette vignette.");
        return;
    }

    // Appeler la méthode modifier de la classe medicament pour effectuer la modification
    QVariant nouvelleValeurVariant = QVariant(nouvelleValeur);
    bool success = m.modifier(vignette, attribut, nouvelleValeurVariant);


    if (success) {
        // Si la modification réussit, afficher un message et rafraîchir la vue
        QMessageBox::information(this, "Succès", "Le médicament a été modifié avec succès.");
        QFile file("C:/Users/tasn8/Desktop/tasnim/ficher.txt");
                    if (!file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
                        qDebug() << "Impossible d'ouvrir le fichier.";
                        return;
                                  }
                    // Obtenir la date et l'heure actuelles
                               QDateTime currentDateTime = QDateTime::currentDateTime();
                               QString formattedDateTime = currentDateTime.toString("dd.MM.yyyy hh:mm:ss");

                               // Écrire la date et l'heure actuelles suivies du texte dans le fichier
                               QTextStream out(&file);
                               out << "<font color=\"yellow\">Le medicament avec l'ID : " << vignette << " a ete modifie le " << formattedDateTime << "</font> \n" ; // endl ajoute un saut de ligne

                              file.close();
        ui->tableView->setModel(m.afficher());  // Mettre à jour l'affichage des médicaments
    } else {
        // Si la modification échoue, afficher un message d'erreur
        QMessageBox::critical(this, "Erreur", "Échec de la modification du médicament.");
    }
}
*/

void MainWindow::on_pdf_clicked()
{
    mt.exportDataToPDF();
}

void MainWindow::on_stats_clicked()
{
    QLayoutItem* item;
   while ((item = ui->stats_3->layout()->takeAt(0)) != nullptr) {
         delete item->widget();
        delete item;
    }




   QString selectedOption = ui->comboBox_3->currentText();
   QChartView *chartView = nullptr;

   if (selectedOption == "Prix") {
       // Create the pie chart based on 'etat'
       chartView = mt.createPieChartPrix();
   } else if (selectedOption == "Stock") {
       // Create the pie chart based on 'poids'
       chartView = mt.createBarChartByFamily();
   }

   // Add the chart view to the layout if it was successfully created
   if (chartView != nullptr) {
       ui->stats_3->layout()->addWidget(chartView);
   }





}

void MainWindow::on_comboBox_currentIndexChanged(const QString &arg1)
{

}

void MainWindow::on_comboBox_currentTextChanged(const QString &arg1)
{
    QSqlQueryModel* sortedModel = mt.trier(arg1);

       // Assuming you have a QTableView named tableView in your MainWindow UI to display the sorted data
       ui->tableView->setModel(sortedModel);
}



void MainWindow::on_le_recherche_textChanged(const QString &arg1)
{
    QString selectedOption = ui->comboBox_2->currentText(); // Get the selected search option

    if (arg1.isEmpty() && ui->le_recherche->hasFocus() && !ui->le_recherche->hasSelectedText()) {
        ui->tableView->setModel(mt.afficher());
        ui->le_recherche->setToolTip("Entrez un critère de recherche");
        return; // Exit early if there's no input
    }

    // Call the rechercher function with the selected option and input value
    ui->tableView->setModel(mt.rechercher(selectedOption, arg1));
    ui->tableView->clearSelection();
}

void MainWindow::on_histo_clicked()
{
    QDialog *dialog = new QDialog(this);
       dialog->resize(600, 600);
       QVBoxLayout *layout = new QVBoxLayout(dialog);
        QTextEdit *textEdit = new QTextEdit(dialog);

       // Ouvrir le fichier en lecture seule
       QFile file("C:/Users/tasn8/Desktop/tasnim/ficher.txt");

       if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
           // Lire le contenu du fichier ligne par ligne et l'ajouter à QTextEdit
           QTextStream in(&file);
           while (!in.atEnd()) {
               QString line = "evenement : "+in.readLine();
               textEdit->append(line);
           }
           // Fermer le fichier
           file.close();
       } else {
           // Afficher un message d'erreur si le fichier n'a pas pu être ouvert
           qDebug() << "Erreur lors de l'ouverture du fichier.";
       }
       layout->addWidget(textEdit);
       dialog->exec();

}

void MainWindow::on_map_2_clicked()
{

}

void MainWindow::on_modifier_f_clicked()
{
    // Récupérer les données saisies par l'utilisateur
    int vignette = ui->lineEdit_2->text().toInt();  // L'ID du médicament (vignette)
    QString attribut = ui->comboBox_MOD->currentText();  // L'attribut à modifier (nom, dosage, etc.)
    QString nouvelleValeur = ui->lineEdit_3->text();  // La nouvelle valeur


    // Vérifier que l'ID (vignette) n'est pas vide
    if (vignette == 0) {
        QMessageBox::warning(this, "Erreur", "Veuillez entrer un ID valide pour le médicament.");
        return;
    }

    // Vérifier que la nouvelle valeur n'est pas vide
    if (nouvelleValeur.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez entrer la nouvelle valeur pour l'attribut.");
        return;
    }

    // Vérifier si le médicament existe déjà dans la base de données
    medicament m;
    if (!m.existe(vignette)) {
        QMessageBox::warning(this, "Erreur", "Aucun médicament trouvé avec cette vignette.");
        return;
    }

    // Appeler la méthode modifier de la classe medicament pour effectuer la modification
    QVariant nouvelleValeurVariant = QVariant(nouvelleValeur);
    bool success = m.modifier(vignette, attribut, nouvelleValeurVariant);


    if (success) {
        // Si la modification réussit, afficher un message et rafraîchir la vue
        QMessageBox::information(this, "Succès", "Le médicament a été modifié avec succès.");
        QFile file("C:/Users/tasn8/Desktop/tasnim/ficher.txt");
                    if (!file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
                        qDebug() << "Impossible d'ouvrir le fichier.";
                        return;
                                  }
                    // Obtenir la date et l'heure actuelles
                               QDateTime currentDateTime = QDateTime::currentDateTime();
                               QString formattedDateTime = currentDateTime.toString("dd.MM.yyyy hh:mm:ss");

                               // Écrire la date et l'heure actuelles suivies du texte dans le fichier
                               QTextStream out(&file);
                               out << "<font color=\"yellow\">Le medicament avec l'ID : " << vignette << " a ete modifie le " << formattedDateTime << "</font> \n" ; // endl ajoute un saut de ligne

                              file.close();
        ui->tableView->setModel(m.afficher());  // Mettre à jour l'affichage des médicaments
    } else {
        // Si la modification échoue, afficher un message d'erreur
        QMessageBox::critical(this, "Erreur", "Échec de la modification du médicament.");
    }
}


void MainWindow::on_pushButton_3_clicked()
{

}


void MainWindow::on_upload_image_2_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Sélectionnez une image", "", "Images (*.png *.jpg *.jpeg)");
    if (filePath.isEmpty()) { return; // Aucun fichier sélectionné
    }
    // Lire le fichier image et l'encoder en base64
        QFile file(filePath); if (!file.open(QIODevice::ReadOnly)) { QMessageBox::critical(this, "Erreur", "Impossible d'ouvrir l'image sélectionnée.");
            return;
        }
        QByteArray imageData = file.readAll();
        QString encodedImage = QString(imageData.toBase64()); // Affecter l'image encodée à un objet medicament
        QString nom = ui->lineEdit_nom->text();
        int vignette = ui->lineEdit_vignette->text().toInt();
        QString famille = ui->lineEdit_famille->text();
        QString date_fabrication = ui->lineEdit_date_fabrication->text();
        QString date_expiration = ui->lineEdit_date_expiration->text();
        QString seuil_min = ui->lineEdit_seuil_min->text();
        int dosage = ui->lineEdit_dosage->text().toInt();
        int prix = ui->lineEdit_prix->text().toDouble();
        int stock = ui->lineEdit_stock->text().toInt();
        medicament med(nom, vignette, date_fabrication, date_expiration, seuil_min, dosage, famille,encodedImage, prix, stock);
        if (med.ajouter()) {

            QMessageBox::information(this, "Succès", "Médicament ajouté avec image.");
            ui->tableView->setModel(med.afficher());
        } else {

            QMessageBox::critical(this, "Erreur", "Échec de l'ajout du médicament.");
        }
    }


// ************* encoder image base 64 *********************


QString MainWindow::encodeImageToBase64(const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, "Erreur", "Impossible d'ouvrir l'image.");
        return "";
    }

    QByteArray imageData = file.readAll(); // stocker les datas en binaires

    file.close();
    return imageData.toBase64(); // Convertir en Base64
}


// ********** afficher image ***************

void MainWindow::on_afficher_image_clicked()
{
    int id = ui->id_image->text().toInt(); // Récupère l'ID depuis un champ d'entrée (par exemple, un QLineEdit)

    medicament e;
    QString base64Data = e.getImageById(id); // Récupère l'image en Base64 depuis la base de données

    if (base64Data.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Aucune image trouvée pour cet ID.");
        return;
    }

    QByteArray imageData = QByteArray::fromBase64(base64Data.toUtf8()); // Décodage Base64
    QPixmap pixmap;
    if (!pixmap.loadFromData(imageData)) {
        QMessageBox::critical(this, "Erreur", "Échec du chargement de l'image.");
        return;
    }

    ui->imm->setPixmap(pixmap.scaled(ui->imm->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation)); // Affiche l'image
}

