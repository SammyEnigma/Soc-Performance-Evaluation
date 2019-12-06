/*
 * @Author: MRXY001
 * @Date: 2019-12-04 17:38:19
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-12-04 17:39:52
 * @Description: 形状的属性设置对话框
 */
#ifndef SHAPEPROPERTYDIALOG_H
#define SHAPEPROPERTYDIALOG_H

#include <QObject>
#include <QDialog>
#include <QColorDialog>
#include <QDebug>
#include "shapebase.h"
#include "ui_shapepropertydialog.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class ShapePropertyDialog;
}
QT_END_NAMESPACE

class ShapePropertyDialog : public QDialog
{
    Q_OBJECT
public:
    ShapePropertyDialog(QList<ShapeBase*>shapes);
    ~ShapePropertyDialog();

    friend class MainWindow;
    friend class GraphicArea;
    friend class ShapeBase;

private:
    void setBtnColor(QPushButton* btn, QColor c, bool fore = false);

signals:

public slots:
	
private slots:
    void on_text_color_btn_clicked();
    void on_pixmap_name_btn_clicked();
    void on_pixmap_color_btn_clicked();
    void on_text_lineEdit_textEdited(const QString &text);
    void on_text_align_comboBox_activated(int index);
    void on_pixmap_scale_combo_activated(int index);

    void on_border_size_spin_valueChanged(int value);

    void on_border_color_btn_clicked();

private:
    Ui::ShapePropertyDialog *ui;
	ShapeBase* shape;
    QList<ShapeBase*>shapes;

    QList<Qt::Alignment> aligns = {Qt::AlignTop | Qt::AlignHCenter, Qt::AlignBottom | Qt::AlignHCenter, Qt::AlignLeft | Qt::AlignVCenter, Qt::AlignRight | Qt::AlignVCenter, Qt::AlignCenter,
                                    Qt::AlignTop | Qt::AlignLeft, Qt::AlignTop | Qt::AlignRight, Qt::AlignBottom | Qt::AlignLeft, Qt::AlignBottom | Qt::AlignRight};
};

#endif // SHAPEPROPERTYDIALOG_H
