#include <unit++.h>
#include "TimeOfDay.h"
#include "BTI.h"
#include <unistd.h>
#include "CreatePacket.h"

using namespace std;
#include <unit++.h>
using namespace unitpp;
namespace
{
  class Test : public suite
  {
      void testTimeDiff()
      {
         TimeOfDay t;
         double t1 = t.getCurrentTime();
         int unslept = sleep(2);
         double t2 = t.getCurrentTime();
         double tdiff = t.getTimeDifference(t1,t2);
         assert_true("time diff second",tdiff>1.0);
      }

     void testTimeOfDay()
     {
         struct timeval tp;
         struct timezone tzp;
         gettimeofday(&tp,&tzp);
         double dtval = (double)tp.tv_sec;
         TimeOfDay t;
         double retTOD =  t.getCurrentTime();
         double res = retTOD - dtval;
         assert_true("time diff should be less than 1 second",res < 1.0);
     }

  public:

      Test() : suite ("projectTestSuite")
      {
         add("t1",testcase(this,"testTimeOfDay",&Test::testTimeOfDay));
         add("t2",testcase(this,"testTimeDiff",&Test::testTimeDiff));
         suite::main().add("projectTestSuite",this);
      }
  } *theTest = new Test();
}
