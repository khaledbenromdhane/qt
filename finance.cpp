#include "finance.h"
#include <QString>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QSqlQueryModel>
#include <QPdfWriter>
#include <QPainter>
#include <QSqlQuery>
#include <QFileDialog>
#include <QPixmap>
#include <QRect>
#include <QFont>
#include <QColor>


finance::finance(QString id, QString dt, int mt,QString tp,QString cat,QString mode)

{
    this->id = id;
    this->dt = dt;
    this->mt = mt;
    this->tp = tp;
    this->cat = cat;
    this->mode = mode;
}

bool finance::ajouter()
{
    QSqlQuery query;

    // Préparation de la requête SQL
    query.prepare("INSERT INTO FINANCE (ID_T, DATE_TRANSACTION, MONTANT,TYPE,CATEGORIE,MODE_PAIEMENT) "
                  "VALUES (:id, :tr, :mt, :tp, :cat, :mode)");

    // Création des variables liées
    query.bindValue(":id", id);                           // Liaison de l'identifiant (id)
    query.bindValue(":tr", dt);                           // Liaison de la date de transaction (dt)
    query.bindValue(":mt", QString::number(mt));          // Conversion de mt en QString pour la liaison
    query.bindValue(":tp", tp);
    query.bindValue(":cat", cat);
    query.bindValue(":mode", mode);


    // Exécution de la requête
    return query.exec();
}


QSqlQueryModel * finance::afficher()
{
    auto * model = new QSqlQueryModel();
    QSqlQuery query;

    // Execute the query and check for errors
    if (!query.exec("SELECT * FROM FINANCE")) {
        // Log the error message
        qDebug() << "Error executing query:" << query.lastError().text();

        delete model; // Clean up in case of error
        return nullptr; // Indicate failure
    }

    // Use std::move to pass the QSqlQuery by move ID_T, DATE_TRANSACTION, MONTANT,TYPE,CATEGORIE,MODE_PAIEMENT
    model->setQuery(std::move(query));

    model->setHeaderData(0, Qt::Horizontal, QObject::tr("ID_TRANSACTION"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("DATE_TRANSACTION"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("MONTANT"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("TYPE"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("CATEGORIE"));
    model->setHeaderData(5, Qt::Horizontal, QObject::tr("MODE_PAIEMENT"));

    return model;
}

 bool finance::supprimer(QString ID_T){
     QSqlQuery query;


         // Prepare the DELETE query
         query.prepare("DELETE FROM FINANCE WHERE ID_T = :id");

         // Bind the QString ID_T directly to the query
         query.bindValue(":id", ID_T);

         // Execute the query and return the result
         return query.exec();
     }
 bool finance::existe(const QString &ID_T) {
     QSqlQuery query;

      query.prepare("SELECT * FROM FINANCE WHERE ID_T = :id");
     query.bindValue(":id", ID_T);

     query.prepare("SELECT * FROM FINANCE WHERE montant = :id");
     query.bindValue(":id", ID_T);

     if (!query.exec()) {
         qDebug() << "Erreur : impossible d'exécuter la requête SELECT.";
         return false;
     }

     return query.next();  // Si l'ID existe, on retourne true
 }

 bool finance::modifier(const QString &ID_T, const QString &attribut, const QString &nouvelleValeur) {
     QSqlQuery query;

     // Prépare la requête UPDATE en fonction de l'attribut sélectionné
     query.prepare(QString("UPDATE FINANCE SET %1 = :nouvelleValeur WHERE ID_T = :id").arg(attribut));
     query.bindValue(":nouvelleValeur", nouvelleValeur);
     query.bindValue(":id", ID_T);

     if (!query.exec()) {
         qDebug() << "Erreur : impossible d'exécuter la requête UPDATE.";
         return false;
     }

     return true;  // Modification réussie
 }
 bool finance::modifierAttribut(const QString &ID_T, const QString &attribut, const QString &nouvelleValeur) {
     QSqlQuery query;
     QString queryStr = QString("UPDATE FINANCE SET %1 = :nouvelleValeur WHERE ID_T = :id").arg(attribut);
     query.prepare(queryStr);
     query.bindValue(":nouvelleValeur", nouvelleValeur);
     query.bindValue(":id", ID_T);

     if (query.exec()) {
         qDebug() << "Modification dans la base de données réussie.";
         return true;
     } else {
         qDebug() << "Erreur de requête SQL :" << query.lastError();
         return false;
     }
 }

 QPieSeries* finance::generateFinanceStatistics() {
     qDebug() << "Début de la fonction generateFinanceStatistics()";  // Vérifier l'entrée dans la fonction

     QPieSeries *series = new QPieSeries();
     QSqlQuery query;
     int ventesCount = 0;
     int achatsCount = 0;

     // Vérification des ventes
     if (query.exec("SELECT COUNT(*) FROM FINANCE WHERE CATEGORIE = 'Ventes'")) {
         if (query.next()) {
             ventesCount = query.value(0).toInt();
             qDebug() << "Ventes Count: " << ventesCount;  // Affiche le nombre de ventes
         } else {
             qDebug() << "Aucune donnée trouvée pour 'Ventes'.";
         }
     } else {
         qDebug() << "Erreur lors de l'exécution de la requête pour 'Ventes':" << query.lastError().text();
     }

     // Vérification des achats
     if (query.exec("SELECT COUNT(*) FROM FINANCE WHERE CATEGORIE = 'Achat'")) {
         if (query.next()) {
             achatsCount = query.value(0).toInt();
             qDebug() << "Achats Count: " << achatsCount;  // Affiche le nombre d'achats
         } else {
             qDebug() << "Aucune donnée trouvée pour 'Achat'.";
         }
     } else {
         qDebug() << "Erreur lors de l'exécution de la requête pour 'Achat':" << query.lastError().text();
     }

     // Calcul du total
     int totalFinance = ventesCount + achatsCount;
     qDebug() << "Total Finance: " << totalFinance;  // Affiche le total des ventes et achats

     if (totalFinance == 0) {
         qDebug() << "Aucune donnée de finance trouvée.";
         return series;
     }

     // Ajout des données dans le graphique
     series->append("Ventes", ventesCount);
     series->append("Achats", achatsCount);

     // Personnalisation des segments du graphique
     QPieSlice *sliceVentes = series->slices().at(0);
     sliceVentes->setBrush(QColor("#4CAF50")); // Vert
     sliceVentes->setLabel(QString("Ventes (%1%)")
                                .arg((ventesCount * 100.0) / totalFinance, 0, 'f', 1));

     QPieSlice *sliceAchats = series->slices().at(1);
     sliceAchats->setBrush(QColor("#FF5722")); // Orange-Rouge
     sliceAchats->setLabel(QString("Achats (%1%)")
                                .arg((achatsCount * 100.0) / totalFinance, 0, 'f', 1));

     series->setLabelsVisible(true);
     return series;
 }

 void finance::genererPDF() {
     // Ask the user where to save the PDF file
     QString filePath = QFileDialog::getSaveFileName(nullptr, "Enregistrer le fichier PDF", "", "PDF Files (.pdf);;All Files (*)");
     if (filePath.isEmpty())
         return;

     // Set up the PDF writer
     QPdfWriter pdfWriter(filePath);
     QPainter painter(&pdfWriter);

     // Set PDF page size and orientation
     pdfWriter.setPageSize(QPageSize(QPageSize::A4));  // Make sure the page size is A4
     pdfWriter.setPageOrientation(QPageLayout::Portrait);

     // Set margins for the page
     int leftMargin = 40;
     int topMargin = 80;
     int rightMargin = 40;
     int bottomMargin = 40;

     // Start the first page of the PDF
     pdfWriter.newPage();

     // Set background color for the top section
     painter.fillRect(QRect(leftMargin, topMargin, pdfWriter.width() - rightMargin, 400), QColor(214, 231, 213));  // Light green background

     // Title font and text (black color)
     QFont titleFont("Arial", 30, QFont::Bold);
     painter.setFont(titleFont);
     painter.setPen(QColor("black"));  // Set title color to black
     painter.drawText(leftMargin + 1800, topMargin + 1100, "Transactions Financières");

     // Content font (black color)
     QFont contentFont("Arial", 10);
     painter.setFont(contentFont);
     painter.setPen(QColor("black"));  // Set content color to black

     // Database query to fetch financial transaction data
     QSqlQuery query;
     query.prepare("SELECT DATE_TRANSACTION, MONTANT, CATEGORIE, MODE_PAIEMENT FROM FINANCE");  // Remove TYPE and ID_T
     query.exec();

     // Define initial vertical position, line height, and table cell width
     int x = leftMargin + 2000;           // X-position for table
     int y = 2000;      // Y-position for table, starting after the title
     int rowHeight = 500;          // Height of each row
     int columnWidth = 1600;       // Width of each column

     // Draw table header with black color (removed ID_T and TYPE)
     painter.setFont(QFont("Arial", 12, QFont::Bold));
     painter.drawText(x, y, "DATE ");
     painter.drawText(x + columnWidth, y, "MONTANT");
     painter.drawText(x + 2 * columnWidth, y, "CATEGORIE");
     painter.drawText(x + 3 * columnWidth, y, "MODE ");

     // Draw line after header
     painter.drawLine(x, y + 20, x + 4 * columnWidth, y + 20);  // Adjusted to 4 columns

     y += rowHeight;  // Move to the next row

     // Reset font for content
     painter.setFont(QFont("Arial", 10));

     // Loop to handle multiple rows
     while (query.next()) {
         // Draw each column's data (removed ID_T and TYPE)
         painter.drawText(x, y, query.value(0).toString());  // DATE_TRANSACTION
         painter.drawText(x + columnWidth, y, query.value(1).toString());  // MONTANT
         painter.drawText(x + 2 * columnWidth, y, query.value(2).toString());  // CATEGORIE
         painter.drawText(x + 3 * columnWidth, y, query.value(3).toString());  // MODE_PAIEMENT

         // Draw borders for the current row
         painter.drawLine(x, y - rowHeight + 20, x + 4 * columnWidth, y - rowHeight + 20); // Top border
         painter.drawLine(x, y + 20, x + 4 * columnWidth, y + 20); // Bottom border

         // Draw vertical lines between columns
         for (int i = 1; i <= 3; ++i) {  // Adjusted to 3 vertical lines
             painter.drawLine(x + i * columnWidth, y - rowHeight + 20, x + i * columnWidth, y + 20);
         }

         y += rowHeight; // Move to the next row for the next record

         // Check if we need to add a new page for overflow
         if (y > pdfWriter.height() - bottomMargin) {
             pdfWriter.newPage();  // Start a new page
             y = topMargin + 120;  // Reset Y to the top of the next page
         }
     }

     // Load and scale up the logo (optional)
     QPixmap logo("C:/path_to_logo/logo.png");  // Replace with your logo path
     double scaleFactor = 2.0;  // Scale logo if needed
     int newWidth = static_cast<int>(logo.width() * scaleFactor);
     int newHeight = static_cast<int>(logo.height() * scaleFactor);

     // Draw the scaled logo in the top-right corner (optional)
     painter.drawPixmap(pdfWriter.width() - newWidth - rightMargin, topMargin + 10, newWidth, newHeight, logo);

     // End the painter to finish writing to the PDF
     painter.end();
 }


 QString finance::chatbot(const QString &userMessage) {
     QString question = userMessage.toLower().trimmed();

     // Greeting
     if (question.contains("bonjour")) {
         return "Bonjour😊! Comment puis-je vous aider avec les finances ?";
     } else if (question.contains("aide")) {
         return "Bien sûr ! Vous pouvez me poser des questions sur les finances, comme 'total des transactions', 'transactions par catégorie', 'répartition des paiements', ou 'supprimer une transaction'.";
     } else if (question.contains("merci")) {
         return "Avec plaisir ! 😊";
     }

     // Total des transactions
     else if (question.contains("total des transactions")) {
         // You can calculate total transactions with a query or function,
         // but since you don't have a direct function for it, you can use `afficher()`.
         QSqlQueryModel *model = afficher(); // Assuming afficher() gets all transactions
         double total = 0;
         for (int row = 0; row < model->rowCount(); ++row) {
             total += model->data(model->index(row, 2)).toDouble(); // Summing the `MONTANT` column
         }
         return "Le total des transactions est de " + QString::number(total) + " DT.";
     }

     // Transactions par catégorie (you could adapt based on your own logic)
     else if (question.contains("transactions par catégorie")) {
         // Assuming you use the `afficher()` function to get all transactions
         QSqlQueryModel *model = afficher();
         QMap<QString, double> repartition;
         for (int row = 0; row < model->rowCount(); ++row) {
             QString category = model->data(model->index(row, 4)).toString();  // CATEGORIE column
             double amount = model->data(model->index(row, 2)).toDouble();  // MONTANT column
             repartition[category] += amount;
         }
         QString response = "Transactions par catégorie :\n";
         for (auto it = repartition.begin(); it != repartition.end(); ++it) {
             response += it.key() + ": " + QString::number(it.value()) + " DT\n";
         }
         return response.isEmpty() ? "Aucune donnée disponible pour les catégories." : response;
     }

     // Répartition des paiements (assuming a similar logic for payment modes)
     else if (question.contains("répartition des paiements")) {
         QSqlQueryModel *model = afficher();
         QMap<QString, int> repartitionPaiement;
         for (int row = 0; row < model->rowCount(); ++row) {
             QString mode = model->data(model->index(row, 5)).toString(); // MODE_PAIEMENT column
             repartitionPaiement[mode]++;
         }
         QString response = "Répartition par mode de paiement :\n";
         for (auto it = repartitionPaiement.begin(); it != repartitionPaiement.end(); ++it) {
             response += it.key() + ": " + QString::number(it.value()) + " transactions\n";
         }
         return response.isEmpty() ? "Aucune donnée disponible pour la répartition des paiements." : response;
     }

     // Supprimer une transaction (based on `supprimer` function you already have)
     else if (question.startsWith("supprimer une transaction")) {
         return "Veuillez préciser l'ID de la transaction à supprimer.";
     } else if (question.startsWith("supprimer la transaction avec l'id")) {
         QStringList parts = question.split(" ");
         bool ok;
         QString id = parts.last();  // Get ID from the user input
         if (ok && existe(id)) { // Check if the transaction exists using `existe()`
             if (supprimer(id)) { // Call the `supprimer()` function to delete it
                 return "La transaction avec l'ID " + id + " a été supprimée avec succès.";
             } else {
                 return "Échec de la suppression de la transaction. Assurez-vous que l'ID est correct.";
             }
         }
         return "L'ID fourni n'est pas valide.";
     }

     // Rechercher une transaction par ID (similar to employee search)
     else if (question.startsWith("rechercher la transaction avec l'id")) {
         QStringList parts = question.split(" ");
         bool ok;
         QString id = parts.last();  // Get the transaction ID from the question
         if (ok && existe(id)) { // Check if the transaction exists using `existe()`
             QSqlQueryModel *model = afficher();  // Get all transactions, or implement a specific query
             QString result;
             for (int row = 0; row < model->rowCount(); ++row) {
                 if (model->data(model->index(row, 0)).toString() == id) {
                     result += "ID : " + id + "\n";
                     result += "Montant : " + model->data(model->index(row, 2)).toString() + " DT\n";
                     result += "Date : " + model->data(model->index(row, 1)).toString() + "\n";
                     result += "Catégorie : " + model->data(model->index(row, 4)).toString() + "\n";
                     result += "Mode de paiement : " + model->data(model->index(row, 5)).toString() + "\n";
                     result += "--------------------------------\n";
                 }
             }
             return result.isEmpty() ? "Aucune transaction trouvée avec l'ID " + id + "." : result;
         }
         return "L'ID fourni n'est pas valide.";
     }

     // Default response for unclear or short messages
     if (question.isEmpty() || question.length() < 3) {
         return "Veuillez poser une question plus claire.";
     }

     return "Je suis désolé, je ne comprends pas encore cela.";
 }
