#include "SimulatorWindow.h"
#include <QApplication>

int main(int argc, char * argv[])
{
  QApplication app(argc, argv);
  QCoreApplication::setApplicationName("Jakes XLE Simulator");
  QCoreApplication::setApplicationVersion("0.1");

  SimulatorWindow window;
  window.show();

  return app.exec();
}
