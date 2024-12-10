#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "finance.h"
#include <QMessageBox>
#include <QSqlError>
#include <QRegularExpressionValidator>
#include <QValidator>
#include <QDate>
#include <QRegularExpression>
#include <QPdfWriter>
#include <QPainter>
#include <QFileDialog>
#include <QMessageBox>
#include <QAbstractItemModel>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QGraphicsTextItem>
#include <QGraphicsScene>
#include <QVector>
#include <QPieSeries>
#include <QPieSlice>
#include <QChart>
#include <QChartView>
#include <QTableView>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrlQuery>
#include <QRandomGenerator>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)

{
    ui->setupUi(this);
    networkManager = new QNetworkAccessManager(this);
    ui->tableViewtr->setModel(f.afficher());
    // Initialize QNetworkAccessManager

connect(networkManager, &QNetworkAccessManager::finished, this, &MainWindow::onNetworkReply);
  // Connect buttons to respective slots
    connect(ui->projet, &QPushButton::clicked, this, &MainWindow::on_projet_clicked);
    connect(ui->employer, &QPushButton::clicked, this, &MainWindow::on_employer_clicked);
    connect(ui->materielle, &QPushButton::clicked, this, &MainWindow::on_materielle_clicked);
    connect(ui->fournisseur, &QPushButton::clicked, this, &MainWindow::on_fournisseur_clicked);
    connect(ui->facture, &QPushButton::clicked, this, &MainWindow::on_facture_clicked);
    connect(ui->ajouter_f, &QPushButton::clicked, this, &MainWindow::on_pushButton_ajouter_clicked);
    connect(ui->supprimer_f, &QPushButton::clicked, this, &MainWindow::on_pushButton_supprimer_clicked);
    connect(ui->modifier_f, &QPushButton::clicked, this, &MainWindow::on_pushButton_modifierer_clicked);
    connect(ui->actu, &QPushButton::clicked, this, &MainWindow::on_ACTUALISER_clicked);
    connect(ui->comboBox_trii, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::on_comboBox_trii_changed);
    connect(ui->modifier_f, &QPushButton::clicked, this, &MainWindow::on_pushButton_9_clicked);
    connect(ui->generatePieChartButton, &QPushButton::clicked, this, &MainWindow::on_generatePieChartButton_clicked);



}



MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::on_send_clicked()
{
    QString phoneNumber = ui->numero->text();  // Phone number from the line edit
    QString id_t = ui->text->text();  // Additional text from the line edit

    QSqlQuery query;
    query.prepare("SELECT CATEGORIE, MONTANT FROM FINANCE WHERE ID_T = :id_t");
    query.bindValue(":id_t", id_t);

    if (!query.exec()) {
        qDebug() << "Database query failed:" << query.lastError().text();
        QMessageBox::critical(this, "Database Error", "Failed to query the database.");
        return;
    }

    if (query.next()) {
        QString categorie = query.value("CATEGORIE").toString();
        double montant = query.value("MONTANT").toDouble();

        // Check if the category is "Achat" (case-sensitive)
        if (categorie == "Achat") {
            QString message = QString("The amount for ID %1 is: %2")
                                .arg(id_t)
                                .arg(QString::number(montant, 'f', 2));

            // Send SMS with the message
            sendSMS(phoneNumber, message);
            qDebug() << "SMS sent successfully to" << phoneNumber << "with message:" << message;
        } else {
            // Log the actual category value if it's not 'Achat'
            qDebug() << "The CATEGORIE for ID_T" << id_t << "is not 'Achat' but '" << categorie << "'. No SMS sent.";
        }
    } else {
        qDebug() << "No record found for ID_T:" << id_t;
    }
}

void MainWindow::sendSMS(QString num, QString text)
{
      // Generate random key
    QString messageBody = "Your PHARMWAVE Achat: "  + text;

    QString recipient = "+216" + num;  // Format the recipient phone number
    QUrl url("https://api.twilio.com/2010-04-01/Accounts/ACeca0a69f8a04a526ccfa8c2712824b23/Messages.json");  // Replace with your new SID
    QNetworkRequest request(url);

    QString accountSid = "ACeca0a69f8a04a526ccfa8c2712824b23";  // Replace with your new Account SID
    QString authToken = "06ecf675117d6a743c37418e8177251c";   // Replace with your new Auth Token
    QString credentials = accountSid + ":" + authToken;
    QString authHeader = "Basic " + credentials.toUtf8().toBase64();

    request.setRawHeader("Authorization", authHeader.toUtf8());
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    QUrlQuery params;
    params.addQueryItem("To", recipient);
    params.addQueryItem("From", "+14797778127");  // Replace with your new Twilio number
    params.addQueryItem("Body", messageBody);

    networkManager->post(request, params.toString(QUrl::FullyEncoded).toUtf8());
}


void MainWindow::onNetworkReply(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        QMessageBox::information(this, "Success", "SMS sent successfully!");
    } else {
        QMessageBox::critical(this, "Error", "Failed to send SMS: " + reply->errorString());
    }
    reply->deleteLater();
}


// Slot to switch to "projet_3" widget
void MainWindow::on_projet_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);  // Assuming "projet_3" widget is at index 0
}

// Slot to switch to "employer_3" widget
void MainWindow::on_employer_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);  // Assuming "employer_3" widget is at index 1
}

// Slot to switch to "materielle_3" widget
void MainWindow::on_materielle_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);  // Assuming "materielle_3" widget is at index 2
}

// Slot to switch to "fournisseur_3" widget
void MainWindow::on_fournisseur_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);  // Assuming "fournisseur_3" widget is at index 3
}

// Slot to switch to "facture_3" widget
void MainWindow::on_facture_clicked()
{
    ui->stackedWidget->setCurrentIndex(4);  // Assuming "facture_3" widget is at index 4
}

void MainWindow::on_pushButton_ajouter_clicked()
{
    // Récupérer les informations saisies dans les champs
    QString ID_T = ui->lineEdit_id->text();
    QString DATE_TRANSACTION = ui->lineEdit_date->text();
    int MONTANT = ui->lineEdit_montant->text().toInt();
    QString TYPE = ui->lineEdit_tp->text();
    QString CATEGORIE = ui->lineEdit_cat->text();
    QString MODE_PAIEMENT = ui->lineEdit_mode->text();

    // Validation de l'ID_T (Doit commencer par T et suivi de 1 à 3 chiffres)
    QRegularExpression idRegex("^T\\d{1,3}$");
    QRegularExpressionValidator idValidator(idRegex);
    int pos = 0;

    if (idValidator.validate(ID_T, pos) != QValidator::Acceptable) {
        QMessageBox::warning(this, "Erreur", "L'ID doit être sous la forme T1, T12, ou T123.");
        return;
    }

    // Validation de la Date (Format dd/MM/yyyy)
    if (!QDate::fromString(DATE_TRANSACTION, "dd/MM/yyyy").isValid()) {
        QMessageBox::warning(this, "Erreur", "Veuillez entrer une date valide au format dd/MM/yyyy.");
        return;
    }

    // Validation du Montant (entre 1 et 999)
    if (MONTANT < 1 || MONTANT > 999) {
        QMessageBox::warning(this, "Erreur", "Le montant doit être un nombre entre 1 et 999.");
        return;
    }

    // Validation du Type (Doit être Revenu ou Dépense)
    if (TYPE != "Revenu" && TYPE != "Dépense") {
        QMessageBox::warning(this, "Erreur", "Le type doit être 'Revenu' ou 'Dépense'.");
        return;
    }

    // Validation de la Categorie (Doit être Achat ou Ventes)
    if (CATEGORIE != "Achat" && CATEGORIE != "Ventes") {
        QMessageBox::warning(this, "Erreur", "La catégorie doit être 'Achat' ou 'Ventes'.");
        return;
    }

    // Validation du Mode de Paiement (Doit être Espèce ou Chèque)
    if (MODE_PAIEMENT != "Espèce" && MODE_PAIEMENT != "Chèque") {
        QMessageBox::warning(this, "Erreur", "Le mode de paiement doit être 'Espèce' ou 'Chèque'.");
        return;
    }

    // Vérifier la relation entre la catégorie et le type
    if ((CATEGORIE == "Achat" && TYPE != "Dépense") || (CATEGORIE == "Ventes" && TYPE != "Revenu")) {
        QMessageBox::warning(this, "Erreur", "Si la catégorie est 'Achat', le type doit être 'Dépense'. Si la catégorie est 'Ventes', le type doit être 'Revenu'.");
        return;
    }

    // Instancier l'objet de la classe finance
    finance f(ID_T, DATE_TRANSACTION, MONTANT, TYPE, CATEGORIE, MODE_PAIEMENT);

    // Tenter d'ajouter l'élément dans la base de données
    bool test = f.ajouter();

    // Vérifier si l'ajout a réussi
    if (test) {
        ui->tableViewtr->setModel(f.afficher());
        QMessageBox::information(this, "Succès", "Ajout effectué avec succès !");
    } else {
        QMessageBox::critical(this, "Erreur", "Ajout non effectué.");
    }
}

void MainWindow::on_pushButton_supprimer_clicked()
{
    QString ID_T = ui->supp->text();

    // Vérification : si l'ID est vide, afficher un message d'erreur
    if (ID_T.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez entrer un ID.");
        return;
    }

    // Préparer la requête SQL pour vérifier si l'ID existe dans la base de données
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM FINANCE WHERE ID_T = :id");
    query.bindValue(":id", ID_T);
    if (!query.exec() || !query.next() || query.value(0).toInt() == 0) {
        QMessageBox::warning(this, "Erreur", "ID non trouvé dans la base de données.");
        return;
    }

    bool test = f.supprimer(ID_T);
    if (test) {
        ui->tableViewtr->setModel(f.afficher());
        QMessageBox::information(this, "Succès", "Suppression effectuée avec succès !");
    } else {
        QMessageBox::critical(this, "Erreur", "Suppression non effectuée.");
    }
}

void MainWindow::on_pushButton_modifierer_clicked()
{
    QString ID_T = ui->lineEdit_2->text();

    // Vérification si l'ID est vide
    if (ID_T.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez entrer un ID.");
        return;
    }

    QString attribut = ui->comboBox_MOD->currentText();
    QString nouvelleValeur = ui->lineEdit_3->text();

    if (nouvelleValeur.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez entrer une nouvelle valeur.");
        return;
    }

    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM FINANCE WHERE ID_T = :id");
    query.bindValue(":id", ID_T);
    if (!query.exec() || !query.next() || query.value(0).toInt() == 0) {
        QMessageBox::warning(this, "Erreur", "L'ID spécifié n'existe pas.");
        return;
    }

    QString requete = QString("UPDATE FINANCE SET %1 = :valeur WHERE ID_T = :id").arg(attribut);
    query.prepare(requete);
    query.bindValue(":valeur", nouvelleValeur);
    query.bindValue(":id", ID_T);

    if (query.exec()) {
        ui->tableViewtr->setModel(f.afficher());
        QMessageBox::information(this, "Succès", "Modification réussie !");
    } else {
        QMessageBox::critical(this, "Erreur", "Échec de la modification : " + query.lastError().text());
    }
}
void MainWindow::on_pushButton_modifier_f_clicked() {
    // Récupérer l'ID depuis le champ de texte
    QString ID_T = ui->lineEdit_2->text();

    // Vérifier si l'ID est vide
    if (ID_T.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez entrer un ID.");
        return;
    }

    // Récupérer l'attribut à modifier et la nouvelle valeur
    QString attribut = ui->comboBox_MOD->currentText();  // Attribut à modifier
    QString nouvelleValeur = ui->lineEdit_3->text();  // Nouvelle valeur

    // Vérifier si la nouvelle valeur est vide
    if (nouvelleValeur.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez entrer une nouvelle valeur.");
        return;
    }

    // Appeler la fonction modifier
    bool result = f.modifier(ID_T, attribut, nouvelleValeur);

    if (result) {
        //  modification est réussie actualiser la table
        ui->tableViewtr->setModel(f.afficher());
        QMessageBox::information(this, "Succès", "Modification effectuée avec succès !");
    } else {
        QMessageBox::critical(this, "Erreur", "La modification a échoué.");
    }
}

void MainWindow::on_ACTUALISER_clicked()
{
     ui->tableViewtr->setModel(f.afficher());
}


void MainWindow::on_comboBox_trii_changed(int index)
{
    QString selectedFilter = ui->comboBox_trii->currentText();

    QSqlQueryModel* model = new QSqlQueryModel();
    QSqlQuery query;

    if (selectedFilter == "Montant") {
        // Tri par Montant en ordre croissant
        query.prepare("SELECT  MONTANT, TYPE, CATEGORIE, MODE_PAIEMENT FROM FINANCE ORDER BY MONTANT ASC");
    } else if (selectedFilter == "Type") {
        // Tri par Type
        query.prepare("SELECT ID_T, DATE_TRANSACTION, MONTANT, TYPE, CATEGORIE, MODE_PAIEMENT FROM FINANCE ORDER BY TYPE");
    }

    if (query.exec()) {
        model->setQuery(query);

        // Définir les en-têtes des colonnes
        model->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
        model->setHeaderData(1, Qt::Horizontal, QObject::tr("Date Transaction"));
        model->setHeaderData(2, Qt::Horizontal, QObject::tr("Montant"));
        model->setHeaderData(3, Qt::Horizontal, QObject::tr("Type"));
        model->setHeaderData(4, Qt::Horizontal, QObject::tr("Catégorie"));
        model->setHeaderData(5, Qt::Horizontal, QObject::tr("Mode Paiement"));

        ui->tableViewtr->setModel(model);
        ui->tableViewtr->resizeColumnsToContents();
    } else {
        QMessageBox::critical(this, "Erreur", "Échec de l'application du filtre : " + query.lastError().text());
    }
}

void MainWindow::on_lineEdit_15_cursorPositionChanged(const QString& text)
{
    // Here you can adjust the search criteria. Let's assume we are searching by ID_T (which is VARCHAR)
    QString referenceRecherche = text;

    QSqlQueryModel* model = new QSqlQueryModel();

    QSqlQuery query;

    // Modify this query to search the FINANCE table
    // If you want to search by ID_T (referance in your case), you can use this
    query.prepare("SELECT ID_T, DATE_TRANSACTION, MONTANT, TYPE, CATEGORIE, MODE_PAIEMENT FROM FINANCE WHERE ID_T = :id");
    query.bindValue(":id", referenceRecherche);

    // Execute the query and update the table model
    if (query.exec())
    {
        model->setQuery(query);

        // Set headers based on your FINANCE table's structure
        model->setHeaderData(0, Qt::Horizontal, QObject::tr("ID_T"));
        model->setHeaderData(1, Qt::Horizontal, QObject::tr("Date Transaction"));
        model->setHeaderData(2, Qt::Horizontal, QObject::tr("Montant"));
        model->setHeaderData(3, Qt::Horizontal, QObject::tr("Type"));
        model->setHeaderData(4, Qt::Horizontal, QObject::tr("Catégorie"));
        model->setHeaderData(5, Qt::Horizontal, QObject::tr("Mode Paiement"));

        // Display the results in your tableViewtr
        ui->tableViewtr->setModel(model);
        ui->tableViewtr->resizeColumnsToContents();
    }
    else
    {
        // Show an error message if the query fails
        QMessageBox::critical(this, "Erreur", "Échec de la recherche : " + query.lastError().text());
    }
}
void MainWindow::on_pushButton_9_clicked() {
    QString searchID = ui->CHER->text(); // Assuming you have a QLineEdit for entering the ID

    if (searchID.isEmpty()) {
        QMessageBox::warning(this, "Recherche", "Veuillez entrer un ID pour effectuer la recherche.");
        return;
    }

    QSqlQuery query;
    query.prepare("SELECT * FROM FINANCE WHERE ID_T = :id");
    query.bindValue(":id", searchID);

    if (!query.exec()) {
        QMessageBox::critical(this, "Erreur", "Échec de l'exécution de la requête : " + query.lastError().text());
        return;
    }

    // Set up a model to display the results in a QTableView
    QSqlQueryModel *model = new QSqlQueryModel(this);
    model->setQuery(query);

    if (model->rowCount() == 0) {
        QMessageBox::information(this, "Résultat", "Aucun enregistrement trouvé avec cet ID.");
        delete model; // Clean up the model since it's not used
        return;
    }

    ui->tableViewtr->setModel(model); // Assuming you have a QTableView named tableViewtr
    ui->tableViewtr->resizeColumnsToContents(); // Optional: resize columns for better display
}



void MainWindow::on_generatePieChartButton_clicked() {
    finance finance;  // Create an instance of the Finance class

    // Generate the pie chart data
    QPieSeries* series = finance.generateFinanceStatistics();

    // Create a chart and add the pie series
    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Finance Statistics (Ventes vs Achats)");
    chart->legend()->setAlignment(Qt::AlignBottom);

    // Create a QChartView to display the chart
    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    // Embed the chart into the UI (inside frame_3)
    chartView->setParent(ui->frame_3);
    chartView->resize(ui->frame_3->size());
    chartView->show();
}

void MainWindow::on_pdf_clicked()
{
    finance f;  // Assuming 'Finance' is the class where 'genererPDF' is defined
    f.genererPDF();  // Call the PDF generation function
}

void MainWindow::on_question_returnPressed() {
    finance f;  // Assuming 'Finance' is the class where 'genererPDF' is defined

    QString userMessage = ui->question->text().trimmed();
    if (userMessage.isEmpty())
        return;
    ui->textBrowser->append("<span style='color: blue; font-weight: bold;'>Vous :</span> " + userMessage);
    QString botResponse = f.chatbot(userMessage);  // Use the finance chatbot now
    ui->textBrowser->append("<span style='color: green; font-weight: bold;'>Chatbot :</span> " + botResponse);
    ui->question->clear();
}





