#pragma once

#include <iostream>
#include <vector>
#include <utility>
using namespace std;


//将该仿函数放在了命名空间外，使得开散列和闭散列都可以使用该仿函数
//可以直接强转的类型，将它们强转为size_t类型,因为即使是整数也可能为负
template<class K>
struct HashFunc
{
	size_t operator()(const K& key)
	{
		return (size_t)key;
	}
};

//对HashFunc采用特化处理，如果是int等就会用上面的仿函数，如果是string就会用该特化版本的HashFunc
template<>
struct HashFunc<string>
{
	size_t operator()(const string& key)
	{
		size_t hash = 0;
		for (auto ch : key)
		{
			hash = hash * 131 + ch;
		}
		return hash;
	}
};


namespace open_address
{
	enum State
	{
		EMPTY,
		EXIST,
		DELETE
	};

	template<class K, class V>
	struct HashDate
	{
		pair<K, V> _kv;
		State _state = EMPTY;//默认情况下为EMPTY
	};


	//处理字符串类型的强制类型转换
	struct StringHashFunc
	{
		size_t operator()(const string& key)
		{
			size_t hashi = 0;
			for (auto ch : key)
			{
				hashi = hashi * 131 + ch;
			}
			return hashi;
		}
	};


	template <class K, class V, class Hash = HashFunc<K>>//强转方式的缺省值是强转为整型
	class HashTable
	{
	public:
		//实例化哈希表时，将哈希表的大小和底层数组的容量均调整为10
		HashTable()
		{
			_table.resize(10);
		} 

		bool Insert(const pair<K, V>& kv)
		{
			//防止冗余
			if (Find(kv.first))
				return false;

			//当负载因子大于等于0.7时进行扩容，以空间换时间
			if (_n * 10 / _table.size() >= 0.7)//采用/取整得不到小数，所以直接令_n * 10 
			{
				//新建一个哈希表
				HashTable<K,V, Hash> newHT;
				//确定新哈希表的大小
				newHT._table.resize(_table.size() * 2);
				 
				//将旧表中的键重新映射到新表中
				for (size_t i = 0; i < _table.size(); i++)
				{
					if (_table[i]._state ==  EXIST)//原表中某下标的状态为存在时，将原表某下标中存放的键值对插入新表中
					{
						newHT.Insert(_table[i]._kv);//直接复用Insert操作
					}
				}
				_table.swap(newHT._table);
			}

			Hash sh;//处理强转为整型的仿函数

			size_t hashi = sh(kv.first) % _table.size();//获取映射位置，应该键值的key % 数组中有效元素的个数

			//线性探测
			while (_table[hashi]._state == EXIST)//探查到目标映射位置存在数字时就进行探测
			{
				++hashi;
				hashi %= _table.size();//取模的方式防止hashi越界
			}

			//确定好位置后进行插入
			_table[hashi]._kv = kv;//在合适的映射位置插入
			_table[hashi]._state = EXIST;//然后后将该位置的状态标识为EXIST
			++_n;//哈希表中有效数据元素+1
			return true;
		}


		HashDate<K, V>* Find(const K& key)
		{
			Hash hs;//处理强转为整型的仿函数

			//计算要查早的键的映射位置
			size_t hashi = hs(key) % _table.size();//获取映射位置，应该键值的key % 数组中有效元素的个数

			//线性探测
			while (_table[hashi]._state != EMPTY)//到映射位置上去查找，如果该位置上不为空，
			{
				if (_table[hashi]._state == EXIST && _table[hashi]._kv.first == key)//不为空还有两种删除和存在两种情况，需要满足位置的状态为存在时才可以
					//错误的：if (_table[hashi]._kv.first == key)//不为空还有两种删除和存在两种情况，需要满足位置的状态为存在时才可以
				{
					return &_table[hashi];//返回该位置的地址
				}
				++hashi;
				hashi %= _table.size();//取模的方式防止hashi越界
			}
			return nullptr;//找不到就返回空

		}


		bool Erase(const K& key)
		{
			HashDate<K, V>* ret = Find(key);
			if (ret == nullptr)
			{
				return false;
			}
			else
			{
				ret->_state = DELETE;//删除时只是将该位置的标识符置为了DELETE该位置上还存放着一个键，只不过该键可以被随时替换
				--_n;
				return true;
			}

		}


	private:
		vector<HashDate<K, V>> _table;//哈希表的大小和底层数组的容量都是通过 _table来管理的，正常情况下哈希表的大小_table.size和底层数组的容量是要分开的
		size_t _n;//哈希表中的有效数据个数
	};

	void TeshHT1()
	{
		int a[] = { 10001,11,55,24,19,12,31 };
		HashTable<int, int>  ht;//使用缺省的强转方式
		for (auto e : a)
		{
			ht.Insert(make_pair(e, e));
		}

		cout << ht.Find(55) << endl;
		cout << ht.Find(31) << endl;

		ht.Erase(55);

		cout << ht.Find(55) << endl;
		cout << ht.Find(31) << endl;
	}

	void TeshHT2()
	{
		////使用StringHashFunc
		//HashTable<string, int, StringHashFunc> ht;

		//ht.Insert(make_pair("sort", 1));
		//ht.Insert(make_pair("left", 1));
		//ht.Insert(make_pair("insert", 1));

		//查看是否可以用仿函数StringHashFunc将string转换为整型（与上面的内容无关）
		////StringHashFunc()实例化一个匿名的仿函数类对象，然后("bacd")向该仿函数传参
		//cout << StringHashFunc()("bacd") << endl;
		//cout << StringHashFunc()("abcd") << endl;
		//cout << StringHashFunc()("aadd") << endl;
	

		//使用特化版本
		HashTable<string, int> ht;

		ht.Insert(make_pair("sort", 1));
		ht.Insert(make_pair("left", 1));
		ht.Insert(make_pair("insert", 1));

		//查看是否可以用仿函数的特化版本HashFunc<string>将string转换为整型
		cout << HashFunc<string>()("bacd") << endl;
		cout << HashFunc<string>()("abcd") << endl;
		cout << HashFunc<string>()("aadd") << endl;

	}

}

namespace hash_bucket
{
	template<class K, class V>
	struct HashNode
	{
		pair<K, V> _kv;
		HashNode<K, V>* _next;
	
		HashNode(const pair<K, V>& kv)
		:_kv(kv)
		, _next(nullptr)
		{}
	};
	//template<class T>
	//struct HashNode
	//{
	//	T _data;
	//	HashNode<T>* _next;

	//	HashNode(const T& data)
	//		:_data(data)
	//		, _next(nullptr)
	//	{}
	//};

	template<class K,class V>
	class HashTable
	{
		typedef HashNode<K, V> Node;

	public:
		HashTable()
		{
			_table.resize(10, nullptr);//初始化时，数组中有十个位置，每个位置都存放的是空指针
			_n = 0;
		}

		//哈希表的底层容器vector函数结束后自动析构，但是vector中各个位置存放的都是自定义类型Node，自定义类型的析构时会调用它们的析构函数，所以还要重新写一个析构函数
		~HashTable() 
		{
			//遍历删除
			for (size_t i = 0; i < _table.size(); i++)
			{
				Node* cur = _table[i];
				while (cur)
				{
					Node* next = cur->_next;
					delete cur;

					cur = next;
				}

				_table[i] = nullptr;//删除一个位置上的一整条链表就将该位置存放的指针置空
			}
		
		}


//采用头插的方式
bool Insert(const pair<K, V>& kv)
{
	//防止冗余
	if(Find(kv.first))
		return false;

	//如果还是重新建立映射关系，如果结点个数过多，在新表创建结点和移动后删除旧表的结点需要很多的资源
	//因为是队列是一个个挂在数组上的，所以当存放的有效元素个数 == 数组大小时才需要扩容
	//if (_n == _table.size())
	//{
	//	//新建一个哈希表
	//	HashTable<K, V> newHT;
	//	//确定新哈希表的大小
	//	newHT._table.resize(_table.size() * 2);

	//	//将旧表中的键重新映射到新表中
	//	for (size_t i = 0; i < _table.size(); i++)//每一次的for循环都是将某一个数组下标中的结点都链接到新表中
	//	{
	//		Node* cur = _table[i];
	//		while (cur)
	//		{
	//			newHT.Insert(_table[i]._kv);//直接复用Insert操作
	//			cur = cur->_next;
	//		}
	//	}
	//	_table.swap(newHT._table);//使用交换新旧表名中代表的地址信息
	//}

	//应该在确认了映射关系后，直接将旧表中的所有结点移动到新表中，这样移动后旧表中只用析构一个vector，且不用创建新的结点
	if (_n == _table.size())
	{
		vector<Node*> newTable(_table.size() * 2, nullptr);//设定新表大小及初始化各个位置
		for (size_t i = 0; i < _table.size(); i++)
		{
			Node* cur = _table[i];//cur指向旧表的下标为i位置的结点（该结点是一条链表的头结点）
					
			//遍历一条链表
			while (cur)
			{
				Node* next = cur->_next;//next存放cur的下一个结点的位置
				//确定将旧表cur指向的结点要头插新表的哪个位置
				size_t hashi = cur->_kv.first % newTable.size();
				cur->_next = newTable[hashi];//头插（忘了回去看外面的注释，这点就是有点会让人发懵）
				newTable[hashi] = cur;

				cur = next;//cur指向自己的下一个结点
			}

			_table[i] = nullptr;//置空
		}
		/*cout <<"交换前旧表的地址为:" <<  & _table << endl;
		cout <<"交换前新表的地址为:" <<  &newTable << endl;*/

		_table.swap(newTable);//交换

	/*	cout << "交换后旧表的地址为:" << &_table << endl;
		cout << "交换后新表的地址为:" << &newTable << endl;*/
	}

	size_t hashi = kv.first % _table.size();
	Node* newnode = new Node(kv);//匿名结点对象
	//头插（数组中某个位置没有结点插入时_table[hashi] == nullptr）
	newnode->_next = _table[hashi];//_新结点的next结点指向当前链表的头指针指向的结点
	_table[hashi] = newnode;//令链表头指针指向新结点
	++_n;
	return true;

	//哈希桶的头插类似于:
	//Node* head = nullptr; // 初始化一个空的链表，现在数组中每个位置都是一个空指针
	//void insertAtHead(int value) {
	//	Node* newNode = new Node(value); // 创建一个新节点
	//	newNode->next = head; // 将新节点的后继节点指向当前的头节点
	//	head = newNode; // 更新头节点指针，使其指向新节点
	//}
}

		//链表的遍历
		Node* Find(const K& key)
		{
			size_t hashi = key % _table.size();
			Node* cur = _table[hashi];//直接去映射位置上查找
			while (cur)
			{
				if (cur->_kv.first == key)
				{
					return cur;
				}

				cur = cur->_next;
			}

			return nullptr;
		}


		bool Erase(const K& key)
		{
		
		
		}

	private:
		vector<Node*> _table;//数组中的每个位置存放的都是结点的指针
		size_t _n;
	};

	void HashTest1()
	{
		int a[] = { 10001,11,55,24,19,12,31,4,34,44 };
			HashTable<int, int> ht;
			for (auto e : a)
			{
				ht.Insert(make_pair(e, e));
			}


			ht.Insert(make_pair(32, 32));
			ht.Insert(make_pair(32, 32));

		///*	ht.Erase(31);
		//	ht.Erase(11);*/


	}


}