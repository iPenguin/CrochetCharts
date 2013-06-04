#ifndef COLORREPLACER_H
#define COLORREPLACER_H

#include <QDialog>

namespace Ui {
class ColorReplacer;
}

class ColorReplacer : public QDialog
{
    Q_OBJECT
    
public:
    explicit ColorReplacer(QList<QString> colorList, QWidget *parent = 0);
    ~ColorReplacer();
    
    QColor originalColor;
    QColor newColor;
    int selection;

public slots:
    void accept();
    void setSelection();

private slots:
    void origColorChanged(QString color);
    void newColorChanged(QString color);

private:
    Ui::ColorReplacer *ui;
    QList<QString> mOriginalColorList;

    void populateColorLists();
};

#endif // COLORREPLACER_H
