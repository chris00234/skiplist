#ifndef ___SKIP_LIST_HPP
#define ___SKIP_LIST_HPP
#include <cmath> // for log2
#include <string>
#include <vector>

#include "runtimeexcept.hpp"

// If we are inserting unsigned integer x into a skip list,
// and have made i previous coin flips on this particular insertion
// this will return the result of a coin flip.
// A return of true indicates heads and false means tails.
static bool flipCoin(unsigned x, unsigned i)
{
	char c;
	unsigned first8Bits = (x & 0xFF000000) / 0x01000000;
	unsigned next8Bits = (x & 0x00FF0000) / 0x00010000;
	unsigned andThen = (x & 0x0000FF00) / 0x00000100;
	unsigned lastBits = (x & 0x000000FF);
	c = first8Bits ^ next8Bits ^ andThen ^ lastBits;
	i = i % 8;
	return (c & (1 << i)) != 0;
}

// If we are inserting a std::string into the skip list, we use this instead.
static bool flipCoin(std::string s, unsigned i)
{
	char c = s[0];
	for (unsigned j = 1; j < s.length(); j++)
	{
		c = c ^ s[j];
	}
	i = i % 8;
	return (c & (1 << i)) != 0;
}



template<typename Key, typename Value>
class SkipList
{


private:
	// private variables go here.
	struct Node
	{
		Key key{};
		Value value{};
		Node* down = nullptr;
		Node* next = nullptr;

	};
	Node* head;

public:


	SkipList()
	{
		Node* fastLane = new Node;
		head = fastLane;
		Node* baseLane = new Node;
		head->down = baseLane;
	}

	~SkipList()
	{
		Node* curr = head;
		Node* currNext = curr->next;

		while (curr != nullptr)
		{
			Node* currtmp = curr;

			while (currNext != nullptr)
			{
				Node* currNexttmp = currNext;
				currNext = currNext->next;
				delete currNexttmp;

			}

			curr = curr->down;
			if (curr != nullptr)
				currNext = curr->next;
			delete currtmp;
		}

	}

	
	size_t size() const noexcept
	{
		Node* tmp = head;
		if (isEmpty())
			return 0;
		while (tmp->down != nullptr)
		{
			tmp = tmp->down;
		}
		size_t total = 0;
		while (tmp->next != nullptr)
		{
			tmp = tmp->next;
			total++;
		}
		return total;
	}

	bool isEmpty() const noexcept
	{
		Node* tmp = head;
		while (tmp->down != nullptr)
		{
			tmp = tmp->down;
		}
		if (tmp->next == nullptr)
			return true;
		return false;

	}

	unsigned numLayers() const noexcept
	{
		Node* tmp = head;
		unsigned layerCount = 0;
		while (tmp->down != nullptr)
		{
			tmp = tmp->down;
			layerCount++;
		}
		return layerCount + 1;
	}



	unsigned height(const Key& k) const
	{
		unsigned layer = 0;
		Node* curr = head;
		Node* currNext = curr->next;
		bool found = false;
		if (isEmpty())
			return 1;
		while (curr != nullptr)
		{

			if (currNext != nullptr)
			{
				if (currNext->key == k)
				{
					found = true;
					layer++;
					Node* find = currNext;
					while (find->down != nullptr)
					{
						find = find->down;
						layer++;
					}
					break;

				}
				else if (currNext->key < k)
				{
					curr = curr->next;
					currNext = curr->next;
				}
				else if (currNext->key > k)
				{
					curr = curr->down;
					if (curr != nullptr)
						currNext = curr->next;
				}

			}
			else if (currNext == nullptr)
			{
				curr = curr->down;
				if (curr != nullptr)
					currNext = curr->next;
			}



		}
		if (found == false)
			throw RuntimeException("The element is not in list");


		return layer;

	}


	
	Key nextKey(const Key& k) const
	{
		Node* curr = head;
		Node* currNext = curr->next;
		bool foundSubsequence = false;
		bool findelement = false;
		Key key{}; 

		while (curr != nullptr)
		{
			if (currNext != nullptr)
			{
				if (currNext->key == k)
				{

					Node* find = currNext;
					findelement = true;
					while (find->down != nullptr)
					{
						find = find->down;

					}
					if (find->next != nullptr)
					{
						foundSubsequence = true;
						key = find->next->key;

					}
					else if (find->next == nullptr)
					{
						foundSubsequence = false;
					}
					break;



				}
				else if (currNext->key < k)
				{
					curr = curr->next;
					currNext = curr->next;
				}
				else if (currNext->key > k)
				{
					curr = curr->down;
					if (curr != nullptr)
						currNext = curr->next;
				}

			}
			else if (currNext == nullptr)
			{
				curr = curr->down;
				if (curr != nullptr)
					currNext = curr->next;
			}



		}
		if (findelement == false)
		{
			throw RuntimeException("There is no element");
		}
		else if (foundSubsequence == false)
		{
			throw RuntimeException("There is no subsequence element");
		}
		return key;

	}

	
	Key previousKey(const Key& k) const
	{
		Node* curr = head;
		Node* currNext = curr->next;
		Node* checker = head;
		bool foundprev = false;
		bool foundkey = false;
		Key prev{}; 
		while (curr != nullptr)
		{
			if (currNext != nullptr)
			{
				if (currNext->key > k)
				{
					curr = curr->down;
					if (curr != nullptr)
					{
						currNext = curr->next;
					}
				}
				else if (currNext->key < k)
				{
					curr = curr->next;
					currNext = curr->next;
				}
				else if (currNext->key == k)
				{
					Node* currtmp = curr;
					foundkey = true;
					while (currtmp->down != nullptr)
					{
						currtmp = currtmp->down;
					}
					while (currtmp->next->key < currNext->key)
					{
						currtmp = currtmp->next;
					}
					while (checker->down != nullptr)
					{
						checker = checker->down;
					}
					if (currtmp == checker)
					{
						foundprev = false;
					}
					else if (currtmp != checker)
					{
						foundprev = true;
						prev = currtmp->key;
					}
					break;
				}
			}
			else if (currNext == nullptr)
			{
				curr = curr->down;
				if (curr != nullptr)
				{
					currNext = curr->next;
				}
			}
		}
		if (foundkey == false)
		{
			throw RuntimeException("There is no element");
		}
		if (foundprev == false)
		{
			throw RuntimeException("There is no previous element");
		}
		return prev;
	}

	
	Value& find(const Key& k)
	{
		Node* curr = head;
		Node* currNext = curr->next;
		Value foundvalue{}; 
		bool foundkey = false;

		while (curr != nullptr)
		{
			if (currNext != nullptr)
			{
				if (currNext->key > k)
				{
					curr = curr->down;
					if (curr != nullptr)
					{
						currNext = curr->next;
					}
				}
				else if (currNext->key < k)
				{
					curr = curr->next;
					currNext = curr->next;
				}
				else if (currNext->key == k)
				{

					foundkey = true;
					foundvalue = currNext->value;
					return currNext->value;
					break;
				}
			}
			else if (currNext == nullptr)
			{
				curr = curr->down;
				if (curr != nullptr)
				{
					currNext = curr->next;
				}
			}
		}


		throw RuntimeException("There is no element");



	}

	
	const Value& find(Key k) const
	{
		Node* curr = head;
		Node* currNext = curr->next;
		Value foundvalue{}; 
		bool foundkey = false;

		while (curr != nullptr)
		{
			if (currNext != nullptr)
			{
				if (currNext->key > k)
				{
					curr = curr->down;
					if (curr != nullptr)
					{
						currNext = curr->next;
					}
				}
				else if (currNext->key < k)
				{
					curr = curr->next;
					currNext = curr->next;
				}
				else if (currNext->key == k)
				{

					foundkey = true;
					foundvalue = currNext->value;
					return currNext->value;
					break;
				}
			}
			else if (currNext == nullptr)
			{
				curr = curr->down;
				if (curr != nullptr)
				{
					currNext = curr->next;
				}
			}
		}

		throw RuntimeException("There is no element");

	}



	bool insert(const Key& k, const Value& v)
	{
		unsigned flipCount = 0;
		while (flipCoin(k, flipCount))
		{
			flipCount++;
			if (size() + 1 <= 16)
			{
				if ((flipCount + 2) == 13)
					break;
			}
			else if (size() + 1 > 16)
			{

				if (flipCount == 3 * ceil(log2(size() + 1)) - 1)
				{
					break;
				}
					
			}
		}

		unsigned layerIncrement = 0;
		if (numLayers() <= (flipCount + 1))
		{
			layerIncrement = (flipCount + 1) - numLayers() + 1;
		}
	
		while (layerIncrement >= 1)
		{
			Node* sentinel = new Node;
			sentinel->down = head;
			head = sentinel;
			layerIncrement--;
		}
	

		int lvl = numLayers();
		Node* previousInsertNode = nullptr;
		bool set_previousNode = false;
		
		Node* curr = head;
		Node* currNext = head->next;


		while (lvl >= 1)
		{
			if (lvl > (flipCount + 1))
			{
				if (currNext != nullptr)
				{
					if (currNext->key > k)
					{
						curr = curr->down;
						currNext = curr->next;
						lvl--;
					}
					else if (currNext->key == k)
						return false;
					else if (currNext->key < k)
					{
						curr = curr->next;
						currNext = currNext->next;
					}
				}
				else if (currNext == nullptr)
				{
					curr = curr->down;
					currNext = curr->next;
					lvl--;
				}

			}
			else if (lvl == (flipCount + 1))
			{
				if (currNext != nullptr)
				{
					if (currNext->key > k)
					{

						Node* insertNode = new Node;
						insertNode->key = k;
						insertNode->value = v;
						curr->next = insertNode;
						insertNode->next = currNext;
						previousInsertNode = insertNode;
						set_previousNode = true;
						curr = curr->down;
						if (lvl != 1)
							currNext = curr->next;
						lvl--;
					}
					else if (currNext->key == k)
					{
						return false;
					}
					else if (currNext->key < k)
					{
						curr = curr->next;
						currNext = currNext->next;
					}
				}
				else if (currNext == nullptr)
				{

					Node* insertNode = new Node;

					curr->next = insertNode;
					insertNode->next = currNext;


					insertNode->key = k;
					insertNode->value = v;
					previousInsertNode = insertNode;
					set_previousNode = true;
					curr = curr->down;



					if (lvl != 1)
						currNext = curr->next;
					lvl--;

				}
			}
			else if (lvl < (flipCount + 1))
			{
				if (currNext != nullptr)
				{
					if (currNext->key > k)
					{
						Node* insertNode = new Node;
						insertNode->key = k;
						insertNode->value = v;
						if (set_previousNode == true)
						{
							previousInsertNode->down = insertNode;
						}
						curr->next = insertNode;
						insertNode->next = currNext;
						previousInsertNode = insertNode;
						set_previousNode = true;
						curr = curr->down;
						if (lvl != 1)
							currNext = curr->next;
						lvl--;
					}
					else if (currNext->key == k)
						return false;
					else if (currNext->key < k)
					{
						curr = curr->next;
						currNext = currNext->next;
					}

				}
				else if (currNext == nullptr)
				{
					Node* insertNode = new Node;
					insertNode->key = k;
					insertNode->value = v;
					if (set_previousNode == true)
					{
						previousInsertNode->down = insertNode;
					}
					curr->next = insertNode;
					insertNode->next = currNext;

					previousInsertNode = insertNode;

					set_previousNode = true;
					curr = curr->down;

					if (lvl != 1)
						currNext = curr->next;

					lvl--;

				}
			}

		}

		return true;


	}



	std::vector<Key> allKeysInOrder() const
	{
		std::vector<Key> r;
		Node* tmp = head;
		while (tmp->down != nullptr)
		{
			tmp = tmp->down;
		}

		while (tmp->next != nullptr)
		{
			tmp = tmp->next;
			r.push_back(tmp->key);
		}
		return r;
	}

	bool isSmallestKey(const Key& k) const
	{
		Node* tmp = head;
		while (tmp->down != nullptr)
		{
			tmp = tmp->down;
		}
		Node* exceptioncheck = tmp;
		bool found = false;
		while (exceptioncheck->next != nullptr)
		{
			exceptioncheck = exceptioncheck->next;
			if (exceptioncheck->key == k)
				found = true;
		}
		if (found == false)
		{
			throw RuntimeException("There is no existing element");
		}
		else if (tmp->next->key == k)
		{
			return true;
		}
		return false;
	}


	bool isLargestKey(const Key& k) const
	{
		Node* tmp = head;
		while (tmp->down != nullptr)
		{
			tmp = tmp->down;
		}
		Node* exceptioncheck = tmp;
		bool found = false;
		while (exceptioncheck->next != nullptr)
		{
			exceptioncheck = exceptioncheck->next;
			if (exceptioncheck->key == k)
				found = true;
		}
		if (found == false)
		{
			throw RuntimeException("There is no existing element");
		}
		else
		{
			while (tmp->next != nullptr)
			{
				tmp = tmp->next;
			}
			if (tmp->key == k)
				return true;
		}
		return false;
	}
};

#endif




