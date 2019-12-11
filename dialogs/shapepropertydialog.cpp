/*
 * @Author: MRXY001
 * @Date: 2019-12-04 17:38:19
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-12-04 17:40:00
 * @Description: 形状的属性设置对话框
 */
#include "shapepropertydialog.h"

ShapePropertyDialog::ShapePropertyDialog(ShapeList shapes) : QDialog(shapes.first()), ui(new Ui::ShapePropertyDialog), shape(shapes.first()), shapes(shapes)
{
    ui->setupUi(this);

    QSet<QString>class_set;
    foreach (ShapeBase* shape, shapes)
    {
        if (shape->getText() == shape->getClass())
            class_set.insert(shape->getText());
        else
            class_set.insert(QString("%1(%2)").arg(shape->getText()).arg(shape->getClass()));
    }
    ui->class_lineEdit->setText(class_set.toList().join(", "));
    ui->text_lineEdit->setText(shape->getText());

    int index = aligns.indexOf(shape->_text_align);
    ui->text_align_comboBox->setCurrentIndex(index!=-1 ? index : 1/*默认=下*/);

    ui->checkBox->setChecked(shape->_pixmap_scale);

    ui->border_size_spin->setValue(shape->_border_size);

    if (shape->getLargeType() == CableType)
    {
        CableBase* cable = static_cast<CableBase*>(shape);
        if (cable->_line_type == 0)
            ui->radioButton->setChecked(true);
        else if (cable->_line_type == 1)
            ui->radioButton_2->setChecked(true);
        else if (cable->_line_type == 2)
            ui->radioButton_3->setChecked(true);
    }

}

ShapePropertyDialog::~ShapePropertyDialog()
{
    delete ui;
}

void ShapePropertyDialog::setBtnColor(QPushButton *btn, QColor c, bool fore)
{
    QString cs = QVariant(c).toString();
    if (fore)
    {
        btn->setStyleSheet("color:" + cs + ";");
    }
    else
    {
        btn->setStyleSheet("background-color:" + cs + ";");
    }
    btn->setText(cs);
}

void ShapePropertyDialog::on_text_color_btn_clicked()
{
    QColorDialog* cd = new QColorDialog(this);
    cd->setCurrentColor(shape->_text_color);
    cd->setOption(QColorDialog::ShowAlphaChannel);
    cd->show();

    // 更换颜色预览
    connect(cd, &QColorDialog::currentColorChanged, this, [=](QColor c){
        setBtnColor(ui->text_color_btn, c, true);
    });

    // 确定选择颜色
    connect(cd, &QColorDialog::colorSelected, this, [=](QColor c){
        setBtnColor(ui->text_color_btn, c, true);

        foreach (ShapeBase* shape, shapes) {
            shape->_text_color = c;
            shape->update();
        }
    });

    // 取消选择颜色
    connect(cd, &QColorDialog::rejected, this, [=]{
        setBtnColor(ui->text_color_btn, shape->_text_color, true);
        ui->text_color_btn->setText("保持");
    });
}

void ShapePropertyDialog::on_pixmap_name_btn_clicked()
{

}

void ShapePropertyDialog::on_pixmap_color_btn_clicked()
{
    QColorDialog* cd = new QColorDialog(this);
    cd->setCurrentColor(shape->_pixmap_color);
    cd->setOption(QColorDialog::ShowAlphaChannel);
    cd->show();

    // 更换颜色预览
    connect(cd, &QColorDialog::currentColorChanged, this, [=](QColor c){
        setBtnColor(ui->pixmap_color_btn, c, true);
    });

    // 确定选择颜色
    connect(cd, &QColorDialog::colorSelected, this, [=](QColor c){
        setBtnColor(ui->pixmap_color_btn, c, true);

        foreach (ShapeBase* shape, shapes) {
            shape->_pixmap_color = c;
            shape->update();
        }
    });

    // 取消选择颜色
    connect(cd, &QColorDialog::rejected, this, [=]{
        setBtnColor(ui->pixmap_color_btn, shape->_pixmap_color, true);
        ui->pixmap_color_btn->setText("保持");
    });
}

void ShapePropertyDialog::on_text_lineEdit_textEdited(const QString &text)
{
    foreach (ShapeBase* shape, shapes) {
        shape->setText(text);
        shape->update();
    }
}

void ShapePropertyDialog::on_text_align_comboBox_activated(int index)
{
    foreach (ShapeBase* shape, shapes) {
        shape->_text_align = aligns.at(index);
        shape->update();
    }
}

void ShapePropertyDialog::on_border_size_spin_valueChanged(int value)
{
    foreach (ShapeBase* shape, shapes) {
        shape->_border_size = value;
        shape->update();
    }
}

void ShapePropertyDialog::on_border_color_btn_clicked()
{
    QColorDialog* cd = new QColorDialog(this);
    cd->setCurrentColor(shape->_border_color);
    cd->setOption(QColorDialog::ShowAlphaChannel);
    cd->show();

    // 更换颜色预览
    connect(cd, &QColorDialog::currentColorChanged, this, [=](QColor c){
        setBtnColor(ui->border_color_btn, c, true);
    });

    // 确定选择颜色
    connect(cd, &QColorDialog::colorSelected, this, [=](QColor c){
        setBtnColor(ui->border_color_btn, c, true);

        foreach (ShapeBase* shape, shapes) {
            shape->_border_color = c;
            shape->update();
        }
    });

    // 取消选择颜色
    connect(cd, &QColorDialog::rejected, this, [=]{
        setBtnColor(ui->border_color_btn, shape->_border_color, true);
        ui->border_color_btn->setText("保持");
    });
}

void ShapePropertyDialog::on_checkBox_clicked()
{
    bool state = ui->checkBox->checkState();
    foreach (ShapeBase* shape, shapes) {
        shape->_pixmap_scale = state;
        shape->update();
    }
}

void ShapePropertyDialog::on_radioButton_clicked()
{
    foreach (ShapeBase* shape, shapes) {
        if (shape->getLargeType() == CableType)
        {
            CableBase* cable = static_cast<CableBase*>(shape);
            cable->_line_type = 0;
            cable->update();
        }
    }
}

void ShapePropertyDialog::on_radioButton_2_clicked()
{
    foreach (ShapeBase* shape, shapes) {
        if (shape->getLargeType() == CableType)
        {
            CableBase* cable = static_cast<CableBase*>(shape);
            cable->_line_type = 1;
            cable->update();
        }
    }
}

void ShapePropertyDialog::on_radioButton_3_clicked()
{
    foreach (ShapeBase* shape, shapes) {
        if (shape->getLargeType() == CableType)
        {
            CableBase* cable = static_cast<CableBase*>(shape);
            cable->_line_type = 2;
            cable->update();
        }
    }
}
