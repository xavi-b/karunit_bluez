#include "settingswidget.h"

SettingsWidget::SettingsWidget(QWidget* parent)
    : QWidget(parent)
{
    QVBoxLayout* layout = new QVBoxLayout;

    this->devicesListWidget = new QListWidget;
    connect(this->devicesListWidget, &QListWidget::doubleClicked, this, [=](QModelIndex const& index)
    {
        if(!index.data(Qt::UserRole + 1).toBool())
            emit connectToDevice({ index.data().toString(), index.data(Qt::UserRole).toString(), false });
        else
            emit disconnectFromDevice({ index.data().toString(), index.data(Qt::UserRole).toString(), true });
    });
    layout->addWidget(this->devicesListWidget, 1);

    this->setLayout(layout);
}

void SettingsWidget::deviceConnected(KU::PLUGIN::DeviceInfo const& info)
{
    emit log("deviceConnected " + info.name + " " + info.address);
    auto items = this->devicesListWidget->findItems(info.name, Qt::MatchExactly);

    if(items.count() > 0)
    {
        for(auto& i : items)
        {
            if(i->data(Qt::UserRole).toString() == info.address)
            {
                i->setData(Qt::ForegroundRole, QColor("green"));
                i->setData(Qt::UserRole + 1, true);
            }
        }
    }
    else
    {
        QListWidgetItem* item = new QListWidgetItem;
        item->setText(info.name);
        item->setData(Qt::UserRole, info.address);
        item->setData(Qt::ForegroundRole, QColor("green"));
        item->setData(Qt::UserRole + 1, true);
        this->devicesListWidget->addItem(item);
        this->devicesListWidget->sortItems();
    }
}

void SettingsWidget::deviceDisconnected(KU::PLUGIN::DeviceInfo const& info)
{
    emit log("deviceDisconnected " + info.name + " " + info.address);
    auto items = this->devicesListWidget->findItems(info.name, Qt::MatchExactly);

    if(items.count() > 0)
    {
        for(auto& i : items)
        {
            if(i->data(Qt::UserRole).toString() == info.address)
            {
                i->setData(Qt::ForegroundRole, QColor("red"));
                i->setData(Qt::UserRole + 1, false);
            }
        }
    }
    else
    {
        QListWidgetItem* item = new QListWidgetItem;
        item->setText(info.name);
        item->setData(Qt::UserRole, info.address);
        item->setData(Qt::ForegroundRole, QColor("red"));
        item->setData(Qt::UserRole + 1, false);
        this->devicesListWidget->addItem(item);
        this->devicesListWidget->sortItems();
    }
}

void SettingsWidget::setDevices(const QList<KU::PLUGIN::DeviceInfo>& list)
{
    for(auto& d : list)
    {
        emit log("setDevices " + d.name + " " + d.address);
        auto items = this->devicesListWidget->findItems(d.name, Qt::MatchExactly);

        if(items.count() > 0)
        {
            for(auto& i : items)
            {
                if(i->data(Qt::UserRole).toString() == d.address)
                {
                    i->setData(Qt::ForegroundRole, d.connected ? QColor("green") : QColor("red"));
                    i->setData(Qt::UserRole + 1, d.connected);
                }
            }
        }
        else
        {
            QListWidgetItem* item = new QListWidgetItem;
            item->setText(d.name);
            item->setData(Qt::UserRole, d.address);
            item->setData(Qt::ForegroundRole, d.connected ? QColor("green") : QColor("red"));
            item->setData(Qt::UserRole + 1, d.connected);
            this->devicesListWidget->addItem(item);
            this->devicesListWidget->sortItems();
        }
    }
}
