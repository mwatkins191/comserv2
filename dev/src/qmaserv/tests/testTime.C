#include <unit++.h>
#include "TimeOfDay.h"
#include "BTI.h"
#include <unistd.h>
#include "TimeServer.h"


using namespace std;
#include <unit++.h>
using namespace unitpp;
namespace
{

  class Test : public suite
  {
      void testAddSamples()
      {
         TimeServer ts;
         BTI stime;
         stime.sec_offset = 9000;
         stime.usec_offset = 00000;
         double hertz = 200;
         int samplecount = 100;
         BTI otime = ts.addSamplesToBTI(samplecount,
				        hertz,
			                stime);
         assert_eq("Seconds dont match",otime.sec_offset,9000);
         assert_eq("Usecs dont' match",otime.usec_offset,500000);

         stime.sec_offset = 900000;
         stime.usec_offset = 750000;
         hertz = 200;
         samplecount = 100;
         otime = ts.addSamplesToBTI(samplecount,
				        hertz,
			                stime);
         assert_eq("seconds dont' match.",900001,otime.sec_offset);
         assert_eq("usecs dont match.",250000,otime.usec_offset);
      }

      void testUsecs()
      {
        TimeServer ts;
        timeval stime;
        stime.tv_sec = 9000;
        stime.tv_usec = 123456;
        qma_uint8 res = ts.getUsecsFromTimeval(stime);
        assert_eq("usecs should be equal 56",56,res);
        long *ptr = NULL;
        gettimeofday(&stime,(void*) ptr);
        res = ts.getUsecsFromTimeval(stime);
        qma_uint8 highv = 99;
        qma_uint8 lowv = 0;
        assert_true("usec should be less than 100",res<=highv);
        assert_true("usec should be more or equal to 0",res>=lowv);
      }

    public:

      Test() : suite ("timeSuite")
      {
         add("time1",testcase(this,"addSamples",&Test::testAddSamples));
         add("time2",testcase(this,"testUsecs",&Test::testUsecs));
         suite::main().add("timeSuite",this);
      }
  } *theTest = new Test();
}
