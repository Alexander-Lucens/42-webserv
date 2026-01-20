#include <string>
#include "Colors.hpp"

bool testRequest();
bool testResponse();
bool testSocket();

int main() {

    bool requestTestResult = testRequest();
    bool responseTestResult = testResponse();
    bool socketTestResult = testSocket();

    std::string finalMessage = (requestTestResult && responseTestResult && socketTestResult) ? "All tests passed!" : "Some tests failed.";
    
    INFO(finalMessage);

    return 0;
}
	