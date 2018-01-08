//
//#define DEBUG 
//
//#include <cassert>
//#include <iostream>
//#include "../include/segmenter/BigramSegmenter/BigramSeger.hpp"
//#include "../include/utils/TimeUtils.hpp"
//
//
//int main() {
//	JLP::TimeUtils::showPresentTime("start");
//
//	std::string dictPath = "include/segmenter/corpus/jiebaUnigram";
//	std::string dictPath1 = "include/segmenter/corpus/Bigram";
//	std::string testPath = "include/segmenter/corpus/test3";
//
//	JLP::BigramSeger<int,double>	bigramSeger;
//	bigramSeger.buildDict(dictPath,dictPath1);
//	JLP::TimeUtils::showPresentTime("end dict building");
//
//	assert(bigramSeger.trie.getValue(u"Æç·Ö")==984);
//	assert(abs(bigramSeger.trie1.getValue(u"ÊÇ@¾¹¾¿") - 0.23529) <= 0.0001);
//	
//
//	bigramSeger.batchCut(testPath);
//
//	JLP::TimeUtils::showPresentTime("end");
//
//	system("pause");
//}
//
