#include "asterid/signal.hh"
#include "tests.hh"

using testsig1 = asterid::signal<int, float>;

void tests::signal_tests() {
	testsig1 sig1;
	asterid::observer ob;
	sig1.connect(ob, [](int i, float f){
		printf("Int: %i, Float: %f\n", i, f);
	});
	sig1.fire(5, 3.5f);
	//sig1.disconnect(1);
}
