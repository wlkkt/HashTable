#pragma once

#include <iostream>
#include <vector>
#include <utility>
using namespace std;


//���÷º��������������ռ��⣬ʹ�ÿ�ɢ�кͱ�ɢ�ж�����ʹ�ø÷º���
//����ֱ��ǿת�����ͣ�������ǿתΪsize_t����,��Ϊ��ʹ������Ҳ����Ϊ��
template<class K>
struct HashFunc
{
	size_t operator()(const K& key)
	{
		return (size_t)key;
	}
};

//��HashFunc�����ػ����������int�Ⱦͻ�������ķº����������string�ͻ��ø��ػ��汾��HashFunc
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
		State _state = EMPTY;//Ĭ�������ΪEMPTY
	};


	//�����ַ������͵�ǿ������ת��
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


	template <class K, class V, class Hash = HashFunc<K>>//ǿת��ʽ��ȱʡֵ��ǿתΪ����
	class HashTable
	{
	public:
		//ʵ������ϣ��ʱ������ϣ��Ĵ�С�͵ײ����������������Ϊ10
		HashTable()
		{
			_table.resize(10);
		} 

		bool Insert(const pair<K, V>& kv)
		{
			//��ֹ����
			if (Find(kv.first))
				return false;

			//���������Ӵ��ڵ���0.7ʱ�������ݣ��Կռ任ʱ��
			if (_n * 10 / _table.size() >= 0.7)//����/ȡ���ò���С��������ֱ����_n * 10 
			{
				//�½�һ����ϣ��
				HashTable<K,V, Hash> newHT;
				//ȷ���¹�ϣ��Ĵ�С
				newHT._table.resize(_table.size() * 2);
				 
				//���ɱ��еļ�����ӳ�䵽�±���
				for (size_t i = 0; i < _table.size(); i++)
				{
					if (_table[i]._state ==  EXIST)//ԭ����ĳ�±��״̬Ϊ����ʱ����ԭ��ĳ�±��д�ŵļ�ֵ�Բ����±���
					{
						newHT.Insert(_table[i]._kv);//ֱ�Ӹ���Insert����
					}
				}
				_table.swap(newHT._table);
			}

			Hash sh;//����ǿתΪ���͵ķº���

			size_t hashi = sh(kv.first) % _table.size();//��ȡӳ��λ�ã�Ӧ�ü�ֵ��key % ��������ЧԪ�صĸ���

			//����̽��
			while (_table[hashi]._state == EXIST)//̽�鵽Ŀ��ӳ��λ�ô�������ʱ�ͽ���̽��
			{
				++hashi;
				hashi %= _table.size();//ȡģ�ķ�ʽ��ֹhashiԽ��
			}

			//ȷ����λ�ú���в���
			_table[hashi]._kv = kv;//�ں��ʵ�ӳ��λ�ò���
			_table[hashi]._state = EXIST;//Ȼ��󽫸�λ�õ�״̬��ʶΪEXIST
			++_n;//��ϣ������Ч����Ԫ��+1
			return true;
		}


		HashDate<K, V>* Find(const K& key)
		{
			Hash hs;//����ǿתΪ���͵ķº���

			//����Ҫ����ļ���ӳ��λ��
			size_t hashi = hs(key) % _table.size();//��ȡӳ��λ�ã�Ӧ�ü�ֵ��key % ��������ЧԪ�صĸ���

			//����̽��
			while (_table[hashi]._state != EMPTY)//��ӳ��λ����ȥ���ң������λ���ϲ�Ϊ�գ�
			{
				if (_table[hashi]._state == EXIST && _table[hashi]._kv.first == key)//��Ϊ�ջ�������ɾ���ʹ��������������Ҫ����λ�õ�״̬Ϊ����ʱ�ſ���
					//����ģ�if (_table[hashi]._kv.first == key)//��Ϊ�ջ�������ɾ���ʹ��������������Ҫ����λ�õ�״̬Ϊ����ʱ�ſ���
				{
					return &_table[hashi];//���ظ�λ�õĵ�ַ
				}
				++hashi;
				hashi %= _table.size();//ȡģ�ķ�ʽ��ֹhashiԽ��
			}
			return nullptr;//�Ҳ����ͷ��ؿ�

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
				ret->_state = DELETE;//ɾ��ʱֻ�ǽ���λ�õı�ʶ����Ϊ��DELETE��λ���ϻ������һ������ֻ�����ü����Ա���ʱ�滻
				--_n;
				return true;
			}

		}


	private:
		vector<HashDate<K, V>> _table;//��ϣ��Ĵ�С�͵ײ��������������ͨ�� _table������ģ���������¹�ϣ��Ĵ�С_table.size�͵ײ������������Ҫ�ֿ���
		size_t _n;//��ϣ���е���Ч���ݸ���
	};

	void TeshHT1()
	{
		int a[] = { 10001,11,55,24,19,12,31 };
		HashTable<int, int>  ht;//ʹ��ȱʡ��ǿת��ʽ
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
		////ʹ��StringHashFunc
		//HashTable<string, int, StringHashFunc> ht;

		//ht.Insert(make_pair("sort", 1));
		//ht.Insert(make_pair("left", 1));
		//ht.Insert(make_pair("insert", 1));

		//�鿴�Ƿ�����÷º���StringHashFunc��stringת��Ϊ���ͣ�������������޹أ�
		////StringHashFunc()ʵ����һ�������ķº��������Ȼ��("bacd")��÷º�������
		//cout << StringHashFunc()("bacd") << endl;
		//cout << StringHashFunc()("abcd") << endl;
		//cout << StringHashFunc()("aadd") << endl;
	

		//ʹ���ػ��汾
		HashTable<string, int> ht;

		ht.Insert(make_pair("sort", 1));
		ht.Insert(make_pair("left", 1));
		ht.Insert(make_pair("insert", 1));

		//�鿴�Ƿ�����÷º������ػ��汾HashFunc<string>��stringת��Ϊ����
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
			_table.resize(10, nullptr);//��ʼ��ʱ����������ʮ��λ�ã�ÿ��λ�ö���ŵ��ǿ�ָ��
			_n = 0;
		}

		//��ϣ��ĵײ�����vector�����������Զ�����������vector�и���λ�ô�ŵĶ����Զ�������Node���Զ������͵�����ʱ��������ǵ��������������Ի�Ҫ����дһ����������
		~HashTable() 
		{
			//����ɾ��
			for (size_t i = 0; i < _table.size(); i++)
			{
				Node* cur = _table[i];
				while (cur)
				{
					Node* next = cur->_next;
					delete cur;

					cur = next;
				}

				_table[i] = nullptr;//ɾ��һ��λ���ϵ�һ��������ͽ���λ�ô�ŵ�ָ���ÿ�
			}
		
		}


//����ͷ��ķ�ʽ
bool Insert(const pair<K, V>& kv)
{
	//��ֹ����
	if(Find(kv.first))
		return false;

	//����������½���ӳ���ϵ��������������࣬���±��������ƶ���ɾ���ɱ�Ľ����Ҫ�ܶ����Դ
	//��Ϊ�Ƕ�����һ�������������ϵģ����Ե���ŵ���ЧԪ�ظ��� == �����Сʱ����Ҫ����
	//if (_n == _table.size())
	//{
	//	//�½�һ����ϣ��
	//	HashTable<K, V> newHT;
	//	//ȷ���¹�ϣ��Ĵ�С
	//	newHT._table.resize(_table.size() * 2);

	//	//���ɱ��еļ�����ӳ�䵽�±���
	//	for (size_t i = 0; i < _table.size(); i++)//ÿһ�ε�forѭ�����ǽ�ĳһ�������±��еĽ�㶼���ӵ��±���
	//	{
	//		Node* cur = _table[i];
	//		while (cur)
	//		{
	//			newHT.Insert(_table[i]._kv);//ֱ�Ӹ���Insert����
	//			cur = cur->_next;
	//		}
	//	}
	//	_table.swap(newHT._table);//ʹ�ý����¾ɱ����д���ĵ�ַ��Ϣ
	//}

	//Ӧ����ȷ����ӳ���ϵ��ֱ�ӽ��ɱ��е����н���ƶ����±��У������ƶ���ɱ���ֻ������һ��vector���Ҳ��ô����µĽ��
	if (_n == _table.size())
	{
		vector<Node*> newTable(_table.size() * 2, nullptr);//�趨�±��С����ʼ������λ��
		for (size_t i = 0; i < _table.size(); i++)
		{
			Node* cur = _table[i];//curָ��ɱ���±�Ϊiλ�õĽ�㣨�ý����һ�������ͷ��㣩
					
			//����һ������
			while (cur)
			{
				Node* next = cur->_next;//next���cur����һ������λ��
				//ȷ�����ɱ�curָ��Ľ��Ҫͷ���±���ĸ�λ��
				size_t hashi = cur->_kv.first % newTable.size();
				cur->_next = newTable[hashi];//ͷ�壨���˻�ȥ�������ע�ͣ��������е�����˷��£�
				newTable[hashi] = cur;

				cur = next;//curָ���Լ�����һ�����
			}

			_table[i] = nullptr;//�ÿ�
		}
		/*cout <<"����ǰ�ɱ�ĵ�ַΪ:" <<  & _table << endl;
		cout <<"����ǰ�±�ĵ�ַΪ:" <<  &newTable << endl;*/

		_table.swap(newTable);//����

	/*	cout << "������ɱ�ĵ�ַΪ:" << &_table << endl;
		cout << "�������±�ĵ�ַΪ:" << &newTable << endl;*/
	}

	size_t hashi = kv.first % _table.size();
	Node* newnode = new Node(kv);//����������
	//ͷ�壨������ĳ��λ��û�н�����ʱ_table[hashi] == nullptr��
	newnode->_next = _table[hashi];//_�½���next���ָ��ǰ�����ͷָ��ָ��Ľ��
	_table[hashi] = newnode;//������ͷָ��ָ���½��
	++_n;
	return true;

	//��ϣͰ��ͷ��������:
	//Node* head = nullptr; // ��ʼ��һ���յ���������������ÿ��λ�ö���һ����ָ��
	//void insertAtHead(int value) {
	//	Node* newNode = new Node(value); // ����һ���½ڵ�
	//	newNode->next = head; // ���½ڵ�ĺ�̽ڵ�ָ��ǰ��ͷ�ڵ�
	//	head = newNode; // ����ͷ�ڵ�ָ�룬ʹ��ָ���½ڵ�
	//}
}

		//����ı���
		Node* Find(const K& key)
		{
			size_t hashi = key % _table.size();
			Node* cur = _table[hashi];//ֱ��ȥӳ��λ���ϲ���
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
		vector<Node*> _table;//�����е�ÿ��λ�ô�ŵĶ��ǽ���ָ��
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