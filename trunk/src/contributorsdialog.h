#ifndef CONTRIBUTORSDIALOG_H
#define CONTRIBUTORSDIALOG_H

#include <QDialog>

namespace Ui {
    class contributorsDialog;
}

class contributorsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit contributorsDialog(QWidget *parent = 0, int contest = 0);
    ~contributorsDialog();

private:
    Ui::contributorsDialog *ui;

    void showEvent ( QShowEvent * );
};

#endif // CONTRIBUTORSDIALOG_H
