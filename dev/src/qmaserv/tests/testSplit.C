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
      void testMod()
      {
        int res = 30;
        int wordlen = res/4;
        int modlen = res%4;
        if(modlen!= 0)
        {
          ++wordlen;
        }
        assert_eq("Mod should be 8",8,wordlen);
      }

      void testByteLen()
      {
        int res = 30;
        int byteLen = res;
        int modlen = res%4;
        if(modlen!= 0)
        {
          byteLen = byteLen + modlen;
        }
        assert_eq("ByteLen should be 32",32,byteLen);
      }



    public:

      Test() : suite ("modSuite")
      {
         add("mod1",testcase(this,"modTest",&Test::testMod));
         add("mod2",testcase(this,"modByteLen",&Test::testByteLen));
         suite::main().add("modSuite",this);
      }
  } *theTest = new Test();
}
