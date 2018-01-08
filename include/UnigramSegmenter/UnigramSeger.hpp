#ifndef JLP_SEGMENTER_FIRSTORDERMARKOVSEGMENTER_H_
#define JLP_SEGMENTER_FIRSTORDERMARKOVSEGMENTER_H_

#include <vector>
#include <string>
#include <cmath>
#include <cctype>

#include "../../utils/FileUtils.hpp"
#include "../container/Trie.hpp"

namespace JLP {
	
template <class T>
class UnigramSeger {
public:
	long long totalFreq;
	JLP::Trie<T> trie;
	std::vector<int> prevNode;
	std::vector<double> maxProbOfNode;
	double smoothLogProb;
	size_t seqsAlNumCounter;
public:
	explicit UnigramSeger() {	}
public:
	bool buildDict(const std::string& dictPath) {
		std::vector<T> values;
		std::vector<std::u16string> words;
		std::string freqsDictPath = dictPath + "Counter";

		JLP::FileUtil::readUnicodeLines(dictPath, words);
		for (auto& word : words) {
			std::reverse(word.begin(), word.end()); // Reverse each term entry
		}
		this->totalFreq=JLP::FileUtil::readFreqs(freqsDictPath, values);
		this->smoothLogProb = log(0.2 * 1 / totalFreq);

#ifdef DEBUG
		std::cout << "size of words:" << words.size() << " size of values:" << values.size() << "\n";
#endif 

		trie.buildTrie(words, values);
		return true;
	}
public:
	void computeProbOfNode(size_t i, const std::u16string& src) {	// 计算节点i的最大概率
		if (0 == i) {
			prevNode.push_back(0);				// 记录node_i的最佳前趋节点
			maxProbOfNode.push_back(0);   // maxProbOfNode(0)==>log( P(node 0) )==>log( 1 )==0
			return;
		}

		std::vector<std::u16string> prevWords;
		double MaxProb = -100000; // 候选节点概率  注意取log后得到负值
		size_t bestPrevNode = 0; // 候选最佳前趋节点

		// 构造前趋词集合
		trie.matchPrevWords(src, i - 1, prevWords);

		if (prevWords.size() == 0) {  // 平滑方法 针对未在词典中找到的词
			// 假定 smoothLogProb=log(P(newWord)) <===>P(newWord)=0.2*1/totalFreq 这个取值应该比 P=1/totalFreq 小一个数量级即可
			if (isAlNum(src.at(i - 1)) && seqsAlNumCounter > 0) {
				bestPrevNode = i - 1- seqsAlNumCounter;
				MaxProb = maxProbOfNode[i - 1]; // 
				++seqsAlNumCounter;
			}
			else {
				if (isAlNum(src.at(i - 1)) && seqsAlNumCounter == 0) {
					bestPrevNode = i - 1;
					MaxProb = maxProbOfNode[i - 1] + smoothLogProb; // 
					++seqsAlNumCounter;
				}
				else {
					bestPrevNode = i - 1;
					MaxProb = maxProbOfNode[i - 1] + smoothLogProb; // 
					seqsAlNumCounter = 0; //reset the variable --> seqsAlNumCounter
				}
			}
		}
		else {
			this->seqsAlNumCounter = 0;
			// 遍历前趋词集合 计算出最佳前趋节点
			for (auto& word : prevWords) {
				double wordProb_log = std::log(trie.getValue(word)) - std::log(totalFreq);
				int curPrevNode = i - word.size();// curPrevNode为此前趋词对应的前趋节点
																					//double curNodeProb = maxProbOfNode(curPrevNode) + wordProb_log;
				double curNodeProb = maxProbOfNode[curPrevNode] + wordProb_log;

				if (curNodeProb > MaxProb) {
					bestPrevNode = curPrevNode;
					MaxProb = curNodeProb;
				}
			}
		}
		prevNode.push_back(bestPrevNode); // 记录node_i的最佳前趋节点
		maxProbOfNode.push_back(MaxProb);	// 记录node_i的最大概率 供以后的节点查询，以实现DP效果
		return;
	}
	
	void backTracking(const std::u16string& src,std::vector<std::u16string>& out) {
		if (prevNode.empty()) {
			// 把这些处理错误信息的封装成一个宏
			std::cout << "the processed data is empty --> class UnigramSeger::prevNode \n"; 
			exit(-1);
		}

		std::vector<size_t> cutPos;
		cutPos.push_back(prevNode.size() - 1);
		for (size_t i = prevNode.size() - 1; i > 0;) {
			cutPos.push_back(prevNode[i]);
			i = prevNode[i];
		}
		std::reverse(cutPos.begin(), cutPos.end());
		if (cutPos[0] != 0) {
			std::cout << "the processed data is wrong --> class UnigramSeger::backTracking()::cutPos \n";
			exit(-1);
		}

		size_t prevPos = 0;
		for (size_t i = 1; i < cutPos.size(); ++i) {
			out.push_back(src.substr(prevPos, cutPos[i] - prevPos));
			prevPos = cutPos[i];
		}

	}
	
	void cut(const std::u16string& src, std::vector<std::u16string>& out) {
		out.clear();
		if (src.empty()) return;
		size_t nodeNum = src.size()+1; // range of node:[0,1,2,...,nodeNum-1]
		
		this->prevNode.clear();
		this->maxProbOfNode.clear();
		this->seqsAlNumCounter = 0;
		for (size_t i{}; i < nodeNum; ++i) {
			computeProbOfNode(i, src);
		}

		backTracking(src,out);
		int a = 100;
		return;
	}
	
	void batchCut(const std::string& srcFilePath) {
		std::vector<std::u16string> sentences;
		std::vector<std::u16string> words;
		JLP::FileUtil::readUnicodeLines(srcFilePath, sentences);
		for (auto& sentence : sentences) {
			cut(sentence, words);
			printWords(words);
			words.clear();

		}
	}

	inline void printWords(std::vector<std::u16string>& words) {
		for (auto& e : words) {
			std::cout << JLP::StringUtils::unicodeToString(e) << "/";
		}
		std::cout << '\n';
		std::string outFilePath = "include/segmenter/corpus/outputByUnigram";
		FileUtil::writeUnicodeLines(outFilePath, words);
	}
private:
	inline bool isAlNum(const char16_t Char){
		if (Char >= u'0'&&Char <= u'9'
			|| Char >= u'A'&&Char <= u'Z'
			|| Char >= u'a'&&Char <= u'z'
			|| Char >= u'０'&&Char <= u'９'
			|| Char >= u'Ａ'&&Char <= u'Ｚ'
			|| Char >= u'ａ'&&Char <= u'ｚ')
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}; // end class UnigramSeger<T>

} // end namespace JLP

#endif

