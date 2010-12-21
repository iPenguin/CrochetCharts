#include "testlicense.h"
#include "teststitch.h"

int main(int argc, char** argv) 
{

    QApplication app(argc, argv);
    int retval(0);

    QObject *test;

    test = new TestLicense();
    retval +=QTest::qExec(test, argc, argv);

    test = new TestStitch();
    retval +=QTest::qExec(test, argc, argv);

    return (retval ? 1 : 0);
}
