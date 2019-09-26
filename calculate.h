// calculate.h: User input in dialog box.
#ifndef CALCULATE_H
#define CALCULATE_H
#include <QtWidgets>

class Calculate : public QMainWindow {
   Q_OBJECT
public:
   Calculate(QWidget *parent = nullptr);
protected:
   QPushButton *pButton;
   QLabel *pLabelA, *pLabelB, *pLabelSum, *pLabelProduct;
   QLineEdit *pA, *pB, *pSum, *pProduct;
   qreal a, b;
private slots:
   void handleButton();
};
#endif // CALCULATE_H
