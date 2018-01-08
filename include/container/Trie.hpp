#ifndef JLP_SEGMENTER_CONTAINER_TRIE_H_
#define JLP_SEGMENTER_CONTAINER_TRIE_H_

#include <iostream>
#include <unordered_map>
#include <memory>
#include <iterator>

#include "../../utils/FileUtils.hpp"
#include "../../utils/StringUtils.hpp"

namespace JLP {

template <class T>
struct TrieNode {
	typedef std::unordered_map<char16_t, std::shared_ptr<TrieNode<T>>> Children;
	Children children;
	bool tag{false};	// ָʾ�ýڵ��Ƿ�Ϊ��ֹ�ڵ� 
	T value;					// �ýڵ㱾��ĸ������ݣ�Ҫ��T��Ĭ�Ϲ��캯��
};

template <class T>
class Trie {
private:
	std::shared_ptr<TrieNode<T>> rootNode;
	//!!! ����ֻ�������������ڹ��캯���н���󶨵�heap�ڴ���ĳ�������ϣ�����rootNodeΪnullptr
	bool loadTag{false}; 
public:
	explicit Trie() {
		rootNode = std::make_shared<TrieNode<T>>();
	}
public:
	// ��Trie����words�е����д�������ӦƵ��
	bool buildTrie(const std::vector<std::u16string>&  words,
								 const std::vector<T>& values) 
	{
		if (loadTag) return true;
		if (words.size() != values.size()) {
			std::cout << __DATE__ << " " << __TIME__ << " "
				<< __FILE__ << " " << __LINE__
				<< "The size of words does not match the size of values!!!" << std::endl;
			exit(-1);
		}

		int i = 0;
		for (auto& word : words) {
			this->put(word, values.at(i)); //ע�⸽�����ݵ����
			++i;
		}
		loadTag = true;
		std::cout << "The dictionary is ready now ...\n";
		return true;
	}
	// ��Trie����words�е����д���
	bool buildTrie(const std::vector<std::u16string>&  words)
	{
		if (loadTag) return true;
		
		for (auto& word : words) {
			this->put(word, T{}); //ע�⸽�����ݵ����
	
		}
		loadTag = true;
		std::cout << "The dictionary is ready now ...\n";
		return true;
	}

	// ��Trie�в��뵥����
	void put(const std::u16string& key,const T& value) 
	{  
		std::shared_ptr<TrieNode<T>> walkPtr=rootNode;
		auto iter = key.cbegin();
		for (; iter != key.cend(); ++iter) {
			auto funded= walkPtr->children.find(*iter);
			if (walkPtr->children.end() == funded) {
				walkPtr->children[*iter] = std::make_shared<TrieNode<T>>();
				walkPtr = walkPtr->children[*iter];
			}
			else {
				walkPtr = funded->second;
			}
		}
		// ����ֹ�ڵ�������tag�͸�������
		walkPtr->tag = true;
		walkPtr->value=value;
	}

	// ����src�����ƥ�䳤��
	size_t getMaxLen(const std::u16string& src) 
	{
		if (src.empty()) return 0;
		if (1 == src.size()) return 1; // src����Ϊ1ʱ�������Ƿ�����ڴʵ��ж����յ����ַ��� 

		size_t len{};
		std::shared_ptr<TrieNode<T>> walkPtr = rootNode;
		auto iter = src.cbegin();
		auto start = iter;
		for (; iter != src.cend(); ++iter) {
			auto funded = walkPtr->children.find(*iter);
			if (walkPtr->children.end() == funded) {
				return  (len == 0) ? 1 : len;
			}
			else {
				if(funded->second->tag)	len = std::distance(start,iter)+1;
				walkPtr = funded->second;
			}
		}
		return (len == 0) ? 1 : len;
	}

	void matchPrevWords(const std::u16string& src,size_t i, std::vector<std::u16string>& prevWords) {
		prevWords.clear();
		std::u16string subSrc = src.substr(0, i + 1);
		std::reverse(subSrc.begin(), subSrc.end());
		if (subSrc.empty()) return;
		//if (1 == src.size()) return; // src����Ϊ1ʱ�������Ƿ�����ڴʵ��ж����յ����ַ��� 

		std::shared_ptr<TrieNode<T>> walkPtr = rootNode;
		auto iter = subSrc.cbegin();
		auto start = iter;
		for (; iter != subSrc.cend(); ++iter) {
			auto funded = walkPtr->children.find(*iter);
			if (walkPtr->children.end() != funded) {
				if (funded->second->tag) { prevWords.push_back(subSrc.substr(0, std::distance(start, iter) + 1)); }
				walkPtr = funded->second;
			}
			else {
				return;
			}
		}
		return ;		
	}

	// �ж�key�Ƿ���Trie����
	bool isIn(const std::u16string& key) 
	{
		if (key.empty()) return false;
		if (key.size() > 1) {
			return (key.size() == getMaxLen(key));
		}
		else {
			return true; //TODO:�жϵ������Ƿ���Trie����
		}
	}
	
	// ����key��ֵ
	T getValue(const std::u16string& src) {
		if (src.empty()) { 
			std::cout << "The key is empty!!!\n"; 
			exit(-1);
		}

		std::shared_ptr<TrieNode<T>> walkPtr = rootNode;
		auto iter = src.cbegin();
		for (; iter != src.cend(); ++iter) {
			auto funded = walkPtr->children.find(*iter);
			if (walkPtr->children.end() != funded) {
				walkPtr = funded->second;
			}
		}

		if (walkPtr->tag) return walkPtr->value;
		return	T{0};
	}

};

} // end namespace JLP

#endif
