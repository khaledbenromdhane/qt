#include "medicament.h"
#include <QString>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QSqlQueryModel>
#include <utility>
#include <QVariant>
#include <QStringList>
#include <QPrinter>
#include <QFileDialog>
#include <QTextDocument>
#include <QPainter>
#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QChart>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts>
#include <QFile>

// Constructeur par défaut
medicament::medicament() {
    // Initialisation des membres (si nécessaire)
    vignette = 0;
    dosage = 0;
    // Vous pouvez initialiser d'autres membres si nécessaire
}



medicament::medicament(QString nom, int vignette, QString date_fabrication, QString date_expiration, QString seuil_min, int dosage, QString famille,QString image,int prix,int stock )
{
    this->vignette = vignette;             // Initialize vignette
    this->dosage = dosage;                 // Initialize dosage
    this->nom = nom;                       // Initialize name of the medicine
    this->famille = famille;               // Initialize family (category)
    this->date_fabrication = date_fabrication;  // Initialize manufacturing date
    this->date_expiration = date_expiration;    // Initialize expiration date
    this->seuil_min = seuil_min;
    this->prix = prix;
    this->stock = stock;
    this->image = image;

}

QString medicament::getImageById(int id) {
    QSqlQuery query;
    query.prepare("SELECT image FROM medicament1 WHERE vignette = :id");
    query.bindValue(":id", id);

    if (query.exec() && query.next()) {
        return query.value(0).toString(); // Retourne la chaîne Base64
    }

    return QString(); // Retourne une chaîne vide si l'ID est introuvable
}

bool medicament::ajouter()
{
    QSqlQuery query;

    // Préparation de la requête SQL pour insérer un médicament
    query.prepare("INSERT INTO MEDICAMENT1 (VIGNETTE, NOM, FAMILLE, DOSAGE, DATE_FABRICATION, DATE_EXPIRATION, SEUIL_MIN,PRIX,STOCK,IMAGE) "
                  "VALUES (:vignette, :nom, :famille, :dosage, :date_fabrication, :date_expiration, :seuil_min, :prix, :stock, :image )");

    // Liaison des variables de l'objet à la requête SQL
    query.bindValue(":vignette", vignette);                         // ID (vignette)
    query.bindValue(":nom", nom);                                     // NOM
    query.bindValue(":famille", famille);                             // FAMILLE
    query.bindValue(":dosage", QString::number(dosage));              // DOSAGE (int, converti en QString)
    query.bindValue(":date_fabrication", date_fabrication);           // DATE_FABRICATION
    query.bindValue(":date_expiration", date_expiration);             // DATE_EXPIRATION
    query.bindValue(":seuil_min", seuil_min);                         // SEUIL_MIN (string)
    query.bindValue(":prix", prix);                         // SEUIL_MIN (string)
    query.bindValue(":stock", stock);                         // SEUIL_MIN (string)
    query.bindValue(":image",image);

    // Exécution de la requête et retour de la réussite
    return query.exec();
}



bool medicament::modifier(int vignette, const QString& attribut, const QVariant& nouvelleValeur) {
    QSqlQuery query;

    // Vérification si le médicament existe dans la base de données
    QSqlQuery checkQuery;
    checkQuery.prepare("SELECT COUNT(*) FROM medicament1 WHERE vignette = :vignette");
    checkQuery.bindValue(":vignette", vignette);
    if (!checkQuery.exec()) {
        qDebug() << "Erreur de vérification de la vignette:" << checkQuery.lastError().text();
        return false;
    }

    checkQuery.next();
    if (checkQuery.value(0).toInt() == 0) {
        qDebug() << "Aucun médicament trouvé avec la vignette:" << vignette;
        return false;
    }

    // Préparer la requête de mise à jour
    QString queryStr = QString("UPDATE medicament1 SET %1 = :nouvelleValeur WHERE vignette = :vignette")
                              .arg(attribut);
    query.prepare(queryStr);
    query.bindValue(":nouvelleValeur", nouvelleValeur);
    query.bindValue(":vignette", vignette);

    qDebug() << "Requête SQL : " << query.lastQuery();  // Debug: Afficher la requête SQL

    // Exécuter la requête et vérifier les erreurs
    if (!query.exec()) {
        qDebug() << "Erreur de modification:" << query.lastError().text();
        return false;
    }

    qDebug() << "Modification effectuée pour le médicament1 avec vignette:" << vignette;
    return true;
}

bool medicament::modifierAttribut(int vignette, const QString &attribut, const QString &nouvelleValeur) {
    QStringList attributsValides = {"NOM", "FAMILLE", "DOSAGE", "DATE_FABRICATION", "DATE_EXPIRATION", "SEUIL_MIN","PRIX","STOCK"};
    if (!attributsValides.contains(attribut)) {
        qDebug() << "Erreur : l'attribut spécifié n'est pas valide.";
        return false;
    }

    // Vérifier si le médicament existe
    QSqlQuery checkQuery;
    checkQuery.prepare("SELECT COUNT(*) FROM MEDICAMENT1 WHERE VIGNETTE = :vignette");
    checkQuery.bindValue(":vignette", vignette);
    if (!checkQuery.exec() || !checkQuery.next() || checkQuery.value(0).toInt() == 0) {
        qDebug() << "Erreur : Aucun médicament trouvé avec cette vignette.";
        return false;
    }

    // Préparer la requête
    QSqlQuery query;
    QString queryStr = QString("UPDATE MEDICAMENT SET %1 = :nouvelleValeur WHERE VIGNETTE = :vignette").arg(attribut);
    if (!query.prepare(queryStr)) {
        qDebug() << "Erreur lors de la préparation de la requête : " << query.lastError().text();
        return false;
    }

    // Conversion de la nouvelle valeur selon le type de l'attribut
    QVariant valeur;
    if (attribut == "PRIX" || attribut == "STOCK") {
        valeur = nouvelleValeur.toDouble();
    } else if (attribut == "DATE_FABRICATION" || attribut == "DATE_EXPIRATION") {
        valeur = QDate::fromString(nouvelleValeur, "yyyy-MM-dd");
    } else {
        valeur = nouvelleValeur;
    }

    query.bindValue(":nouvelleValeur", valeur);
    query.bindValue(":vignette", vignette);

    // Exécution de la requête
    QSqlDatabase db = QSqlDatabase::database();
    if (db.transaction()) {
        if (query.exec()) {
            db.commit();
            qDebug() << "Modification réussie.";
            return true;
        } else {
            db.rollback();
            qDebug() << "Erreur d'exécution : " << query.lastError().text();
            return false;
        }
    } else {
        qDebug() << "Erreur : Impossible de démarrer une transaction.";
        return false;
    }
}

QSqlQueryModel* medicament::afficher() {
    QSqlQueryModel *model = new QSqlQueryModel();
    QSqlQuery query;

    query.prepare("SELECT vignette, nom, famille, date_fabrication, date_expiration, seuil_min, dosage, stock, prix FROM medicament1");

    if (query.exec()) {
        model->setQuery(std::move(query));
        model->setHeaderData(0, Qt::Horizontal, QObject::tr("Vignette"));
        model->setHeaderData(1, Qt::Horizontal, QObject::tr("Nom"));
        model->setHeaderData(2, Qt::Horizontal, QObject::tr("Famille"));
        model->setHeaderData(3, Qt::Horizontal, QObject::tr("Date Fabrication"));
        model->setHeaderData(4, Qt::Horizontal, QObject::tr("Date Expiration"));
        model->setHeaderData(5, Qt::Horizontal, QObject::tr("Seuil Min"));
        model->setHeaderData(6, Qt::Horizontal, QObject::tr("Dosage"));
        model->setHeaderData(7, Qt::Horizontal, QObject::tr("Stock"));
        model->setHeaderData(8, Qt::Horizontal, QObject::tr("Prix"));



    } else {
        qDebug() << "Erreur lors de l'exécution de la requête:" << query.lastError().text();
    }

    return model;
}

bool medicament::supprimer(int vignette)
{
    // Assurez-vous que la connexion à la base de données est ouverte
    if (!QSqlDatabase::database().isOpen()) {
        qDebug() << "La connexion à la base de données est fermée!";
        return false;
    }

    QSqlQuery query;

    // Préparation de la requête SQL DELETE pour supprimer un médicament
    query.prepare("DELETE FROM MEDICAMENT1 WHERE VIGNETTE = :vignette");
    query.bindValue(":vignette", vignette);  // Ici, on passe directement l'ID

    // Exécution de la requête et gestion des erreurs
    if (query.exec()) {
        qDebug() << "Médicament supprimé avec succès.";
        return true;
    } else {
        // Si l'exécution échoue, afficher l'erreur dans le debug
        qDebug() << "Erreur lors de la suppression du médicament :" << query.lastError().text();
        return false;
    }
}

bool medicament::existe( int vignette) {
    QSqlQuery query;
    query.prepare("SELECT * FROM MEDICAMENT1 WHERE vignette = :vignette");
    query.bindValue(":vignette", vignette);

    if (!query.exec()) {
        qDebug() << "Erreur : impossible d'exécuter la requête SELECT.";
        return false;
    }

    return query.next();  // Si la vignette existe, on retourne true
}
void medicament::exportDataToPDF()
{
    QSqlQuery query;

    // Prompt user to select file location
    QString fileName = QFileDialog::getSaveFileName(nullptr, "Save PDF", "", "PDF Files (*.pdf)");
    if (fileName.isEmpty()) {
        qDebug() << "No file selected. Aborting PDF generation.";
        return;
    }

    // Prepare query to fetch data (only Vignette, Nom, and Stock) from the MEDICAMENT table
    query.prepare("SELECT vignette, nom, stock FROM MEDICAMENT1");

    // Create a QPrinter for PDF generation
    QPrinter printer(QPrinter::PrinterResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);

    QPainter painter;
    if (!painter.begin(&printer)) {
        qDebug() << "Failed to begin painting on printer.";
        return;
    }

    // Pharmacy-specific styling for the PDF
    QString html = R"(
        <html>
        <head>
            <style>
                body {
                    font-family: 'Arial', sans-serif;
                    margin: 0; /* Remove default margins */
                    padding: 0;
                    display: flex;
                    justify-content: center; /* Horizontally center the container */
                    align-items: center; /* Vertically center the container */
                    height: 100vh; /* Full-page height */
                    background-color: #f9f9f9; /* Optional: Light background for contrast */
                }
                .container {
                    width: 60%; /* Width of the container */
                    max-width: 800px; /* Maximum width */
                    background-color: white;
                    padding: 2px 2px; /* Padding left and right */
                    box-shadow: 0px 4px 6px rgba(0, 0, 0, 0.1); /* Optional: Shadow for style */
                    border-radius: 8px; /* Optional: Rounded corners */
                }
                h1 {
                    text-align: center;
                    color: #4CAF50;
                    margin-bottom: 20px;
                }
                table {
                    width: 60%; /* Full width inside the container */
                    border-collapse: collapse;
                    margin-top: 20px;
                }
                th, td {
                    border: 1px solid #ddd;
                    padding: 10px;
                    text-align: center;
                }
                th {
                    background-color: #4CAF50;
                    color: white;
                }
                tr:nth-child(even) {
                    background-color: #f2f2f2;
                }
                tr:hover {
                    background-color: #ddd;
                }
            </style>
        </head>
        <body>
            <div class="container">
                <h1>Pharmacy - Medicament Inventory</h1>
                <table>
                    <tr>
                        <th>Vignette</th>
                        <th>Nom</th>
                        <th>Stock</th>
                    </tr>
    )";

    // Execute the query and fetch rows
    if (query.exec()) {
        while (query.next()) {
            html += QString(R"(
                <tr>
                    <td>%1</td>
                    <td>%2</td>
                    <td>%3</td>
                </tr>
            )")
                .arg(query.value("vignette").toString())
                .arg(query.value("nom").toString())
                .arg(query.value("stock").toString());
        }
    } else {
        qDebug() << "Error retrieving data for MEDICAMENT table:" << query.lastError().text();
    }

    html += R"(
            </table>
        </body>
        </html>
    )";

    // Create a QTextDocument and set its HTML content
    QTextDocument document;
    document.setHtml(html);

    // Draw the document to the painter
    document.drawContents(&painter);
    painter.end();

    qDebug() << "PDF generated successfully!";
}

QChartView* medicament::createPieChartPrix() {
    // Initialize price categories counters
    int prixRange1 = 0; // 0-20
    int prixRange2 = 0; // 21-50
    int prixRange3 = 0; // 51-100
    int prixRange4 = 0; // 101+
    int total = 0;      // Initialize total to 0

    QPieSeries *series = new QPieSeries();

    // Query to get all prices from the MEDICAMENT table
    QSqlQuery query("SELECT PRIX FROM MEDICAMENT1");

    // Loop through the results to categorize prices
    while (query.next()) {
        float prix = query.value(0).toFloat();

        if (prix >= 0 && prix <= 20) {
            prixRange1++;
        } else if (prix > 20 && prix <= 50) {
            prixRange2++;
        } else if (prix > 50 && prix <= 100) {
            prixRange3++;
        } else {
            prixRange4++;
        }
    }

    // Calculate percentages and add each price range as a slice in the pie chart
        if (prixRange1 > 0) {
            float percentage1 = (static_cast<float>(prixRange1) / total) * 100;
            QPieSlice *slice1 = series->append(QString("0-20DT: %1%").arg(percentage1, 0, 'f', 1), prixRange1);
            slice1->setBrush(QColor("#4CAF50")); // Pharmacy green
        }

        if (prixRange2 > 0) {
            float percentage2 = (static_cast<float>(prixRange2) / total) * 100;
            QPieSlice *slice2 = series->append(QString("21-50DT: %1%").arg(percentage2, 0, 'f', 1), prixRange2);
            slice2->setBrush(QColor("#8BC34A")); // Light green
        }

        if (prixRange3 > 0) {
            float percentage3 = (static_cast<float>(prixRange3) / total) * 100;
            QPieSlice *slice3 = series->append(QString("51-100DT: %1%").arg(percentage3, 0, 'f', 1), prixRange3);
            slice3->setBrush(QColor("#FFEB3B")); // Pharmacy yellow
        }

        if (prixRange4 > 0) {
            float percentage4 = (static_cast<float>(prixRange4) / total) * 100;
            QPieSlice *slice4 = series->append(QString("101DT+: %1%").arg(percentage4, 0, 'f', 1), prixRange4);
            slice4->setBrush(QColor("#F44336")); // Pharmacy red
        }


    // Create and set up the chart
    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Répartition des Médicaments par Prix (€)");
    chart->setTitleFont(QFont("Arial", 14, QFont::Bold));
    chart->setBackgroundBrush(QColor("#F9F9F9")); // Light background
    chart->legend()->setAlignment(Qt::AlignRight);
    chart->legend()->setFont(QFont("Arial", 10));
    series->setLabelsVisible();

    // Customize animations for a smooth appearance
    chart->setAnimationOptions(QChart::SeriesAnimations);

    // Set up chart view
    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setMinimumSize(600, 400); // Ensure a good default size

    return chartView;
}

QChartView* medicament::createBarChartByFamily() {
    // Initialize counters for each family category
    QMap<QString, int> familyCount;
    int total = 0; // Total count for percentage calculation

    // Query to select all families from the MEDICAMENT table
    QSqlQuery query("SELECT famille FROM MEDICAMENT1");

    // Process the query result
    while (query.next()) {
        QString family = query.value(0).toString();
        familyCount[family]++;
        total++; // Count each medicine
    }

    // Create a bar set for the chart
    QBarSet *familySet = new QBarSet("Familles");

    // Populate the bar set with percentage values
    QStringList categories;
    for (auto it = familyCount.begin(); it != familyCount.end(); ++it) {
        float percentage = (static_cast<float>(it.value()) / total) * 100;
        *familySet << percentage;
        categories << it.key();
    }

    // Create a bar series and add the set to it
    QBarSeries *series = new QBarSeries();
    series->append(familySet);

    // Create a chart and add the series to it
    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Répartition des Médicaments par Famille en Pourcentage");
    chart->setTitleFont(QFont("Arial", 14, QFont::Bold));
    chart->setAnimationOptions(QChart::SeriesAnimations);

    // Set up categories for the x-axis
    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    // Configure the y-axis to show percentage
    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("Pourcentage des Médicaments (%)");
    axisY->setLabelFormat("%.1f"); // Display as a percentage with one decimal
    axisY->setRange(0, 100); // Set range to 0-100 for percentage
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    // Create a chart view
    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    return chartView;
}


QSqlQueryModel* medicament::trier(const QString& sortColumn) {
    QSqlQueryModel* model = new QSqlQueryModel();
    QString queryStr;

    // Construct the SQL query based on the sort column
    if (sortColumn == "dosage") {
        queryStr = "SELECT vignette, nom, famille, date_fabrication, date_expiration, seuil_min, dosage, stock, prix FROM MEDICAMENT1 ORDER BY dosage";
    } else if (sortColumn == "vignette") {
        queryStr = "SELECT vignette, nom, famille, date_fabrication, date_expiration, seuil_min, dosage, stock, prix FROM MEDICAMENT1 ORDER BY vignette";
    } else if (sortColumn == "nom") {
        queryStr = "SELECT vignette, nom, famille, date_fabrication, date_expiration, seuil_min, dosage, stock, prix FROM MEDICAMENT1 ORDER BY nom";
    } else {
        queryStr = "SELECT vignette, nom, famille, date_fabrication, date_expiration, seuil_min, dosage, stock, prix FROM MEDICAMENT1 ORDER BY date_expiration";
    }

    // Display the query string for debugging
    qDebug() << "SQL Query for sorting:" << queryStr;

    QSqlQuery query(queryStr);
    model->setQuery(query);

    // Set headers for the model
    model->setHeaderData(0, Qt::Horizontal, QObject::tr("Vignette"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Nom"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Famille"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Date Fabrication"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("Date Expiration"));
    model->setHeaderData(5, Qt::Horizontal, QObject::tr("Seuil Min"));
    model->setHeaderData(6, Qt::Horizontal, QObject::tr("Dosage"));
    model->setHeaderData(7, Qt::Horizontal, QObject::tr("Stock"));
    model->setHeaderData(8, Qt::Horizontal, QObject::tr("Prix44"));

    return model; // Return the model with sorted data
}



QSqlQueryModel* medicament::rechercher(const QString& searchType, const QString& value) {
    QSqlQueryModel *model = new QSqlQueryModel();
    QSqlQuery query;

    QString columnName;
    if (searchType == "vignette") {
        columnName = "vignette";
    } else if (searchType == "nom") {
        columnName = "nom";
    } else if (searchType == "famille") {
        columnName = "famille";
    } else if (searchType == "dosage") {
        columnName = "dosage";
    } else if (searchType == "date_fabrication") {
        columnName = "date_fabrication";
    } else if (searchType == "date_expiration") {
        columnName = "date_expiration";
    } else if (searchType == "seuil_min") {
        columnName = "seuil_min";
    } else if (searchType == "prix") {
        columnName = "prix";
    } else if (searchType == "stock") {
        columnName = "stock";
    } else {
        qDebug() << "Type de recherche invalide.";
        return model; // Retourne un modèle vide si le type de recherche est invalide
    }

    // Display the query parameters for debugging
    qDebug() << "Search Type:" << searchType << "Value:" << value;

    query.prepare(QString("SELECT vignette, nom, famille, dosage, date_fabrication, date_expiration, seuil_min, prix, stock "
                          "FROM MEDICAMENT1 WHERE %1 LIKE :value").arg(columnName));
    query.bindValue(":value", "%" + value + "%");

    // Display the prepared query for debugging
    qDebug() << "SQL Query for search:" << query.executedQuery();

    if (query.exec()) {
        model->setQuery(query);
        model->setHeaderData(0, Qt::Horizontal, QObject::tr("Vignette"));
        model->setHeaderData(1, Qt::Horizontal, QObject::tr("Nom"));
        model->setHeaderData(2, Qt::Horizontal, QObject::tr("Famille"));
        model->setHeaderData(3, Qt::Horizontal, QObject::tr("Date Fabrication"));
        model->setHeaderData(4, Qt::Horizontal, QObject::tr("Date Expiration"));
        model->setHeaderData(5, Qt::Horizontal, QObject::tr("Seuil Min"));
        model->setHeaderData(6, Qt::Horizontal, QObject::tr("Dosage"));
        model->setHeaderData(7, Qt::Horizontal, QObject::tr("Stock"));
        model->setHeaderData(8, Qt::Horizontal, QObject::tr("Prix"));



    } else {
        qDebug() << "Erreur lors de l'exécution de la requête:" << query.lastError().text();
    }

    return model;
}
