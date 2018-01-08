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
	void computeProbOfNode(size_t i, const std::u16string& src) {	// ����ڵ�i��������
		if (0 == i) {
			prevNode.push_back(0);				// ��¼node_i�����ǰ���ڵ�
			maxProbOfNode.push_back(0);   // maxProbOfNode(0)==>log( P(node 0) )==>log( 1 )==0
			return;
		}

		std::vector<std::u16string> prevWords;
		double MaxProb = -100000; // ��ѡ�ڵ����  ע��ȡlog��õ���ֵ
		size_t bestPrevNode = 0; // ��ѡ���ǰ���ڵ�

		// ����ǰ���ʼ���
		trie.matchPrevWords(src, i - 1, prevWords);

		if (prevWords.size() == 0) {  // ƽ������ ���δ�ڴʵ����ҵ��Ĵ�
			// �ٶ� smoothLogProb=log(P(newWord)) <===>P(newWord)=0.2*1/totalFreq ���ȡֵӦ�ñ� P=1/totalFreq Сһ������������
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
			// ����ǰ���ʼ��� ��������ǰ���ڵ�
			for (auto& word : prevWords) {
				double wordProb_log = std::log(trie.getValue(word)) - std::log(totalFreq);
				int curPrevNode = i - word.size();// curPrevNodeΪ��ǰ���ʶ�Ӧ��ǰ���ڵ�
																					//double curNodeProb = maxProbOfNode(curPrevNode) + wordProb_log;
				double curNodeProb = maxProbOfNode[curPrevNode] + wordProb_log;

				if (curNodeProb > MaxProb) {
					bestPrevNode = curPrevNode;
					MaxProb = curNodeProb;
				}
			}
		}
		prevNode.push_back(bestPrevNode); // ��¼node_i�����ǰ���ڵ�
		maxProbOfNode.push_back(MaxProb);	// ��¼node_i�������� ���Ժ�Ľڵ��ѯ����ʵ��DPЧ��
		return;
	}
	
	void backTracking(const std::u16string& src,std::vector<std::u16string>& out) {
		if (prevNode.empty()) {
			// ����Щ���������Ϣ�ķ�װ��һ����
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
			|| Char >= u'��'&&Char <= u'��'
			|| Char >= u'��'&&Char <= u'��'
			|| Char >= u'��'&&Char <= u'��')
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

