/********************************************************************************
** Form generated from reading UI file 'chatdialog.ui'
**
** Created by: Qt User Interface Compiler version 6.6.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CHATDIALOG_H
#define UI_CHATDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <clickedbtn.h>

QT_BEGIN_NAMESPACE

class Ui_ChatDialog
{
public:
    QHBoxLayout *horizontalLayout;
    QWidget *side_bar;
    QVBoxLayout *verticalLayout;
    QWidget *widget;
    QVBoxLayout *verticalLayout_3;
    QLabel *side_chat_lb;
    QLabel *side_contact_lb;
    QLabel *side_head_lb;
    QSpacerItem *verticalSpacer;
    QWidget *chat_user_wid;
    QVBoxLayout *verticalLayout_2;
    QWidget *search_wid;
    QHBoxLayout *horizontalLayout_2;
    QLineEdit *search_edit;
    QSpacerItem *horizontalSpacer;
    ClickedBtn *add_btn;
    QListWidget *chat_user_list;
    QListWidget *search_list;
    QWidget *chat_data_wid;
    QVBoxLayout *verticalLayout_4;
    QWidget *title_wid;
    QVBoxLayout *verticalLayout_5;
    QWidget *widget_2;
    QVBoxLayout *verticalLayout_6;
    QLabel *title_lb;
    QWidget *chat_data_list;
    QWidget *tool_wid;
    QHBoxLayout *horizontalLayout_3;
    QSpacerItem *horizontalSpacer_2;
    QLabel *file_lb;
    QSpacerItem *horizontalSpacer_3;
    QLabel *emo_lb;
    QSpacerItem *horizontalSpacer_4;
    QTextEdit *chat_edit;
    QWidget *widget_5;
    QHBoxLayout *horizontalLayout_4;
    QSpacerItem *horizontalSpacer_5;
    QPushButton *receive_btn;
    QSpacerItem *horizontalSpacer_7;
    QPushButton *send_btn;
    QSpacerItem *horizontalSpacer_6;

    void setupUi(QDialog *ChatDialog)
    {
        if (ChatDialog->objectName().isEmpty())
            ChatDialog->setObjectName("ChatDialog");
        ChatDialog->resize(737, 623);
        horizontalLayout = new QHBoxLayout(ChatDialog);
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        side_bar = new QWidget(ChatDialog);
        side_bar->setObjectName("side_bar");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(side_bar->sizePolicy().hasHeightForWidth());
        side_bar->setSizePolicy(sizePolicy);
        side_bar->setMinimumSize(QSize(56, 0));
        side_bar->setMaximumSize(QSize(56, 16777215));
        verticalLayout = new QVBoxLayout(side_bar);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(10, 30, 0, 0);
        widget = new QWidget(side_bar);
        widget->setObjectName("widget");
        verticalLayout_3 = new QVBoxLayout(widget);
        verticalLayout_3->setSpacing(30);
        verticalLayout_3->setObjectName("verticalLayout_3");
        verticalLayout_3->setContentsMargins(0, 0, -1, 0);
        side_chat_lb = new QLabel(widget);
        side_chat_lb->setObjectName("side_chat_lb");
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(side_chat_lb->sizePolicy().hasHeightForWidth());
        side_chat_lb->setSizePolicy(sizePolicy1);
        side_chat_lb->setMinimumSize(QSize(30, 30));
        side_chat_lb->setMaximumSize(QSize(30, 30));

        verticalLayout_3->addWidget(side_chat_lb);

        side_contact_lb = new QLabel(widget);
        side_contact_lb->setObjectName("side_contact_lb");
        side_contact_lb->setMinimumSize(QSize(30, 30));
        side_contact_lb->setMaximumSize(QSize(30, 30));

        verticalLayout_3->addWidget(side_contact_lb);

        side_head_lb = new QLabel(widget);
        side_head_lb->setObjectName("side_head_lb");
        side_head_lb->setMinimumSize(QSize(30, 30));
        side_head_lb->setMaximumSize(QSize(30, 30));

        verticalLayout_3->addWidget(side_head_lb);


        verticalLayout->addWidget(widget);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout->addItem(verticalSpacer);


        horizontalLayout->addWidget(side_bar);

        chat_user_wid = new QWidget(ChatDialog);
        chat_user_wid->setObjectName("chat_user_wid");
        chat_user_wid->setMaximumSize(QSize(250, 16777215));
        verticalLayout_2 = new QVBoxLayout(chat_user_wid);
        verticalLayout_2->setSpacing(0);
        verticalLayout_2->setObjectName("verticalLayout_2");
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        search_wid = new QWidget(chat_user_wid);
        search_wid->setObjectName("search_wid");
        search_wid->setMinimumSize(QSize(0, 60));
        search_wid->setMaximumSize(QSize(16777215, 60));
        horizontalLayout_2 = new QHBoxLayout(search_wid);
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        horizontalLayout_2->setContentsMargins(9, 9, 9, 9);
        search_edit = new QLineEdit(search_wid);
        search_edit->setObjectName("search_edit");

        horizontalLayout_2->addWidget(search_edit);

        horizontalSpacer = new QSpacerItem(5, 20, QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        add_btn = new ClickedBtn(search_wid);
        add_btn->setObjectName("add_btn");
        add_btn->setMinimumSize(QSize(24, 24));
        add_btn->setMaximumSize(QSize(24, 24));

        horizontalLayout_2->addWidget(add_btn);


        verticalLayout_2->addWidget(search_wid);

        chat_user_list = new QListWidget(chat_user_wid);
        chat_user_list->setObjectName("chat_user_list");

        verticalLayout_2->addWidget(chat_user_list);

        search_list = new QListWidget(chat_user_wid);
        search_list->setObjectName("search_list");

        verticalLayout_2->addWidget(search_list);


        horizontalLayout->addWidget(chat_user_wid);

        chat_data_wid = new QWidget(ChatDialog);
        chat_data_wid->setObjectName("chat_data_wid");
        verticalLayout_4 = new QVBoxLayout(chat_data_wid);
        verticalLayout_4->setSpacing(0);
        verticalLayout_4->setObjectName("verticalLayout_4");
        verticalLayout_4->setContentsMargins(0, 0, 0, 0);
        title_wid = new QWidget(chat_data_wid);
        title_wid->setObjectName("title_wid");
        title_wid->setMinimumSize(QSize(0, 60));
        title_wid->setMaximumSize(QSize(16777215, 60));
        verticalLayout_5 = new QVBoxLayout(title_wid);
        verticalLayout_5->setObjectName("verticalLayout_5");
        widget_2 = new QWidget(title_wid);
        widget_2->setObjectName("widget_2");
        verticalLayout_6 = new QVBoxLayout(widget_2);
        verticalLayout_6->setObjectName("verticalLayout_6");
        title_lb = new QLabel(widget_2);
        title_lb->setObjectName("title_lb");

        verticalLayout_6->addWidget(title_lb);


        verticalLayout_5->addWidget(widget_2);


        verticalLayout_4->addWidget(title_wid);

        chat_data_list = new QWidget(chat_data_wid);
        chat_data_list->setObjectName("chat_data_list");

        verticalLayout_4->addWidget(chat_data_list);

        tool_wid = new QWidget(chat_data_wid);
        tool_wid->setObjectName("tool_wid");
        tool_wid->setMinimumSize(QSize(0, 60));
        tool_wid->setMaximumSize(QSize(16777215, 60));
        horizontalLayout_3 = new QHBoxLayout(tool_wid);
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        horizontalLayout_3->setContentsMargins(2, 2, 2, 2);
        horizontalSpacer_2 = new QSpacerItem(5, 20, QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_2);

        file_lb = new QLabel(tool_wid);
        file_lb->setObjectName("file_lb");
        file_lb->setMinimumSize(QSize(25, 25));
        file_lb->setMaximumSize(QSize(25, 25));

        horizontalLayout_3->addWidget(file_lb);

        horizontalSpacer_3 = new QSpacerItem(5, 20, QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_3);

        emo_lb = new QLabel(tool_wid);
        emo_lb->setObjectName("emo_lb");
        emo_lb->setMinimumSize(QSize(25, 25));
        emo_lb->setMaximumSize(QSize(25, 25));

        horizontalLayout_3->addWidget(emo_lb);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_4);


        verticalLayout_4->addWidget(tool_wid);

        chat_edit = new QTextEdit(chat_data_wid);
        chat_edit->setObjectName("chat_edit");
        chat_edit->setMinimumSize(QSize(0, 150));
        chat_edit->setMaximumSize(QSize(16777215, 150));

        verticalLayout_4->addWidget(chat_edit);

        widget_5 = new QWidget(chat_data_wid);
        widget_5->setObjectName("widget_5");
        widget_5->setMinimumSize(QSize(0, 60));
        widget_5->setMaximumSize(QSize(16777215, 60));
        horizontalLayout_4 = new QHBoxLayout(widget_5);
        horizontalLayout_4->setObjectName("horizontalLayout_4");
        horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_5);

        receive_btn = new QPushButton(widget_5);
        receive_btn->setObjectName("receive_btn");
        receive_btn->setMinimumSize(QSize(100, 30));
        receive_btn->setMaximumSize(QSize(100, 30));

        horizontalLayout_4->addWidget(receive_btn);

        horizontalSpacer_7 = new QSpacerItem(10, 20, QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_7);

        send_btn = new QPushButton(widget_5);
        send_btn->setObjectName("send_btn");
        send_btn->setMinimumSize(QSize(100, 30));
        send_btn->setMaximumSize(QSize(100, 30));

        horizontalLayout_4->addWidget(send_btn);

        horizontalSpacer_6 = new QSpacerItem(10, 20, QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_6);


        verticalLayout_4->addWidget(widget_5);


        horizontalLayout->addWidget(chat_data_wid);


        retranslateUi(ChatDialog);

        QMetaObject::connectSlotsByName(ChatDialog);
    } // setupUi

    void retranslateUi(QDialog *ChatDialog)
    {
        ChatDialog->setWindowTitle(QCoreApplication::translate("ChatDialog", "Dialog", nullptr));
        side_chat_lb->setText(QString());
        side_contact_lb->setText(QString());
        side_head_lb->setText(QString());
        add_btn->setText(QString());
        title_lb->setText(QCoreApplication::translate("ChatDialog", "\346\225\221\344\270\226\344\270\273\346\242\243", nullptr));
        file_lb->setText(QString());
        emo_lb->setText(QString());
        receive_btn->setText(QCoreApplication::translate("ChatDialog", "\346\216\245\346\224\266", nullptr));
        send_btn->setText(QCoreApplication::translate("ChatDialog", "\345\217\221\351\200\201", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ChatDialog: public Ui_ChatDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CHATDIALOG_H
