// menus.cpp: Menus for a simple text editor.
#include "menus.h"

int main(int argc, char *argv[]) {
   QApplication a(argc, argv);
   Menus w;
   w.show();
   return a.exec();
}

Menus::Menus(QWidget *parent) : QMainWindow(parent) {
   QRect rec = QApplication::desktop()->screenGeometry();
   int h = rec.height(), w = rec.width();
   setGeometry(w / 10, h / 10, 8 * w / 10, 8 * h / 10);
   setWindowTitle("A simple file menu in action");

   // Create actions:
   QAction *openAct = new QAction("Open...", this);
   openAct->setShortcut(Qt::CTRL | Qt::Key_O);
   connect(openAct, &QAction::triggered, this, &Menus::open);

   QAction *saveAct = new QAction("Save...", this);
   saveAct->setShortcut(Qt::CTRL | Qt::Key_S);
   connect(saveAct, &QAction::triggered, this, &Menus::save);

   QAction *exitAct = new QAction("Exit", this);
   exitAct->setShortcut(Qt::CTRL | Qt::Key_Q);
   connect(exitAct, &QAction::triggered, this, &QWidget::close);

   QAction *infoAct = new QAction("Info", this);
   connect(infoAct, &QAction::triggered, this, &Menus::info);

   // Create menus:
   QMenu *fileMenu = menuBar()->addMenu("&File");
   fileMenu->addAction(openAct);
   fileMenu->addAction(saveAct);
   fileMenu->addAction(exitAct);

   QMenu *helpMenu = menuBar()->addMenu("&Help");
   helpMenu->addAction(infoAct);

   // Text box to display text:
   pTextBox = new QTextEdit(this);
   setCentralWidget(pTextBox);
}

void Menus::open() {
   QString qFileName = QFileDialog::getOpenFileName(this,
      "Open File", "", "All Files (*)");
   if (qFileName.length() > 0) {
      QFile qf(qFileName);
      qf.open(QFile::ReadOnly | QFile::Text);
      QTextStream in(&qf);
      QString str = in.readAll();
      pTextBox->setText(str);
      qf.close();
   }
}

void Menus::save() {
   QString qFileName = QFileDialog::getSaveFileName(this,
      "Save as...", "", "All Files (*)");
   if (qFileName.length() > 0) {
      QFile qf(qFileName);
      qf.open(QFile::WriteOnly | QFile::Text);
      QTextStream out(&qf);
      QString str = pTextBox->toPlainText();
      out << str;
      qf.close();
   }
}

void Menus::info()const {
   QMessageBox msgBox;
   msgBox.setWindowTitle("Click OK");
   msgBox.setText("Demo of menus and file input/output\n");
   msgBox.exec();
}
