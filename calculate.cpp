// calculate.cpp: User input in dialog box.
#include "calculate.h"

int main(int argc, char *argv[]) {
   QApplication a(argc, argv);
   Calculate w;
   w.show();
   return a.exec();
}

Calculate::Calculate(QWidget *parent) :
   QMainWindow(parent) {
   QRect rec = QApplication::desktop()->screenGeometry();
   int h = rec.height(), w = rec.width();
   setGeometry(42 * w / 100, 42 * h / 100,
      16 * w / 100, 16 * h / 100);
   setWindowTitle("Calculate");

   pLabelA = new QLabel("a = ");
   pLabelB = new QLabel("b = ");
   pLabelSum = new QLabel("a + b = ");
   pLabelProduct = new QLabel("a x b = ");
   a = 2;
   b = 3;
   pA = new QLineEdit(QString::number(a));
   pB = new QLineEdit(QString::number(b));
   pSum = new QLineEdit(QString::number(a + b));
   pProduct = new QLineEdit(QString::number(a * b));

   QHBoxLayout *pHLayoutA = new QHBoxLayout;
   pHLayoutA->addWidget(pLabelA);
   pHLayoutA->addWidget(pA);

   QHBoxLayout *pHLayoutB = new QHBoxLayout;
   pHLayoutB->addWidget(pLabelB);
   pHLayoutB->addWidget(pB);

   pButton = new QPushButton("Compute a + b and a x b");

   QHBoxLayout *pHLayoutSum = new QHBoxLayout;
   pHLayoutSum->addWidget(pLabelSum);
   pHLayoutSum->addWidget(pSum);

   QHBoxLayout *pHLayoutProduct = new QHBoxLayout;
   pHLayoutProduct->addWidget(pLabelProduct);
   pHLayoutProduct->addWidget(pProduct);

   QVBoxLayout *pAll = new QVBoxLayout;
   pAll->addLayout(pHLayoutA);
   pAll->addLayout(pHLayoutB);
   pAll->addWidget(pButton);
   pAll->addLayout(pHLayoutSum);
   pAll->addLayout(pHLayoutProduct);

   QWidget *pW = new QWidget(this);
   pW->setLayout(pAll);
   setCentralWidget(pW);
   connect(pButton, SIGNAL(released()),
      this, SLOT(handleButton()));
}

void Calculate::handleButton() {
   QString strA = pA->text(), strB = pB->text();
   bool okA, okB;
   qreal a1 = strA.toDouble(&okA),
      b1 = strB.toDouble(&okB);
   if (!okA)
      pA->setText("Incorrect input");
   if (!okB)
      pB->setText("Incorrect input");
   if (okA && okB && (a1 != a || b1 != b)) {
      a = a1;
      b = b1;
      pSum->setText(QString::number(a + b));
      pProduct->setText(QString::number(a * b));
   }
}
