#ifndef LISTITEMBASE_H
#define LISTITEMBASE_H
#include <QWidget>
#include "global.h"
class ListItemBase : public QWidget
{
    Q_OBJECT
public:
    // 必须实现构造
    explicit ListItemBase(QWidget *parent = nullptr);
    void SetItemType(ListItemType itemType);
    ListItemType GetItemType();
protected:
    void paintEvent(QPaintEvent *event) override;
private:
    ListItemType _itemType;
public slots:
signals:
};
#endif // LISTITEMBASE_H
