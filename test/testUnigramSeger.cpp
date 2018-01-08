//
//#define DEBUG 
//
//#include <cassert>
//#include <iostream>
//#include "../include/segmenter/UnigramSegmenter/UnigramSeger.hpp"
//#include "../include/utils/TimeUtils.hpp"
//
//
//int main() {
//	JLP::TimeUtils::showPresentTime("start");
//	
//	std::string dictPath = "include/segmenter/corpus/jiebaUnigram";
//	std::string testPath = "include/segmenter/corpus/test2";
//
//	JLP::UnigramSeger<int>	unigramSeger;
//	unigramSeger.buildDict(dictPath);
//	JLP::TimeUtils::showPresentTime("end dict building");
//
//	unigramSeger.batchCut(testPath);
//
//	JLP::TimeUtils::showPresentTime("end");
//
//}
//
