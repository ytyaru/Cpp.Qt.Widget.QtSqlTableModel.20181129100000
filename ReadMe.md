# このソフトウェアについて

　Qt5学習。QtでSQLite3を使う。QSqlTableModel.setFilter("Field = 値")に該当するレコードのみ更新、削除した。

* `QSqlTableModel::setFilter("Field = 値")`にて対象レコードを絞り込める
    * 前回のプロセスで保存したレコードが対象に含まれない……
* `model.submitAll();`: insertRecord, setRecord, removeRecordの直後でsubmitAll()を実行しないと、その後の操作が反映されない

## 前回まで

* https://github.com/ytyaru/Cpp.Qt.Widget.QtSqlTableModel.20181129060000
* https://github.com/ytyaru/Cpp.Qt.Widget.QtSqlTableModel.20181128190000
* https://github.com/ytyaru/Cpp.Qt.Widget.QtSqlTableModel.20181128180000
* https://github.com/ytyaru/Cpp.Qt.Widget.QtSqliteDb.20181128160000
* https://github.com/ytyaru/Cpp.Qt.Widget.QtSqliteDb.20181128120000
* https://github.com/ytyaru/Cpp.Qt.Widget.QSql.SQLite3.Transaction.20181128070000
* https://github.com/ytyaru/Cpp.Qt.Widget.QSql.SQLite3.Class.20181127180000
* https://github.com/ytyaru/Cpp.Qt.Widget.QSql.SQLite3.Class.20181127170000
* https://github.com/ytyaru/Cpp.Qt.Widget.QSql.SQLite3.Class.20181127160000
* https://github.com/ytyaru/Cpp.Qt.Widget.QSql.SQLite3.Class.20181127130000

## コード抜粋

1. DB接続
2. テーブル作成
3. モデル作成
4. モデルからレコードを絞り込んで更新＆削除
5. 編集の確定
6. select文でレコード確認

## 1. DB接続

```cpp
QSqlDatabase db = QSqlDatabase::database("Memo");
```

## 2. テーブル作成

```cpp
QSqlQuery query(db);
query.exec(tr("create table Memo(id INTEGER PRIMARY KEY AUTOINCREMENT, Memo TEXT, Created TEXT)"));
query.exec(tr("insert into Memo(Memo,Created) values('メモ内容だよ', '1999-12-31 23:59:59')"));
```

## 3. モデル作成

```cpp
QSqlTableModel model(nullptr, db);
model.setTable("Memo");
model.setEditStrategy(QSqlTableModel::EditStrategy::OnManualSubmit); // これがないとmodel.removeRow, removeRowsが反映されない
model.select(); // これがないとquery.exec()で発行したinsert文のデータが残る
```

## 4. モデルからレコードを絞り込んで更新＆削除

* `model.setFilter(QString("Created <= '1999-12-31 23:59:59'"));`のように条件を絞り込む

```cpp
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
```

```cpp
// 指定した条件のレコードを削除する
model.setFilter(QString("Created = '1999-06-15 12:00:00'"));
qDebug() << model.rowCount();
model.removeRows(0, model.rowCount()); // filterに該当する全件削除
qDebug() << model.lastError().text();
```

　条件式がSQL文のwhere句と同様に文字列で入力するのが残念。`model.where(QSqlField, ...)`みたいにできたら良かったのに。

## 5. 編集の確定

```cpp
if( model.submitAll() ) {
    model.database().commit();
    qDebug() << "commit()";
} else {
    model.database().rollback();
    qDebug() << "rollback()";
}
```

## 6. select文でレコード確認

```cpp
query.exec(tr("select * from Memo"));
while (query.next()) {
    qDebug() << query.value(0) << "|" << query.value(1) << "|" << query.value(2);
}
```

# 問題

* 前回保存したレコードが対象に含まれない

# 参考

* https://lists.qt-project.org/pipermail/interest/2014-February/011226.html
* https://webcache.googleusercontent.com/search?q=cache:Hc3-4LuQAV4J:https://www.qtcentre.org/threads/27353-delete-row-from-QSqlTableModel-in-QTableView+&cd=1&hl=ja&ct=clnk&gl=jp

## Qt要素

* http://doc.qt.io/qt-5/qsql.html
    * http://doc.qt.io/qt-5/qsqldatabase.html
    * http://doc.qt.io/qt-5/qsqlquery.html
    * http://doc.qt.io/qt-5/qsqlerror.html
    * http://doc.qt.io/qt-5/qsqltablemodel.html
    * http://doc.qt.io/qt-5/qsqlrecord.html
    * http://doc.qt.io/qt-5/qsqlfield.html

# 開発環境

* [Raspberry Pi](https://ja.wikipedia.org/wiki/Raspberry_Pi) 3 Model B+
    * [Raspbian](https://www.raspberrypi.org/downloads/raspbian/) GNU/Linux 9.0 (stretch) 2018-06-27
        * Qt 5.7.1

## 環境構築

* [Raspbian stretch で Qt5.7 インストールしたもの一覧](http://ytyaru.hatenablog.com/entry/2019/12/17/000000)

# ライセンス

　このソフトウェアはCC0ライセンスである。

[![CC0](http://i.creativecommons.org/p/zero/1.0/88x31.png "CC0")](http://creativecommons.org/publicdomain/zero/1.0/deed.ja)

## 利用ライブラリ

ライブラリ|ライセンス|ソースコード
----------|----------|------------
[Qt](http://doc.qt.io/)|[LGPL](http://doc.qt.io/qt-5/licensing.html)|[GitHub](https://github.com/qt)

* [参考1](https://www3.sra.co.jp/qt/licence/index.html)
* [参考2](http://kou-lowenergy.hatenablog.com/entry/2017/02/17/154720)
* [参考3](https://qiita.com/ynuma/items/e8749233677821a81fcc)

