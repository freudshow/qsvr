#include "libdb.h"
#include "basedef.h"

sqliteDb::sqliteDb(QObject* parent) : QObject(parent)
{
    m_sqlDb = QSqlDatabase::addDatabase("QSQLITE");
    m_sqlDb.setDatabaseName(DB_PATH);
}

sqliteDb::~sqliteDb()
{
    closeDB();
}

void sqliteDb::startThread()
{
    qDebug() << "sqliteDb::startThread: " << QThread::currentThreadId();
    openDB();
}

bool sqliteDb::openDB()
{
    return m_sqlDb.open();
}

void sqliteDb::closeDB()
{
    m_sqlDb.close();
}

void sqliteDb::queryOneRow()
{
    QString sqlStmt;
    QSqlQuery query(m_sqlDb);


    query.exec(sqlStmt);

    if (!query.first())
    {
        emit oneRowNotExist();
        qDebug() << "oneRowNotExist emitted";
        return;
    }

    emit oneRowExist();
}

int sqliteDb::rowCnt()
{
    QString sqlStmt;
    QSqlQuery query(m_sqlDb);

    if(!query.first())
        return 0;

    return query.value(0).toInt();
}

void sqliteDb::insertOneRow()
{
    bool result = false;

    if (result) {
        emit insertOK();
    } else {
        emit insertFail();
    }
}

void sqliteDb::updateOneRow()
{
    bool result = false;


    if (result) {
        emit updateOK();
    } else {
        emit updateFail();
    }
}

bool sqliteDb::insertIntoDb()
{
    QString sqlStmt;
    QSqlQuery query(m_sqlDb);



    return query.exec(sqlStmt);
}

bool sqliteDb::updateIntoDb()
{
    QString sqlStmt;
    QSqlQuery query(m_sqlDb);


    return query.exec(sqlStmt);
}
