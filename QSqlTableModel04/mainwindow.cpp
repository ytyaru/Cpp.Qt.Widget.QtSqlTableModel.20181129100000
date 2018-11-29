#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtSql/QSqlTableModel>
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlField>
#include <QDir>
#include <QDebug>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QSqlDatabase _db = QSqlDatabase::addDatabase("QSQLITE", "Memo");
    QString dbPath = QDir(QApplication::applicationDirPath()).filePath("Memo.sqlite3");
    _db.setDatabaseName(dbPath);
    qDebug() << dbPath;

    QSqlDatabase db = QSqlDatabase::database("Memo");
    QSqlQuery query(db);
    query.exec(tr("create table Memo(id INTEGER PRIMARY KEY AUTOINCREMENT, Memo TEXT, Created TEXT)"));
    query.exec(tr("insert into Memo(Memo,Created) values('メモ内容だよ1', '1999-01-01 00:00:00')"));
    query.exec(tr("insert into Memo(Memo,Created) values('メモ内容だよ2', '1999-06-15 12:00:00')"));
    query.exec(tr("insert into Memo(Memo,Created) values('メモ内容だよ3', '1999-12-31 23:59:59')"));

    QSqlTableModel model(nullptr, db);
    qDebug() << model.tableName();
    model.setTable("Memo");
    model.setEditStrategy(QSqlTableModel::EditStrategy::OnManualSubmit); // これがないとmodel.removeRow, removeRowsが反映されない
    model.select(); // これがないとquery.exec()で発行したinsert文のデータが残る
    qDebug() << model.tableName();
    qDebug() << "columnCount: " << model.columnCount();
    qDebug() << "rowCount: " << model.rowCount();

    QSqlRecord rec = model.record();
    for (int i = 0; i < rec.count(); i++) {
        qDebug() << rec.field(i) << ": " << rec.value(i);
    }
    rec.setValue("Memo", QVariant("あああ"));
    rec.setValue("Created", QVariant("2000-01-01 00:00:00"));
    model.insertRecord(0, rec); // 0行目に追加
    model.submitAll(); // なぜか以下コードの前に実行しないとinsertRecordが反映されない！

    // 指定した条件のレコードを更新する
    model.setFilter(QString("Created <= '1999-12-31 23:59:59'"));
    qDebug() << model.rowCount();
    for (int i = 0; i < model.rowCount(); i++) {
        QSqlRecord r = model.record(i);
        r.setValue("Memo", QVariant("filterで絞り込んでupdateRecordしたよ"));
        model.setRecord(i, r);
        if (!model.lastError().text().trimmed().isEmpty()) { qDebug() << model.lastError().text(); }
    }
    model.submitAll(); // なぜか以下コードの前に実行しないとsetRecordが反映されない！

    // 指定した条件のレコードを削除する
    model.setFilter(QString("Created = '1999-06-15 12:00:00'"));
    qDebug() << model.rowCount();
    model.removeRows(0, model.rowCount()); // filterに該当する全件削除
    qDebug() << model.lastError().text();

    if( model.submitAll() ) {
        model.database().commit();
        qDebug() << "commit()";
    } else {
        model.database().rollback();
        qDebug() << "rollback()";
    }

    query.exec(tr("select * from Memo"));
    while (query.next()) {
        qDebug() << query.value(0) << "|" << query.value(1) << "|" << query.value(2);
    }

    db.close();
    QSqlDatabase::removeDatabase("Memo");
}

MainWindow::~MainWindow()
{
    delete ui;
}
