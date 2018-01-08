#ifndef JLP_UTILS_TIMEUTILS_H_
#define JLP_UTILS_TIMEUTILS_H_

#include <iostream>
#include <string>
#include <ctime>

namespace JLP {

class TimeUtils {
public:
	// format: "%Y-%m-%d %H:%M:%S"
	inline static void getTime(const std::string& format, std::string&  timeStr) {
		time_t timeNow;
		time(&timeNow);
		timeStr.resize(64);
		size_t len = strftime((char*)timeStr.c_str(), timeStr.size(), format.c_str(), localtime(&timeNow));
		// 需要将_CRT_SECURE_NO_WARNINGS加入预编译头中 
		timeStr.resize(len);
	}
	// default format
	inline static void getTime(std::string&  timeStr) {
		std::string format = "%Y-%m-%d %H:%M:%S";
		getTime(format, timeStr);
	}
	// print present time and message in the screen
	inline static void showPresentTime(const std::string& message="" ) {
		std::string timeNow;
		getTime(timeNow);
		std::cout <<"--- "<< timeNow<<":  "<<message << " ---" << '\n';
	}

};

} // end namespace JLP

#endif 
