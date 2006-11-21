#include <unit++.h>
#include "TimeOfDay.h"
#include "BTI.h"
#include <unistd.h>
#include "TimeServer.h"
#include "Continuity.h"
#include "StateInfoVO.h"

using namespace std;
#include <unit++.h>
using namespace unitpp;
namespace
{

  class Test : public suite
  { void testContinuityFile()
      {
           BTI timeInfo;
           timeInfo.drsn = 1; 
	   timeInfo.sec_offset = 2;
	   timeInfo.usec_offset = 3;
	   timeInfo.clockQuality = 4;
	   timeInfo.minutesSinceLock = 5;
	   timeInfo.filter_delay = 6;
	   
	   StateInfoVO inSI;
	   inSI.setTimeInfo(timeInfo);

           Continuity cf;
           cf.saveContinuityInfo(inSI);

	   Continuity ocf;
	   StateInfoVO outSI = ocf.getContinuityInfo();

	   BTI retTime = outSI.getTimeInfo();

           assert_eq("drsn should match",timeInfo.drsn,
                                              retTime.drsn);
           assert_eq("sec_offset should match",timeInfo.sec_offset,
			                      retTime.sec_offset);
           assert_eq("usec_offset should match",timeInfo.usec_offset,
			                      retTime.usec_offset);
           assert_eq("clockQuality should match",timeInfo.clockQuality,
			                      retTime.clockQuality);
      }

    public:
      Test() : suite ("fileSuite")
      {
         add("file1",testcase(this,"continuity",&Test::testContinuityFile));
         suite::main().add("fileSuite",this);
      }
  } *theTest = new Test();
}
