#include <unit++.h>
#include "ClockUtils.h"

using namespace std;
#include <unit++.h>
using namespace unitpp;
namespace
{

  class Test : public suite
  { void testTranslateClock()
      {
           qma_uint16 qual;
           qma_uint16 loss;
           qma_uint8  clockVal;
           
           qual = 100;
           loss = 100;
           clockVal = translate_clock(qual,loss);
           assert_eq("clockQuality should be 100",100,clockVal);
      }

    public:
      Test() : suite ("fileSuite")
      {
        add("clock1",testcase(this,"translateclock",&Test::testTranslateClock));
        suite::main().add("fileSuite",this);
      }
  } *theTest = new Test();
}
