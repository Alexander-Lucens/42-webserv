#!/usr/bin/env bash
set -e

OUTPUT_VAL=0

PS_BEFORE=$(ps aux | grep webserv | grep -v grep | awk '{print $2}')
if [ -n "$PS_BEFORE" ]; then
	echo -e "\033[0;33m Warning: Found running webserv processes before tests start. \033[0m"
	echo -e "\033[0;33m It could cause false tests or port conflicts. \033[0m"
	echo -e "\033[0;33m Kill them, or insure that they not in range" "\033[0;32m8080-8089\033[0m"
fi

if make; then 
	echo -e "\033[0;32m./webserv - created!\033[0m"
	sleep 1
fi

for test_script in tests/scripts/test_*.sh; do
	echo -e "\033[0;35mRunning $test_script...\033[0m"
	if bash "$test_script"; then
		echo -e "\033[0;32m$test_script passed.\033[0m"
	else
		echo -e "\033[0;31m$test_script failed.\033[0m"
		OUTPUT_VAL=1
	fi
	echo ""
done

if [ $OUTPUT_VAL -eq 0 ]; then
	echo -e "\033[0;32mAll tests passed successfully!\033[0m"
else
	echo -e "\033[0;31mSome tests failed. Please check the output above for details.\033[0m"
fi

PS_LINES=$(ps aux | grep webserv | grep -v grep | awk '{print $2}')

if [ -n "$PS_LINES" ] && [ "$PS_LINES" != "$PS_BEFORE" ]; then
	echo -e "\033[0;33mWarning: Found running ZOMBIE webserv processes.\033[0m"
	echo "$PS_LINES" | xargs kill -9 2>/dev/null || true
fi

exit $OUTPUT_VAL