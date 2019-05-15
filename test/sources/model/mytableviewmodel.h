#ifndef MYTABLEVIEWMODEL_H
#define MYTABLEVIEWMODEL_H

#include <QObject>
#include <QAbstractTableModel>
#include <QDir>
#include <QStack>

class MyTableViewModel : public QAbstractTableModel
{
    Q_OBJECT
    Q_PROPERTY(QStringList roles READ roles WRITE setRoles)
public:
    explicit MyTableViewModel(QObject *parent = nullptr);
    ~MyTableViewModel();

    void showFileInfoList(QFileInfoList list);
    void onShowSlot(QDir dir);
    void onDirShowSlot(QString &filename);

    QVariant data(const QModelIndex &index, int role) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QHash<int, QByteArray> roleNames() const; //重新实现roleNames()

    QStringList roles() const;
    void setRoles(const QStringList roles);
    void refrushModel();
signals:
    void dataChanged();
private:
    QList<QVariantList> m_data;
    QStringList m_roleList;
    QString mCurrentPath;
    QStack <QString> mPath;

};

#endif // MYTABLEVIEWMODEL_H
