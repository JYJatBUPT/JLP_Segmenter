//
//#include <iostream>
//
//#include "../include/segmenter/MaxMatchSegmenter/MaxMatchSegmenter.hpp"
//#include "../include/utils/TimeUtils.hpp"
//
//
//int main() {
//	JLP::TimeUtils::showPresentTime("start");
//
//	std::string dictPath = "include/segmenter/corpus/unigram";
//	std::string testPath = "include/segmenter/corpus/test2";
//
//	JLP::MaxMatchSeger<bool>	maxMatchSeger("FD");
//	maxMatchSeger.buildDict(dictPath);
//	maxMatchSeger.batchCut(testPath);
// 
//	JLP::MaxMatchSeger<bool>	maxMatchSegerR("RD");
//	maxMatchSegerR.buildDict(dictPath);
//	maxMatchSegerR.batchCut(testPath);
//
//	JLP::TimeUtils::showPresentTime("end");
//}
